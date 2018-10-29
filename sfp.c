#include <netdb.h>
#include <assert.h>

#include "sfp.h"
#include "util.h"
#include "sfp_opt.h"

FILE *logfp = NULL;
struct prog_opt sfp_opt;

struct sockaddr_in *
sinsock(struct sockaddr_in *sin, struct prog_opt *sfp_opt)
{
	memset(sin, 0, sizeof(struct sockaddr_in));
	sin->sin_family = AF_INET;
	sin->sin_port = htons(sfp_opt->listen_port);
	if( 0 == sfp_opt->listen_addr[0] ) {
		sin->sin_addr.s_addr = INADDR_ANY;
	}
	else {
		if( 0 == inet_aton(sfp_opt->listen_addr, &sin->sin_addr) )
			return NULL;
	}
	return sin;
}

int
server_socket(struct sockaddr_in *sin)
{
	int fd;
	int one = 1;
	struct linger ling = { 0, 0 };
	int nonblock = 1;
	int sndbufsize = 1024*1024;

	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		error_log(errno , "Server socket create error");
		return -1;
	}

	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one)) == -1 ||
			setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &one, sizeof(one)) == -1 ||
			setsockopt(fd, SOL_SOCKET, SO_LINGER, &ling, sizeof(ling)) == -1)
	{
		error_log(errno, "Server setsockopt error");
		close(fd);
		return -1;
	}

	if (setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &sndbufsize, sizeof(sndbufsize)) == -1) {
		error_log(errno, "Sevrer socket sendbuf error");
		close(fd);
		return -1;
	}
/*
	if (setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &one, sizeof(one)) == -1) {
		error_log(errno, "Server tcp_nodelay set error");
		close(fd);
		return -1;
	}
*/
	if (ioctl(fd, FIONBIO, &nonblock) < 0) {
		error_log(errno, "Server socket set nonblock error");
		close(fd);
		return -1;
	}


	if (bind(fd, (struct sockaddr *)sin, sizeof(*sin)) == -1) {
		error_log(errno, "Server socket bind error");
		close(fd);
		return -1;
	}


	if (listen(fd, 64) == -1) {
		error_log(errno, "Server socket listen error");
		close(fd);
		return -1;
	}

	return fd;
}

void
server_accept(EV_P_ ev_io *w, int revents)
{
	int fd = accept(w->fd, NULL, NULL);
	if (fd < 0) {
		wrlog(L_CRITICAL, "Client accept error: %s", strerror(errno));
		return;
	}

	int one = 1;
	if (ioctl(fd, FIONBIO, &one) < 0) {
		wrlog(L_CRITICAL, "Client nonblock ioctl error: %s", strerror(errno));
		close(fd);
		return;
	}

	if (setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &one, sizeof(one)) == -1) {
		wrlog(L_CRITICAL, "Client tcp_nodelay setsockopt error: %s", strerror(errno));
		/* Do nothing, not a fatal error.  */
	}

	int len;
	for (len = 1 << 21; len > 0; len -= 1 << 18)
		if (setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &len, sizeof(len)) == 0) {
			break;
		}


/*
	struct client *client = calloc(sizeof(*client), 1);
	const char *peerip = get_peerip(fd);
	strncpy(client->addr, peerip, sizeof(client->addr));
	client->bytes = 0;
	client->errors = 0;
	client->starttime = time(NULL);
	client->state = CLI_CONNECT;

	void client_cbread(ev_io *w, int revents);
	ev_io_init(&client->io, (void *)client_cbread, fd, EV_READ);
	ev_io_start(&client->io);
*/
}

int main(int argc, char* const argv[])
{
	int rc;
	struct sockaddr_in sin, *ssin;

	ev_default_loop(ev_recommended_backends() | EVFLAG_SIGNALFD);
	rc = get_opt(argc, argv);
	if (rc) {
		return rc;
	}

	ssin = sinsock(&sin, &sfp_opt);
	if(NULL == ssin) {
		error_log(errno, "Bad listen");
		abort();
	}

	int fd = server_socket(ssin);
	if (fd < 0) {
		exit(1);
	}

	if (sfp_opt.logfile) {
		logfp = fopen(sfp_opt.logfile, "a");
		if (!logfp) {
			fprintf(stderr,"Can't open log '%s'! %s\n", sfp_opt.logfile, strerror(errno));
			exit(EXIT_FAILURE);
		}
	}

	if (! sfp_opt.is_foreground) {
		if(NULL == logfp) {
			fprintf(stderr,"Must specify log file when run as daemon!\n");
			exit(EXIT_FAILURE);
		}
		if (0 != (rc = daemonize(0))) {
			fprintf(stderr,"Can't run as daemon!\n");
			exit(EXIT_FAILURE);
		}
	}
	wrlog(L_EMERGENCY, APP_NAME " start");

	if( sfp_opt.pidfile && 0 != (rc = make_pidfile( sfp_opt.pidfile, getpid())) ) {
		fprintf(stderr, "Can't create pidfile %s!\n", sfp_opt.pidfile);
		exit(EXIT_FAILURE);
	}

	ev_io io;
	ev_io_init(&io, server_accept, fd, EV_READ);
	ev_io_start(&io);

	ev_run(0);

	wrlog(L_EMERGENCY, APP_NAME " stopped");
	if (sfp_opt.pidfile) {
		if( -1 == unlink(sfp_opt.pidfile) ) {
			error_log(errno, "unlink [%s]", sfp_opt.pidfile );
		}
	}

	free_opt(&sfp_opt);
	if (logfp)
		fclose(logfp);

	return 0;
}
