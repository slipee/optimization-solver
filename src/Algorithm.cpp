//
//  Algorithm.cpp
//  OprimizationSolver
//
//  Created by Denis Kropotov on 05/02/15.
//  Copyright (c) 2015 CMC MSU. All rights reserved.
//

#include "Algorithm.h"

Algorithm::Algorithm(int size, float (*target_func)(const float*, int), float left_edge, float right_edge):
_size(size),
_target_function(target_func),
_max_iter(MAX_ITER_DEFAULT),
_eps(EpsDefault),
_count_func(0),
_left_edge(left_edge),
_right_edge(right_edge)
{
	
}

float Algorithm::call_taget_function(const float *params)
{
	increment_count_func();
	return _target_function(params, _size);
}
