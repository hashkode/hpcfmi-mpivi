#!/bin/bash
cd "$(dirname "$0")"

python3 preparedata.py -d ../data/data_debug
python3 preparedata.py -d ../data/data_small
python3 preparedata.py -d ../data/data_normal