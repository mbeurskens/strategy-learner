# Strategy Learner
Strategy learner is a library for the [Tech United](https://www.techunited.nl/en/) Middle Sized League soccer robots that
uses machine learning to pick tasks for robots during play. It is part of the Mechnical Engineering Master
Thesis project of [Mickey Beurskens](https://mickeybeurskens.nl) at the [Eindhoven University of Technology](https://www.techunited.nl/), Netherlands. 

This repository accompanies the [Master's thesis] research on reinforcement learning for robot soccer, and is 
hosted for the purpose of academic completeness. It is intended as a part of the full [Tech United soccer robots software stack](https://robocup.wtb.tue.nl/svn/techunited/trunk/src/Turtle2/).
I do not have permission to host the full software stack, which makes installing and using this software
 convoluted at best. This software is not user friendly at all. The architecture has evolved over time, subject to
  all kinds of limitations imposed by the soccer software stack, rather than it being properly designed. I do not
   recommend using this software yourself. It is best used as inspiration
   for a newly designed system.
 
With that out of the way the remainder of this document treats the software structure and installation instructions
 of the strategy learner.

## Structure
Strategy learner includes five parts: 
- **Inference: A C++ based inference library in pytorch.** Used by the robots during matches to perform forward passes
 through
 the strategy learner network. It needs to be started separately before the robots are started.
- **Training: A Python based training library running PyTorch.** Used to train the strategy learner models offline. It
 needs
 data generated by running the robots in the MSL simulator to train.
- **Src: A C based interface that runs on the soccer robot software stack.** It is used to read the values predicted by
 the inference library through shared memory. It also logs match data, which is used by the training library and the
  data analysis library. Finally, it contains some debugging tools and the rule based variant of the soccer robot
   behaviour.
- **Evaluation: An analysis library.** It reads match data and produces plots to analyse training and performance.
- **Experiments: Matlab library that interfaces with the CI API in the Tech United simulator.** Used to run training
 and evaluation automatically.

The repository root contains a CMake file in the root that correspond to the *inference* directory. It also contains
 *make_strategy_learner.m*, *get_strategy_learner_includes.m* and
  *get_strategy_learner_libs.m* which are necessary to compile *src* together with the soccer robot software stack. A
   *requirements.txt* file contains the necessary python packages for *training* and *evaluation*. Finally
   , *make_plots.py* contains an interface to create evaluation plots from the command line. It is used by the
    *experiments* library.
    
At the time of writing it is necessary to use Ubuntu 16.04 as an operating system to run the soccer robot software
 stack together with Matlab 2016a. If it has not already happened, this information should stop you from using this
  software as Ubuntu 16.04 is deprecated and does not receive any security updates anymore.

## Setup
This repository should be recursively cloned into the [Tech United software](https://robocup.wtb.tue.nl/svn/techunited/trunk/src/Turtle2/) at ```home/robocup/trunk/src/Turtle2/Strategy/src/STP/strategy_learner```
and is treated as a static library. It has been created for Tech United MSL software ```Revision 12030```. 

### Installation
The matlab make file *make_strategy_learner.m* should be included in the build chain of ```/home/robocup/svn/trunk/src
/Turtle2/Strategy``` in the soccer robot software stack. The source files of this repository can then be used in the
 main software stack.

### Experiments
Turtle 2 should always be running. This turtle manages the inference initialization and closing. Take a look at 
```sl_model.c``` for more details. You need to run the inference library first, and keep in on for the duration of the experiments.
After the last time the robots shut down the inference executable can be stopped. If not done correctly, shared memory might
not be closed properly. In order to run experiments manually you need to start the tech united simulator. In order to
 run experiments automatically, take a look at ```/experiments/train.m```.

To troubleshoot problems with shared memory when doing these steps out of order:
 ```
If one of the programs crashed before its resources could be freed, you
can use the 'ipcs -m' shell command to display the currently allocated
shared memory segments, and 'ipcs -s' to display the semaphores.
With 'ipcrm shm <shmKey>', shared memory segments can be removed.
Semaphores can be removed using 'ipcrm sem <semKey>'.
```
Read ```src/shared_memory.h``` for more details.

