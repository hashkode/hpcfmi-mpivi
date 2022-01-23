#!/bin/bash
cd "$(dirname "$0")"
_root=$1
_maketarget=$2

_target_user=$(ssh ${_root} 'whoami')

_project_path="/home/${_target_user}/projects/hpcmi/mpi-vi/"

ssh ${_root} -o 'StrictHostKeyChecking no' "cd;cd ${_project_path};cd utl;make ${_maketarget}"
