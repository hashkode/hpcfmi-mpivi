#!/bin/bash
cd "$(dirname "$0")"
_host=$1
_runtype=$2

# inform about target host and active git user-email
_git_user_email=$(git config --get user.email)
echo -e "Preparing with <"${_runtype}"> on host <"${_host}"> with git user-email <"${_git_user_email}">"

# local paths
local_var_path="/var/tmp/$(whoami)/data/"

# target paths
target_user=$(ssh ${_host} 'whoami')
target_var_path="/var/tmp/${target_user}/data/"
target_project_path="/home/${target_user}/projects/hpcmi/mpi-vi/"

# run build on target
if [ ${_runtype} == build ]; then
  # synchronize project files
  ssh ${_host} "cd;mkdir -p ${target_project_path}"
  rsync -e "ssh -o 'StrictHostKeyChecking no'" ../ ${_host}:${target_project_path} -azrc -q --exclude=.idea --delete

  ssh ${_host} -o 'StrictHostKeyChecking no' "cd;cd ${target_project_path};make build"
fi

# run rebuild on target
if [ ${_runtype} == rebuild ]; then
  # synchronize project files
  ssh ${_host} "cd;mkdir -p ${target_project_path}"
  rsync -e "ssh -o 'StrictHostKeyChecking no'" ../ ${_host}:${target_project_path} -azr -q --exclude=.idea --delete

  ssh ${_host} -o 'StrictHostKeyChecking no' "cd;cd ${target_project_path};make rebuild"
fi

# run init on target
if [ ${_runtype} == init ]; then
  # synchronize data set
  ssh ${_host} -o 'StrictHostKeyChecking no' "mkdir -p ${target_var_path}"
  rsync -e "ssh -o 'StrictHostKeyChecking no'" ${local_var_path} ${_host}:${target_var_path} -azr -q --delete --exclude=hpcmi-data.tar.gz
fi
