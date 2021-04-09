#include <stdlib.h>

/*****************************************************************************/
/*                              CUSTOM LIBRARIES                             */
/*****************************************************************************/
#include "server_kassbriik.h"

void initClientList(t_clients_list *a, size_t size) {
  int i;
  a->clients = malloc(size * sizeof(int));
  a->size = size;
  for(i = 0; i < size; i++)
    a->clients[0] = -1;
}
