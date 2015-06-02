//
//  algorithmfactory.h
//  OprimizationSolver
//
//  Created by Denis Kropotov on 08/03/15.
//  Copyright (c) 2015 CMC MSU. All rights reserved.
//

#ifndef __OprimizationSolver__algorithmfactory__
#define __OprimizationSolver__algorithmfactory__

#include <string>
#include <vector>
#include "Algorithm.h"

using std::vector;

class AlgorithmFactory {
	
public:
	static Algorithm* create(std::string name, int size, float (*targetFunc)(const float* params, int),
							 float leftEdge, float rightEdge, vector<double> additional = vector<double>());
};

#endif /* defined(__OprimizationSolver__algorithmfactory__) */
