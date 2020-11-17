#!/usr/bin/env bash

# file update_client.sh
# author Corentin DESTREZ & Valentin GUIBERTEAU
# date 16 Nov 2020
# brief Update client bash script
#
# This script update the bash script running the client.
# It puts the client.sh content in the /bin/client file.

touch /bin/client
cat client.sh > /bin/client

exit 0
