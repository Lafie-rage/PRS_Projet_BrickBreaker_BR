#!/usr/bin/env bash

# file client
# author Corentin DESTREZ & Valentin GUIBERTEAU
# date 16 Nov 2020
# brief Executable for the client
#
# This script move the client executable in each player's directory

for playerNb in {1..8}
do
  cp client /home/player$playerNb/
done

exit 0
