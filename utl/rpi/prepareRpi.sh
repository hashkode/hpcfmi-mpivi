#!/bin/bash
cd "$(dirname "$0")"
_root=$1
_node1=$2
_node2=$2
_node3=$3

cd ../
make _prepareTarget host=${_root} runtype=init & make _prepareTarget host=${_node1} runtype=init & make _prepareTarget host=${_node2} runtype=init & make _prepareTarget host=${_node3} runtype=init
make _prepareTarget host=${_root} runtype=rebuild & make _prepareTarget host=${_node1} runtype=rebuild & make _prepareTarget host=${_node2} runtype=rebuild & make _prepareTarget host=${_node3} runtype=rebuild
