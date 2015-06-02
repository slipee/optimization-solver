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
_pop_size(size * SizeCoef),
//_pop_size(512),
_probability(DefaultProbability),
_mutation_power(DefaultMutPower),
_iter(0),
_res_vec_ind(0),
_best_val(false),
_res_vector(new float[size])
{
	int mpi_size;
	MPI_Comm_size(MPI_COMM_WORLD,&mpi_size);
	_pop_size /= mpi_size;
	
	if (params.size() != 0) {
		_pop_size = (int)params[0];
		_probability = (int)params[1];
		_mutation_power = (int)params[2];
	}
	
	_old_generation = new float[_pop_size * get_size()];
	_new_generation = new float[_pop_size * get_size()];
	
	init();
	
	_min = call_taget_function(_old_generation);
}


float DifferentialEvolution::solve()
{
	float tmp_min;
	int mpi_rank;
	MPI_Comm_rank(MPI_COMM_WORLD,&mpi_rank);
	
	//while (_iter++ < get_max_iter()) {
	while (get_cout_func() < 10000) {
		_best_val = false;
		float prev_min = _min;
		crossover();
		selection();
		MPI_Allreduce(&_min, &tmp_min, 1, MPI_FLOAT, MPI_MIN, MPI_COMM_WORLD);
		if (tmp_min == _min) {
			_best_val = true;
		}
		_min = tmp_min;
		
		if (prev_min - _min >= get_eps()) {
//			if (mpi_rank == 0) {
//				std::cout << _min << std::endl;
//			}
			_iter = 0;
		}
		
	}
	
	if (mpi_rank == 0) {
		std::cout << "iter " << get_cout_func() << std::endl;
	}
	
	return _min;
}

void DifferentialEvolution::crossover()
{
	int mpi_size, mpi_rank;
	MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);
	MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
	
	float *whole_pop = _old_generation;
	if (mpi_size > 1) {
		whole_pop = new float[_pop_size * mpi_size * get_size()];
		
		MPI_Barrier(MPI_COMM_WORLD);
		MPI_Allgather(_old_generation, _pop_size*get_size(), MPI_FLOAT, whole_pop, _pop_size*get_size(), MPI_FLOAT, MPI_COMM_WORLD);
	}
	
	for (int i = 0; i < _pop_size; i++) {
		float new_val = 0;
		
		std::set<int> indexes;
		int partner = uniq_in_set(indexes);
		indexes.insert(partner);
		
		int noise_one = uniq_in_set(indexes);
		indexes.insert(noise_one);
		
		int noise_two = uniq_in_set(indexes);
		indexes.insert(noise_two);
		
		for (int j = 0; j < get_size(); j++) {
			do {
				float prob = rand()/(float)RAND_MAX;
				
				if (prob < _probability) {
					new_val = mutation(whole_pop, j, partner, noise_one, noise_two);
				} else {
					new_val = _old_generation[i*get_size() + j];
				}
				
			} while (new_val < get_left_board() || new_val > get_right_board());
			_new_generation[i*get_size() + j] = new_val;
		}
	}
	
	if (mpi_size > 1) {
		delete [] whole_pop;
	}
}

float DifferentialEvolution::mutation(float *whole_pop, int attr, int partner, int noise_one, int noise_two)
{
	float noise = _mutation_power*(whole_pop[noise_one*get_size() + attr] - whole_pop[noise_two*get_size() + attr]);
	return whole_pop[partner*get_size() + attr] + noise;
}

void DifferentialEvolution::selection()
{
	for (int i = 0; i < _pop_size; i++) {
		float old_value = call_taget_function(_old_generation + i*get_size());
		float new_value = call_taget_function(_new_generation + i*get_size());
		
		if (new_value < old_value) {
			std::copy(_new_generation + i*get_size(), _new_generation + (i+1)*get_size(), _old_generation + i*get_size());
			
			if (new_value < _min) {
				_min = new_value;
				_res_vec_ind = i;
			}
		}
	}
}

int DifferentialEvolution::uniq_in_set(std::set<int> set)
{
	int mpi_size;
	MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);
	
	int uniq = rand() % (_pop_size * mpi_size);
	while (set.find(uniq) != set.end()) {
		uniq = rand() % (_pop_size * mpi_size);
	}
	
	return uniq;
}

void DifferentialEvolution::init()
{
	for (int i = 0; i < _pop_size; i++) {
		for (int j = 0; j < get_size(); j++) {
			_old_generation[i*get_size() + j] = (rand() / (float)RAND_MAX)*(fabs(get_right_board()) + fabs(get_left_board()) + get_left_board());
		}
	}
}

const float* DifferentialEvolution::get_result_vector() const
{
	if (_best_val) {
		return (_old_generation + _res_vec_ind*get_size());
	} else {
		return NULL;
	}
}

DifferentialEvolution::~DifferentialEvolution()
{
	
}
