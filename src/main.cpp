#include <iostream>
#include <cmath>
#include <string>

#include "algorithmfactory.h"

#include <mpi.h>

std::string type;
int size = 0;
float leftBorder = 0.;
float rightBorder = 0.;

extern float rosenbrock(const float *params, int size);

int main(int argc, char *argv[])
{
    MPI_Init(&argc,&argv);

    int mpiSize, mpiRank;
    MPI_Comm_size(MPI_COMM_WORLD,&mpiSize);
    MPI_Comm_rank(MPI_COMM_WORLD,&mpiRank);

    srand(time(NULL) + mpiSize*mpiRank*100);

    type = static_cast<std::string>(argv[1]);
    size = atoi(argv[2]);
    leftBorder = atof(argv[3]);
    rightBorder = atof(argv[4]);
    
    vector<double> params;
    int ind = -1;
    for (int i = 0; i < argc; i++) {
        if (std::string("-p").compare(argv[i])) {
            ind = i+2;
            break;
        }
    }
    if (ind != -1) {
        while (ind < argc) {
            params.push_back(atof(argv[ind++]));
        }
    }

    double start = MPI_Wtime();
    
    Algorithm *algorithm = AlgorithmFactory::create(type, size, rosenbrock, leftBorder, rightBorder);
    float algRes = algorithm->solve();

    double end = MPI_Wtime();

    if (mpiRank == 0) {
        std::cout << "ans " << algRes << std::endl;
        std::cout << "time = " << end - start << std::endl;
    }

    const float *resVec = algorithm->getResultVector();
    if (resVec != NULL) {
            std::cout << "(" << resVec[0] << ", ";
            for (int i = 1; i < size-1; i++) {
            std::cout << resVec[i] << ", ";
        }
        std::cout << resVec[0] << ")" << std::endl;
    }

    MPI_Finalize();
    
    return 0;
} 

