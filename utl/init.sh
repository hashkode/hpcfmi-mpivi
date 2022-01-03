#!/bin/bash
cd "$(dirname "$0")"
echo "> running init routine"

# prepare files 
home_path="/home/$(whoami)/data"

if [ -d "$home_path" ]
then
  echo ">> data directory already created" 
else
  mkdir $home_path
fi

scp -i sshkey hpcfmi@scp.hidrive.strato.com:hpcfmi/hpcmi-data.tar.gz $home_path

tar -xvf $home_path/hpcmi-data.tar.gz -C $home_path
mv $home_path/hpcmi-data/data_debug $home_path/data_debug
mv $home_path/hpcmi-data/data_small $home_path/data_small
mv $home_path/hpcmi-data/data_normal $home_path/data_normal
rm -r $home_path/hpcmi-data

if [ -d "$home_path/data_debug" ] 
then
  python3 preparedata.py -d $home_path/data_debug
  echo "debug dataset prepared"
else
  echo "no debug dataset available in $home_path"
fi

if [ -d "$home_path/data_small" ] 
then
  python3 preparedata.py -d $home_path/data_small
  echo "small dataset prepared"
else
  echo "no small dataset available in $home_path"
fi

if [ -d "$home_path/data_normal" ] 
then
  python3 preparedata.py -d $home_path/data_normal
  echo "normal dataset prepared"
else
  echo "no normal dataset available in $home_path"
fi

