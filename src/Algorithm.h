//
//  Algorithm.h
//  OprimizationSolver
//
//  Created by Denis Kropotov on 05/02/15.
//  Copyright (c) 2015 CMC MSU. All rights reserved.
//

#ifndef __OprimizationSolver__Algorithm__
#define __OprimizationSolver__Algorithm__

#include <string>
#include <vector>

using std::vector;

const int MaxIterDefault = 50;
const float EpsDefault = 0.0001;

class Algorithm {
private:
	float (*_targetFunction)(const float*, int);
	int _size;
	int _maxIter;
	
	float _eps;
	
	float _leftEdge;
	float _rightEdge;
	long long _countFunc;
	
	
public:
	Algorithm(int size, float (*targetFunc)(const float*, int), float leftEdge, float rightEdge);
	
	virtual ~Algorithm(){}
	
	void set_max_iter(int iter) { _maxIter = iter;}
	
	long long getCoutFunc() const { return _countFunc;}
	int getSize() const { return _size;}
	int getMaxIter() const { return _maxIter;}
	float getLeftBoard() const { return _leftEdge;}
	float getRightBoard() const {return _rightEdge;}
	float getEps() const {return _eps;}
	
	virtual float solve() = 0;
	virtual const float* getResultVector() const = 0;
	
protected:
	float callTagetFunction(const float *params);
	
private:
	void incrementCountFunc() { _countFunc++;}
};


#endif /* defined(__OprimizationSolver__Algorithm__) */
