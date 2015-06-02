
DEFAULT_CONFIG_NAME = "config.cfg"
CONFIG = """
#adress for remote and "local" for local calculation
host = local

#user name for remote and empty for local
user = 

#password for private key
password = 

#private key for remote access
private_key_path = 

#process number to start
process_number = 4

#AA for annealing, MA for mutation, DE for differential
algorithm_name = MA

#.cpp file that contains function
function_file_name = rosenbrock.cpp

#function name in file that will be calculated
function_name = rosenbrock

#number of parameters in function
size = 20

#min value of parameter in function
left_border = -2.048

#max value of parameter in function
right_border = 2.048

#list of parameters to start algorithm
parameters = 
""" 

def readFromConfig(configFileName):
    options = {}

    config = open(configFileName, "r")

    for line in config.read().splitlines():
        if line and line[0] == '#':
            continue

        configOptions = line.split(" = ")

        if len(configOptions) == 2:
            options[configOptions[0].replace(" ", "")] = configOptions[1].replace(" ", "")
        else:
            options[configOptions[0].replace(" ", "")] = ""

    config.close()
    return options


def createNewConfig():
    config = open(DEFAULT_CONFIG_NAME, "w")
    config.write(CONFIG)
    config.close()

