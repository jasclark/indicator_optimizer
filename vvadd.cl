
#pragma OPENCL EXTENSION cl_khr_fp64 : enable

__kernel void vvadd(__global double* out,
                    __global double* v1,
                    __global double* v2,
                    __global double* v3,
                    int outlen,
                    int v1len,
                    int v2len,
                    int v3len,
                    double dummy_var1,
                    double dummy_var2,
                    double dummy_var3,
                    double dummy_var4
                    )
{
  size_t idx = get_global_id(0);
  if (idx < outlen)
    out[idx] = idx;
}
