//
//  algorithmfactory.cpp
//  OprimizationSolver
//
//  Created by Denis Kropotov on 08/03/15.
//  Copyright (c) 2015 CMC MSU. All rights reserved.
//

#include <iostream>

#include "algorithmfactory.h"
#include "Algorithm.h"
#include "AnnealingAlgorithm.h"
#include "differentialevolution.h"
#include "GeneticAlgorithm.h"

Algorithm* AlgorithmFactory::create(std::string name, int size, float (*target_func)(const float*, int),
									float left_edge, float right_edge, vector<double> additional)
{
	if (name.compare("AA") == 0) {
		return new AnnealingAlgorithm(size, target_func, left_edge, right_edge, additional);
	} else if (name.compare("MA") == 0) {
		return new GeneticAlgorithm(size, target_func, left_edge, right_edge, additional);
	} else if (name.compare("DE") == 0) {
		return new DifferentialEvolution(size, target_func, left_edge, right_edge, additional);
	} else {
		throw;
	}
}
