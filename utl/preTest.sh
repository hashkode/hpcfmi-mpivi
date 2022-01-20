#!/bin/bash
cd "$(dirname "$0")"

# home_path is the directory with given data, var_path is the directory where the program will later look for data
#_target_user=$(ssh ${_host} "whoami")
#echo ">> _target_user: "${_target_user}

home_path="/home/$(whoami)/data/"
var_path="/var/tmp/$(whoami)/data"
results_path="/var/tmp/$(whoami)/results"

### sync results directory from server
echo ">> fetch results"
mkdir -p ${results_path}
rsync -e "ssh -i sshkey" hpcfmi@rsync.hidrive.strato.com:users/hpcfmi/results/ $results_path -azr -q

### sync data set from home drive to tmp
echo ">> sync data set to /var/tmp/"
mkdir -p ${var_path}
rsync -azrq ${home_path} ${var_path} --delete
