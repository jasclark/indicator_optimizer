#pragma OPENCL EXTENSION cl_khr_fp64 : enable

__kernel void testBollinger(	__global double * decision_array,
 							__global double * price_array,
                                                        __global double * dummy_array,
 							int d_size,
 							int p_size,
                                                        int dum_size,
 							double weight,
 							double p,
 							double upper_std,
 							double lower_std)
{
/*	int period = floor(p);
	size_t gl_id = get_global_id(0);
	size_t gl_sz = get_global_size(0);
	
        // Semi sequential stage
	while (gl_id < p_size)
	{
		// Determine index on price array
		int index = gl_id ;

		// Offset
		int offset = 1;
		
		// MAVG
		double avg = 0;

		// Number of prics taken into consideration
		int n = (index < period) ? index : period - 1;


		// Calculate the moving avg
		for (int idx = offset*(index - n); idx <= index; idx += offset)
		{
			avg += price_array[idx];
		}
		avg = avg / (n + 1);


		// Calculate std deviation
		double std = 0;
                double accumulator = 0;
		for (int i = index - n; i <= index; i++)
		{
			accumulator += ((avg - price_array[i]) * (avg - price_array[i]));
		}

		std = sqrt(accumulator) / (n + 1);


		
		
		// Update decision array

		if (price_array[gl_id] > avg + upper_std * std){
			decision_array[gl_id] += -1 * weight;
		} else if (price_array[gl_id] < avg - lower_std * std) {

			decision_array[gl_id] += 1 * weight;
		} else {
			decision_array[gl_id] += 0;
		}

		// Update global index
		gl_id += gl_sz;

	}
	
	// Notes: 
*/
}
