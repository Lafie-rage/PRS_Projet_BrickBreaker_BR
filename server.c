/**
 * \file server.c
 * \author Corentin DESTREZ & Valentin GUIBERTEAU
 * \version 1.0
 * \date 09 Apr 2021
 * \brief Source code of the client library
 *
 * This file containts the functions defining the behavior of the server.
 * It also define its behavior depending on the messages recieved.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/*****************************************************************************/
/*                              CUSTOM LIBRARIES                             */
/*****************************************************************************/
#include "kassbriik.h"
#include "server_kassbriik.h"

#define CHECK(sts, msg) if ((sts) == -1) { perror(msg); exit(-1);}

/**
 * \fn void getMessageQueue(int *msgId)
 * \brief Create the message queue to communicate between the serveur and the client.
 *
 * Verify if the message queue isn't already created. If it is, delete it.
 * In any case, end by creating the message queue and store is id in msgId.
 *
 * \param msgId Pointer where the file descriptor of the message queue will be stored.
 */
void getMessageQueue(int *msgId);

/**
 * \fn void sendMessageToClient(int msgId, int type, const char *msg)
 * \brief Send a message to a client via the message queue.
 *
 * Send the message to a client which pid is send as argument.
 * The message is composed of the message as string to send, the pid of the destination client and the pid of the server.
 *
 * \param msgId Pointer where the file descriptor of the message queue that must be used.
 * \param clientPid The pid of the destination client.
 * \param buffer The string that will be send in the message.
 */
void sendMessageToClient(int msgId, int clientPid, const char *buffer);

/**
 * \fn void sendMessageToClient(int msgId, int type, const char *msg)
 * \brief Send a message to multiple clients via the message queue.
 *
 * Send the message to a list of clients that is send as argument.
 * The message is composed of the message as string to send and the pid of the server.
 * Its type will be the pid of the client whose message is destinated.
 *
 * \param msgId Pointer where the file descriptor of the message queue that must be used.
 * \param clients The list of the clients that will be notified.
 * \param buffer The string that will be send in the message.
 */
void notifyClients(int msgId, t_clients_list clients, const char *buffer);

int main(int argc, char *argv[]) {
  if(argc != 2 || atoi(argv[1]) < 1) {
    fprintf(stderr, "--- ERROR : You cannot run this program without saying how many clients will play. Please do it following this format ./server <nbClients> with a number of client greater than 0\n");
    exit(-1);
  }
  int msgId, i;
  int nbOfClientsToWait = atoi(argv[1]);
  t_clients_list clients;
  t_request request;
  initClientList(&clients, nbOfClientsToWait);

  printf("Waiting for %d clients to be connected before starting...\n", nbOfClientsToWait);

  getMessageQueue(&msgId);
  for(i = 0; i < nbOfClientsToWait; i++) {
    printf("Waiting client n° %d to connect\n", i);
    CHECK(msgrcv(msgId, &request, sizeof(t_body), CONNECTION_MSG_TYPE, 0), "--- ERROR : Cannot read message from mail box");
    clients.clients[i] = request.body.pid;
    printf("Client %d connected\n", clients.clients[i]);
    sendMessageToClient(msgId, clients.clients[i], "");
  }
  puts("Notifying client that they must start");
  notifyClients(msgId, clients, "");

  CHECK(msgctl(msgId, IPC_RMID, NULL), "--- ERROR : Cannot delete the mail box between server and client.");
}

void getMessageQueue(int *msgId) {
  if((*msgId = msgget(MSG_KEY, MSG_MODE)) != -1)
    CHECK(msgctl(*msgId, IPC_RMID, NULL), "--- ERROR : Cannot delete last mail box... Please use ipcs -q to find it and delete it with ipcrm msg <msqid>");
  CHECK(*msgId = msgget(MSG_KEY, MSG_MODE | IPC_CREAT | IPC_EXCL), "--- ERROR : Cannot create mail box");
}

void sendMessageToClient(int msgId, int clientPid, const char *buffer) {
  t_request request;
  request.type = clientPid;
  request.body.pid = getpid();
  strcpy(request.body.msg, buffer);
  CHECK(msgsnd(msgId, &request, sizeof(t_body), 0), "--- ERROR : Cannot send message to client");
}


void notifyClients(int msgId, t_clients_list clients, const char *buffer) {
  int i;
  t_request notify;
  notify.body.pid = getpid();
  strcpy(notify.body.msg, buffer);
  for(i = 0; i < clients.size; i++) {
    printf("i : %d\tclient pid : %d\n", i, clients.clients[i]);
    notify.type = clients.clients[i];
    CHECK(msgsnd(msgId, &notify, sizeof(t_body), 0), "--- ERROR : Cannot notify client via mail box");
  }
}
