//
//  Algorithm.cpp
//  OprimizationSolver
//
//  Created by Denis Kropotov on 05/02/15.
//  Copyright (c) 2015 CMC MSU. All rights reserved.
//

#include "Algorithm.h"

Algorithm::Algorithm(int size, float (*targetFunc)(const float*, int), float leftEdge, float rightEdge):
_size(size),
_targetFunction(targetFunc),
_maxIter(MaxIterDefault),
_eps(EpsDefault),
_countFunc(0),
_leftEdge(leftEdge),
_rightEdge(rightEdge)
{
	
}

float Algorithm::callTagetFunction(const float *params)
{
	incrementCountFunc();
	return _targetFunction(params, _size);
}
