#!/bin/bash
cd "$(dirname "$0")"
_root=$1
_nruns=$2
_nproca=$3
_nprocb=$4
_maketarget=$5

_target_user=$(ssh ${_root} 'whoami')

_project_path="/home/${_target_user}/projects/hpcmi/mpi-vi/"

ssh ${_root} "cd;cd ${_project_path};make ${_maketarget} nruns=${_nruns} nproca=${_nproca} nprocb=${_nprocb}"
