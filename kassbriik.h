/**
 * \file kaasbriik_signal.h
 * \author Corentin DESTREZ & Valentin GUIBERTEAU
 * \date 09 Apr 2021
 * \brief Header of the client's and server's library
 *
 * This library containts the definition of some useful values to communicate
 * between the server and the clients
 *
 * This library also include thoose :
 *  -> sys/msg.h
 *  -> sys/types.h
 *  -> sys/ipc.h
 *  -> sempahore.h
 *  -> sys/stat.h
 *  -> fcntl.h
 *
 */

#ifndef KASSBRIIK_H
#define KASSBRIIK_H

#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <pthread.h>


/*****************************************************************************/
/*                                    GLOBAL                                 */
/*---------------------------------------------------------------------------*/
/*                Define the values used for the inter-process               */
/*              communication by mail box as well as their value             */
/*****************************************************************************/

/**
 * \def CHECK(sts, msg)
 * \brief Check the return status of a function.
 *
 * If the return is -1, priting "msg" in stderr and the errno. Then exit with -1 code.
 *
 */
#define CHECK(sts, msg) if ((sts) == -1) { perror(msg); exit(-1);}

/**
 * \def MAX_CLIENTS
 * \brief Number max of clients that the server can accept.
 *
 */
#define MAX_CLIENTS 9

/*****************************************************************************/
/*                                  SEMAPHORE                                */
/*---------------------------------------------------------------------------*/
/*                Define the values used for the inter-process               */
/*           acces management via semaphores as well as their value          */
/*****************************************************************************/


/**
 * \def SHM_NAME
 * \brief Name of the shm shared between the clients and the server.
 *
 */
#define SEM_SHM_NAME "kassbriik_shm_sem"


/*****************************************************************************/
/*                                SHARED MEMORY                              */
/*---------------------------------------------------------------------------*/
/*                Define the values used for the inter-process               */
/*                communication by shm as well as their value                */
/*****************************************************************************/

/**
 * \def SHM_KEY
 * \brief Key to acces to the shm.
 *
 */
#define SHM_KEY ftok("./shm_kassbriik", 1)

/**
 * \struct t_score
 * \brief Structure of a score view by the client.
 *
 * Contains the username of the player and its score.
 *
 */
typedef struct score {
  int score;
  char username[50];
} t_score;

/**
 * \struct t_scores_list
 * \brief A list of t_score knonwing its size.
 *
 * Contains a list of scores and the size of the list.
 *
 */
typedef struct scores_list {
  t_score scores[MAX_CLIENTS];
  int size;
} t_scores_list;

/**
 * \fn void sortScoreList(t_scores_list *scores)
 * \brief Sort the list if scores.
 *
 * Sort the score list from lower score to the greater.
 *
 * \param scores The list of scores that must be sorted.
 *
 */
void sortScoreList(t_scores_list *scores);

/**
 * \fn void copyScoreList(t_scores_list *dest, t_scores_list dest)
 * \brief Copy a source list in the dest list.
 *
 * Copy the src list in the dest list preventing from accessing to the dest list when modifying the src list.
 *
 * \param dest The destiantion list.
 * \param src The source list.
 *
 */
void copyScoreList(t_scores_list *dest, t_scores_list src);

/*****************************************************************************/
/*                                   MAIL BOX                                */
/*---------------------------------------------------------------------------*/
/*                Define the values used for the inter-process               */
/*              communication by mail box as well as their value             */
/*****************************************************************************/

/**
 * \def MAX_BODY_MSG_SIZE
 * \brief Size maximal of a message to send or read from the message queue.
 *
 */
#define MAX_BODY_MSG_SIZE 256

/**
 * \def MSG_MODE
 * \brief Opening mode of the message queue.
 *
 */
#define MSG_MODE 0666

/**
 * \def MSG_KEY
 * \brief Key ot acces to the message queue.
 *
 */
#define MSG_KEY ftok("./msg_kassbriik", 1)

/**
 * \def CONNECTION_MSG_TYPE
 * \brief Message type for client connection.
 *
 * Message send by the client to connect to the server.
 *
 */
#define CONNECTION_MSG_TYPE 1

/**
 * \def CONNECTION_NOTIFY_MSG
 * \brief Response of the server when the client is connected.
 *
 */
#define CONNECTION_NOTIFY_MSG "You are connected"

/**
 * \def START_GAME_MSG
 * \brief Message to start the game.
 *
 * The message send by server to all clients to notify them that they must start the game.
 *
 */
#define START_GAME_MSG "Game must start"

/**
 * \def GAME_STARTED_MSG_TYPE
 * \brief Message type to notify that the game is started.
 *
 * Type of the message send by clients to the server to notify him that it has started the game.
 *
 */
#define GAME_STARTED_MSG_TYPE 2

/**
 * \def END_GAME_MSG_TYPE
 * \brief Message type to notify end of the game.
 *
 * Type of the message send by a client to notify the server that a player has end his game.
 * When a message of this type is send, it must contain the score of the player.
 *
 */
#define END_GAME_MSG_TYPE 3

/**
 * \def SCORE_RECEIVED_MSG_TYPE
 * \brief Message type to notify that score is received
 *
 * Type of the message send by the server to notify a client that it has received its player's score.
 *
 */
#define SCORE_RECEIVED_MSG_TYPE 4

/**
 * \def SCORE_WRITTEN_MSG
 * \brief Message send to notify that scores can be read
 *
 * Message send to a client to notify him that he can read the scores in the shm
 *
 */
#define SCORE_WRITTEN_MSG "Scores can be read"

/**
 * \def SCORE_READ_MSG_TYPE
 * \brief Message type to notify that scores has been read
 *
 * Type of the message send by the server to notify a client that it has read the scores.
 *
 */
#define SCORE_READ_MSG_TYPE 5

/**
 * \struct t_body
 * \brief Body of a t_request.
 *
 * Containts the message to send and the source pid.
 *
 */
typedef struct body {
  pid_t pid;
  char message[MAX_BODY_MSG_SIZE];
} t_body;

/**
 * \struct t_request
 * \brief Request send via the message queue.
 *
 * t_reuest are the messages send via the message queue.
 * It's composed of the body (as t_body) and the type of request.
 *
 */
typedef struct request {
  long type;
  t_body body;
} t_request;

#endif
