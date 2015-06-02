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

AnnealingAlgorithm::AnnealingAlgorithm(int size, float (*target_func)(const float*, int), float left, float right, vector<double> params)
: Algorithm(size, target_func, left, right),
_temp_start(TEMP_START_DEFAULT),
_temp_curr(TEMP_START_DEFAULT),
_temp_end(TEM_END_DEFAULT),
_iter_to_exchange(5),
_result_vector(new float[get_size()]),
_candidate(new float[get_size()])
{
	if (params.size() != 0) {
		_temp_start = params[0];
		_temp_curr = _temp_start;
		_temp_end = params[1];
	}
	
	int mpi_size;
	MPI_Comm_size(MPI_COMM_WORLD,&mpi_size);
	
	//_temp_start /= sqrt(mpi_size);
}

float AnnealingAlgorithm::solve()
{
	for (int i = 0 ; i < get_size(); i++) {
		_result_vector[i] = (rand() / (float)RAND_MAX)*(fabs(get_right_board()) + fabs(get_left_board()) + get_left_board());
	}
	
	float res_value = call_taget_function(_result_vector);
	float curr_value = res_value;
	
	int mpi_rank, mpi_size;
	MPI_Comm_rank(MPI_COMM_WORLD,&mpi_rank);
	MPI_Comm_size(MPI_COMM_WORLD,&mpi_size);
	
	int iter = 0;
	int iter2 = 0;
	//while (_temp_curr >= _temp_end) {
	while (get_cout_func() < 10000) {
		float prev_min = res_value;
		cauchy_dist();
		
		curr_value = call_taget_function(_candidate);
		
		if (curr_value < res_value) {
			for (int i = 0; i < get_size(); i++) {
				_result_vector[i] = _candidate[i];
			}
			res_value = curr_value;
			
			cauchy_temp();
		} else {
			float probability = expf(-(curr_value - res_value)/_temp_curr);
			if (probability < 0) {
				exit(-99);
			}
			
			float a = rand() / (float)RAND_MAX;
			
			if (a < probability) {
				for (int i = 0; i < get_size(); i++) {
					_result_vector[i] = _candidate[i];
				}
				
				res_value = curr_value;
				
				cauchy_temp();
			}
		}
		if (mpi_rank == 0) {
			//std::cout << res_value << std::endl;
		}
		iter++;
		
		if (mpi_size > 100 && iter % _iter_to_exchange == 0) {
			res_value = exchange();
		}
		
		cauchy_temp();
	}
	
	MPI_Barrier(MPI_COMM_WORLD);
	float tmp_min;
	MPI_Allreduce(&res_value, &tmp_min, 1, MPI_FLOAT, MPI_MIN, MPI_COMM_WORLD);
	
	res_value = tmp_min;
	
	if (mpi_rank == 0) {
		std::cout << "iter " << get_cout_func() << std::endl;
	}
	
	return res_value;
}

float AnnealingAlgorithm::exchange()
{
	int mpi_rank, mpi_size;
	MPI_Comm_rank(MPI_COMM_WORLD,&mpi_rank);
	MPI_Comm_size(MPI_COMM_WORLD,&mpi_size);
	
	float *all_vectors = new float[get_size() * mpi_size];
	
	MPI_Allgather(_result_vector, get_size(), MPI_FLOAT, all_vectors, get_size(), MPI_FLOAT, MPI_COMM_WORLD);
	
	float cur_min = call_taget_function(all_vectors);
	int ind = 0;
	for (int i = 1; i < mpi_size; i++) {
		float tmp_min = call_taget_function(all_vectors + i*get_size());
		if (tmp_min < cur_min) {
			cur_min = tmp_min;
			ind = i;
		}
	}
	
	for (int i = 0; i < get_size(); i++) {
		_result_vector[i] = all_vectors[ind*get_size() + i];
	}
	
	return cur_min;
}

void AnnealingAlgorithm::cauchy_dist()
{
	float new_val;
	for (int i = 0; i < get_size(); i++) {
		float a = rand() / (float)RAND_MAX;
		
		float z = _temp_curr * tan(M_PI*a - M_PI/2.);
		new_val = _result_vector[i] + z*(get_right_board() - get_left_board());
		if (new_val < get_left_board() || new_val > get_right_board()) {
			_candidate[i] = _result_vector[i];
		} else {
			_candidate[i] = new_val;
		}
	}
}

void AnnealingAlgorithm::cauchy_temp()
{
	_temp_curr = _temp_start / sqrt((double)get_cout_func());
}

void AnnealingAlgorithm::very_fast_dist()
{
	float new_val;
	for (int i = 0; i < get_size(); i++) {
		float a = rand() / (float)RAND_MAX;
		float z = _temp_curr * (pow(1. + 1./_temp_curr, 2*a - 1) - 1);
			
		if (z < 0.5) {
			z = -z;
		}
		
		new_val = _result_vector[i] + z*(get_right_board() - get_left_board());
		
		if (new_val < get_left_board() || new_val > get_right_board()) {
			return;
		} else {
			_candidate[i] = new_val;
		}
	}
}

void AnnealingAlgorithm::very_fast_temp()
{
	//static float c = exp(160. / _size);
	static float c = 1.1;
	
	_temp_curr = _temp_start * exp(-pow(get_cout_func(), 1. / get_size()));
}

const float* AnnealingAlgorithm::get_result_vector() const
{
	return _result_vector;
}

AnnealingAlgorithm::~AnnealingAlgorithm()
{
	//delete[] _result_vector;
	//delete[] _candidate;
}
