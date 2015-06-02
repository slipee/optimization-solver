#include <vector>

using std::vector;

float rosenbrock(const float *params, int size)
{
	double sum = 0.;
	
	for (int i = 0; i < size-1; i++) {
		double  sqr = params[i+1] - params[i] * params[i];
		sum += 100*sqr*sqr + (1 - params[i])*(1 - params[i]);
	}
	
	return sum;
}
