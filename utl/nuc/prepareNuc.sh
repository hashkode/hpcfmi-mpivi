#!/bin/bash
cd "$(dirname "$0")"
_root=$1
_node1=$2

cd ../../
make _prepareTarget host=${_root} runtype=build & make _prepareTarget host=${_node1} runtype=build
