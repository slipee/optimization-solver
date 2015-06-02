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
_num_pop(NUM_POP_DEFAULT),
_num_desc(NUM_DESC_DEFAULT),
_max_mut(5),
_iter(0),
_res_vec_ind(0),
_best_val(false)
{
	if (params.size() != 0) {
		_num_pop = (int)params[0];
		_num_desc = (int)params[1];
		_max_mut = (int)params[2];
	}
	
	int mpi_size;
	MPI_Comm_size(MPI_COMM_WORLD,&mpi_size);
	_num_pop /= mpi_size;
	
	_population = vector<vector<float> >(_num_pop, vector<float>(get_size()));
	_intermediate_min = vector<float>(_num_pop);
	_descendants = vector<vector<float> >(_num_desc+1, vector<float>(get_size()));
	
	init();
	
	_min = call_taget_function(&_population[0][0]);
}

float GeneticAlgorithm::solve()
{	
	float tmp_min;
	int mpi_rank;
	MPI_Comm_rank(MPI_COMM_WORLD,&mpi_rank);

	while (_iter++ < get_max_iter()) {
		selection();
		
		float prev_min = _min;
		float inter_min = _min;
		for (int i = 0; i < _intermediate_min.size(); i++) {
			if (inter_min >= _intermediate_min[i]) {
				_res_vec_ind = i;
				inter_min = _intermediate_min[i];
			}
		}
		_min = inter_min;
		
		MPI_Barrier(MPI_COMM_WORLD);
		MPI_Allreduce(&_min, &tmp_min, 1, MPI_FLOAT, MPI_MIN, MPI_COMM_WORLD);
		if (prev_min != tmp_min) {
			_best_val = false;
			if (_min == tmp_min) {
				_best_val = true;
			}
		}
		_min = tmp_min;
		
		if (prev_min - _min >= get_eps()) {
			_iter = 0;
		}
	}
	
	if (mpi_rank == 0) {
//		std::cout << "iter " << get_cout_func() << std::endl;
	}
	return _min;
}

void GeneticAlgorithm::selection()
{
	for (int i_pop = 0; i_pop < _num_pop; i_pop++) {
		mutation(i_pop);

		float min = call_taget_function(&_descendants[0][0]);
		int best_ind = 0;
		float cur_min;
		for (int i_desc = 1; i_desc < _num_desc+1; i_desc++) {
			cur_min = call_taget_function(&_descendants[i_desc][0]);
			if (cur_min < min) {
				min = cur_min;
				best_ind = i_desc;
			}
		}

		_intermediate_min[i_pop] = min;
		for (int i = 0; i < get_size(); i++) {
			_population[i_pop][i] = _descendants[best_ind][i];
		}
	}
}

void GeneticAlgorithm::mutation(int index)
{
	for (int i = 0; i < _num_desc; i++) {
		for (int j = 0; j < get_size() ; j++) {
			_descendants[i][j] = _population[index][j];
		}
	}
	for (int i = 0; i < get_size(); i++) {
		_descendants[_num_desc][i] = _population[index][i];
	}

	for (int i_desc = 0; i_desc < _num_desc; i_desc++) {
		int n_mut = rand() % get_size();

		std::set<int> indexes;
		for (int i = 0; i < n_mut; i++) {
			int uniq = rand() % (get_size());
			while (indexes.find(uniq) != indexes.end()) {
				uniq = rand() % (get_size());
			}
			indexes.insert(uniq);
		}
		
		float new_val;
		for(std::set<int>::const_iterator it = indexes.begin(); it != indexes.end(); it++) {
			do {
				float m = 2 * (rand() / (float)RAND_MAX) - 1;
				float p = (rand() % (PMax - PMin)) + PMin;
				new_val = _descendants[i_desc][*it] + m * pow(Base, p);
			} while (new_val < get_left_board() || new_val > get_right_board());
			_descendants[i_desc][*it] = new_val;
		}
	}
	
}

void GeneticAlgorithm::init()
{
	for (int i = 0; i < _num_pop; i++) {
		for (int j = 0; j < get_size(); j++) {
			_population[i][j] = (rand() / (float)RAND_MAX)*(fabs(get_right_board()) + fabs(get_left_board()) + get_left_board());
		}
	}
}

const float* GeneticAlgorithm::get_result_vector() const
{
	if (_best_val) {
		return &_population[_res_vec_ind][0];
	} else {
		return NULL;
	}
}

GeneticAlgorithm::~GeneticAlgorithm()
{
	//delete[] _intermediate_min;
}





























