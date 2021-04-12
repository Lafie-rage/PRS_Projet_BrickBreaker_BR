/**
 * /file server_kaasbriik.h
 * /author Corentin DESTREZ & Valentin GUIBERTEAU
 * /date 09 Apr 2021
 * /brief Header of the server's library
 *
 * This library containts the definition of some useful values, functions
 * and structures that are used by the server.
 *
 * This library also include thoose :
 *  -> sys/types.h
 *
 */

#ifndef SERVER_H
#define SERVER_H

#include <sys/types.h>

/*****************************************************************************/
/*                              CLIENTS LIST                                 */
/*---------------------------------------------------------------------------*/
/*               Define structure and behavior of dynamic arrays             */
/*              that will be used by the server to store clients             */
/*****************************************************************************/


/**
 * \struct t_client
 * \brief Structure of a client view by the server.
 *
 * Contains the PID of the client & the username of the player.
 *
 */
typedef struct client {
  int pid;
  char *username;
  int score;
} t_client;

/**
 * \struct clients_list
 * \brief A list of clients.
 *
 * Containts the lists of the clients the server knows or want to communicate to as array of t_client.
 * Also contains is size.
 * The clients undefined have the PID -1.
 * Those clients are in the client's array from size to the length of the array.
 * Avoid browsing the array with anything else than a for loop from 0 to size-1.
 *
 */
typedef struct clients_list {
  t_client *clients;
  size_t size;
} t_clients_list;

/**
 * \fn void initClientList(t_clients_list *a, size_t size)
 * \brief Initialise the t_clients_list structure.
 *
 * Initialise the send structure depending on the send size.
 * This function will set up the size of the list and initialise every value of the array to -1 as default value.
 *
 * \param a The t_clients_list to initialise.
 * \param size The size of the list.
 *
 */
void initClientList(t_clients_list *a, size_t size);

/**
 * \fn void removeClientFromList(t_clients_list *a, client_t client)
 * \brief Remove a client from the t_clients_list sent.
 *
 * Check if the list containts the client's pid send in parameter.
 * If it is, remove it and decrease the size of the list.
 *
 * \param a The list of clients in which you want to remove a client.
 * \param clientPid The client's pid to remove from the list.
 *
 */
void removeClientFromList(t_clients_list *a, int clientPid);

/**
 * \fn t_client *getPlayerNameByPid(t_clients_list a, int clientPid)
 * \brief Return the client identifying it by it's pid.
 *
 * Check if the list containts the client's pid send in parameter.
 * If it is, return the client.
 *
 * \param a The list clients that should contain the client we're looking for.
 * \param clientPid The pid of the client we are looking for.
 *
 * \return The client that we're looking for. If the client you're looking for is not in the list, return NULL.
 *
 */
t_client *getClientByPid(t_clients_list a, int clientPid);

#endif
