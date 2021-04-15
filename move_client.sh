#!/usr/bin/env bash

# file client
# author Corentin DESTREZ & Valentin GUIBERTEAU
# date 16 Nov 2020
# brief Executable for the client
#
# This script move the client executable in the player's directory
# Must be executed as root

# Check if your rooted
if [ $(whoami) != root ]
  then
    echo "Run this script as root"
    exit 1
fi

cp build/client /bin/client
chmod +r /bin/client # remove the restictired mode on this command

exit 0
