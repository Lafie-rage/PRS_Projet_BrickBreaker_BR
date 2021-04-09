# file makefile
# author Corentin DESTREZ & Valentin GUIBERTEAU
# date 09 Apr 2021
# brief Makefile to compile sources
#
# This makefile provides multiple sources in order to compile both client and server.


CC = gcc -Wall
BUILD_FOLDER := build
client_exec = client
server_exec = server
custom_libs_server = $(addprefix $(BUILD_FOLDER)/, server_kassbriik.o)
exec = $(client_exec) $(server_exec)


all : $(addprefix $(BUILD_FOLDER)/, $(exec))

client : $(BUILD_FOLDER)/client

server : $(BUILD_FOLDER)/server

# Compile server only
$(BUILD_FOLDER)/server : server.c $(BUILD_FOLDER)/server_kassbriik.a
	$(CC) -o $@ $^

# Compile client only
$(BUILD_FOLDER)/client : client.c
	$(CC) -o $@ $^

# Refresh client executable and move it to the player's homes
#sudo ./move_client.sh

$(BUILD_FOLDER)/server_kassbriik.a : $(custom_libs_server)
	ar rcs $@ $^

$(BUILD_FOLDER)/server_kassbriik.o : server_kassbriik.c server_kassbriik.h
	$(CC) -c -o $@ $<

move_client : $(BUILD_FOLDER)/client
	sudo ./move_client.sh

clean :
	rm $(BUILD_FOLDER)/*
