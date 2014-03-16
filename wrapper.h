#ifndef WRAPPER_H
#define WRAPPER_H


#include <map>
#include "opencl_setup.h"

  struct kern_info_t {
    cl_runtime_env env;
    map<string, data_array> cpu_data;
  };


  /*
   * @param chromosomes is one organisms the set of kernel parameter values
   * @param gpu_out is the global array that each kernel adds to.
   * @param start_index is the start index for the input data
   * @param end_index is the end index for the input data
   * @param info is the static data needed to run the kernels(ie price)
   */
  void handle_gpu(double** chromosomes,
                  double* gpu_out,
                  int start_index,
                  int end_index,
                  kern_info_t info); 
  /*
   * @return - returns the fitness score for one organism's gpu_result
   * @param gpu_result is the final output of all sequential the kernel runs
   * @param start_index is the start index for the input data
   * @param end_index is the end index for the input data
   * @param info is the static data needed to run the fitness(ie price)
   */
  double handle_cpu(double** chromosomes,
                    double* gpu_result, 
                    int start_index,
                    int end_index, 
                    kern_info_t info);

#endif
