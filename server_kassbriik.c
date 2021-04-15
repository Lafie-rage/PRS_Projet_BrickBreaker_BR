/**
 * /file server_kassbriik.c
 * /author Corentin DESTREZ & Valentin GUIBERTEAU
 * /date 09 Apr 2021
 * /brief Source code of the server's library
 *
 * This library containts the definition of some useful values, functions
 * and structures that are used by the server.
 *
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/*****************************************************************************/
/*                              CUSTOM LIBRARIES                             */
/*****************************************************************************/
#include "server_kassbriik.h"

void initClientList(t_clients_list *a, size_t size) {
  int i;
  a->clients = malloc(size * sizeof(t_client));
  a->size = size;
  for(i = 0; i < size; i++)
    a->clients[0].pid = -1;
}

void removeClientFromList(t_clients_list *a, int clientPid) {
  int i, j;
  for(i = 0; i < a->size; i++) {
    if(a->clients[i].pid == clientPid) {
      for(j = i; j < a->size - 1; j++)
        a->clients[j] = a->clients[j+1];
    }
  }
  a->size--;
}

void copyClientsList(t_clients_list *dest, t_clients_list src) {
  int i;
  dest->size = src.size;
  dest->clients = malloc(dest->size * sizeof(t_client));
  for(i = 0; i < dest->size; i++) {
    dest->clients[i].pid = src.clients[i].pid;
    dest->clients[i].username = malloc(strlen(src.clients[i].username) * sizeof(char));
    strcpy(dest->clients[i].username, src.clients[i].username);
    dest->clients[i].score = src.clients[i].score;
  }
}

t_client *getClientByPid(t_clients_list a, int clientPid) {
  int i;
  for(i = 0; i < a.size; i++) {
    if(a.clients[i].pid == clientPid) {
      return &(a.clients[i]);
    }
  }
  return NULL;
}
