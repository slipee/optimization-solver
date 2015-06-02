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

const int NumPopDefault = 5;
const int NumDescDefault = 25;

const int PMin = -9;
const int PMax = 1;
const int Base = 10;

class GeneticAlgorithm: public Algorithm {
private:
	int _numPop;
	int _numDesc;
	int _maxMut;
	
	int _iter;
	float _min;
	
	int _resVecInd;
	bool _bestVal;
	
	vector<vector<float> > _population;
	vector<vector<float> > _descendants;
	vector<float> _intermediateMin;
	
	vector<float> _result;
	
public:
	GeneticAlgorithm(int size, float (*targetFunc)(const float*, int), float left, float right,
					 vector<double> params = vector<double>());
	
	~GeneticAlgorithm();

	const float* getResultVector() const;
	float solve();
	
private:
	void init();
	void mutation(int index);
	void selection();
};

#endif /* defined(__OprimizationSolver__GeneticAlgorithm__) */
