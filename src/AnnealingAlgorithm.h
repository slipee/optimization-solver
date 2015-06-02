//
//  AnnealingAlgorithm.h
//  OprimizationSolver
//
//  Created by Denis Kropotov on 12/02/15.
//  Copyright (c) 2015 CMC MSU. All rights reserved.
//

#ifndef __OprimizationSolver__AnnealingAlgorithm__
#define __OprimizationSolver__AnnealingAlgorithm__

#include <vector>
#include <cmath>
#include "Algorithm.h"

using std::vector;

//const double TEMP_START_DEFAULT = .2;
//const double TEM_END_DEFAULT = 0.0002; //without borders

//const double TEMP_START_DEFAULT = .001;
//const double TEM_END_DEFAULT = 0.0000015; //rosenbrock

//const double TEMP_START_DEFAULT = .01;
//const double TEM_END_DEFAULT = 0.000015; //rast
const double TEM_END_DEFAULT = 0.0000;

const double TEMP_START_DEFAULT = .001;
//const double TEM_END_DEFAULT = 0.00001; //measure


class AnnealingAlgorithm: public Algorithm
{
private:
	double _temp_start;
	double _temp_curr;
	double _temp_end;
	
	int _iter_to_exchange;
	
	float *_result_vector;
	float *_candidate;
	
public:
	AnnealingAlgorithm(int size, float (*target_func)(const float*, int), float left, float right,
					   vector<double> params = vector<double>());
	
	~AnnealingAlgorithm();
	
	float solve();
	const float* get_result_vector() const;
	
private:
	void cauchy_dist();
	void cauchy_temp();
	
	void very_fast_dist();
	void very_fast_temp();
	
	float exchange();
};

#endif /* defined(__OprimizationSolver__AnnealingAlgorithm__) */
