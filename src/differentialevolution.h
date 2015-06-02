//
//  differentialevolution.h
//  OprimizationSolver
//
//  Created by Denis Kropotov on 05/03/15.
//  Copyright (c) 2015 CMC MSU. All rights reserved.
//

#ifndef __OprimizationSolver__differentialevolution__
#define __OprimizationSolver__differentialevolution__

#include <stdio.h>
#include <set>
#include <vector>
#include "AnnealingAlgorithm.h"

const int SizeCoef = 5;
//const float DefaultProbability = 0.9;
//const float DefaultMutPower = 0.5; // rosenbrock

//const float DefaultProbability = 0.5;
//const float DefaultMutPower = 0.2; // rast

const float DefaultProbability = 0.5;
const float DefaultMutPower = 0.3; // measure


class DifferentialEvolution: public Algorithm
{
private:
	int _pop_size;
	float _probability;
	float _mutation_power;
	
	int _iter;
	float _min;
	
	int _res_vec_ind;
	bool _best_val;
	
	float *_old_generation;
	float *_new_generation;
	float *_res_vector;
	
public:
	DifferentialEvolution(int size, float (*target_func)(const float*, int), float left, float right, vector<double> params = vector<double>());
	
	~DifferentialEvolution();
	
	float solve();
	const float* get_result_vector() const;
	
private:
	void crossover();
	void selection();
	float mutation(float *whole_pop, int attr, int partner, int noise_one, int noise_two);
	
	void init();
	int uniq_in_set(std::set<int>);
};

#endif /* defined(__OprimizationSolver__differentialevolution__) */
