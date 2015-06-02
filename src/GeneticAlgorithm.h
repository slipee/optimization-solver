//
//  GeneticAlgorithm.h
//  OprimizationSolver
//
//  Created by Denis Kropotov on 05/02/15.
//  Copyright (c) 2015 CMC MSU. All rights reserved.
//

#ifndef __OprimizationSolver__GeneticAlgorithm__
#define __OprimizationSolver__GeneticAlgorithm__

#include "Algorithm.h"

const int NUM_POP_DEFAULT = 5;
const int NUM_DESC_DEFAULT = 25;

const int PMin = -9;
const int PMax = 1;
const int Base = 10;

class GeneticAlgorithm: public Algorithm {
private:
	int _num_pop;
	int _num_desc;
	int _max_mut;
	
	int _iter;
	float _min;
	
	int _res_vec_ind;
	bool _best_val;
	
	vector<vector<float> > _population;
	vector<vector<float> > _descendants;
	vector<float> _intermediate_min;
	
	vector<float> _result;
	
public:
	GeneticAlgorithm(int size, float (*target_func)(const float*, int), float left, float right,
					 vector<double> params = vector<double>());
	
	~GeneticAlgorithm();

	const float* get_result_vector() const;
	float solve();
	
private:
	void init();
	void mutation(int index);
	void selection();
};

#endif /* defined(__OprimizationSolver__GeneticAlgorithm__) */
