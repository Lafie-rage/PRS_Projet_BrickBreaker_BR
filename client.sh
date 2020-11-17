#!/usr/bin/env bash

# file client
# author Corentin DESTREZ & Valentin GUIBERTEAU
# date 16 Nov 2020
# brief Executable for the client
#
# This script launch the client with the correct arguments provided
# It must be placed in /bin

username=$(whoami)
pid=$(ps aux | grep -w './server$' | awk '{print $2}')

/home/$username/client $username $pid

exit 0
