#include <map>
#include "kernel_struct.h"
#include "libcsv/csv.h"
#include "libcsv/libcsv.c"
#include "count_lines.cpp"
#include "csv_help.c"
#include <string>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "read_csv_data.cpp"

using namespace std;

struct array_def {
  string filename;
  string type;
};

struct temp_kernel {
  string name;
  int num_arrays;
  int num_vars;
  int num_arrays_filled;
  int num_vars_filled;
  array_def* array_names;
  range* ranges;
  int kernel_num;
  kernel* kernels;
  map<string, data_array>* data_arrays;
};

void create_arrays(int num_arrays, array_def* array_names, map <string, data_array>* data_arrays) {
  for (int i = 0; i < num_arrays; i++) {
    if ((*data_arrays).count(array_names[i].filename + array_names[i].type) == 0) {
      (*data_arrays)[array_names[i].type] = read_csv_data((char*) array_names[i].filename.c_str(), (char*) array_names[i].type.c_str());
    }
  }
}

string* concat_array_names(int num_arrays, array_def* array_names) {
  string* out = new string[num_arrays+1];
  for (int i = 0; i < num_arrays; i++) {
    out[i] = array_names[i].type;//array_names[i].filename + array_names[i].type;
  }
  out[num_arrays] = string("out");
  delete[] array_names;
  return out;
}

void cb1 (void *s, size_t len, void *data) {
  struct temp_kernel* temp = (struct temp_kernel*) data;
  if (temp->name == "") {
    temp->name = string((const char*) s, len);
  } else if (temp->num_arrays == -1) {
    temp->num_arrays = atoi(string((const char*) s, len).c_str());
    temp->num_arrays_filled = 0;
    temp->array_names = new array_def[temp->num_arrays];
  } else if (temp->num_vars == -1) {
    temp->num_vars = atoi((const char*) s);
    temp->num_vars_filled = 0;
    temp->ranges = new range[temp->num_vars];
  } else if (temp->num_arrays_filled/2 < temp->num_arrays) {
    if (temp->num_arrays_filled%2 == 0) {
      temp->array_names[temp->num_arrays_filled/2].filename = string((const char*) s, len);
    } else {
      temp->array_names[temp->num_arrays_filled/2].type = string((const char*) s, len);
    }
    temp->num_arrays_filled++;
  } else if (temp->num_vars_filled/2 < temp->num_vars) {
    if (temp->num_vars_filled%2 == 0) {
      temp->ranges[temp->num_vars_filled/2].lo = atof(string((const char*) s, len).c_str());
    } else {
      temp->ranges[temp->num_vars_filled/2].hi = atof(string((const char*) s, len).c_str());
    }
    temp->num_vars_filled++;
  }
}

void cb2 (int c, void *data) {
  if (c != -1) {
    struct temp_kernel* temp = (struct temp_kernel*) data;
    temp->kernels[temp->kernel_num].name = temp->name;
    temp->kernels[temp->kernel_num].num_arrays = temp->num_arrays + 1;
    create_arrays(temp->num_arrays, temp->array_names, temp->data_arrays);
    temp->kernels[temp->kernel_num].arrays = concat_array_names(temp->num_arrays, temp->array_names);
    temp->kernels[temp->kernel_num].num_vars = temp->num_vars;
    temp->kernels[temp->kernel_num].ranges = temp->ranges;
    temp->name = "";
    temp->num_arrays = -1;
    temp->num_vars = -1;
    temp->num_arrays_filled = -1;
    temp->num_vars_filled = -1;
    temp->array_names = NULL;
    temp->kernel_num++;
    temp->ranges = NULL;
  }
}

kernel_set*
kernel_parser (char* filename)
{
  FILE *fp;
  struct csv_parser p;
  char buf[1024];
  size_t bytes_read;
  unsigned char options = 0;
  map <string, data_array>* data_arrays = new map<string, data_array>;
  struct temp_kernel temp = {"", -1, -1, -1, -1, NULL, NULL, 0, NULL, data_arrays};

  if (csv_init(&p, options) != 0) {
    fprintf(stderr, "Failed to initialize csv parser\n");
    exit(EXIT_FAILURE);
  }

  csv_set_space_func(&p, is_space);
  csv_set_term_func(&p, is_term);

  fp = fopen(filename, "rb");
  if (!fp) {
    fprintf(stderr, "Failed to open %s: %s\n", filename, strerror(errno));
    exit(EXIT_FAILURE);
  }

  temp.kernels = new kernel[count_lines(fp)];

  rewind(fp);

  while ((bytes_read=fread(buf, 1, 1024, fp)) > 0) {
    if (csv_parse(&p, buf, bytes_read, cb1, cb2, &temp) != bytes_read) {
      fprintf(stderr, "Error while parsing file: %s\n", csv_strerror(csv_error(&p)));
    }
  }

  csv_fini(&p, cb1, cb2, &temp);

  if (ferror(fp)) {
    fprintf(stderr, "Error while reading file %s\n", filename);
    fclose(fp);
    exit(EXIT_FAILURE);
  }

  fclose(fp);

  csv_free(&p);

  kernel_set* output = new kernel_set;
  output->num_kernels = temp.kernel_num;
  output->data_arrays = temp.data_arrays;
  output->kernels = temp.kernels;

  int maxlen = 0;
  data_array out;

  map<string, data_array>::iterator iterator;
  for(iterator = (*data_arrays).begin(); iterator != (*data_arrays).end(); iterator++) {
    if (iterator->second.len > maxlen) {
      maxlen = iterator->second.len;
    }
  }

  out.len = maxlen;
  out.array = new double[maxlen];
  for (int i = 0; i < maxlen; i++) {
    out.array[i] = 0.0;
  }

  (*output->data_arrays)[string("out")] = out;

  return output;
}
