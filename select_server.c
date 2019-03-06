#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int main(int argc, char *argv[])
{
	int fd1, fd2;
	struct sockaddr_in saddr;
	struct sockaddr_in caddr;

	int len;
	int port;
	char buf[1024];

	int read_line;
	int fd_width;
	fd_set mask;
	fd_set readOK;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s Port_number\n", argv[0]);
		exit(1);
	}
	port = atoi(argv[1]);


	if ((fd1 = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		exit(1);
	}
	memset(&saddr, 0, sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_addr.s_addr = INADDR_ANY;
	saddr.sin_port = htons(port);

	if (bind(fd1, (struct sockaddr *)&saddr, sizeof(saddr)) < 0) {
		perror("bind");
		exit(1);
	}
	if (listen(fd1, 1) < 0) {
		perror("listen");
		exit(1);
	}
	while (1) {
		fprintf(stderr, "Waiting for connection... \n");
		len = sizeof(caddr);
		if ((fd2 = accept(fd1, (struct sockaddr *)&caddr, &len)) < 0) {
			perror("accept");
			exit(1);
		}
		fprintf(stderr, "Connection established: socket %d used.\n", fd2);
		fprintf(stderr, "Client: IP=%s, PORT=%d.\n",inet_ntoa(caddr.sin_addr), ntohs(caddr.sin_port));
		fd_width = fd2 + 1;
		FD_ZERO(&mask);
		FD_SET(0, &mask);
		FD_SET(fd2, &mask);

		while (1) {
			readOK = mask;
			select(fd_width, &readOK, NULL, NULL, NULL);

			if (FD_ISSET(0, &readOK)) {
				fgets(buf, 1024, stdin);
				buf[strlen(buf) - 1] = '\0';
				write(fd2, buf, strlen(buf) + 1);
				if (!strcmp(buf, "QUIT"))
					break;
			} else if (FD_ISSET(fd2, &readOK)) {
				memset(buf, 0, sizeof(buf));
				read_line = read(fd2, buf, 1024);

				if (read_line <= 0 || !strcmp(buf, "QUIT")) {
					fprintf(stdout, "QUIT received.\n");
					break;
				}
				strcat(buf, "\n");
				fputs(buf, stdout);
			}
		}
		fprintf(stdout, "Connection closed.\n");
		close(fd2);
	}

	close(fd1);
	return 0;
}
