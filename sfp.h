#ifndef SFP_H
#define SFP_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <errno.h>
#include <sys/types.h>
#include <time.h>

#include "util.h"

/* application error codes */
static const int ERR_PARAM      =  1;    /* invalid parameter(s) */
static const int ERR_REQ        =  2;    /* error parsing request */
static const int ERR_INTERNAL   =  3;    /* internal error */

/* max size of string with TCP/UDP port */
#define PORT_STR_SIZE   6

#endif
