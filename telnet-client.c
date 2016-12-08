#include "telnet-client.h"

static const telnet_telopt_t telopts[] = {
	{ TELNET_TELOPT_ECHO,		TELNET_WONT, TELNET_DO   },
	{ TELNET_TELOPT_TTYPE,		TELNET_WILL, TELNET_DONT },
	{ TELNET_TELOPT_COMPRESS2,	TELNET_WONT, TELNET_DO   },
	{ TELNET_TELOPT_MSSP,		TELNET_WONT, TELNET_DO   },
	{ -1, 0, 0 }
};

static void _cleanup(void) {
	tcsetattr(STDOUT_FILENO, TCSADRAIN, &orig_tios);
}

static void _input(char *buffer, int size) {
	static char crlf[] = { '\r', '\n' };
	int i;

	for (i = 0; i != size; ++i) {
		/* if we got a CR or LF, replace with CRLF
		 * NOTE that usually you'd get a CR in UNIX, but in raw
		 * mode we get LF instead (not sure why)
		 */
		if (buffer[i] == '\r' || buffer[i] == '\n') {
			if (do_echo)
				printf("\r\n");
			telnet_send(telnet, crlf, 2);
		} else {
			if (do_echo)
				putchar(buffer[i]);
			telnet_send(telnet, buffer + i, 1);
		}
	}
	fflush(stdout);
}

static void _send(int sock, const char *buffer, size_t size) {
	int rs;

	/* send data */
	while (size > 0) {
		if ((rs = send(sock, buffer, size, 0)) == -1) {
			fprintf(stderr, "send() failed: %s\n", strerror(errno));
			exit(1);
		} else if (rs == 0) {
			fprintf(stderr, "send() unexpectedly returned 0\n");
			exit(1);
		}

		/* update pointer and size to see if we've got more to send */
		buffer += rs;
		size -= rs;
	}
}

static void _event_handler(telnet_t *telnet, telnet_event_t *ev,
		void *user_data) {
	int sock = *(int*)user_data;

	switch (ev->type) {
	/* data received */
	case TELNET_EV_DATA:
		printf("%.*s", (int)ev->data.size, ev->data.buffer);
		fflush(stdout);
		break;
	/* data must be sent */
	case TELNET_EV_SEND:
		_send(sock, ev->data.buffer, ev->data.size);
		break;
	/* request to enable remote feature (or receipt) */
	case TELNET_EV_WILL:
		/* we'll agree to turn off our echo if server wants us to stop */
		if (ev->neg.telopt == TELNET_TELOPT_ECHO)
			do_echo = 0;
		break;
	/* notification of disabling remote feature (or receipt) */
	case TELNET_EV_WONT:
		if (ev->neg.telopt == TELNET_TELOPT_ECHO)
			do_echo = 1;
		break;
	/* request to enable local feature (or receipt) */
	case TELNET_EV_DO:
		break;
	/* demand to disable local feature (or receipt) */
	case TELNET_EV_DONT:
		break;
	/* respond to TTYPE commands */
	case TELNET_EV_TTYPE:
		/* respond with our terminal type, if requested */
		if (ev->ttype.cmd == TELNET_TTYPE_SEND) {
			telnet_ttype_is(telnet, getenv("TERM"));
		}
		break;
	/* respond to particular subnegotiations */
	case TELNET_EV_SUBNEGOTIATION:
		break;
	/* error */
	case TELNET_EV_ERROR:
		fprintf(stderr, "ERROR: %s\n", ev->error.msg);
		exit(1);
	default:
		/* ignore */
		break;
	}
}

/* Next functions are added as part of our project. */
int prepare_command(char *command, char *buffer) {
	int length;
	length = strlen(command);
	strcpy(buffer, command);
	return length;
}

void func(telnet_config_t *config, char *command) {
	int len;
	
	len = prepare_command(command, config->buffer);
	_input(config->buffer, len);

	if ((config->rs = recv(config->sock, config->buffer, sizeof(config->buffer), 0)) > 0) {		
		telnet_recv(telnet, config->buffer, config->rs);
	} else if (config->rs == 0) {
		printf("error");
	} else {
		printf("recv(client) failed: %s\n", strerror(errno));
		exit(1);
	}
	
	return;
}


int telnet_construct(telnet_config_t *config) {
	/* look up server host */
	memset(&config->hints, 0, sizeof(config->hints));
	config->hints.ai_family = AF_UNSPEC;
	config->hints.ai_socktype = SOCK_STREAM;
	if ((config->rs = getaddrinfo("127.0.0.1", "7070", &(config->hints), &(config->ai))) != 0) {
		fprintf(stderr, "getaddrinfo() failed for 127.0.0.1: %s\n",
				gai_strerror(config->rs));
		return 1;
	}
	
	/* create server socket */
	if ((config->sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		fprintf(stderr, "socket() failed: %s\n", strerror(errno));
		return 1;
	}

	/* bind server socket */
	memset(&(config->addr), 0, sizeof(config->addr));
	config->addr.sin_family = AF_INET;
	if (bind(config->sock, (struct sockaddr *)&(config->addr), sizeof(config->addr)) == -1) {
		fprintf(stderr, "bind() failed: %s\n", strerror(errno));
		return 1;
	}

	/* connect */
	if (connect(config->sock, config->ai->ai_addr, config->ai->ai_addrlen) == -1) {
		fprintf(stderr, "connect() failed: %s\n", strerror(errno));
		return 1;
	}

	/* free address lookup info */
	freeaddrinfo(config->ai);
	
	/* set input echoing on by default */
	do_echo = 1;

	/* initialize telnet box */
	telnet = telnet_init(telopts, _event_handler, 0, &(config->sock));

	/* initialize poll descriptors */
	memset(config->pfd, 0, sizeof(config->pfd));
	config->pfd[0].fd = STDIN_FILENO;
	config->pfd[0].events = POLLIN;
	config->pfd[1].fd = config->sock;
	config->pfd[1].events = POLLIN;
	
	return 1;
}

int telnet_deconstruct (telnet_config_t * config) {
	telnet_free(telnet);
	close(config->sock);

	return 0;
}
	

