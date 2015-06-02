//
//  differentialevolution.cpp
//  OprimizationSolver
//
//  Created by Denis Kropotov on 05/03/15.
//  Copyright (c) 2015 CMC MSU. All rights reserved.
//

#include "differentialevolution.h"

#include <iostream>
#include <cstdlib>
#include <cmath>
#include <set>
#include <algorithm>
#include <mpi.h>


DifferentialEvolution::DifferentialEvolution(int size, float (*target_func)(const float*, int), float left, float right, vector<double> params):
Algorithm(size, target_func, left, right),
_popSize(size * SizeCoef),
//_pop_size(512),
_probability(DefaultProbability),
_mutationPower(DefaultMutPower),
_iter(0),
_resVecInd(0),
_bestVal(false),
_resVector(new float[size])
{
	int mpi_size;
	MPI_Comm_size(MPI_COMM_WORLD,&mpi_size);
	_popSize /= mpi_size;
	
	if (params.size() != 0) {
		_popSize = (int)params[0];
		_probability = (int)params[1];
		_mutationPower = (int)params[2];
	}
	
	_oldGeneration = new float[_popSize * getSize()];
	_newGeneration = new float[_popSize * getSize()];
	
	init();
	
	_min = callTagetFunction(_oldGeneration);
}


float DifferentialEvolution::solve()
{
	float tmpMin;
	int mpiRank;
	MPI_Comm_rank(MPI_COMM_WORLD,&mpiRank);
	
	while (_iter++ < getMaxIter()) {
		_bestVal = false;
		float prevMin = _min;
		
		crossover();
		selection();
		
		MPI_Allreduce(&_min, &tmpMin, 1, MPI_FLOAT, MPI_MIN, MPI_COMM_WORLD);
		if (tmpMin == _min) {
			_bestVal = true;
		}
		_min = tmpMin;
		
		if (prevMin - _min >= getEps()) {
			_iter = 0;
		}
		
	}
	
	return _min;
}

void DifferentialEvolution::crossover()
{
	int mpiSize, mpiRank;
	MPI_Comm_size(MPI_COMM_WORLD, &mpiSize);
	MPI_Comm_rank(MPI_COMM_WORLD, &mpiRank);
	
	float *wholePop = _oldGeneration;
	if (mpiSize > 1) {
		wholePop = new float[_popSize * mpiSize * getSize()];
		
		MPI_Barrier(MPI_COMM_WORLD);
		MPI_Allgather(_oldGeneration, _popSize*getSize(), MPI_FLOAT, wholePop, _popSize*getSize(), MPI_FLOAT, MPI_COMM_WORLD);
	}
	
	for (int i = 0; i < _popSize; i++) {
		float newVal = 0;
		
		std::set<int> indexes;
		int partner = uniqInSet(indexes);
		indexes.insert(partner);
		
		int noiseOne = uniqInSet(indexes);
		indexes.insert(noiseOne);
		
		int noiseTwo = uniqInSet(indexes);
		indexes.insert(noiseTwo);
		
		for (int j = 0; j < getSize(); j++) {
			do {
				float prob = rand()/(float)RAND_MAX;
				
				if (prob < _probability) {
					newVal = mutation(wholePop, j, partner, noiseOne, noiseTwo);
				} else {
					newVal = _oldGeneration[i*getSize() + j];
				}
				
			} while (newVal < getLeftBoard() || newVal > getRightBoard());
			_newGeneration[i*getSize() + j] = newVal;
		}
	}
	
	if (mpiSize > 1) {
		delete [] wholePop;
	}
}

float DifferentialEvolution::mutation(float *wholePop, int attr, int partner, int noiseOne, int noiseTwo)
{
	float noise = _mutationPower*(wholePop[noiseOne*getSize() + attr] - wholePop[noiseTwo*getSize() + attr]);
	return wholePop[partner*getSize() + attr] + noise;
}

void DifferentialEvolution::selection()
{
	for (int i = 0; i < _popSize; i++) {
		float oldValue = callTagetFunction(_oldGeneration + i*getSize());
		float newValue = callTagetFunction(_newGeneration + i*getSize());
		
		if (newValue < oldValue) {
			std::copy(_newGeneration + i*getSize(), _newGeneration + (i+1)*getSize(), _oldGeneration + i*getSize());
			
			if (newValue < _min) {
				_min = newValue;
				_resVecInd = i;
			}
		}
	}
}

int DifferentialEvolution::uniqInSet(std::set<int> set)
{
	int mpiSize;
	MPI_Comm_size(MPI_COMM_WORLD, &mpiSize);
	
	int uniq = rand() % (_popSize * mpiSize);
	while (set.find(uniq) != set.end()) {
		uniq = rand() % (_popSize * mpiSize);
	}
	
	return uniq;
}

void DifferentialEvolution::init()
{
	for (int i = 0; i < _popSize; i++) {
		for (int j = 0; j < getSize(); j++) {
			_oldGeneration[i*getSize() + j] = (rand() / (float)RAND_MAX)*(fabs(getRightBoard()) + fabs(getLeftBoard()) + getLeftBoard());
		}
	}
}

const float* DifferentialEvolution::getResultVector() const
{
	if (_bestVal) {
		return (_oldGeneration + _resVecInd*getSize());
	} else {
		return NULL;
	}
}

DifferentialEvolution::~DifferentialEvolution()
{
	
}
