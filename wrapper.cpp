#include "ga.h"
#include "kernel_struct.h"
#include "clhelp.h"
#include "opencl_setup.h"
#include "kernel_parser.cpp"
#include <sys/time.h>
using namespace std;

int main(int argc, char* argv[]){

    struct timeval tv1, tv2;

    //parsing: will be filled in with Lucas's code
    kernel_set* parsed_kernel_set = kernel_parser(argv[1]);
    int numArgs = 2;
    for (int i = 0; i < (*parsed_kernel_set).num_kernels; i++)
    {
        numArgs += (*parsed_kernel_set).kernels[i].num_vars;
    }
    int num_kerns = (*parsed_kernel_set).num_kernels;
    kernel* kernels = (*parsed_kernel_set).kernels;
    map<string, data_array> cpu_data = *((*parsed_kernel_set).data_arrays);

    //structure data for the genetic_algo
    kern_info_t info;
    info.cpu_data = cpu_data;
    setupEnvironment(&(info.env), kernels, cpu_data, num_kerns);
    //run genetic algorithm
    gettimeofday(&tv1, 0);
    genetic_algo(numArgs, info.cpu_data["out"].len, info);
    gettimeofday(&tv2, 0);

    double time = tv2.tv_sec - tv1.tv_sec + 1e-6 * (tv2.tv_usec - tv1.tv_usec);
    printf("Total runtime: %f seconds.\n", time);
  }
 
