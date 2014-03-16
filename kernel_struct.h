#ifndef KERNEL_STRUCT_H
#define KERNEL_STRUCT_H

#include <string>
#include <map>
using namespace std;

struct range {
    double lo;
    double hi;
};

struct data_array {
    int len;
    double* array;
};

struct kernel {
    std::string name;
    
    int num_arrays;
    string* arrays;

    int num_vars;
    range* ranges;
};

struct kernel_set {
    int num_kernels;
    kernel* kernels;
    map<string, data_array>* data_arrays;
};

#endif
