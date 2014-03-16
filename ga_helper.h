#include <stdio.h>
#include <cstdlib>
#include "omp.h"
#include "wrapper.h"
#include "indicators_serial/bollinger.cpp"
#include "fitness.h"
#include "opencl_setup.h"
#include "adx_recomendation.cpp"

void handle_gpu(double** chromosomes,
                  double* gpu_out,
                  int start_index,
                  int end_index,
                  kern_info_t info) {
      for (int i = 1; i < info.env.num_kerns; i++)
      {
          string kernel_name = info.env.kernels[i].name;
          runKernel(info.env,
               kernel_name,
               chromosomes[i],
               gpu_out,
               start_index,
               end_index - start_index);
      }
      int num_zeros = 0;
  }

  double handle_cpu(double** chromosomes, 
                    double* gpu_result, 
                    int start_index,
                    int end_index, 
                    kern_info_t info) {
    int length;
    double stop_prof, stop_loss;
    data_array close_price = info.cpu_data[string("cPrice")];
    data_array high_price = info.cpu_data[string("hPrice")];
    data_array low_price = info.cpu_data[string("lPrice")];
    length = end_index - start_index;
    stop_prof = chromosomes[0][0];
    stop_loss = chromosomes[0][1];

    //leaving printf for debugging
    double result = fitness_func(high_price.array, 
                   low_price.array, 
                   close_price.array, 
                   gpu_result, 
                   length, 
                   stop_prof, 
                   stop_loss); 
    return result;
  }
  //initializes one individual's chromosomes
  double** init_ind(kernel* kernels, int numkerns) { 
    double** ind_chr = new double*[numkerns];
    for (int i = 0; i < numkerns; i++) {
      ind_chr[i] = new double[kernels[i].num_vars];
      for (int j = 0; j < kernels[i].num_vars; j++) {
        ind_chr[i][j] = ((double)(kernels[i].ranges[j].hi - kernels[i].ranges[j].lo)) * 
                     ((double)rand()/(double)RAND_MAX) + kernels[i].ranges[j].lo;
        //printf("%d, %d, %f\n", i, j, ind_chr[i][j]);
      }
    }
    return ind_chr;
  }
  
  //evalutes one generation of chromosomes and returns
  //the indices of the chromosomes that survive
  void evaluate_gen (double* fitness, int* fittest, int num_survive, int gen_size) {
    for (int i = 0; i < num_survive; i++) {
      fittest[i] = -1;
    }
    for (int i = 0; i < gen_size; i++){
      double score = fitness[i];
      for (int k = 0; k < num_survive; k++) {
        int index = fittest[k];
        if (index == -1 || score > fitness[index]){
          for (int j = k; j < num_survive - 1; j++){
            fittest[j+1] = fittest[j];
          }
          fittest[k] = i;
          break;
        }
      }
    }
  }

  //merge fittest of previous generation to make next generation
  void create_new_generation(double*** chr, int* fittest, int num_survive, 
                             kernel* kernels, int numkerns, int gen_size) {
    const float MUT_RATE = .5; //percentage of genes that get mutated
    double*** old_generation = (double***)malloc(sizeof(double**)*num_survive);
    double** copy;
    double gene;
    
    for (int i = 0; i < num_survive; i++){
      copy = (double**)malloc(sizeof(double**)*numkerns);
      for (int j = 0; j < numkerns; j++){
        copy[j] = new double[kernels[j].num_vars];
        for (int k = 0; k < kernels[j].num_vars; k++) {
          copy[j][k] = chr[fittest[i]][j][k];
        }
      }
      old_generation[i] = copy;
    }

    for (int i = 0; i < gen_size; i++){
      double** parent1 = old_generation[rand()%num_survive];
      double** parent2 = old_generation[rand()%num_survive];
      for (int j = 0; j < numkerns; j++) {
        for (int k = 0; k < kernels[j].num_vars; k++) {
          if ((rand() % (int) (1.0/MUT_RATE)) == 0) {
            chr[i][j][k] = ((double)(kernels[j].ranges[k].hi - kernels[j].ranges[k].lo)) * ((double)rand()/(double)RAND_MAX) + kernels[j].ranges[k].lo;
	  }
          else if (rand() % 2 == 0) {
            chr[i][j][k] = parent1[j][k];
	  }
          else {
            chr[i][j][k] = parent2[j][k];
          }
        }
      }
    }
    /*for (int i = 0; i < num_survive; i++){
      delete [] old_generation[i];
    }
    delete [] old_generation; */  
  }

    //pipelines the GPU and CPU elements of the backtester for one generation
 void run_one_generation(double*** chr, double* gpu_outA, double* gpu_outB,
                            double* fitness, int start_index, int end_index, 
                            int gen_size, kern_info_t info){
      double* temp;
      handle_gpu(chr[0], gpu_outA, start_index, end_index, info);
      for (int i = 0; i < gen_size; i++) {
        #pragma omp task 
        fitness[i] = handle_cpu(chr[i], gpu_outA, start_index, end_index, info);
        if (i < gen_size - 1){
          #pragma omp task
          handle_gpu(chr[i+1], gpu_outB, start_index, end_index, info);
        }
        #pragma omp taskwait
        temp = gpu_outA;
        gpu_outA = gpu_outB;
        gpu_outB = temp;
      }
 }

  void run_one_generation_serial(double*** chr, double* recommendation, double* fitness,
                                 int start_index, int end_index,
				 int gen_size, kern_info_t info){
    for (int i = 0; i < gen_size; i++) {
      /*printf("%f\n", chr[i][0][0]);
      printf("%f\n", chr[i][0][1]);
      printf("%f\n", chr[i][1][0]);
      printf("%f\n", chr[i][1][1]);*/
      //printf("i equals %d\n", i);

      for (int j = 0; j < info.cpu_data[string("hPrice")].len; j++) {
        recommendation[j] = 0.0;
      }

      //printf("zeroed recommendation\n");

      /*adx_recommendation(info.cpu_data[string("hPrice")].array,
                         info.cpu_data[string("lPrice")].array,
                         info.cpu_data[string("cPrice")].array,
                         info.cpu_data[string("hPrice")].len,
                         chr[i][1][0], chr[i][1][1], chr[i][1][2],
                         recommendation);*/

      //printf("ADX still works\n");

      bollinger_bands(&(info.cpu_data[string("cPrice")]),
	              recommendation,
                      chr[i][2][0], chr[i][2][1],
                      chr[i][2][2], 1.0 - chr[i][1][2]);

      //printf("BB works?\n");

      //printf("created recommendation\n");
      fitness[i] = fitness_func(info.cpu_data[string("hPrice")].array,
                                info.cpu_data[string("lPrice")].array, 
                                info.cpu_data[string("cPrice")].array, 
                                recommendation, 
				info.cpu_data[string("hPrice")].len, 
				chr[i][0][0], chr[i][0][1]);
    }
  }
