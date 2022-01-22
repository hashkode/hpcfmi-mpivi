#!/bin/bash
cd "$(dirname "$0")"
_root=$1
_node1=$2
_node2=$3
_node3=$4

cd ../
make _prepareTarget host=${_node1} runtype=init
make _prepareTarget host=${_node2} runtype=init
make _prepareTarget host=${_node3} runtype=init
make _prepareTarget host=${_root} runtype=rebuild
