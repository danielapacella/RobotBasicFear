# RobotBasicFear
Code for the PLOS paper "Basic emotions and adaptation. A computational and evolutionary model"
The code represents a modified version of the software http://laral.istc.cnr.it/evorobotstar/ Manual is also available.

## How to setup:
- Clone the repository
- Open the project, compile it in release version
- In the newly created bin folder, copy all files from the folder "Configuration"
- To start with a pre-trained network with parameters from the paper, copy and paste the files from the "Configuration/sample_results" folder to the bin folder as well
- Run the program

## Parameters
- The evorobot.net file contains the details about the neural network used for training, unit by unit, connection by connection
- The evorobot.cf file contains all the settings of the environment, genetic algorithm training and test, fitness function to use
- The fitness function is defined in the source file "epuck.cpp", along with the obstacles and color of the environment
- The type of log files can be defined as well in the same file
- *.gen* files contain the weights of each generation
- *best.gen* files contain the performances of the best robot for each generation tested in the test environment
- *.fit* files contain fitness values for each robot
