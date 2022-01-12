#!/bin/bash
cd "$(dirname "$0")"
# setup routine
echo "> entering setup routine"
## apt packages, apapted from https://stackoverflow.com/questions/10608004/auto-install-packages-from-inside-makefile
echo ">> installing debian packages with apt"
### misc.
if ! dpkg -l | grep git -c >>/dev/null; then sudo apt-get install git; fi
if ! dpkg -l | grep cmake -c >>/dev/null; then sudo apt-get install cmake; fi
if ! dpkg -l | grep doxygen -c >>/dev/null; then sudo apt-get install doxygen; fi
if ! dpkg -l | grep liblapack-dev -c >>/dev/null; then sudo apt-get install liblapack-dev; fi
if ! dpkg -l | grep liblapacke-dev -c >>/dev/null; then sudo apt-get install liblapacke-dev; fi
if ! dpkg -l | grep graphviz -c >>/dev/null; then sudo apt-get install graphviz; fi
### openmpi
if ! dpkg -l | grep openmpi-bin -c >>/dev/null; then sudo apt-get install openmpi-bin; fi
if ! dpkg -l | grep openmpi-common -c >>/dev/null; then sudo apt-get install openmpi-common; fi
if ! dpkg -l | grep libopenmpi-dev -c >>/dev/null; then sudo apt-get install libopenmpi-dev; fi

## pip packages
echo ">> installing python packages with pip"
pip install pytest cffi numpy scipy matplotlib

# init routine
echo "> running init routine"

# prepare files 
home_path="/home/$(whoami)/data"

if [ -d "$home_path" ]
then
  echo ">> data directory already created" 
else
  mkdir $home_path
fi
# permission adjustment for sshkey
chmod 400 sshkey
# download data
scp -i sshkey hpcfmi@scp.hidrive.strato.com:hpcfmi/hpcmi-data.tar.gz $home_path
# extract data
tar -xvf $home_path/hpcmi-data.tar.gz -C $home_path
mv $home_path/hpcmi-data/data_debug $home_path/data_debug
mv $home_path/hpcmi-data/data_small $home_path/data_small
mv $home_path/hpcmi-data/data_normal $home_path/data_normal
rm -r $home_path/hpcmi-data

# prepare data, i.e. convert parameters in txt-format for c++ handling
if [ -d "$home_path/data_debug" ] 
then
  python3 preparedata.py -d $home_path/data_debug
  echo "debug dataset prepared"
else
  echo "no debug dataset available in $home_path"
fi

if [ -d "$home_path/data_small" ] 
then
  python3 preparedata.py -d $home_path/data_small
  echo "small dataset prepared"
else
  echo "no small dataset available in $home_path"
fi

if [ -d "$home_path/data_normal" ] 
then
  python3 preparedata.py -d $home_path/data_normal
  echo "normal dataset prepared"
else
  echo "no normal dataset available in $home_path"
fi

