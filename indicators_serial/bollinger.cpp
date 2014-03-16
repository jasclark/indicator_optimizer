/* very naive serial implementation of bollinger bands */

#include "bollinger.h"

double standard_deviation(data_array * input,
	double average,
	double period, 
	int index);

double moving_average(data_array * input, 
	double period,
	int index);

void bollinger_bands(data_array * input, 
		     double* output,
		     double period, //number of time steps considered for MA, Upper, Lower Calc
		     double upperStd, //number of standard deviations for upper bound
		     double lowerStd,
		     double weight) //number of standard deviations for lower bou nd
{
  double temp;
	for (int idx = 0; idx < input->len; idx += 1) 
	{
		double mavg = moving_average(input, period, idx);
		double std = standard_deviation(input, mavg, period, idx);
		double upperBB = mavg + upperStd * std;
		double lowerBB = mavg - upperStd * std;
		int decision;
		//TODO: Check input array format
		double close_price = input->array[idx];
                temp = output[idx];
		if (close_price > upperBB){
			temp += -1*weight;
		} else if (close_price < lowerBB) {
			temp += weight;
		}
                output[idx] = temp;
	}
}

double standard_deviation(data_array * input,
	double average,
	double period,
	int index)
{
	if (index < input->len) 
	{
		int n = min(index, static_cast<int>(period + 0.5) - 1);
		double accumulator = 0;
		for (int i = index - n; i <= index; i++)
		{
			double close_price = input->array[i];
			accumulator += ((average - close_price) * (average - close_price));
		}
		return sqrt(accumulator) / (n + 1);
	} else {
		return -1;
	}
}

double moving_average(data_array * input, 
	double period, 
	int index)
{
	if (index < input->len)
	{
		int n = min(index, static_cast<int>(period + 0.5) - 1);
		double accumulator = 0;
		for (int i = index - n; i <= index; i++)
		{
			double close_price = input->array[i];
			accumulator += close_price;
		}
		return accumulator / (n + 1);
	} else {
		return -1;
	}
}
