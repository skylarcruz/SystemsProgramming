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
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

#define PORT_NUM 49999 

int main(int argc, char *argv[]){

	int listenfd;
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (listenfd < 0){
		perror("connect");
		exit(1);
	}

	struct sockaddr_in servAddr;

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(PORT_NUM);
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(listenfd,
			(struct sockaddr *) &servAddr, sizeof(servAddr)) < 0){
				perror("bind");
				exit(1);
	}

	listen(listenfd, 1);

	int connectfd;
	int length = sizeof(struct sockaddr_in);
	struct sockaddr_in clientAddr;

	int childpid;
	int status;


	while(1){

		connectfd = accept(listenfd, (struct sockaddr *) &clientAddr, &length);

		childpid = fork();

		// parent
		if (childpid > 0) {
			
			while(waitpid(-1, NULL, WNOHANG) != 0){
				// continue
			}

		}
		// child
		else if (childpid == 0) {

			struct hostent* hostEntry;
			char* hostName;
			char buffer[5];

			hostEntry = gethostbyaddr(&(clientAddr.sin_addr),
							sizeof(struct in_addr), AF_INET);

			hostName = hostEntry->h_name;

			printf("New connection: %s\n", hostName); //Check later

			time_t curtime; 
		    time(&curtime);
		    strcpy(buffer, ctime(&curtime));

			write(connectfd, buffer, 25);

			exit(0);

		}

		else if (childpid < 0){
			perror("fork");
			exit(1);
		}

		close(connectfd);

	}

	return 0;

}