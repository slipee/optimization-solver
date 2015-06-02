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
const float DefaultProbability = 0.9;
const float DefaultMutPower = 0.5; // rosenbrock

//const float DefaultProbability = 0.5;
//const float DefaultMutPower = 0.2; // rast

//const float DefaultProbability = 0.5;
//const float DefaultMutPower = 0.3; // measure


class DifferentialEvolution: public Algorithm
{
private:
	int _popSize;
	float _probability;
	float _mutationPower;
	
	int _iter;
	float _min;
	
	int _resVecInd;
	bool _bestVal;
	
	float *_oldGeneration;
	float *_newGeneration;
	float *_resVector;
	
public:
	DifferentialEvolution(int size, float (*targetFunc)(const float*, int), float left, float right, vector<double> params = vector<double>());
	
	~DifferentialEvolution();
	
	float solve();
	const float* getResultVector() const;
	
private:
	void crossover();
	void selection();
	float mutation(float *wholePop, int attr, int partner, int noiseOne, int noiseTwo);
	
	void init();
	int uniqInSet(std::set<int>);
};

#endif /* defined(__OprimizationSolver__differentialevolution__) */
