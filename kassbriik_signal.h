/**
 * @file kaasbriik_signal.h
 * @author Corentin DESTREZ & Valentin GUIBERTEAU
 * @date 07 Nov 2020
 * @brief Header of the client library
 *
 * This library containts the definition of some useful values to communicate
 * between the server and the clients
 *
 */

#include <signal.h>

#ifndef KAASBRIIK_SIGNAL_H
#define KAASBRIIK_SIGNAL_H

/*****************************************************************************/
/*                                   SIGNALS                                 */
/*---------------------------------------------------------------------------*/
/*                Define the signals used for the inter-process              */
/*                    communication as well as their value                   */
/*****************************************************************************/

#define SIGNAL_IS_READY SIGUSR1
#define SIGNAL_SCORE_WRITTEN SIGUSR2

#endif
