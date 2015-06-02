import os

def connect():
    pass


def compileProgram():
    print "Compiling..."
    os.system('make CPL="mpicxx"')


def startProgram(processNumber, fileName, errFile, options):
    optionsStr = " ".join(options)

    print "Executing..."
    os.system('mpiexec -np %s optimization.out %s >> %s' % (processNumber, optionsStr, fileName))
    #os.system('mpiexec -np %s optimization.out %s ' % (processNumber, optionsStr))
