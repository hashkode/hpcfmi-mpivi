#!/bin/bash
cd "$(dirname "$0")"
#### Install the complete host setup required for project development, testing and documentation
### Install target toolchain as base
./setupToolchain

### Install extra packages for host setup
echo "> entering setup routine"
## apt packages, apapted from https://stackoverflow.com/questions/10608004/auto-install-packages-from-inside-makefile
echo ">> installing debian packages with apt"
# misc.
if ! dpkg -l | grep doxygen -c >>/dev/null; then sudo apt-get install doxygen; fi
if ! dpkg -l | grep graphviz -c >>/dev/null; then sudo apt-get install graphviz; fi
if ! dpkg -l | grep texlive-full -c >>/dev/null; then sudo apt-get install texlive-full; fi

## pip packages
echo ">> installing python packages with pip"
pip3 install matplotlib pandas seaborn

#TODO: remove, binaries now shipped with project
## PlantUML
#wget http://sourceforge.net/projects/plantuml/files/plantuml.jar/download -O plantuml.jar
#sudo mv plantuml.jar /usr/bin
