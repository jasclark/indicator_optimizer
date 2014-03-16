#include "../kernel_struct.h"
#include <math.h>       /* floor */

void bollinger_bands(data_array * input, 
	data_array * output, 
	double period, //number of time steps considered for MA, Upper, Lower Calc
	double upperStd, //number of standard deviations for upper bound
	double lowerStd //number of standard deviations for lower bound
	);
