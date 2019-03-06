#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int main(int argc, char *argv[])
{
	struct sockaddr_in saddr;
	int soc;
	char ip_addr[100];
	int port;
	char buf[1024];

	int read_line;
	int fd_width;
	fd_set mask;
	fd_set readOK;

	if (argc != 3) {
		fprintf(stderr, "Usage: %s IP_address Port\n", argv[0]);
		exit(1);
	}
	strcpy(ip_addr, argv[1]);
	port = atoi(argv[2]);

	if ((soc = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		exit(1);
	}
	memset(&saddr, 0, sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_addr.s_addr = inet_addr(ip_addr);
	saddr.sin_port = htons(port);

	if (connect(soc, (struct sockaddr *)&saddr, sizeof(saddr)) < 0) {
		perror("connect");
		exit(1);
	}
	fprintf(stderr, "Connection established: socket %d used.\n", soc);
	fd_width = soc + 1;
	FD_ZERO(&mask);
	FD_SET(0, &mask);
	FD_SET(soc, &mask);

	while (1) {
		readOK = mask;
		select(fd_width, &readOK, NULL, NULL, NULL);

		if (FD_ISSET(0, &readOK)) {
			fgets(buf, 1024, stdin);
			write(soc, buf, strlen(buf) - 1);
			if (!strcmp(buf, "QUIT"))
				break;
		} else if (FD_ISSET(soc, &readOK)) {
			read_line = read(soc, buf, 1024);
			buf[read_line] = '\0';

			if (read_line <= 0 || !strcmp(buf, "QUIT")) {
				fprintf(stdout, "QUIT received.\n");
				break;
			}
			strcat(buf, "\n");
			fputs(buf, stdout);
		}
	}
	close(soc);
	fprintf(stdout, "Connection closed.\n");
	return 0;
}
