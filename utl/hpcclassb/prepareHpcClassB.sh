#!/bin/bash
cd "$(dirname "$0")"
_root=$1
_node1=$2
_node2=$3
_node3=$4
_node4=$5
_node5=$6
_node6=$7
_node7=$8
_node8=$9
_node9=${10}

cd ../../
make prepareTarget host=${_node1} runtype=init
make prepareTarget host=${_node2} runtype=init
make prepareTarget host=${_node3} runtype=init
make prepareTarget host=${_node4} runtype=init
make prepareTarget host=${_node5} runtype=init
make prepareTarget host=${_node6} runtype=init
make prepareTarget host=${_node7} runtype=init
make prepareTarget host=${_node8} runtype=init
make prepareTarget host=${_node9} runtype=init
make prepareTarget host=${_root} runtype=rebuild
