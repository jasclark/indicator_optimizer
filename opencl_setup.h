#include "clhelp.h"
#include "kernel_struct.h"

#ifndef OPENCL_SETUP_H_
#define OPENCL_SETUP_H_


struct cl_runtime_env
{
    cl_vars_t cv;
    std::map<std::string, cl_mem_data> gpu_data;
    std::map<std::string, cl_kernel> cl_kernels;
    kernel* kernels;
    int num_kerns;
};

/*This function sets up OpenCL. Should only be called once*/
void setupEnvironment(cl_runtime_env* env,
                        kernel* kernels,
                        std::map<std::string, data_array> cpu_data, 
                        int num_kerns);
/*This function calls the kernel "kernel_name". Vars is a chromosome,
 * and start_index is where in the data you want to start running from,
 * and end_index is where you want stop.*/
void runKernel(cl_runtime_env env,
               std::string kernel_name,
               double* vars,
               double* out,
               int start_index,
               int out_len);
void runKernels(cl_vars_t, kernel*, int);
void releaseClMem(std::map<std::string, cl_mem>);
void moveDataToGpu(cl_vars_t cv,
        std::map<std::string, data_array> cpu_data,
        std::map<std::string, cl_mem_data>* gpu_data,
        kernel* kernels, int num_kerns);
void runKernel(cl_vars_t cv,
            cl_kernel cl_kern,
            kernel kern,
            std::map<std::string, cl_mem_data> gpu_data,
            double* vars);
#endif
