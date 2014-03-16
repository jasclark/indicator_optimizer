#include "bollinger.h"
#include <stdio.h>      /* printf */
#include <cstdlib> 
#include <ctime> 
#include <iostream>

int main( int argc, const char* argv[])
{
	int size;
 	cout<< "how large should the prices array be?" << endl;
 	cin >> size;


 	double input_array[size];
	double output_array[size];

	srand((unsigned)time(0));
	for(int i = 0; i < size; i++)
	{
		double f = (double)rand() / RAND_MAX;
		double val = 10 + f * (40 - 10);
		input_array[i] = val;
		output_array[i] = 0;
	}

	data_array input;
	data_array output;
	input.array = input_array;
	input.len = size;
	output.array = output_array;
	output.len = size;
	
	double period = 10;
	double upperStd = 2;
	double lowerStd = 2;

	bollinger_bands(&input, &output, period, upperStd, lowerStd);

	for(int i = 0; i < size; i++) 
	{
		printf("%d \n", (int)output.array[i]);
	}
	return 1;
}