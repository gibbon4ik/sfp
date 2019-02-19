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

#define EV_MULTIPLICITY 0
#include <ev.h>

#include "util.h"
#include "queue.h"
#include "ringbuffer.h"

#define APP_NAME "sfp v0.1"

/* max size of string with TCP/UDP port */
#define PORT_STR_SIZE   6

#define DZ_STDIO_OPEN  1   /* do not close STDIN, STDOUT, STDERR */
#define TVSTAMP_GMT  1

/* max size of string with IPv4 address */
#define IPADDR_STR_SIZE 16

/* application error codes */
static const int ERR_PARAM      =  1;    /* invalid parameter(s) */
static const int ERR_REQ        =  2;    /* error parsing request */
static const int ERR_INTERNAL   =  3;    /* internal error */

static const char CONFIG_NAME[]	= "sfp.cfg";

typedef enum {
	CLI_CONNECT,
	SRV_CONNECT,
	RELAY
} connstate;


#define IOBUFSIZE 16384
struct connect {
	ev_io	cliio;
	char	cliaddr[IPADDR_STR_SIZE];
	char	clireadbuf[IOBUFSIZE];
	size_t  clibufdata;

	ev_io	srvio;
	char	srvaddr[IPADDR_STR_SIZE];
	char	srvreadbuf[IOBUFSIZE];
	size_t 	srvbufdata;

	time_t starttime;
	size_t bytes;
	int errors;
	connstate state;
	LIST_ENTRY(connect) link;
};

#endif
