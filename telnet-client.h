/*
 * Sean Middleditch
 * sean@sourcemud.org
 *
 * The author or authors of this code dedicate any and all copyright interest
 * in this code to the public domain. We make this dedication for the benefit
 * of the public at large and to the detriment of our heirs and successors. We
 * intend this dedication to be an overt act of relinquishment in perpetuity of
 * all present and future rights to this code under copyright law. 
 */

#if !defined(_POSIX_SOURCE)
#	define _POSIX_SOURCE
#endif
#if !defined(_BSD_SOURCE)
#	define _BSD_SOURCE
#endif

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <poll.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <termios.h>
#include <unistd.h>
#include <unistd.h>
#include <time.h>

#ifdef HAVE_ZLIB
#include "zlib.h"
#endif

#include "libtelnet.h"

/**
 * @brief Structure needed for telnet initialization
 */
typedef struct telnet_config {
	char buffer[512];
	int rs;
	int sock;
	struct sockaddr_in addr;
	struct pollfd pfd[2];
	struct addrinfo *ai;
	struct addrinfo hints;
	struct termios tios;
} telnet_config_t;

static struct termios orig_tios;
static telnet_t *telnet;
static int do_echo;

/**
 * @brief  initialize telnet client
 *
 * @param  [in]   telnet_config_t *config - structure filled with necessary data

 * @param  [out]  
 * @return         
 */
int telnet_construct(telnet_config_t *config);

/**
 * @brief  deinitialize telnet client
 *
 * @param  [in]   telnet_config_t *config - structure filled with necessary data

 * @param  [out]  
 * @return         
 */
int telnet_deconstruct (telnet_config_t * config);
