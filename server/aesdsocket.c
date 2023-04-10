#include <stdio.h>
#include <stdlib.h>
#include <sys/syslog.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <syslog.h>
#include "str_ops.h"

#define PORT "9000"  // the port users will be connecting to

#define BACKLOG 10   // how many pending connections queue will hold
#define FILE_NAME "/var/tmp/aesdsocketdata"
int sockfd, new_fd;
FILE* fp = NULL;

void sock_cleanup_handler(int s)
{
	int e = 0;
	if (s == SIGTERM)
	{
		printf("Received SIGTERM\r\n");
		goto on_sig_term_or_int;
	}
	else if (s == SIGINT)
	{
		printf("Received SIGINT\r\n");
		goto on_sig_term_or_int;
	}
	else
	{
		printf("Unregistered signal: %d\r\n", s);
		exit(2);
	}

	on_sig_term_or_int:
	syslog(LOG_INFO, "Caught signal, exiting");
	e = shutdown(sockfd, 2);
	if (e != 0)
	{
		syslog(LOG_ERR, "Closing sockfd failed: %d", e);
	}
	e = shutdown(new_fd, 2);
	if (e != 0)
	{
		syslog(LOG_ERR, "Closing new_fd failed: %d", e);
	}
	if (fp != NULL)
	{
		fclose(fp);
	}
	remove(FILE_NAME);
	exit(0);
}


// get sockaddr, IPv4 or IPv6:
void* get_in_addr(struct sockaddr* sa)
{
	if (sa->sa_family == AF_INET)
	{
		return &(((struct sockaddr_in*) sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*) sa)->sin6_addr);
}

int main(int argc, char** argv)
{

	struct addrinfo hints, * servinfo, * p;
	struct sockaddr_storage            their_addr; // connector's address information
	socklen_t                          sin_size;
	struct sigaction                   sa;
	int                                yes = 1;
	char                               s[INET6_ADDRSTRLEN];
	int                                rv;
	bool runDaemon = false;

	if (argc == 2)
	{
		if (0 == strcmp(argv[1], "-d"))
		{
			printf("Received -d opt\r\n");
			runDaemon = true;
		}
		else
		{
			printf("Invalid arg: %s\r\n", argv[1]);
			exit(1);
		}
	}
	else if (argc < 2)
	{
		runDaemon = false;
		printf("Non-daemon mode\r\n");
	}
	else
	{
		printf("Invalid arg count\r\n");
	}

	openlog("[AESD_SOCKET]", LOG_NDELAY, LOG_USER);

	// Open file for appending text
	fp = fopen(FILE_NAME, "a+");

	if (fp == NULL)
	{
		syslog(LOG_ERR, "Error opening file: %s\n", FILE_NAME);
		exit(1);
	}

	memset(&hints, 0, sizeof hints);
	hints.ai_family   = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags    = AI_PASSIVE; // use my IP

	if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0)
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and bind to the first we can
	for (p = servinfo; p != NULL; p = p->ai_next)
	{
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
		                     p->ai_protocol)) == -1)
		{
			perror("server: socket");
			continue;
		}

		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &yes,
		               sizeof(int)) == -1)
		{
			perror("setsockopt");
			exit(1);
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
		{
			close(sockfd);
			perror("server: bind");
			continue;
		}

		break;
	}

	freeaddrinfo(servinfo); // all done with this structure

	if (p == NULL)
	{
		fprintf(stderr, "server: failed to bind\n");
		exit(1);
	}

	if (!runDaemon || (runDaemon && 0 == fork()))
	{
		if (listen(sockfd, BACKLOG) == -1)
		{
			perror("listen");
			exit(1);
		}

		sa.sa_handler = sock_cleanup_handler; // reap all dead processes
		sigemptyset(&sa.sa_mask);
		sa.sa_flags = SA_RESTART;
		if (sigaction(SIGTERM, &sa, NULL) == -1)
		{
			perror("sigaction SIGTERM");
			exit(1);
		}

		if (sigaction(SIGINT, &sa, NULL) == -1)
		{
			perror("sigaction SIGINT");
			exit(1);
		}

		printf("server: waiting for connections...\n");

		while (1)
		{
			int  read_count     = 0;
			char temp_buf[1024] = { };
			sin_size = sizeof their_addr;
			new_fd   = accept(sockfd, (struct sockaddr*) &their_addr, &sin_size);
			if (new_fd == -1)
			{
				perror("accept");
				continue;
			}

			inet_ntop(their_addr.ss_family,
			          get_in_addr((struct sockaddr*) &their_addr),
			          s, sizeof s);
			printf("Accepted connection from %s\n", s);
			syslog(LOG_INFO, "Accepted connection from %s\n", s);

			do
			{
				read_count = recv(new_fd, temp_buf, 50, 0);
				printf("Recv[%d]: %s\r\n", read_count, temp_buf);

				if (read_count <= 0)
				{
					break;
				}

				bool val = write_to_file(fp, temp_buf, read_count);
				if (val)
				{
					printf("write OK\r\n");
				}

				if (has_new_line(temp_buf, read_count))
				{
					read_complete_file(fp, new_fd);
				}

				memset(temp_buf, 0, sizeof temp_buf);
			}
			while (read_count > 0);

			if (read_count == 0)
			{
				syslog(LOG_INFO, "Closed connection from %s\r\n", s);
				printf("Closed connection from %s\r\n", s);
			}

		}
	}
	else
	{
		printf("Parent process\r\n");
	}


	return 0;
}