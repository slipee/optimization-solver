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
float leftBorder = 0.;
float rightBorder = 0.;

extern float %s(const float *params, int size);

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
    
    Algorithm *algorithm = AlgorithmFactory::create(type, size, %s, leftBorder, rightBorder);
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






