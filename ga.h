#include "ga_helper.h"
  /*
   * @return - returns the most fit value for each gene.
   * @param num_genes is the number of genes per chromosome,
   *        aka the number of parameters to be optimized.
   * @param l_bound is the lower bound of gene values.
   * @param u_bound is the upper bound of gene values.
   * @param data_length is the length of the data arrays(ie price).
   * @param info is the kern_info_t struct that contains the info
   *        that handle_cpu() and handle_gpu() need.
   */
  //core genetic algorithm function
  double** genetic_algo(int num_genes, 
                    int data_length, kern_info_t info) {

    const int GEN_SIZE = 100,
              NUM_SURVIVE = 10, //number of organisms that survive each generation
              NUM_GENERATIONS = 10;
    double*** chr = (double***)malloc(sizeof(double**)*GEN_SIZE);
    int* fittest = (int*)malloc(sizeof(int)*NUM_SURVIVE);
    double* fitness = (double*)malloc(sizeof(double)*GEN_SIZE);
    double* gpu_outA = (double*)malloc(sizeof(double)*data_length);
    double* gpu_outB = (double*)malloc(sizeof(double)*data_length);

    int data_stride = data_length/1;
    int start_index = 0;
    int end_index = start_index + data_stride;
    
    //initialize first generation of chromosomes
    for (int i = 0; i < GEN_SIZE; i++) {
      chr[i] = init_ind(info.env.kernels, info.env.num_kerns);
    }

    //pipeline chromosome evaluations
    omp_set_num_threads(2);
    #pragma omp parallel 
    {
      #pragma omp single
      {
        for(int x = 0; x < NUM_GENERATIONS; x++){
          run_one_generation(chr, gpu_outA, gpu_outB, fitness, 
                             start_index, end_index, GEN_SIZE, info);
          evaluate_gen(fitness, fittest, NUM_SURVIVE, GEN_SIZE);
          create_new_generation(chr, fittest, NUM_SURVIVE, info.env.kernels, 
                                info.env.num_kerns, GEN_SIZE);
          //switches data to avoid overfitting, holds out last data 
          start_index = (end_index + 2*data_stride > data_length) ? 0 : end_index;
          end_index = start_index + data_stride; 
        }
        //do cross-validation and return best
        start_index = data_length - data_stride;
        end_index = data_length;
        run_one_generation(chr, gpu_outA, gpu_outB, fitness, 
                           start_index, end_index, GEN_SIZE, info);
        evaluate_gen(fitness, fittest, NUM_SURVIVE, GEN_SIZE);
      }
    }
    printf("Best profit: %f\n", fitness[fittest[0]]);
    double** best = chr[fittest[0]];
    delete [] fittest; 
    delete [] fitness;   
    delete [] gpu_outA;
    delete [] gpu_outB;
    for (int i = 0; i < GEN_SIZE; i++) {
      delete [] chr[i];
    }
    delete [] chr;  
    return best;
  }
