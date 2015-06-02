//
//  measure.cpp
//  OprimizationSolver
//
//  Created by Denis Kropotov on 13/03/15.
//  Copyright (c) 2015 CMC MSU. All rights reserved.
//

#include <stdio.h>
#include <cmath>
#include <vector>
#include <complex>
#include <algorithm>
#include <iostream>

using std::vector;

#define complex std::complex<float>

unsigned int N;
unsigned int length;
int max_qub;
int EDGE;
int grid_size;
int block_size;

class QuantumState {
private:
	static QuantumState *instance;
	vector<complex> state;
	
	QuantumState() {
		state = vector<complex>(length);
		for (int i = 0; i < length; i++) {
			state[i] = complex(0, 0);
		}
		
		//	state[0].real(sqrt(1./2.));
		//	state[length-1].real(sqrt(1./2.));
		
		for (int i = 0; i < N; i++) {
			state[(int)pow(2., i)] = complex(sqrt(1./N), 0);
		}
	}
	QuantumState( const QuantumState& );
	QuantumState& operator=( QuantumState& );
	
public:
	static QuantumState *getInstance() {
		if (!instance) {
			instance = new QuantumState();
		}
		return instance;
	}
	
	vector<complex> getState() {return state;}
};

QuantumState *QuantumState::instance = 0;

void unit_param(float x, float y, float z, vector<complex>&);
void fill_matrix(const float *params, vector<complex>&);
vector<complex> one_qub_transform(vector<complex> &st, vector<complex> &matr, int nqub);

float measure(const float *params, int size)
{
	N = size/2;
	length = 1 << N;
	max_qub =  N/2 + N%2;
	EDGE = (1 << N) - 1;
	grid_size = 1 << max_qub;
	block_size = grid_size >> N % 2;
	
	vector<complex> matr;
	fill_matrix(params, matr);
	
	vector<complex> st_copy(QuantumState::getInstance()->getState());
//	std::copy(state.begin(), state.end(), st_copy.begin());
	for (int nqub = 0; nqub < N; nqub++) {
		one_qub_transform(st_copy, matr, nqub);
	}
	
	float sum = 0.;
	for (int i = 0; i < length; i++) {
		float mod_sqr = abs(st_copy[i]) * abs(st_copy[i]);
		if (mod_sqr > 0) {
			sum += mod_sqr * log(mod_sqr);
		}
	}
	
	return -sum;
}

void fill_matrix(const float *params, vector<complex> &matr)
{
	for (int i = 0; i < N; i++) {
		unit_param(0, params[i*2], params[i*2 + 1], matr);
	}
}

vector<complex> one_qub_transform(vector<complex> &st, vector<complex> &matr, int nqub)
{
	vector<complex> out(st.size());
	
	for (int i = 0; i < grid_size; i++) {
		complex *tmp_in = new complex[block_size];
		for (int p = 0; p < block_size; p++) {
			int ind = (p << max_qub) + i;
			tmp_in[p] = st[ind];
		}
		for (int j = 0; j < block_size; j++) {
			if (nqub < (N - max_qub)) {
				int ind = (j << max_qub) + i;
				
				int first = (ind & ~(1 << (N - nqub - 1))) >> max_qub;
				int second = (ind | 1 << (N - nqub - 1)) >> max_qub;
				
				int bit_val = (ind & (1 << (N - nqub - 1))) >> (N - nqub - 1);
				
				complex ans1 = matr[nqub * 4 + 2 * bit_val] * tmp_in[first];
				complex ans2 = matr[nqub * 4 + 2 * bit_val + 1] * tmp_in[second];
				
				out[ind] = ans1 + ans2;
			} else {
				int tid_tmp = (((j >> 1) << (N - max_qub + N % 2 + 1)) & EDGE) + (((j << (N - 1)) & EDGE) >> nqub);
				int ind = ((i >> (N - nqub - 1) << (N - nqub))) + ((i << (nqub + 1) & EDGE) >> (nqub + 1)) + tid_tmp;
				
				int first = (ind & ~(1 << (N - nqub - 1)));
				int second = (ind | 1 << (N - nqub - 1));
				
				first = ((first >> (N - max_qub + N % 2 + 1)) << 1) + ((first >> (N - nqub - 1)) & 1);
				second = ((second >> (N - max_qub + N % 2 + 1)) << 1) + ((second >> (N - nqub - 1)) & 1);
				
				int bit_val = (ind & (1 << (N - nqub - 1))) >> (N - nqub - 1);
				
				complex ans1 = matr[nqub * 4 + 2 * bit_val] * tmp_in[first];
				complex ans2 = matr[nqub * 4 + 2 * bit_val + 1] * tmp_in[second];
				
				out[ind] = ans1 + ans2;
			}
		}
		delete [] tmp_in;
	}
	
	for (int i = 0; i < out.size(); i++) {
		st[i] = out[i];
	}
	
	return out;
}

void unit_param(float x, float y, float z, vector<complex> &matr)
{
	
	complex e_x;
	e_x = complex(cos(-x), sin(-x));
	complex e_y;
	e_y = complex(cos(-y), sin(-y));
	complex tmp = e_x * e_y;
	complex tmp2 (float(cos(z)), 0);
	tmp = tmp * tmp2;
	matr.push_back(tmp);
	
	e_x = complex(cos(-x), sin(-x));
	e_y = complex(cos(y), sin(y));
	tmp = e_x * e_y;
	tmp = complex(float(-tmp.real() * sin(z)), float(-tmp.imag() * sin(z)));
	matr.push_back(tmp);
	
	e_x = complex(cos(x), sin(x));
	e_y = complex(cos(-y), sin(-y));
	tmp = e_x * e_y;
	tmp = complex(float(tmp.real()*sin(z)), float(tmp.imag()*sin(z)));
	matr.push_back(tmp);
	
	e_x = complex(cos(x), sin(x));
	e_y = complex(cos(y), sin(y));
	tmp = e_x * e_y;
	tmp = complex(float(tmp.real() * cos(z)), float(tmp.imag() * cos(z)));
	matr.push_back(tmp);
}








