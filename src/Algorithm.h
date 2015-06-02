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

const int MAX_ITER_DEFAULT = 50;
const float EpsDefault = 0.0001;

class Algorithm {
private:
	float (*_target_function)(const float*, int);
	int _size;
	int _max_iter;
	
	float _eps;
	
	float _left_edge;
	float _right_edge;
	long long _count_func;
	
	
public:
	Algorithm(int size, float (*target_func)(const float*, int), float left_edge, float right_edge);
	
	virtual ~Algorithm(){}
	
	void set_max_iter(int iter) { _max_iter = iter;}
	
	long long get_cout_func() const { return _count_func;}
	int get_size() const { return _size;}
	int get_max_iter() const { return _max_iter;}
	float get_left_board() const { return _left_edge;}
	float get_right_board() const {return _right_edge;}
	float get_eps() const {return _eps;}
	
	virtual float solve() = 0;
	virtual const float* get_result_vector() const = 0;
	
protected:
	float call_taget_function(const float *params);
	
private:
	void increment_count_func() { _count_func++;}
};


#endif /* defined(__OprimizationSolver__Algorithm__) */
