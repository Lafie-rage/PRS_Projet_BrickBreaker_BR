/**
 * /file server_kaasbriik.c
 * /author Corentin DESTREZ & Valentin GUIBERTEAU
 * /date 09 Apr 2021
 * /brief Source code of the server's library
 *
 * This library containts the definition of some useful values, functions
 * and structures that are used by the server.
 *
 */

#include <stdlib.h>

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

void removeClientFromList(t_clients_list *a, t_client client) {
  // int i, j;
  // for(i = 0; i < size; i++)
  //   if(a->clients.pid == client.pid) {
  //     for()
  //   }

}
