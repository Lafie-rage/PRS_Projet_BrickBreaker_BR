/**
 * @file server_kaasbriik.h
 * @author Corentin DESTREZ & Valentin GUIBERTEAU
 * @date 09 Apr 2021
 * @brief Header of the server's library
 *
 * This library containts the definition of some useful values, functions
 * and structures that are used by the server
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

typedef struct {
  int *clients;
  size_t size;
} t_clients_list;

void initClientList(t_clients_list *a, size_t size);

#endif
