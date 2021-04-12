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
 *
 */

#ifndef KAASBRIIK_H
#define KAASBRIIK_H


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


/*****************************************************************************/
/*                                   MAIL BOX                                */
/*---------------------------------------------------------------------------*/
/*                Define the values used for the inter-process               */
/*              communication by mail box as well as their value             */
/*****************************************************************************/

#include <sys/msg.h>
#include <sys/types.h>
#include <sys/ipc.h>

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
 * \brief Key of the message queue.
 *
 */
#define MSG_KEY ftok("./", 1)

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
 * \def START_GAME_MSG_TYPE
 * \brief Message type to start the game.
 *
 * Type of the message send by server to all clients to notify them that they must start the game.
 *
 */
#define START_GAME_MSG_TYPE 2

/**
 * \def GAME_STARTED_MSG_TYPE
 * \brief Message type to notify that the game is started.
 *
 * Type of the message send by clients to the server to notify him that it has started the game.
 *
 */
#define GAME_STARTED_MSG_TYPE 3

/**
 * \def END_GAME_MSG_TYPE
 * \brief Message type to notify end of the game.
 *
 * Type of the message send by a client to notify the server that a player has end his game.
 * When a message of this type is send, it must contain the score of the player.
 *
 */
#define END_GAME_MSG_TYPE 4

/**
 * \def SEND_SCORE_MSG_TYPE
 * \brief Message type to notify that score is received
 *
 * Type of the message send by the server to notify a client that it has received its player's score.
 *
 */
#define SCORE_RECEIVED_MSG_TYPE 5

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
