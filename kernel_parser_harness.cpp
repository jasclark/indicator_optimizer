#include "kernel_parser.cpp"

int
main (int argc, char *argv[])
{
  kernel_set* set = kernel_parser(argv[1]);
  struct data_array data;
  int total_arrays = 0;

  printf("Number of kernels: %d\n", set->num_kernels);

  for(int i = 0; i < set->num_kernels; i++) {

    kernel temp = set->kernels[i];

    printf("%s: %d arrays, %d args\n", temp.name.c_str(), temp.num_arrays, temp.num_vars);
    for(int j = 0; j < temp.num_vars; j++) {
      printf("Varible %d range: %f, %f\n", j+1, temp.ranges[j].lo, temp.ranges[j].hi);
    }

    for (int j = 0; j < temp.num_arrays; j++) {
      total_arrays++;
      printf("Arraydef %d: %s\n", j+1, temp.arrays[j].c_str());
    }

    for (int j = 0; j < temp.num_arrays; j++) {
      map<string, data_array> data_arrays = *set->data_arrays;
      struct data_array data = data_arrays[temp.arrays[j]];
      printf("Data array %d of length: %d\n", j+1, data.len);
      if (data.len < 30) {
        for (int k = 0; k < data.len; k++) {
          printf("Timestep %d: %f\n", k+1, data.array[k]);
        }
      }
    }
  }

  for(int i = 0; i < (*set->data_arrays)[string("out")].len; i++) {
    printf("Output timestep %d: %f\n", i+1, (*set->data_arrays)[string("out")].array[i]);
  }

  printf("Number of arrays in kernels: %d\nNumber of arrays in map: %li\n", total_arrays, (*set->data_arrays).size());

  exit(EXIT_SUCCESS);
}
