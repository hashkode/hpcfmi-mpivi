#!/bin/bash
cd "$(dirname "$0")"
_root=$1
_node2=$2
_node3=$3
_node4=$4

cd ../../
make prepareTarget host=${_root} runtype=rebuild & make prepareTarget host=${_node1} runtype=rebuild
