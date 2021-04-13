#!/usr/bin/env bash

# file set_up_config_server_ssh.sh
# author Corentin DESTREZ & Valentin GUIBERTEAU
# date 24 March 2021
# brief save your current ssh server's configuration and set up the one usefull for the game
#
# This script will save you current ssh server's configuration in the OLD_SSH_SERVER_CONFIG folder.
# Then replace /etc/ssh/sshd_config with the ssh_config in the ssh_server_config folder and remove the /etc/ssh/sshd_config.d folder.
# Must be executed as root

# Check if your rooted
if [ $(whoami) != root ]
  then
    echo "Run this script as root"
    exit 1
fi


# Create folder if not exists and manage versions ?
OLD_PATH="OLD_SSH_SERVER_CONFIG"
SSH_CONFIG_PATH="ssh_server_config"

if [ ! -d $OLD_PATH ]
  then
    mkdir -p $OLD_PATH/version1
  else
    version_number=$(ls $OLD_PATH | tail -n1 | cut -d"_" -f2)
    let version_number++
    mkdir $OLD_PATH/version_$version_number
fi

# Save current config
if [ -e /etc/sshd_config ]
  then
    sudo mv /etc/ssh/sshd_config    $OLD_PATH/version_$version_number
fi
if [ -d /etc/sshd_config.d ]
  then
    sudo mv /etc/ssh/sshd_config.d  $OLD_PATH/version_$version_number
fi

# Replace current config with the game's one
sudo cp $SSH_CONFIG_PATH/sshd_config /etc/ssh/sshd_config
