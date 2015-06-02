//
//  GeneticAlgorithm.cpp
//  OprimizationSolver
//
//  Created by Denis Kropotov on 05/02/15.
//  Copyright (c) 2015 CMC MSU. All rights reserved.
//

#include "GeneticAlgorithm.h"
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <vector>
#include <set>

#include <mpi.h>

using std::vector;

GeneticAlgorithm::GeneticAlgorithm(int size, float (*target_func)(const float*, int), float left, float right, vector<double> params)
: Algorithm(size, target_func, left, right),
_numPop(NumPopDefault),
_numDesc(NumDescDefault),
_maxMut(5),
_iter(0),
_resVecInd(0),
_bestVal(false)
{
	if (params.size() != 0) {
		_numPop = (int)params[0];
		_numDesc = (int)params[1];
		_maxMut = (int)params[2];
	}
	
	int mpiSize;
	MPI_Comm_size(MPI_COMM_WORLD,&mpiSize);
	_numPop /= mpiSize;
	
	_population = vector<vector<float> >(_numPop, vector<float>(getSize()));
	_intermediateMin = vector<float>(_numPop);
	_descendants = vector<vector<float> >(_numDesc+1, vector<float>(getSize()));
	
	init();
	
	_min = callTagetFunction(&_population[0][0]);
}

float GeneticAlgorithm::solve()
{	
	float tmpMin;
	int mpiRank;
	MPI_Comm_rank(MPI_COMM_WORLD,&mpiRank);

	while (_iter++ < getMaxIter()) {
		selection();
		
		float prevMin = _min;
		float interMin = _min;
		for (int i = 0; i < _intermediateMin.size(); i++) {
			if (interMin >= _intermediateMin[i]) {
				_resVecInd = i;
				interMin = _intermediateMin[i];
			}
		}
		_min = interMin;
		
		MPI_Barrier(MPI_COMM_WORLD);
		MPI_Allreduce(&_min, &tmpMin, 1, MPI_FLOAT, MPI_MIN, MPI_COMM_WORLD);
		if (prevMin != tmpMin) {
			_bestVal = false;
			if (_min == tmpMin) {
				_bestVal = true;
			}
		}
		_min = tmpMin;
		
		if (prevMin - _min >= getEps()) {
			_iter = 0;
		}
	}
	
	return _min;
}

void GeneticAlgorithm::selection()
{
	for (int i_pop = 0; i_pop < _numPop; i_pop++) {
		mutation(i_pop);

		float min = callTagetFunction(&_descendants[0][0]);
		int best_ind = 0;
		float cur_min;
		for (int i_desc = 1; i_desc < _numDesc+1; i_desc++) {
			cur_min = callTagetFunction(&_descendants[i_desc][0]);
			if (cur_min < min) {
				min = cur_min;
				best_ind = i_desc;
			}
		}

		_intermediateMin[i_pop] = min;
		for (int i = 0; i < getSize(); i++) {
			_population[i_pop][i] = _descendants[best_ind][i];
		}
	}
}

void GeneticAlgorithm::mutation(int index)
{
	for (int i = 0; i < _numDesc; i++) {
		for (int j = 0; j < getSize() ; j++) {
			_descendants[i][j] = _population[index][j];
		}
	}
	for (int i = 0; i < getSize(); i++) {
		_descendants[_numDesc][i] = _population[index][i];
	}

	for (int i_desc = 0; i_desc < _numDesc; i_desc++) {
		int nMut = rand() % getSize();

		std::set<int> indexes;
		for (int i = 0; i < nMut; i++) {
			int uniq = rand() % (getSize());
			while (indexes.find(uniq) != indexes.end()) {
				uniq = rand() % (getSize());
			}
			indexes.insert(uniq);
		}
		
		float newVal;
		for(std::set<int>::const_iterator it = indexes.begin(); it != indexes.end(); it++) {
			do {
				float m = 2 * (rand() / (float)RAND_MAX) - 1;
				float p = (rand() % (PMax - PMin)) + PMin;
				newVal = _descendants[i_desc][*it] + m * pow(Base, p);
			} while (newVal < getLeftBoard() || newVal > getRightBoard());
			_descendants[i_desc][*it] = newVal;
		}
	}
	
}

void GeneticAlgorithm::init()
{
	for (int i = 0; i < _numPop; i++) {
		for (int j = 0; j < getSize(); j++) {
			_population[i][j] = (rand() / (float)RAND_MAX)*(fabs(getRightBoard()) + fabs(getLeftBoard()) + getLeftBoard());
		}
	}
}

const float* GeneticAlgorithm::getResultVector() const
{
	if (_bestVal) {
		return &_population[_resVecInd][0];
	} else {
		return NULL;
	}
}

GeneticAlgorithm::~GeneticAlgorithm()
{
	//delete[] _intermediate_min;
}





























