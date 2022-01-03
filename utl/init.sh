#!/bin/bash
# todo download data set into home directory 

# prepare files 
home_path="/home/$(whoami)/data/"
#var_path="/var/tmp/$(whoami)/data"

cd "$(dirname "$0")"

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

