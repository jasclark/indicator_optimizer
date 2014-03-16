#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <cassert>
#include <cmath>
#include <unistd.h>

#include "clhelp.h"
#include "kernel_struct.h"

struct cl_runtime_env
{
    cl_vars_t cv;
    std::map<std::string, cl_mem_data> gpu_data;
    std::map<std::string, cl_kernel> cl_kernels;
    kernel* kernels;
    int num_kerns;
};

cl_vars_t setupRuntime(kernel*, std::map<std::string, cl_kernel>*, int);
void releaseClMem(std::map<std::string, cl_mem_data>);
void moveDataToGpu(cl_vars_t,
        std::map<std::string, data_array>,
        std::map<std::string, cl_mem_data>*,
        kernel*, int);

void runKernel(cl_vars_t cv,
            cl_kernel cl_kern,
            kernel kern,
            std::map<std::string, cl_mem_data> gpu_data,
            double* vars);

void runKernel(cl_runtime_env env,
               std::string kernel_name,
               double* vars,
               double* out,
               int start_index,
               int out_len)

{
    kernel kern;
    for (int i = 1; i < env.num_kerns; i++)
    {
        if (env.kernels[i].name == kernel_name)
            kern = env.kernels[i];
    }
    runKernel(env.cv, env.cl_kernels[kernel_name],
              kern,
              env.gpu_data,
              vars);
    cl_int err;

    err = clEnqueueReadBuffer(env.cv.commands, env.gpu_data["out"].array,
                              true,
                              start_index,
                              sizeof(double)*out_len,
                              out,
                              0, NULL, NULL);
    CHK_ERR(err);

    err = clFlush(env.cv.commands);
    CHK_ERR(err);
}

void setupEnvironment(cl_runtime_env* env,
                        kernel* kernels,
                        std::map<std::string, data_array> cpu_data,
                        int num_kerns)
{
    (*env).cv = setupRuntime(kernels, &(*env).cl_kernels, num_kerns);
    moveDataToGpu((*env).cv, cpu_data, &(*env).gpu_data, kernels, num_kerns);
    (*env).kernels = kernels;
    (*env).num_kerns = num_kerns;
}


cl_vars_t setupRuntime(kernel* kernels, std::map<std::string, cl_kernel>* 
    kernel_map, int num_kerns)
{
  std::string kernel_source_str[num_kerns-1];
  std::string arraycompact_kernel_file[num_kerns-1];
  cl_vars_t cv;
  std::list<std::string> kernel_names; 
  //get the names of the kernel files
  for (int i = 1; i<num_kerns; i++) {
    arraycompact_kernel_file[i-1] = kernels[i].name + ".cl";
    kernel_names.push_back(kernels[i].name);
  }

  cl_int err = CL_SUCCESS;
 

  //read the kernel files
  readFile(arraycompact_kernel_file,
	   kernel_source_str, num_kerns-1);
  
  initialize_ocl(cv);
  
  compile_ocl_program(*kernel_map, cv, 
		      kernel_source_str,
                      num_kerns-1,
		      kernel_names);

  return cv;
}

void moveDataToGpu(cl_vars_t cv,
        std::map<std::string, data_array> cpu_data,
        std::map<std::string, cl_mem_data>* gpu_data,
        kernel* kernels, int num_kerns)
{
    std::map<std::string, data_array>::iterator it;
    for (it = cpu_data.begin(); it!=cpu_data.end(); ++it)
    {
       int len = (*it).second.len;
       double* arr = (*it).second.array;
       cl_mem g_arr;
  
       cl_int err = CL_SUCCESS;
       g_arr = clCreateBuffer(cv.context,CL_MEM_READ_WRITE,
		       sizeof(double)*len, NULL,&err);
       CHK_ERR(err);

       err = clEnqueueWriteBuffer(cv.commands, g_arr, true, 0, sizeof(double)*len,
			     arr, 0, NULL, NULL);
       CHK_ERR(err);
      
       std::pair<std::string, cl_mem_data> map_elem;
       map_elem.first = (*it).first;
       cl_mem_data gpu_array = {g_arr, len};
       map_elem.second = gpu_array;
       (*gpu_data).insert(map_elem);
    }
}

void runKernel(cl_vars_t cv,
            cl_kernel cl_kern,
            kernel kern,
            std::map<std::string, cl_mem_data> gpu_data,
            double* vars)
{
    int num_arrays = kern.num_arrays;
    string* arrays = kern.arrays;
    int num_vars = kern.num_vars;

    cl_int err = CL_SUCCESS;
    //set up the out array
    err = clSetKernelArg(cl_kern, 0, sizeof(cl_mem), &(gpu_data["out"].array));
    CHK_ERR(err);
    err = clSetKernelArg(cl_kern, 0 + num_arrays, sizeof(int), &(gpu_data["out"].len));
    CHK_ERR(err);
 
    for (int i = 1; i < num_arrays; i++)
    {
        std::string array_name = arrays[i];
        err = clSetKernelArg(cl_kern, i, sizeof(cl_mem), &(gpu_data[array_name].array));
        CHK_ERR(err);
        err = clSetKernelArg(cl_kern, i + num_arrays, sizeof(int), &(gpu_data[array_name].len));
        CHK_ERR(err);
    }

    for (int i = 0; i < num_vars; i++)
    {
        err = clSetKernelArg(cl_kern, i + 2*num_arrays, sizeof(double), &(vars[i]));
        CHK_ERR(err); 
    }
    size_t global_work_size[1] = {gpu_data[arrays[0]].len};
    size_t local_work_size[1] = {256};
 
    adjustWorkSize(global_work_size[0], local_work_size[0]);//pad work groups
    global_work_size[0] = std::max(local_work_size[0], global_work_size[0]);

    err = clEnqueueNDRangeKernel(cv.commands,
                                 cl_kern,
                                 1,
                                 NULL,
                                 global_work_size,
                                 local_work_size,
                                 0,
                                 NULL,
                                 NULL
                                );
    CHK_ERR(err);

}

void releaseClMem(std::map<std::string, cl_mem> gpu_data) {
    std::map<std::string, cl_mem>::iterator it;
    for (it = gpu_data.begin(); it!=gpu_data.end(); ++it)
    {
        clReleaseMemObject((*it).second);
    }
}
 
 
