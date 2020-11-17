/**
 * @file client.c
 * @author Corentin DESTREZ & Valentin GUIBERTEAU
 * @date 07 Nov 2020
 * @brief Source code of the client library
 *
 * This library containts the functions defining the behavior of the clients.
 * It also define its behavior depending on the signal recieved.
 *
 */

#include "kaasbriik_signal.h" // also include signal.h
#include <stdio.h>

int main(int argc, char *argv[]) {
  // Test behavior
  printf("Hello World vous êtes : %s et le pid du server est : %s\n", argv[1], argv[2]);
}
