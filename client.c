/**
 * /file client.c
 * /author Corentin DESTREZ & Valentin GUIBERTEAU
 * /date 09 Apr 2021
 * /brief Source code of the client library
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


/**
 * \fn void initServerConnection(int *msgId, int *serverPid)
 * \brief Intialise the connection to the server
 *
 * Initialise the connection by retrieving the message queue that will be used to communicate
 * with the server. Then wait for the server's reply.
 * Display message to user during process.
 *
 * \param msgId Pointer where the file descriptor of the message queue will be stored.
 * \param serverPid Pointer where the server PID will be stored.
 * \param username The name of the player.
 */
void initServerConnection(int *msgId, int *serverPid, char *username);

/**
 * \fn void connectToServer(int msgId)
 * \brief Send the connection message to the server via message queue.
 *
 * Send the typic message to notify server that the client want to connect to the server.
 * This message is send via the message queue which the id is given in argument.
 * The message sent contents the username of the player.
 *
 * \param msgId Pointer where the file descriptor of the message queue to use to send the message.
 * \param username The name of the player.
 */
void connectToServer(int msgId, char *username);

/**
 * \fn void askServerToLaunchTheGame(int msgId)
 * \brief Wait for the server to launch the game, then start it.
 *
 * Await for the server's message to start the game, reply to him and then launch it.
 * Display message during process.
 *
 * \param msgId Pointer where the file descriptor of the message queue to use to send the message.
 */
void askServerToLaunchTheGame(int msgId);

int main(int argc, char *argv[]) {
  if(argc != 2) {
    fprintf(stderr, "--- ERROR : You cannot run this program this way. Please use the command client from your bash.\n");
    exit(-1);
  }
  int msgId, serverPid;
  char *username = argv[1];
  printf("Hello %s\n", username);
  initServerConnection(&msgId, &serverPid, username);
  askServerToLaunchTheGame(msgId);
  //
}

void initServerConnection(int *msgId, int *serverPid, char *username) {
  t_request request;
  CHECK(*msgId = msgget(MSG_KEY, MSG_MODE), "--- ERROR : Cannot open mail box");
  connectToServer(*msgId, username);
  puts("Waiting to connect to the serveur...");
  CHECK(msgrcv(*msgId, &request, sizeof(t_body), getpid(), 0), "--- ERROR : Cannot receive message from mail box");
  *serverPid = request.body.pid;
  printf("Server's pid : %d\n", *serverPid);
  if(strcmp(request.body.message, CONNECTION_NOTIFY_MSG)) {
    puts("Your are connected to the server !");
  }
}

void connectToServer(int msgId, char *username) {
  t_request request;
  puts("Connecting to the server...");
  request.type = CONNECTION_MSG_TYPE;
  request.body.pid = getpid();
  strcpy(request.body.message, username);
  CHECK(msgsnd(msgId, &request, sizeof(t_body), 0), "--- ERROR : Cannot connect to the server mail box");
}

void askServerToLaunchTheGame(int msgId) {
  t_request request;
  puts("Waiting for server to start the game...");
  CHECK(msgrcv(msgId, &request, sizeof(t_body), getpid(), 0), "--- ERROR : Cannot receive message from mail box");
  puts("Game will start now !");
  // TODO send response
  // TODO : Launch the UI Game
  // Send score
  // Await reponse
}
