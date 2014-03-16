#include "kernel_struct.h"
#include "libcsv/csv.h"
#include "libcsv/libcsv.c"
#include "count_lines.cpp"
#include <string>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "read_csv_data.cpp"

int
main (int argc, char *argv[])
{
  struct data_array data = read_csv_data(argv[1]);

  for(int i = 0; i < data.len; i++) {
    printf("Timestep %d: %f\n", i+1, data.array[i]);
  }

  exit(EXIT_SUCCESS);
}
