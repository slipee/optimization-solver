//
//  AnnealingAlgorithm.cpp
//  OprimizationSolver
//
//  Created by Denis Kropotov on 12/02/15.
//  Copyright (c) 2015 CMC MSU. All rights reserved.
//

#include "AnnealingAlgorithm.h"
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <string>

#include "mpi.h"

using std::string;

AnnealingAlgorithm::AnnealingAlgorithm(int size, float (*targetFunc)(const float*, int), float left, float right, vector<double> params)
: Algorithm(size, targetFunc, left, right),
_tempStart(TEMP_START_DEFAULT),
_tempCurr(TEMP_START_DEFAULT),
_tempEnd(TEM_END_DEFAULT),
_iterToExchange(5),
_resultVector(new float[getSize()]),
_candidate(new float[getSize()])
{
	if (params.size() != 0) {
		_tempStart = params[0];
		_tempCurr = _tempStart;
		_tempEnd = params[1];
	}
	
	int mpiSize;
	MPI_Comm_size(MPI_COMM_WORLD,&mpiSize);
}

float AnnealingAlgorithm::solve()
{
	for (int i = 0 ; i < getSize(); i++) {
		_resultVector[i] = (rand() / (float)RAND_MAX)*(fabs(getRightBoard()) + fabs(getLeftBoard()) + getLeftBoard());
	}
	
	float resValue = callTagetFunction(_resultVector);
	float currValue = resValue;
	
	int mpiRank, mpiSize;
	MPI_Comm_rank(MPI_COMM_WORLD,&mpiRank);
	MPI_Comm_size(MPI_COMM_WORLD,&mpiSize);
	
	int iter = 0;
	int iter2 = 0;
	while (_tempCurr >= _tempEnd) {
		float prevMin = resValue;
		cauchyDist();
		
		currValue = callTagetFunction(_candidate);
		
		if (currValue < resValue) {
			for (int i = 0; i < getSize(); i++) {
				_resultVector[i] = _candidate[i];
			}
			resValue = currValue;
			
			cauchyTemp();
		} else {
			float probability = expf(-(currValue - resValue)/_tempCurr);
			if (probability < 0) {
				exit(-99);
			}
			
			float a = rand() / (float)RAND_MAX;
			
			if (a < probability) {
				for (int i = 0; i < getSize(); i++) {
					_resultVector[i] = _candidate[i];
				}
				
				resValue = currValue;
				
				cauchyTemp();
			}
		}
		iter++;
		
		cauchyTemp();
	}
	
	MPI_Barrier(MPI_COMM_WORLD);
	float tmpMin;
	MPI_Allreduce(&resValue, &tmpMin, 1, MPI_FLOAT, MPI_MIN, MPI_COMM_WORLD);
	
	resValue = tmpMin;
	
	return resValue;
}

void AnnealingAlgorithm::cauchyDist()
{
	float newVal;
	for (int i = 0; i < getSize(); i++) {
		float a = rand() / (float)RAND_MAX;
		
		float z = _tempCurr * tan(M_PI*a - M_PI/2.);
		newVal = _resultVector[i] + z*(getRightBoard() - getLeftBoard());
		if (newVal < getLeftBoard() || newVal > getRightBoard()) {
			_candidate[i] = _resultVector[i];
		} else {
			_candidate[i] = newVal;
		}
	}
}

void AnnealingAlgorithm::cauchyTemp()
{
	_tempCurr = _tempStart / sqrt((double)getCoutFunc());
}

void AnnealingAlgorithm::veryFastDist()
{
	float newVal;
	for (int i = 0; i < getSize(); i++) {
		float a = rand() / (float)RAND_MAX;
		float z = _tempCurr * (pow(1. + 1./_tempCurr, 2*a - 1) - 1);
			
		if (z < 0.5) {
			z = -z;
		}
		
		newVal = _resultVector[i] + z*(getRightBoard() - getLeftBoard());
		
		if (newVal < getLeftBoard() || newVal > getRightBoard()) {
			return;
		} else {
			_candidate[i] = newVal;
		}
	}
}

void AnnealingAlgorithm::veryFastTemp()
{
	//static float c = exp(160. / _size);
	static float c = 1.1;
	
	_tempCurr = _tempStart * exp(-pow(getCoutFunc(), 1. / getSize()));
}

const float* AnnealingAlgorithm::getResultVector() const
{
	return _resultVector;
}

AnnealingAlgorithm::~AnnealingAlgorithm()
{
	//delete[] _result_vector;
	//delete[] _candidate;
}
