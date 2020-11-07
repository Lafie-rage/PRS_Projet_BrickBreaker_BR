CC = gcc

all : refresh_clients

client : client.c
	$(CC) -o $@ $^

refresh_clients : client refresh_clients.sh
	./refresh_clients.sh
