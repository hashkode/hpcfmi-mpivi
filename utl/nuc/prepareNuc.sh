#!/bin/bash
cd "$(dirname "$0")"
_root=$1
_node1=$2

cd ../../
make prepareTarget host=${_root} runtype=rebuild & make prepareTarget host=${_node1} runtype=rebuild