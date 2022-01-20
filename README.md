# MPI applied to Value Iteration, HPCfMI WS20/21, Group 3
Participants

## Measurement objectives

Possible measurements:
- Prio A:
  - execution time (total, per block)
  - iterations until convergence
- Prio B:
  - memory usage (RAM)
- Prio C:
  - I/O consumption

Possible variatiation points:
- Prio A:
  - MPI schemes (minimum 2)
  - MPI synchronization intervall (cycles)
  - MPI processor count
- Prio B:
  - computing hardware (HPC class 1, HPC class 2, HPC class mixed, RPi cluster, NUC cluster)
  - asynchronous vs. synchronous VI with OpenMP
- Prio C:
  - J split algorithm
  - baremetal vs. cluster/kubernetes

## Using the project

# Procedure

**init**
Downloads the dataset and prepares it for cpp.
**test**
Checks integrity of data in /var/tmp/user/ and the downloaded data.

In /automation/jobs/ lay yaml files with the desired parameters/configuration.

# Makefile

- **clean**:
  - Cleans repository and removes build files.
- **compile**:  --> here and at **test** specify which scheme(s)? default all schemes?
  - **clean**
  - doxygen
  - builds project
- **init**:
  - updates submodules
  - checks and installs neccessary packages
  - downloads data-set from strato server
  - prepares data-set according to _preparedata.py_
- **test**:
  - **compile**
  - checks and updates data in local directory according to downloaded one from **init**
  - runs executable
- **report**:
  - makes latex report
- **pack**:
  - **clean**
  - packs project
- **unpack**:
  - unpacks project
- **send**:
  - send packed project to specified host
- **all**:
  - tbd


### Running tests
#### Preconditions
This project assumes certain infrastructure to be available on the targets used for testing. First and foremost, that is make. To yield a working installation of the project, you have to options:
1) make the complete project available in the target location, log-in via ssh and execute the following commands from the top-level directory of the project: 
```bash
sudo apt install make
make setupToolchain
```
2) log-in via ssh and execute the following block (check utl/init.sh for latest version):
```bash
# setup routine
echo "> entering setup routine"
## apt packages, apapted from https://stackoverflow.com/questions/10608004/auto-install-packages-from-inside-makefile
echo ">> installing debian packages with apt"
### misc.
if ! dpkg -l | grep git -c >>/dev/null; then sudo apt-get install git; fi
if ! dpkg -l | grep make -c >>/dev/null; then sudo apt-get install make; fi
if ! dpkg -l | grep cmake -c >>/dev/null; then sudo apt-get install cmake; fi
if ! dpkg -l | grep doxygen -c >>/dev/null; then sudo apt-get install doxygen; fi
if ! dpkg -l | grep liblapack-dev -c >>/dev/null; then sudo apt-get install liblapack-dev; fi
if ! dpkg -l | grep liblapacke-dev -c >>/dev/null; then sudo apt-get install liblapacke-dev; fi
if ! dpkg -l | grep graphviz -c >>/dev/null; then sudo apt-get install graphviz; fi
if ! dpkg -l | grep python3-pip -c >>/dev/null; then sudo apt-get install python3-pip; fi
### openmpi
if ! dpkg -l | grep openmpi-bin -c >>/dev/null; then sudo apt-get install openmpi-bin; fi
if ! dpkg -l | grep openmpi-common -c >>/dev/null; then sudo apt-get install openmpi-common; fi
if ! dpkg -l | grep libopenmpi-dev -c >>/dev/null; then sudo apt-get install libopenmpi-dev; fi

## pip packages
echo ">> installing python packages with pip"
pip3 install pytest cffi numpy scipy matplotlib pandas seaborn
```
