# file makefile
# author Corentin DESTREZ & Valentin GUIBERTEAU
# date 09 Apr 2021
# brief Makefile to compile sources
#
# This makefile provides multiple sources in order to compile both client and server.


CC = gcc -Wall
BUILD_FOLDER := build
client_exec = $(BUILD_FOLDER)/client
server_exec = $(BUILD_FOLDER)/server
lib = -lpthread
custom_libs_game_ui = $(BUILD_FOLDER)/game_ui.o
custom_libs_server = $(BUILD_FOLDER)/server_kassbriik.o
custom_libs_global = $(BUILD_FOLDER)/kassbriik.o
exec = $(client_exec) $(server_exec)


all : $(exec)

server : server.c $(BUILD_FOLDER)/server_kassbriik.a $(BUILD_FOLDER)/kassbriik.a $(BUILD_FOLDER)
	$(CC) -o $(BUILD_FOLDER)/$@ $< $(word 2, $^) $(word 3, $^) $(lib)

client : client.c $(BUILD_FOLDER)/game_ui.a $(BUILD_FOLDER)/kassbriik.a $(BUILD_FOLDER)
	$(CC) -o $(BUILD_FOLDER)/$@ $< $(word 2, $^) $(word 3, $^) $(lib)
	chmod +s $@
	move_client

# Compile server only
$(server_exec) : server.c $(BUILD_FOLDER)/server_kassbriik.a $(BUILD_FOLDER)/kassbriik.a $(BUILD_FOLDER)
	$(CC) -o $@ $< $(word 2, $^) $(word 3, $^) $(lib)

# Compile client only
$(client_exec) : client.c $(BUILD_FOLDER)/game_ui.a $(BUILD_FOLDER)/kassbriik.a $(BUILD_FOLDER)
	$(CC) -o $@ $< $(word 2, $^) $(word 3, $^) $(lib)
	chmod +s $@
	sudo ./move_client.sh

# Server lib creation
$(BUILD_FOLDER)/server_kassbriik.a : $(custom_libs_server)
	ar rcs $@ $^

$(custom_libs_server) : server_kassbriik.c server_kassbriik.h $(BUILD_FOLDER)
	$(CC) -c -o $@ $<

# Client lib creation
$(BUILD_FOLDER)/kassbriik.a : $(custom_libs_global)
	ar rcs $@ $^

$(custom_libs_global) : kassbriik.c kassbriik.h $(BUILD_FOLDER)
	$(CC) -c -o $@ $<

$(BUILD_FOLDER)/game_ui.a : $(custom_libs_game_ui)
	ar rcs $@ $^

$(custom_libs_game_ui): game_ui.c $(BUILD_FOLDER)
	$(CC) -c -o $@ $<

$(BUILD_FOLDER):
	mkdir -p $@

move_client : $(BUILD_FOLDER)/client
	sudo ./move_client.sh

clean :
	rm $(BUILD_FOLDER)/*
