#include <omp.h>
#include "kernel_parser.cpp"
#include "ga_serial.h"

int
main (int argc, char *argv[])
{
  kernel_set* set = kernel_parser(argv[1]);

  kern_info_t info;

  double** best;

  info.cpu_data = *set->data_arrays;

  info.env.kernels = set->kernels;

  info.env.num_kerns = set->num_kernels;

  printf("We managed to copy the kernel set\n");

  printf("data_length: %d\n", info.cpu_data[string("hPrice")].len);

  best = genetic_algo_serial(0, 0, 0, info.cpu_data[string("hPrice")].len, info);

  printf("the genetic algorithm returned\n");

  printf("Fitness %f, %f,\n", best[0][0], best[0][1]);
  printf("ADX %f, %f, %f\n",  best[1][0], best[1][1], best[1][2]);
  printf("BB %f, %f, %f, %f\n", best[2][0], best[2][1], best[2][2], best[2][3]);
}
