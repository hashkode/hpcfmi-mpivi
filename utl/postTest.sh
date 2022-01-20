#!/bin/bash
cd "$(dirname "$0")"

results_path="/var/tmp/$(whoami)/results"

### sync results directory to server
rsync -e "ssh -i sshkey" $results_path hpcfmi@rsync.hidrive.strato.com:users/hpcfmi -azrc -q
