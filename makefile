# file makefile
# author Corentin DESTREZ & Valentin GUIBERTEAU
# date 16 Nov 2020
# brief Makefile to compile sources
#
# This makefile provides multiple sources in order to compile both client and server.


CC = gcc

all : move_client

# Compile client only
client : client.c
	$(CC) -o $@ $^

# Refresh client executable and move it to the player's homes
move_client : client
		sudo ./move_client.sh
