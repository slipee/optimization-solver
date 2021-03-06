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
#include <bitset>

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
        
        //  state[0].real(sqrt(1./2.));
        //  state[length-1].real(sqrt(1./2.));
        
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
void one_qub_transform(vector<complex> &st, vector<complex> &matr, int nqub);

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
//  std::copy(state.begin(), state.end(), st_copy.begin());
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

void one_qub_transform(vector<complex> &st, vector<complex> &matr, int nqub)
{
    vector<complex> new_vector(length);

    // std::cout << "Qbit number: " << nqub << std::endl;
    for (int ind = 0; ind < length; ind++) {
        int qub = N - nqub - 1;
        int first = ind & (~(1 << qub));
        int second = ind | (1 << qub);
        // std::cout << std::bitset<5>(first) << " " << std::bitset<5>(second) << std::endl;
        int bit_val = (ind & (1 << (N - nqub - 1))) >> (N - nqub - 1);

        new_vector[ind] = matr[nqub * 4 + 2 * bit_val]*st[first] + matr[nqub * 4 + 2 * bit_val + 1]*st[second];
    }

    // st = std::move(new_vector);
    for (int ind = 0; ind < length; ind++) {
        st[ind] = new_vector[ind];
    }
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








