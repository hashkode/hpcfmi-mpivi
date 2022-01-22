#!/bin/bash
cd "$(dirname "$0")"

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
pip3 install numpy scipy matplotlib pandas seaborn

## get host keys, not needed currently due to disabled host-key check
#TODO: check security impact of disabled host-key check
#./getHostKeys.sh
