#ifndef TOPOR_OPT_H
#define TOPOR_OPT_H

#include "sfp.h"
#include "util.h"

typedef u_short flag_t;
#if !defined( f_TRUE ) && !defined( f_FALSE )
    #define     f_TRUE  ((flag_t)1)
    #define     f_FALSE ((flag_t)0)
#else
    #error f_TRUE or f_FALSE already defined
#endif

static const char	IPv4_ALL[]	= "0.0.0.0";

/* options */
struct prog_opt {
	flag_t		is_foreground;
	flag_t		is_immediate;
	char		listen_addr[IPADDR_STR_SIZE];
	int		listen_port;
	int		timeout;
	char*		logfile;
	char*		configfile;
	char*		pidfile;
	loglevel	loglevel;
};

#ifdef __cplusplus
    extern "C" {
#endif

/* populate options with default/initial values */
int
init_opt( struct prog_opt* opt );


/* release resources allocated for udpxy options */
void
free_opt( struct prog_opt* opt );

void
usage( const char* app, FILE* fp );

int
get_opt(int argc, char* const argv[] );

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
