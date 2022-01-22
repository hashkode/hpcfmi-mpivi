#!/bin/bash
cd "$(dirname "$0")"

### init routine
echo "> running init routine"

# local paths
home_path="/home/$(whoami)/data/"
var_path="/var/tmp/$(whoami)/data/"

# prepare data set directory
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
rsync -e "ssh -i sshkey -o 'StrictHostKeyChecking no'" hpcfmi@rsync.hidrive.strato.com:hpcfmi/hpcmi-data.tar.gz $home_path -azrc -q

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

### sync data set from home drive to tmp
echo ">> sync data set to /var/tmp/"
mkdir -p ${var_path}
rsync -azrq ${home_path} ${var_path} --delete --exclude=hpcmi-data.tar.gz
