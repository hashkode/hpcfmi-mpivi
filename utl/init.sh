#!/bin/bash
cd "$(dirname "$0")"

# init routine
echo "> running init routine"

# prepare files 
home_path="/home/$(whoami)/data"

if [ -d "$home_path" ]
then
  echo ">> data directory already created" 
  if [ -d "$home_path/data_debug" ]
  then
    rm -rf "$home_path/data_debug" ]
  fi
  if [ -d "$home_path/data_small" ]
  then
    rm -rf "$home_path/data_small"
  fi
  if [ -d "$home_path/data_normal" ]
  then
    rm -rf "$home_path/data_normal"
  fi
else
  mkdir $home_path
fi
# permission adjustment for sshkey
chmod 400 sshkey

# download data
echo ">> fetch data set"
rsync -e "ssh -i sshkey" hpcfmi@rsync.hidrive.strato.com:hpcfmi/hpcmi-data.tar.gz $home_path -azrc -q

# extract data
tar -xvf $home_path/hpcmi-data.tar.gz -C $home_path >/dev/null
mv $home_path/hpcmi-data/data_debug $home_path
mv $home_path/hpcmi-data/data_small $home_path
mv $home_path/hpcmi-data/data_normal $home_path
rm -r $home_path/hpcmi-data

# prepare data, i.e. convert parameters in txt-format for c++ handling
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

