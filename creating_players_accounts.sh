#!/usr/bin/env bash

# file creating_players_accounts.sh
# author Corentin DESTREZ & Valentin GUIBERTEAU
# date 16 Nov 2020
# brief Create player's accounts
#
# This script creates the user's account if they doesn't already exist.
# It also define some rules (uses of rbash, they home directory) and copy the differents exucutables and bash script usefull to run the client.
# Must be executed as root

# Check if your rooted
if [ $(whoami) != root ]
  then
    echo "Run this script as root"
    exit 1
fi

# Creating group
if !  grep -q "^player:" /etc/group
  then
    groupadd player
  fi

# Creating player's account if not exist & configuring their home

# Creating home directory if not exist
mkdir -p /home/player

id -u player &>/dev/null || useradd player -d /home/player -g player -s /bin/rbash
# User's shell will be rbash because the user must only be able to use the command to execute the client
echo "player:player" | chpasswd
# Configuring home
cp build/client /home/player

exit 0
