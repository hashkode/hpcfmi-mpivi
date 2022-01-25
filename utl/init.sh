#!/bin/bash
cd "$(dirname "$0")"

### init routine
echo "> running init routine"

# local paths
var_path="/var/tmp/$(whoami)/data/"

# prepare data set directory
if [ -d "$var_path" ]
then
  echo ">> data directory already created" 
  if [ -d "$var_path/data_debug" ]
  then
    rm -rf "$var_path/data_debug" ]
  fi
  if [ -d "$var_path/data_small" ]
  then
    rm -rf "$var_path/data_small"
  fi
  if [ -d "$var_path/data_normal" ]
  then
    rm -rf "$var_path/data_normal"
  fi
else
  mkdir $var_path
fi

# permission adjustment for sshkey
chmod 400 sshkey

# download data
echo ">> fetch data set"
mkdir -p ${var_path}
rsync -e "ssh -i sshkey -o 'StrictHostKeyChecking no'" hpcfmi@rsync.hidrive.strato.com:hpcfmi/hpcmi-data.tar.gz $var_path -azrc -q

# extract data
tar -xvf $var_path/hpcmi-data.tar.gz -C $var_path >/dev/null
mv $var_path/hpcmi-data/data_debug $var_path
mv $var_path/hpcmi-data/data_small $var_path
mv $var_path/hpcmi-data/data_normal $var_path
rm -r $var_path/hpcmi-data

# prepare data, i.e. convert parameters in txt-format for c++ handling
if [ -d "$var_path/data_debug" ]
then
  python3 preparedata.py -d $var_path/data_debug
  echo "debug dataset prepared"
else
  echo "no debug dataset available in $var_path"
fi

if [ -d "$var_path/data_small" ]
then
  python3 preparedata.py -d $var_path/data_small
  echo "small dataset prepared"
else
  echo "no small dataset available in $var_path"
fi

if [ -d "$var_path/data_normal" ]
then
  python3 preparedata.py -d $var_path/data_normal
  echo "normal dataset prepared"
else
  echo "no normal dataset available in $var_path"
fi

# git confifuration
git config core.attributesfile ~/.gitattributes
