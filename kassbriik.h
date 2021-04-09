/**
 * @file kaasbriik_signal.h
 * @author Corentin DESTREZ & Valentin GUIBERTEAU
 * @date 09 Apr 2021
 * @brief Header of the client's and server's library
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
/*                                   MAIL BOX                                */
/*---------------------------------------------------------------------------*/
/*                Define the values used for the inter-process               */
/*              communication by mail box as well as their value             */
/*****************************************************************************/

#include <sys/msg.h>
#include <sys/types.h>
#include <sys/ipc.h>


#define MAX_BODY_MSG_SIZE 256
#define MSG_MODE 0666
// Request structure for mail box
typedef struct body {
  pid_t pid;
  char msg[MAX_BODY_MSG_SIZE];
} t_body;

typedef struct request {
  long type;
  t_body body;
} t_request;

// Mail box key for its creation
#define MSG_KEY ftok("./", 1)
// Type of message client send to serveur when it's trying to connect.
#define CONNECTION_MSG_TYPE 1
// Type of message client send to serveur when it has end its game.
#define END_GAME_MSG_TYPE 2
// Type of message server send to client when it should start the game.
#define START_GAME_MSG_TYPE 3
// Message send by server to notify the client that he's connected.
#define CONNECTION_NOTIFY_MSG "You are connected"

#endif
