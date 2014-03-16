#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <cassert>
#include <cmath>
#include <unistd.h>

#include "clhelp.h"
#include "kernel_struct.h"
#include "opencl_setup.h"

using namespace std;
    
int main(int argc, char *argv[])
{
    int num_kerns = 1;
    kernel* kernels = new kernel[num_kerns];
    kernels[0].name = std::string("vvadd");
    kernels[0].num_arrays = 3;
    kernels[0].num_vars = 0;
    string array_names[3] = {"out", "v1", "v2"};
    kernels[0].arrays = array_names;
    /*The data to be put on the GPU*/
    int n = 1<<20;
    double* v1 = new double[n];
    double* v2 = new double[n];
    double* out = new double[n];
    for (int i = 0; i<n; i++)
    {
        v1[i] = 1;
        v2[i] = 1;
    }

    data_array d_v1 = {n, v1};
    data_array d_v2 = {n, v2};
    data_array d_out = {n, out};
    map<string, data_array> cpu_data;
    map<string, cl_mem_data> gpu_data;
    cpu_data.insert(pair<string, data_array>(string("v1"), d_v1));
    cpu_data.insert(pair<string, data_array>(string("v2"), d_v2));
    cpu_data.insert(pair<string, data_array>(string("out"), d_out));
    
    cl_runtime_env env;
    setupEnvironment(&env, kernels, cpu_data, num_kerns);

    double t0 = timestamp();

    double vars[0] = {};
/*    runKernel(env,
               "vvadd",
               vars,
               0,
               n);
*/

    runKernel(env,
              "vvadd",
              vars,
              out,
              0,
              n);

//    cl_int err = CL_SUCCESS;
 
//    err = clEnqueueReadBuffer(env.cv.commands, env.gpu_data[string("out")].array, true, 0, sizeof(double)*n,
//			    out, 0, NULL, NULL);
//    CHK_ERR(err);
  
    /* CHECK CORRECTNESS */
    for(int i =0; i < n; i++)
    {
      if(out[i] != 2)
	{
	  printf("scan mismatch @ %d: cpu=%d, gpu=%f\n", 
		 i, 2, out[i]);
	  break;
	}
    }
 

}
