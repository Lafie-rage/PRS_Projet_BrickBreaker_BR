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

/**
 * \fn void getMessageQueue(int *msgId)
 * \brief Create the message queue to communicate between the serveur and the client.
 *
 * Verify if the message queue isn't already created. If it is, delete it.
 * In any case, end by creating the message queue and store is id in msgId.
 *
 * \param msgId Pointer where the file descriptor of the message queue will be stored.
 *
 */
void getMessageQueue(int *msgId);

/**
 * \fn void retrieveClientsInformations(int msgId, t_clients_list *clients, int nbOfClientsToWait)
 * \brief Await for clients' message to get their pid and the player's username.
 *
 * Await for clients' message to get their pid and the player's username and store it
 * in the clients list send in parameters.
 *
 * \param msgId Pointer where the file descriptor of the message queue that must be used.
 * \param clients The list of clients where the client's informations will be stored.
 * \param nbOfClientsToWait The number of clients that the server will get informations.
 *
 */
void retrieveClientsInformations(int msgId, t_clients_list *clients, int nbOfClientsToWait);

/**
 * \fn void confirmConnectionToClient(int msgId, int type, const char *msg)
 * \brief Send a message to a client via the message queue to confirm connection.
 *
 * Send the message to a client which pid is send as argument.
 * The message is composed of the message as string to send, the pid of the destination client and the pid of the server.
 *
 * \param msgId Pointer where the file descriptor of the message queue that must be used.
 * \param clientPid The pid of the destination client.
 * \param buffer The string that will be send in the message.
 *
 */
void confirmConnectionToClient(int msgId, int clientPid, const char *buffer);

/**
 * \fn void notifyClients(int msgId, t_clients_list clients, const char *buffer)
 * \brief Send a message to multiple clients via the message queue.
 *
 * Send the message to a list of clients that is send as argument.
 * The message is composed of the message as string to send and the pid of the server.
 * Its type will be the pid of the client whose message is destinated.
 *
 * \param msgId Pointer where the file descriptor of the message queue that must be used.
 * \param clients The list of the clients that will be notified.
 * \param buffer The string that will be send in the message.
 *
 */
void notifyClients(int msgId, t_clients_list clients, const char *buffer);

/**
 * \fn void waitingClientsResponse(int msgId, t_clients_list clients)
 * \brief Await the response of all clients in the list.
 *
 * Await for each client to send a message of specific type before continuing.
 * Display messages during process.
 *
 * \param msgId Pointer where the file descriptor of the message queue that must be used.
 * \param clients The list of the clients that must respond.
 * \param msgType The type of message that client should send.
 *
 */
void waitingClientsResponse(int msgId, t_clients_list clients, int msgType);

/**
 * \fn void waitingPlayersScore(int msgId, t_clients_list *clients)
 * \brief Await the all clients in the list to send their score.
 *
 * \param msgId Pointer where the file descriptor of the message queue that must be used.
 * \param clients The list of the clients that must send their score.
 *
 */
void waitingPlayersScore(int msgId, t_clients_list *clients);

/**
 * \fn void createAndOpenShmAndSemaphore(int *shmId, struct shmid_ds *shmBuffer, t_scores_list **shmPtr, sem_t *semId)
 * \brief Create and open the shm and its semaphore.
 *
 * If the shm exists, delete it.
 * In any case create the shm and open it. Then set the shmBuffer with the shm's infos.
 * If the semaphore already exists, close it.
 * In any case create the semaphore and open it.
 *
 * \param shmId The id of the shm.
 * \param shmBuffer The buffer that must contains the shmid_ds struct of the shm.
 * \param shmPtr The pointer to the shm used to read and write through it.
 * \param semId The pointer to the semaphore.
 *
 */
void createAndOpenShmAndSemaphore(int *shmId, struct shmid_ds *shmBuffer, t_scores_list **shmPtr, sem_t *semId);

/**
 * \fn void writePlayersScore(int msgId, t_clients_list clients, int *shmId, struct shmid_ds *shmBuffer, sem_t *semId)
 * \brief Write all the scores in the shared memory.
 *
 * Write the scores of all the players in the shm the notify each client that the score have been written.
 *
 * \param msgId Pointer where the file descriptor of the message queue that must be used.
 * \param clients The list of the clients of which the score will be written.
 * \param shmId The id of the shm.
 * \param shmBuffer The buffer that must contains the shmid_ds struct of the shm..
 *
 */
void writePlayersScore(int msgId, t_clients_list clients, int *shmId, struct shmid_ds *shmBuffer);

int main(int argc, char *argv[]) {
  if(argc != 2 || atoi(argv[1]) < 1 || atoi(argv[1]) > MAX_CLIENTS) {
    fprintf(stderr, "--- ERROR : You cannot run this program without saying how many clients will play. Please do it following this format ./server <nbClients> with a number of client between 1 and %d\n", MAX_CLIENTS);
    exit(-1);
  }
  int msgId;
  int *shmId = malloc(sizeof(int));
  int nbOfClientsToWait = atoi(argv[1]);
  t_clients_list clients;
  char *buffer;
  struct shmid_ds shmBuffer;
  initClientList(&clients, nbOfClientsToWait);
  getMessageQueue(&msgId);
  retrieveClientsInformations(msgId, &clients, nbOfClientsToWait);

  // Notify clients that they must start the game
  notifyClients(msgId, clients, START_GAME_MSG);
  waitingClientsResponse(msgId, clients, GAME_STARTED_MSG_TYPE);

  waitingPlayersScore(msgId, &clients);
  writePlayersScore(msgId, clients, shmId, &shmBuffer);

  buffer = malloc((strlen(SCORE_WRITTEN_MSG) + 3)*sizeof(char)); // + 3 because message wille be SCORE_WRITTEN_MSG:<nbOfPlayerInOneDigit>\0
  sprintf(buffer, "%s:%d", SCORE_WRITTEN_MSG, clients.size);
  // Notify clients that they can read the scores
  notifyClients(msgId, clients, buffer);
  waitingClientsResponse(msgId, clients, SCORE_READ_MSG_TYPE);

  // Deleting semaphore, shm and message queue
  CHECK(sem_unlink(SEM_SHM_NAME), "--- ERROR : Cannot delete the semaphore.");
  CHECK(shmctl(*shmId, IPC_RMID, &shmBuffer), "--- ERROR : Cannot delete the shm.");
  CHECK(msgctl(msgId, IPC_RMID, NULL), "--- ERROR : Cannot delete the mail box between server and client.");
}

void retrieveClientsInformations(int msgId, t_clients_list *clients, int nbOfClientsToWait) {
  int i;
  t_request request;
  printf("Waiting for %d clients to be connected before starting...\n", nbOfClientsToWait);
  for(i = 0; i < nbOfClientsToWait; i++) {
    printf("Waiting client n° %d to connect\n", i+1);
    CHECK(msgrcv(msgId, &request, sizeof(t_body), CONNECTION_MSG_TYPE, 0), "--- ERROR : Cannot read message from mail box.");
    clients->clients[i].pid = request.body.pid;
    clients->clients[i].username = malloc(strlen(request.body.message) * sizeof(char));
    strcpy(clients->clients[i].username, request.body.message);
    printf("Client %d with pid %d connected which name is %s\n", i, clients->clients[i].pid, clients->clients[i].username);
    confirmConnectionToClient(msgId, clients->clients[i].pid, "");
  }
}

void getMessageQueue(int *msgId) {
  if((*msgId = msgget(MSG_KEY, MSG_MODE)) != -1)
    CHECK(msgctl(*msgId, IPC_RMID, NULL), "--- ERROR : Cannot delete last mail box.");
  CHECK(*msgId = msgget(MSG_KEY, MSG_MODE | IPC_CREAT | IPC_EXCL), "--- ERROR : Cannot create mail box.");
}

void confirmConnectionToClient(int msgId, int clientPid, const char *buffer) {
  t_request request;
  request.type = clientPid;
  request.body.pid = getpid();
  strcpy(request.body.message, buffer);
  CHECK(msgsnd(msgId, &request, sizeof(t_body), 0), "--- ERROR : Cannot send message to client.");
}


void notifyClients(int msgId, t_clients_list clients, const char *buffer) {
  int i;
  t_request request;
  puts("Notifying client...");
  request.body.pid = getpid();
  strcpy(request.body.message, buffer);
  for(i = 0; i < clients.size; i++) {
    request.type = clients.clients[i].pid;
    CHECK(msgsnd(msgId, &request, sizeof(t_body), 0), "--- ERROR : Cannot notify client via mail box.");
  }
}

void waitingClientsResponse(int msgId, t_clients_list clients, int msgType) {
  t_request request;
  t_clients_list cpyClients; // Copy the list to be sure that we will never modify its original values.
  copyClientsList(&cpyClients, clients);
  puts("Waiting for client to respond...");
  while(cpyClients.size != 0) {
    CHECK(msgrcv(msgId, &request, sizeof(t_body), msgType, 0), "--- ERROR : Cannot receive response from client.");
    printf("Client with pid %d has responded\n", request.body.pid);
    removeClientFromList(&cpyClients, request.body.pid);
  }
}

void waitingPlayersScore(int msgId, t_clients_list *clients) {
  t_request request;
  // Temporary list used to wait that all clients has responded
  t_clients_list tmpClients;
  copyClientsList(&tmpClients, *clients);
  t_client *clientThatSendScore = malloc(sizeof(t_client));
  puts("Waiting for client to send the score of the player...");
  while(tmpClients.size != 0) {
    CHECK(msgrcv(msgId, &request, sizeof(t_body), END_GAME_MSG_TYPE, 0), "--- ERROR : Cannot receive response from client.");
    clientThatSendScore = getClientByPid(*clients, request.body.pid);
    if(clientThatSendScore == NULL) {
      puts("ERROR CLIENT NOT FOUND");
    }
    clientThatSendScore->score = atoi(request.body.message);
    printf("Player %s has scored %d points.\n", clientThatSendScore->username, clientThatSendScore->score);
    removeClientFromList(&tmpClients, request.body.pid);
  }
}

void createAndOpenShmAndSemaphore(int *shmId, struct shmid_ds *shmBuffer, t_scores_list **shmPtr, sem_t *semId) {
  // Mode & init value will be ignore if it already exists
  if((semId = sem_open(SEM_SHM_NAME, O_CREAT, 0666, 1)) == SEM_FAILED) {
    perror("--- ERROR : Cannot open the semaphore");
    exit(-1);
  }
  if((*shmId = shmget(SHM_KEY, sizeof(t_scores_list), 0666 | IPC_CREAT)) == -1) {
    perror("--- ERROR : Cannot retrieve the shm.");
    exit(-1);
  }
  if((*shmPtr = (t_scores_list *)shmat(*shmId, NULL, 0)) == (void *)-1) {
    perror("--- ERROR : Cannot open the shm.");
    exit(-1);
  };
  CHECK(shmctl(*shmId, IPC_STAT, shmBuffer), "--- ERROR : Cannot retrieve shm's infos.");
}

void writePlayersScore(int msgId, t_clients_list clients, int *shmId, struct shmid_ds *shmBuffer) {
  int i;
  t_scores_list *shmPtr;
  sem_t semId;
  puts("Sending scores to clients...");
  // The shm should never be used before the server, but adding it make it safer
  createAndOpenShmAndSemaphore(shmId, shmBuffer, &shmPtr, &semId);
  // Iterates through the list to write the scores in the shm
  printf("List size : %d\n", clients.size);
  shmPtr->size = clients.size;
  for(i = 0; i < shmPtr->size; i++) {
    shmPtr->scores[i].score = clients.clients[i].score;
    strcpy(shmPtr->scores[i].username, clients.clients[i].username);
  }
  for(i = 0; i < shmPtr->size; i++) {
    printf("Client %d\n", i+1);
    printf("\tscore %d\n", shmPtr->scores[i].score);
    printf("\tusername %s\n", shmPtr->scores[i].username);
  }
  CHECK(shmdt(shmPtr), "--- ERROR : Cannot release the shm for the clients.");
  sem_close(&semId);
}
