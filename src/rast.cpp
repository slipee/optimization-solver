#include <cmath>

float rast(const float *params, int size)
{
	double sum = 0.;
	
	for (int i = 0; i < size; i++) {
		sum += params[i] * params[i] - 10*cos(2*M_PI * params[i]);
	}
	
	return 10*size + sum;
}
