#include <sys/un.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define PORT_NUM 49999 

int main(int argc, char *argv[]){

	char buffer[1024];

	if (argc != 2) {
		fprintf(stderr, "Error: One Server hostname/IP Address argument needed.\n");
		exit(EXIT_FAILURE);
	}

	int socketfd;

	socketfd = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in servAddr;
	struct hostent* hostEntry;
	struct in_addr **pptr;

	memset( &servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(PORT_NUM);

	hostEntry = gethostbyname(argv[1]);
	if (hostEntry == NULL){
		herror("gethost");
		exit(1);
	}

	pptr = (struct in_addr **) hostEntry->h_addr_list;
	memcpy(&servAddr.sin_addr, *pptr, sizeof(struct in_addr));

	if (connect(socketfd, (struct sockaddr *) &servAddr,
					   sizeof(servAddr)) < 0){
		perror("connect");
		exit(1);
	}

	read(socketfd, buffer, 25);

	printf("%s\n", buffer);

	return 0;

}