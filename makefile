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
lib = -lpthread
custom_libs_server = $(addprefix $(BUILD_FOLDER)/, server_kassbriik.o)
custom_libs_global = $(addprefix $(BUILD_FOLDER)/, kassbriik.o)
exec = $(client_exec) $(server_exec)


all : $(addprefix $(BUILD_FOLDER)/, $(exec))

server : server.c $(BUILD_FOLDER)/server_kassbriik.a $(BUILD_FOLDER)/kassbriik.a
	$(CC) -o $(BUILD_FOLDER)/$@ $^ $(lib)

client : client.c $(BUILD_FOLDER)/kassbriik.a
	$(CC) -o $(BUILD_FOLDER)/$@ $^ $(lib)
	chmod +s $@

# Compile server only
$(BUILD_FOLDER)/server : server.c $(BUILD_FOLDER)/server_kassbriik.a $(BUILD_FOLDER)/kassbriik.a
	$(CC) -o $@ $^ $(lib)

# Compile client only
$(BUILD_FOLDER)/client : client.c $(BUILD_FOLDER)/kassbriik.a
	$(CC) -o $@ $^ $(lib)
	chmod +s $@

# Refresh client executable and move it to the player's homes
#sudo ./move_client.sh

# Server lib creation
$(BUILD_FOLDER)/server_kassbriik.a : $(custom_libs_server)
	ar rcs $@ $^

$(BUILD_FOLDER)/server_kassbriik.o : server_kassbriik.c server_kassbriik.h
	$(CC) -c -o $@ $<

# Client lib creation
$(BUILD_FOLDER)/kassbriik.a : $(custom_libs_global)
	ar rcs $@ $^

$(BUILD_FOLDER)/kassbriik.o : kassbriik.c kassbriik.h
	$(CC) -c -o $@ $<


move_client : $(BUILD_FOLDER)/client
	sudo ./move_client.sh

clean :
	rm $(BUILD_FOLDER)/*
