#!/bin/bash
cd "$(dirname "$0")"
_root=$1
_node1=$2
_node2=$2
_node3=$2

cd ../../
make prepareTarget host=${_node1} runtype=init
make prepareTarget host=${_node2} runtype=init
make prepareTarget host=${_node3} runtype=init
make prepareTarget host=${_root} runtype=rebuild