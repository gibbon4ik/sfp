#include <assert.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "sfp.h"
#include "sfp_opt.h"

extern struct prog_opt sfp_opt;

/* convert input parameter into an IPv4-address string */
int
get_ipaddr( const char* s, char* buf, size_t len )
{
	struct sockaddr_in saddr;
	int rc = 0;

	assert( s && buf && len );

	if( 1 == inet_aton(s, &(saddr.sin_addr)) ) {
		(void) strncpy( buf, s, len );
	}

	buf[ len - 1 ] = 0;
	return rc;
}

/* populate options with default/initial values */
int
init_opt( struct prog_opt* so )
{
	int rc = 0;
	assert( so );
	so->is_foreground = 0;
	so->is_immediate = 0;
	so->listen_addr[0] = 0;
	so->listen_port = 3128;
	so->timeout = 20,
	so->logfile = so->configfile = so->pidfile = NULL;
	so->loglevel = L_ERROR;
	return rc;
}

/* release resources allocated for strmproxy options */
void
free_opt( struct prog_opt* so )
{
	assert( so );
	if( so->logfile )
		free(so->logfile);
	if( so->configfile )
		free(so->configfile);
	if( so->pidfile )
		free(so->pidfile);
}

void
usage( const char* app, FILE* fp )
{
	(void) fprintf (fp, "usage: %s [-f] [-v level] [-b listenaddr] [-p port] "
		"[-t timeout] "
		"[-c configfile] [-l logfile] [-P pidfile]\n"
		, app );
	(void) fprintf(fp,
		"\t-v : set verbosity level 0-5 [default = 0]\n"
		"\t-f : run foreground, do NOT run as a daemon\n"
		"\t-b : (IPv4) address to listen on [default = %s]\n"
		"\t-p : port to listen on\n"
		"\t-t : timeout, sec [default = %d]\n"
		"\t-l : log file name\n"
		"\t-c : config file name\n"
		"\t-P : pid file name\n"
		,IPv4_ALL, sfp_opt.timeout);
	(void) fprintf( fp, "Examples:\n"
		"  %s -p 4022 \n"
		"\tlisten for HTTP requests on port 4022, all network interfaces\n"
		"  %s -b 192.168.1.1 -p 4022\n"
		"\tlisten for HTTP requests on IP 192.168.1.1, port 4022;\n",
		app, app);
	return;
}

int
get_opt(int argc, char* const argv[])
{
	int rc = 0, ch = 0;
	static const char OPTMASK[] = "fv:b:l:p:t:P:r";

	rc = init_opt( &sfp_opt );
	while( (0 == rc) && (-1 != (ch = getopt(argc, argv, OPTMASK))) ) {
		switch( ch ) {
			case 'v': if (optarg) {
				  	int lvl = atoi( optarg );
					switch (lvl) {
						case 0: sfp_opt.loglevel = L_ERROR;
							break;
						case 1: sfp_opt.loglevel = L_WARNING;
							break;
						case 2: sfp_opt.loglevel = L_NOTICE;
							break;
						case 3: sfp_opt.loglevel = L_INFO;
							break;
						case 4: sfp_opt.loglevel = L_DEBUG;
							break;
						case 5: sfp_opt.loglevel = L_ANNOY;
							break;
						default: fprintf(stderr, "Bad verbosity level %s\n", optarg);
							 rc = ERR_PARAM;
					}
					if (rc) {
						free_opt( &sfp_opt );
						return rc;
					}
				  }
				  else {
					sfp_opt.loglevel = L_WARNING;
				  }
				  break;
			case 'f': sfp_opt.is_foreground = f_TRUE;
				  sfp_opt.loglevel = L_DEBUG;
				  break;

			case 'b':
				  rc = get_ipaddr( optarg, sfp_opt.listen_addr, sizeof(sfp_opt.listen_addr) );
				  if( 0 != rc ) {
					  (void) fprintf( stderr, "Invalid address: [%s]\n",
							  optarg );
					  rc = ERR_PARAM;
				  }
				  break;

			case 'p':
				  sfp_opt.listen_port = atoi( optarg );
				  if( sfp_opt.listen_port <= 0 || sfp_opt.listen_port >= 65536) {
					  (void) fprintf( stderr, "Invalid port number: [%d]\n",
							  sfp_opt.listen_port );
					  rc = ERR_PARAM;
				  }
				  break;

			case 't':
				  sfp_opt.timeout = atoi( optarg );
				  if( sfp_opt.timeout < 0 ) {
					  (void) fprintf( stderr, "Invalid timeout: [%d]\n",
							  sfp_opt.timeout );
					  rc = ERR_PARAM;
				  }
				  break;

			case 'l':
				  sfp_opt.logfile = strdup(optarg);
				  break;

			case 'c':
				  sfp_opt.configfile = strdup(optarg);
				  break;

			case 'P':
				  sfp_opt.pidfile = strdup(optarg);
				  break;

			case ':':
				  (void) fprintf( stderr,
						  "Option [-%c] requires an argument\n",
						  optopt );
				  rc = ERR_PARAM;
				  break;
			case '?':
				  (void) fprintf( stderr,
						  "Unrecognized option: [-%c]\n", optopt );
				  usage( argv[0], stderr );
				  rc = ERR_PARAM;
				  break;

			default:
				  usage( argv[0], stderr );
				  rc = ERR_PARAM;
				  break;
		}
	} /* while getopt */

	if (rc) {
		free_opt( &sfp_opt );
		return rc;
	}
	return rc;
}

/* __EOF__ */
