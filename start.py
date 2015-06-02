import os
import sys
import subprocess
import tarfile
import time
import datetime

import scripts.remote as remote
import scripts.config as conf
import scripts.local as local


programFile = "optimization.out"
options = {}

def readFromConfig(configFileName):
    global options
    options =  conf.readFromConfig(configFileName)


def addUserFunction():
    mainfile = open("src/main.cpp", "w")
    mainfile.write(MAIN % (options['function_name'], options['function_name']))
    mainfile.close()


def connectToHost():
    remote.connect(options)


def compileProgram():
    if options['host'] != 'local':
        remote.compileProgram()
    else:
        local.compileProgram()

def makeStartOptions():
    startOptions = []
    startOptions.append(options['algorithm_name'])
    startOptions.append(options['size'])
    startOptions.append(options['left_border'])
    startOptions.append(options['right_border'])

    return startOptions

def startProgram():
    resultFile = options['algorithm_name'] + '-' + options['function_name'] + '-' + options['size'] + '-' + datetime.datetime.now().strftime('%H:%M:%S' + '.txt')
    errFile = options['algorithm_name'] + '-' + options['function_name'] + '-' + options['size'] + '-' + datetime.datetime.now().strftime('%H:%M:%S' + '-err.txt')

    if options['host'] != 'local':
        remote.startProgram(options['process_number'], resultFile, errFile, makeStartOptions())
    else:
        local.startProgram(options['process_number'], resultFile, errFile, makeStartOptions())

    getResultFile(resultFile, errFile)

def getResultFile(fileName, errFile):
    if options['host'] != 'local':
        remote.getResultFile(fileName, errFile)

def startProgramWithArguments():
    pass


def cleanProgram():
    pass


def makeTarFile():
    with tarfile.open("src.tar", "w:gz") as tar:
        tar.add("src", arcname=os.path.basename("src"))
        tar.add("makefile", arcname=os.path.basename("makefile"))



MAIN = """#include <iostream>
#include <cmath>
#include <string>

#include "algorithmfactory.h"

#include <mpi.h>

std::string type;
int size = 0;
float left_border = 0.;
float right_border = 0.;

extern float %s(const float *params, int size);

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
    
    Algorithm *algorithm = AlgorithmFactory::create(type, size, %s, left_border, right_border);
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

"""


if __name__ == '__main__':
    if len(sys.argv) == 1:
        conf.createNewConfig()
    elif len(sys.argv) == 2:
        readFromConfig(sys.argv[1])
        addUserFunction()
        if options['host'] != 'local':
            makeTarFile()
            connectToHost()
        compileProgram()

        startProgram()

        cleanProgram()

        print "Finished"
    else:
        print "Wrong number of arguments for py-program"






