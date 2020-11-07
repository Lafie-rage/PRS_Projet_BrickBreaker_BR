#!/usr/bin/env bash

if [ $(whoami) != root ]
  then
    echo "Run this script as root"
    exit 1
fi

# Creating players account if not exists & configuring their home
for playerNb in {1..8}
  do
    id -u player$playerNb &>/dev/null || useradd player$playerNb -d /home/player$playerNb
    # Configuring home
    chmod 117 /home/player$playerNb
    cp client /home/player$playerNb
    rm -rf /home/player$playerNb/examples.desktop
  done

exit 0
