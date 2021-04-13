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
#include "game_ui.h"


/**
 * \fn void initServerConnection(int *msgId, int *serverPid)
 * \brief Intialise the connection to the server.
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
 * \param msgId Pointer where their is the file descriptor of the message queue to use to send the message.
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
 * \param msgId Pointer where their is the file descriptor of the message queue to use to send the message.
 */
void askServerToLaunchTheGame(int msgId);

/**
 * \fn void notifyServerThatGameHasStarted(int msgId)
 * \brief Notify the server that the client has started the game.
 *
 * Notify the server that the client has started the game via message queue.
 * Display message during process.
 *
 * \param msgId Pointer where their is the file descriptor of the message queue to use to send the message.
 */
void notifyServerThatGameHasStarted(int msgId);

/**
 * \fn void sendScore(int msgId, int score)
 * \brief Send the score of the player to the server.
 *
 * Send the score of the player to the server via message queue.
 * Display message during process.
 *
 * \param msgId Pointer where their is the file descriptor of the message queue to use to send the message.
 * \param score The score of the player.
 */
void sendScore(int msgId, int score);

/**
 * \fn void openShmAndSemaphore(int *shmId, struct shmid_ds *shmBuffer, t_scores_list **shmPtr, sem_t *semId)
 * \brief Open the shm and its semaphore.
 *
 * Send the score of the player to the server via message queue.
 * Display message during process.
 *
 * \param msgId Pointer where their is the file descriptor of the message queue to use to send the message.
 * \param score The score of the player.
 * \param shmPtr The pointer to the shm.
 * \param semId The semaphore's id.
 */
void openShmAndSemaphore(int *shmId, struct shmid_ds *shmBuffer, t_scores_list **shmPtr, sem_t *semId);

/**
 * \fn void retrieveScores(int smhId, t_scores_list *scores)
 * \brief Retrieve the score of each players from the server.
 *
 * Retieve the socre of each player from the server via message queue.
 * Display message during process.
 *
 * \param msgId Pointer where their is the file descriptor of the message queue to use to send the message.
 * \param scores The list of scores.
 */
void retrieveScores(int msgId, t_scores_list *scores);

/**
 * \fn void printFinalRank(int msgId, t_scores_list scores)
 * \brief Print the scores.
 *
 * Print the scores to the player and say who is the winner.
 *
 - \param msgId Pointer where their is the file descriptor of the message queue to use to send the message.
 * \param scores The list of scores of each players.
 *
 */
void printFinalRank(int msgId, t_scores_list scores);

int main(int argc, char *argv[]) {
  if(argc != 2) {
    fprintf(stderr, "--- ERROR : You cannot run this program this way. Please use the command client from your bash.\n");
    exit(-1);
  }
  int msgId, serverPid;
  char *username = argv[1];
  t_scores_list scores;
  printf("Hello %s\n", username);
  initServerConnection(&msgId, &serverPid, username);
  askServerToLaunchTheGame(msgId);
  notifyServerThatGameHasStarted(msgId);
  puts("Game will start now !");
  int score = startGameUI();
  sendScore(msgId, score);
  retrieveScores(msgId, &scores);
  printFinalRank(msgId, scores);

}

void initServerConnection(int *msgId, int *serverPid, char *username) {
  t_request request;
  CHECK(*msgId = msgget(MSG_KEY, MSG_MODE), "--- ERROR : Cannot open mail box.");
  connectToServer(*msgId, username);
  puts("Waiting to connect to the serveur...");
  CHECK(msgrcv(*msgId, &request, sizeof(t_body), getpid(), 0), "--- ERROR : Cannot receive message from mail box.");
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
  CHECK(msgsnd(msgId, &request, sizeof(t_body), 0), "--- ERROR : Cannot connect to the server mail box.");
}

void askServerToLaunchTheGame(int msgId) {
  t_request request;
  puts("Waiting for server to start the game...");
  do {
    CHECK(msgrcv(msgId, &request, sizeof(t_body), getpid(), 0), "--- ERROR : Cannot receive message from mail box.");
  } while (strcmp(request.body.message, START_GAME_MSG));
}

void notifyServerThatGameHasStarted(int msgId) {
  t_request request;
  request.type = GAME_STARTED_MSG_TYPE;
  request.body.pid = getpid();
  strcpy(request.body.message, "");
  puts("Notifying the server that the game has started...");
  CHECK(msgsnd(msgId, &request, sizeof(t_body), 0), "--- ERROR : Cannot notify the server that the game as started.");
}

void sendScore(int msgId, int score) {
  t_request request;
  char scoreAsString[5];
  // converting score as string
  sprintf(scoreAsString, "%d", score);
  printf("Your score is : %d.\nWell played !\n", score);
  printf("Your score is : %s.\nWell played !\n", scoreAsString);
  request.type = END_GAME_MSG_TYPE;
  request.body.pid = getpid();
  strcpy(request.body.message, scoreAsString);
  puts("Sending score to the server...");
  CHECK(msgsnd(msgId, &request, sizeof(t_body), 0), "--- ERROR : Cannot notify the server that the game as started.");
}

void openShmAndSemaphore(int *shmId, struct shmid_ds *shmBuffer, t_scores_list **shmPtr, sem_t *semId) {
  if((semId = sem_open(SEM_SHM_NAME, 0)) == SEM_FAILED) {
    perror("ERROR : Cannot open the semaphore.");
    exit(-1);
  }
  CHECK(*shmId = shmget(SHM_KEY, sizeof(t_scores_list), 0666), "ERROR : Cannot retrieve the shm.");
  if((*shmPtr = (t_scores_list *)shmat(*shmId, NULL, 0)) == (void *)-1) {
    perror("--- ERROR : Cannot open the shm");
    exit(-1);
  }
  CHECK(shmctl(*shmId, IPC_STAT, shmBuffer), "--- ERROR : Cannot retrieve shm's infos");
}

void retrieveScores(int msgId, t_scores_list *scores) {
  t_request request;
  int nbOfPlayers,
      shmId;
  sem_t semId;
  struct shmid_ds shmBuffer;
  t_scores_list *shmPtr;
  puts("Score sent.");
  puts("Waiting for server to send the score of each player...");
  do {
    CHECK(msgrcv(msgId, &request, sizeof(t_body), getpid(), 0), "--- ERROR : Cannot receive message from mail box");
  } while(strlen(strstr(request.body.message, SCORE_WRITTEN_MSG)) == 0);
  nbOfPlayers = atoi(request.body.message + strlen(SCORE_WRITTEN_MSG) + 1); // strlen(SCORE_WRITTEN_MSG) + 1 because index start from 0 and message format is SCORE_WRITTEN_MSG:<nbOfPlayersAs1Digit>\0
  printf("You've played against %d players\n", nbOfPlayers);
  scores->size = nbOfPlayers;
  openShmAndSemaphore(&shmId, &shmBuffer, &shmPtr, &semId);
  copyScoreList(scores, *shmPtr);
  //puts("1");
  CHECK(shmdt(shmPtr), "--- ERROR : Cannot release the shm.");
  sem_close(&semId);
  sortScoreList(scores);
}

void printFinalRank(int msgId, t_scores_list scores) {
  int i;
  t_request request;
  request.body.pid = getpid();
  request.type = SCORE_READ_MSG_TYPE;
  CHECK(msgsnd(msgId, &request, sizeof(t_body), 0), "--- ERROR : Cannot notify the server that the score has been read.");
  puts("Final ranks :");
  for(i = 0; i < scores.size; i++) {
    printf("%d - %s has scored %d point(s).\n", i+1, scores.scores[i].username, scores.scores[i].score);
  }
}
