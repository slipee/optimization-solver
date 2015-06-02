#include <iostream>
#include <cmath>
#include <string>

#include "algorithmfactory.h"

#include <mpi.h>

std::string type;
int size = 0;
float left_border = 0.;
float right_border = 0.;

extern float rosenbrock(const float *params, int size);

int main(int argc, char *argv[])
{
    MPI_Init(&argc,&argv);

    int mpi_size, mpi_rank;
    MPI_Comm_size(MPI_COMM_WORLD,&mpi_size);
    MPI_Comm_rank(MPI_COMM_WORLD,&mpi_rank);

    srand(time(NULL) + mpi_size*mpi_rank*100);

    type = static_cast<std::string>(argv[1]);
    size = atoi(argv[2]);
    left_border = atof(argv[3]);
    right_border = atof(argv[4]);
    
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
    
    Algorithm *algorithm = AlgorithmFactory::create(type, size, rosenbrock, left_border, right_border);
    float alg_res = algorithm->solve();

    double end = MPI_Wtime();

    if (mpi_rank == 0) {
        std::cout << "ans " << alg_res << std::endl;
        std::cout << "time = " << end - start << std::endl;
    }

    const float *res_vec = algorithm->get_result_vector();
    if (res_vec != NULL) {
            std::cout << "(" << res_vec[0] << ", ";
            for (int i = 1; i < size-1; i++) {
            std::cout << res_vec[i] << ", ";
        }
        std::cout << res_vec[0] << ")" << std::endl;
    }

    MPI_Finalize();
    
    return 0;
} 
