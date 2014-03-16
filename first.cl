#pragma OPENCL EXTENSION cl_khr_fp64 : enable

__kernel void first(__global double * decisions, int length)
{
	size_t idx = get_global_id(0);
	size_t g_size = get_global_size(0);
	decisions[idx] = 0.0;
}

