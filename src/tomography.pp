//
//  tomography.cpp
//  OprimizationSolver
//
//  Created by Denis Kropotov on 09/03/15.
//  Copyright (c) 2015 CMC MSU. All rights reserved.
//

#include "Eigen/Eigen"
#include <iostream>
#include <vector>
#include <cmath>
#include <complex>

using namespace Eigen;

#define complex std::complex<float>

MatrixXcf kron(MatrixXcf, MatrixXcf);

class X_Cube {
	MatrixXcf X;
	
public:
	X_Cube() {
		X = MatrixXcf(6,2);
		X(0,0) = complex(0.8776, -0.4152);
		X(0,1) = complex(-0.1695, -0.1695);
		X(1,0) = complex(0.1695, -0.1695);
		X(1,1) = complex(0.8776, 0.4152);
		X(2,0) = complex(0.7404, -0.4134);
		X(2,1) = complex(0.5007, 0.1737);
		X(3,0) = complex(0.5007, -0.1737);
		X(3,1) = complex(-0.7404, -0.4134);
		X(4,0) = complex(0.5007, -0.4134);
		X(4,1) = complex(0.1737, -0.7404);
		X(5,0) = complex(0.7404, -0.1737);
		X(5,1) = complex(-0.4134, 0.5007);
		
		X = kron(X, X);
	}
	const MatrixXcf getX() { return X;}
};

X_Cube cube;

float avg_loss(const float *params, int size)
{
	MatrixXcf X = cube.getX();
	
	VectorXcf state(size/2);
	
	for (int i = 0; i < size/2; i++) {
		state(i) = complex(params[i*2], params[i*2+1]);
	}
	
	state.normalize();
	state = state.adjoint();
	
	long length = state.size();
	long pn = X.rows();
	
	MatrixXcf H = MatrixXcf::Zero(length, length);
	VectorXcf l(pn);
	
	for (int i = 0; i < pn; i++) {
		VectorXcf X1 = X.row(i);
		MatrixXcf XX(2, length);
		
		for (int i = 0; i < 2; i++) {
			for (int j = 0; j < length; j++) {
				if (i == 0) {
					if (j < length/2) {
						XX(i, j) = X1(j).real();
					} else {
						XX(i, j) = -X1(j-length/2).imag();
					}
				} else {
					if (j < length/2) {
						XX(i, j) = X1(j).imag();
					} else {
						XX(i, j) = X1(j-length/2).real();
					}
				}
			}
		}
		
		MatrixXcf L = XX.adjoint() * XX;
		
		VectorXcf lc = L*state;
		
		MatrixXcf tmp = state.adjoint() * L * state;
		l(i) = tmp(0,0);
		H = H + lc*lc.adjoint()/l(i);
	}
	
	H *= 2;
	
	JacobiSVD<MatrixXcf> svd_H(H, ComputeThinU | ComputeThinV);
	
	long n = svd_H.singularValues().size() - 1;
	
	int pow_n = log2(length) - 1;
	
	double dim = pow(pn, 1.0/pow_n) / 2;
	
	double sum = 0.;
	
	for (int i = 1; i < n; i++) {
		sum += 1./svd_H.singularValues()[i];
	}
	
	return sum/2*pow(dim, pow_n);
}

MatrixXcf kron(MatrixXcf m1, MatrixXcf m2)
{
	MatrixXcf m3(m1.rows()*m2.rows(), m1.cols()*m2.cols());
	
	for (int i = 0; i < m1.rows(); i++) {
		for (int j = 0; j < m1.cols(); j++) {
			m3.block(i*m2.rows(), j*m2.cols(), m2.rows(), m2.cols()) =  m1(i,j)*m2;
		}
	}
	
	return m3;
}




