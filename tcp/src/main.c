#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <net/socket.h>
#include <net/net_ip.h>

#include <data/json.h>
#include <tp.h>

#define TCP_PORT 20

#define perror(fmt, args...)						\
do {									\
	printf("Error: " fmt "(): %s\n" ## args, strerror(errno));	\
	exit(errno);							\
} while (0)								\

void main(void)
{
	int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (fd < 0) {
		perror("socket");
	}

	{
		struct sockaddr_in sin;

		sin.sin_family = AF_INET;
		inet_pton(AF_INET, "127.0.0.1", &sin.sin_addr.s_addr);
		sin.sin_port = htons(TCP_PORT);

		if (bind(fd, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
			perror("bind");
		}

		printf("Listening on TCP port %d\n", TCP_PORT);
	}

	{
#ifndef BUF_SIZE
#define BUF_SIZE 4096
#endif
		char *buf = malloc(BUF_SIZE);

		if (buf == NULL) {
			perror("malloc");
		}

		for (;;) {
			ssize_t len = recv(fd, buf, BUF_SIZE, 0);

			if (len < 0) {
				perror("recv");
			}

			printf("Received %ld bytes\n", (long)len);
			printf("%s\n", buf);
		}
	}
}
