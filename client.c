/**
 * @file client.c
 * @author Corentin DESTREZ & Valentin GUIBERTEAU
 * @date 09 Apr 2021
 * @brief Source code of the client library
 *
 * This file containts the functions defining the behavior of the clients.
 * It also define its behavior depending on the messages recieved.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
/*****************************************************************************/
/*                              CUSTOM LIBRARIES                             */
/*****************************************************************************/
#include "kassbriik.h"

#define CHECK(sts, msg) if ((sts) == -1) { perror(msg); exit(-1);}

void connectToServer(int msgId);

int main(int argc, char *argv[]) {
  if(argc != 2) {
    fprintf(stderr, "--- ERROR : You cannot run this program this way. Please use the command client from your bash.\n");
    exit(-1);
  }
  int msgId, serverPid;
  t_request request;
  printf("Hello World vous êtes : %s\n", argv[1]);
  CHECK(msgId = msgget(MSG_KEY, MSG_MODE), "--- ERROR : Cannot open mail box");
  connectToServer(msgId);
  puts("Waiting to connect to the serveur...");
  CHECK(msgrcv(msgId, &request, sizeof(t_body), getpid(), 0), "--- ERROR : Cannot receive message from mail box");
  serverPid = request.body.pid;
  printf("Server's pid : %d\n", serverPid);
  if(strcmp(request.body.msg, CONNECTION_NOTIFY_MSG)) {
    puts("Your are connected to the server !");
  }
  puts("Waiting for server to start the game...");
  CHECK(msgrcv(msgId, &request, sizeof(t_body), getpid(), 0), "--- ERROR : Cannot receive message from mail box");
  puts("Game will start now !");
}

void connectToServer(int msgId) {
  t_request request;
  puts("Connecting to the server...");
  request.type = CONNECTION_MSG_TYPE;
  request.body.pid = getpid();
  CHECK(msgsnd(msgId, &request, sizeof(t_body), 0), "--- ERROR : Cannot connect to the server mail box");
}
