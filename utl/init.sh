#!/bin/bash
# todo download data set into home directory 

cd "$(dirname "$0")"

if [ -d "../data/data_debug" ] 
then
  python3 preparedata.py -d ../data/data_debug
else
  echo "no debug dataset available"
fi

if [ -d "../data/data_small" ] 
then
  python3 preparedata.py -d ../data/data_small
else
  echo "no small dataset available"
fi

if [ -d "../data/data_normal" ] 
then
  python3 preparedata.py -d ../data/data_normal
else
  echo "no normal dataset available"
fi
