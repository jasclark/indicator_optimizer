#include "adx_dmi.cpp"
#include "adx_recomendation.cpp"
#include "fitness.h"
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
  double** genetic_algo_serial(int num_genes, int l_bound, int u_bound, 
                    int data_length, kern_info_t info) {

    const int GEN_SIZE = 100,
              NUM_SURVIVE = 10, //number of organisms that survive each generation
              NUM_GENERATIONS = 10000;
    double*** chr = (double***)malloc(sizeof(double**)*GEN_SIZE);
    int* fittest = (int*)malloc(sizeof(int)*NUM_SURVIVE);
    double* fitness = (double*)malloc(sizeof(double)*GEN_SIZE);
    double* gpu_out = (double*)malloc(sizeof(double)*data_length);
    int start_index = 0;
    int end_index = data_length/2;



    //printf("We're in the genetic algorithm\n");
    
    //initialize first generation of chromosomes
    for (int i = 0; i < GEN_SIZE; i++) {
      chr[i] = init_ind(info.env.kernels, info.env.num_kerns);
    }

    for (int i = 0; i < GEN_SIZE; i++) {
      printf("num kerns %d\n", info.env.num_kerns);
      for (int j = 0; j < info.env.num_kerns; j++) {
        printf("num vars %d\n", info.env.kernels[j].num_vars);
        for (int k = 0; k < info.env.kernels[j].num_vars; k++) {
          printf("%d individual, %d kernel, %d val: %f\n", i, j, k, chr[i][j][k]);
        }
      }
      printf("Finished generation %d\n", i);
      }

    //printf("We initialized the chromosomes!\n");

    for(int x = 0; x < NUM_GENERATIONS; x++){
      run_one_generation_serial(chr, gpu_out, fitness, start_index,
                         end_index, GEN_SIZE, info);


      evaluate_gen(fitness, fittest, NUM_SURVIVE, GEN_SIZE);
      //printf("GEN EVALUATED\n");
      create_new_generation(chr, fittest, NUM_SURVIVE, info.env.kernels, info.env.num_kerns, GEN_SIZE);
    }

    //printf("BOOGA BOOGA\n");

    //do cross-validation and return best
    start_index = end_index;
    end_index = data_length - 1;
    //printf("BOOGA BOOGA\n");
    run_one_generation_serial(chr, gpu_out, fitness, start_index, 
                       end_index, GEN_SIZE, info);
    evaluate_gen(fitness, fittest, NUM_SURVIVE, GEN_SIZE);

      for (int k = 0; k < GEN_SIZE; k++) {
        printf("%d fitness: %f\n", k, fitness[k]);
	}

    printf("Best fitness %f\n", fitness[fittest[0]]);

    double** best = chr[fittest[0]];
    /*delete [] fittest; 
    delete [] fitness;   
    delete [] gpu_out; 
    for (int i = 0; i < GEN_SIZE; i++) {
      delete [] chr[i];
    }
    delete [] chr;*/
    return best;
  }
