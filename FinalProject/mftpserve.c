#include "mftp.h"

void readSocket(int sfd, char *buff){
	int i = 0;
	int n = 1;
	while(n > 0){
		n = read(sfd, &buff[i], 1);
		if (buff[i] == '\n' || n == -1){
			break;
		}
		i++;
	}
	if (n == 0){
		strcpy(buff, "EOF\n");
	}
}

void writeSocket(int sfd, char *buff){
	int i = 0;
	while(write(sfd, &buff[i], 1) > 0){
		if (buff[i] == '\n'){
			break;
		}
		i++;
	}
}

void transferData(int fdIn, int fdOut){
	char dBuff[1];
	int n = 1;
	while (n > 0){
		n = read(fdIn, &dBuff[0], 1);
		if (n < 1)
			break;
		n = write(fdOut, &dBuff[0], 1);
	}
}

int setDataConn(int Dfd){
	int option = 1;
	setsockopt(Dfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

	struct sockaddr_in DservAddr;

	memset(&DservAddr, 0, sizeof(DservAddr));
	DservAddr.sin_family = AF_INET;
	DservAddr.sin_port = htons(0);
	DservAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(Dfd,
			(struct sockaddr *) &DservAddr, sizeof(DservAddr)) < 0){
				perror("bind");
				return -1;
	}

	int len = sizeof(DservAddr);
	getsockname(Dfd, (struct sockaddr *) &DservAddr, &len);
	int newPort = ntohs(DservAddr.sin_port);

	return newPort;

}

int confirmDataConn(int Dfd){
	listen(Dfd, 1);
	int nDlength = sizeof(struct sockaddr_in);
	struct sockaddr_in nDclientAddr;

	int nDfd = accept(Dfd, (struct sockaddr *) &nDclientAddr, &nDlength);
	
	return nDfd;
}

void splitComm(char *buff, char *ctrl, char *arg){

	int wflag = 0;
	int i = 0;
	int j = 0;
	for(i; buff[i] != '\n'; i++)
		if (i == 0) { 
	 		ctrl[i] = buff[i];
	 		ctrl[i+1] = '\0';
		}
		else {
			arg[j] = buff[i];
			j++;
		}
	arg[j] = '\0';
}

void truncateFname(char *argF){
	char buff[1024];
	strcpy(buff, argF);
	int j = 0;
	for (int i = 0; buff[i] != '\0'; i++){
		if (buff[i] == '/'){
			j = 0;
			continue;
		}
		argF[j] = buff[i];
		j++;
	}
	argF[j] = '\0';
}

// =================
// Command Functions
// =================

// Done
void cdServer(int sfd, char *buff, char *cwd, char *direct, int cpid){
	char errMsg[100];
	char tempPath[1046];
	strcpy(tempPath, cwd);
	if(realpath(direct, tempPath) == NULL){
		strcpy(errMsg, "E");
		strcat(errMsg, "No such file or Directory\n");
		printf("Child %d: cd to %s failed with error %s", cpid, direct, errMsg);
		writeSocket(sfd, errMsg);
		return;
	}

	struct stat temp_stat;
    stat(tempPath, &temp_stat);

	if (S_ISDIR(temp_stat.st_mode) != 1){
		strcpy(errMsg, "E");
		strcat(errMsg, "Not a directory\n");
		printf("Child %d: cd to %s failed with error %s", cpid, direct, errMsg);
		writeSocket(sfd, errMsg);
		return;
	}
	if(access(tempPath, X_OK) == 0){
		strcpy(cwd, tempPath);
		chdir(cwd);
		strcpy(buff, "A\n");
		writeSocket(sfd, buff);
		printf("Child %d: Changed current directory to %s\n", cpid, direct);
	}
	else{
		strcpy(errMsg, "E");
		strcat(errMsg, "Access denied to directory\n");
		printf("Child %d: cd to %s failed with error %s", cpid, direct, errMsg);
		writeSocket(sfd, errMsg);
	}
}

void lsServer(int sfd, int nsfd, char* buff, char* cwd, int cpid){
	char errMsg[100];
	int eFlag = 0;
	int fd[2];
	int n = pipe (fd);
	if (n < 0){
		strcpy(errMsg, "E");
		strcat(errMsg, "Pipe error\n");
		printf("Child %d: remote ls failed with error %s", cpid, errMsg);
		writeSocket(sfd, errMsg);
		return;
	}
	int rdr = fd[0]; 
	int wtr = fd[1];

	int childpid = fork();
	if (childpid < 0){
		strcpy(errMsg, "E");
		strcat(errMsg, "Fork error\n");
		printf("Child %d: remote ls failed with error %s", cpid, errMsg);
		writeSocket(sfd, errMsg);
		return;
	}

		// parent
	if (childpid > 0) {
		waitpid(childpid, NULL, 0);
		close(wtr);
		transferData(rdr, nsfd);
		printf("Child %d: 'ls -l' sent to Client\n", cpid);
		
	}
	// child
	else {
		close(rdr);
		close(1); // close stdout
		n = dup(wtr);
		close(wtr);

		execlp("ls", "ls", "-l", cwd, NULL);
		strcpy(errMsg, "E");
		strcat(errMsg, "Exec ls error\n");
		printf("Child %d: remote ls failed with error %s", cpid, errMsg);
		writeSocket(sfd, errMsg);
		exit(1);
	}
	if(eFlag == 0){
	strcpy(buff, "A\n");
	writeSocket(sfd, buff);
	}
}

void getS(int sfd, int nsfd, char* buff, char* cwd, char* arg, int cpid){
	char errMsg[100];
	char tempPath[1046];
	strcpy(tempPath, cwd);
	if(realpath(arg, tempPath) == NULL){
		printf("Child %d Error: Server path does not exist\n", cpid);
		strcpy(errMsg, "E");
		strcat(errMsg, "Server path does not exist\n");
		writeSocket(sfd, errMsg);
		return;
	}

	struct stat cwd_stat;
    stat(tempPath, &cwd_stat);

	if (S_ISREG(cwd_stat.st_mode) != 1){
		printf("Child %d Error: Server path is not a file\n", cpid);
		strcpy(errMsg, "E");
		strcat(errMsg, "Server path is not a file\n");
		writeSocket(sfd, errMsg);
	}
	else {
		strcpy(buff, "A\n");
		writeSocket(sfd, buff);
		int fd = open(tempPath, O_RDONLY);
		if (fd == -1){
			perror("open");
		}
		else {
			transferData(fd, nsfd);
			close(fd);
			printf("Child %d: %s sent to Client\n", cpid, arg);
		}
	}
}

void putS(int sfd, int nsfd, char* buff, char* cwd, char* arg, int cpid){
	char errMsg[100];
	truncateFname(arg);
	if(access(arg, F_OK) == 0){
		printf("Child %d Error: File already exists on server\n", cpid);
		strcpy(errMsg, "E");
		strcat(errMsg, "File already exists on server\n");
		writeSocket(sfd, errMsg);
	}
	else{
		int fd = open(arg, O_WRONLY | O_CREAT);
		if (fd == -1){
			printf("Child %d Error: Error creating file\n", cpid);
			strcpy(errMsg, "E");
			strcat(errMsg, "Error creating file\n");
			writeSocket(sfd, errMsg);
		}
		else {
			strcpy(buff, "A\n");
			writeSocket(sfd, buff);
			transferData(nsfd, fd);
			close(fd);
			chmod(arg, S_IRWXU);
			printf("Child %d: '%s' copied to server\n at %s\n", cpid, arg, cwd);
		}
	}
}

int main(int argc, char *argv[]){

	int listenfd;
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (listenfd < 0){
		perror("connect");
		exit(1);
	}
	int option = 1;
	setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

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

	listen(listenfd, 4);

	int connectfd;
	int length = sizeof(struct sockaddr_in);
	struct sockaddr_in clientAddr;

	int childpid;
	int status;
	bool exitFlag = false;


	while(1){

		connectfd = accept(listenfd, (struct sockaddr *) &clientAddr, &length);

		childpid = fork();

		// parent
		if (childpid > 0) {
			
			while(waitpid(-1, NULL, WNOHANG) != 0){
				// for removing zombie processes
			}

		}
		// child
		else if (childpid == 0) {


			struct hostent* hostEntry;
			char* hostName;
			char buffer[4096];
			char control[1024];
			char sArg[1024];
			char serverWD[1024];

			hostEntry = gethostbyaddr(&(clientAddr.sin_addr),
							sizeof(struct in_addr), AF_INET);

			hostName = hostEntry->h_name;

			int cpid = getpid();

			printf("Child %d: %s Connected.\n", cpid, hostName); //Check later

			if(getcwd(serverWD, sizeof(serverWD)) == NULL){
				perror("getcwd");
				exit(1);
			}

			// Process user commands
			while (exitFlag == false){

				char errMsg[100];

				readSocket(connectfd, buffer);

				splitComm(buffer, control, sArg);

				if (control[0] == 'D'){
					int Dfd;
					Dfd = socket(AF_INET, SOCK_STREAM, 0);
					if (Dfd < 0){
						strcpy(errMsg, "E");
						strcat(errMsg, "Error creating Data Connection\n");
						writeSocket(connectfd, errMsg);
						continue;
					}
					int newPort = setDataConn(Dfd);
					if (newPort < 0){
						strcpy(errMsg, "E");
						strcat(errMsg, "Error creating Data Connection\n");
						writeSocket(connectfd, errMsg);
						continue;
					}
					char newPortC[10];
					sprintf(newPortC, "%d", newPort);
					char ackn[10];
					strcpy(ackn, "A");
					strcat(ackn, newPortC);
					strcat(ackn, "\n");
					writeSocket(connectfd, ackn);

					//int nDfd = accept(Dfd, (struct sockaddr *) &nDclientAddr, &nDlength);
					int nDfd = confirmDataConn(Dfd);
					if (nDfd == -1){
						strcpy(errMsg, "E");
						strcat(errMsg, "Error creating Data Connection\n");
						writeSocket(connectfd, errMsg);
						continue;
					}

					readSocket(connectfd, buffer);

					if (buffer[0] == 'G'){
						splitComm(buffer, control, sArg);
						getS(connectfd, nDfd, buffer, serverWD, sArg, cpid);
					}
					else if (buffer[0] == 'P'){
						splitComm(buffer, control, sArg);
						putS(connectfd, nDfd, buffer, serverWD, sArg, cpid);
					}
					else if (buffer[0] == 'L'){
						lsServer(connectfd, nDfd, buffer, serverWD, cpid);
					}
					close(nDfd);
				}
				else if (control[0] == 'C'){
					cdServer(connectfd, buffer, serverWD, sArg, cpid);
				}
				else if (control[0] == 'Q'){
					strcpy(buffer, "A\n");
					writeSocket(connectfd, buffer);
					exitFlag = true;
					printf("Child %d: Quiting...\n", cpid);
				}
				else if (strcmp(buffer, "EOF\n") == 0){
					printf("Child %d: Control Socket EOF detected, exiting\n", cpid);
					exitFlag = 1;
				}
				else {
  					printf("Ignoring unknown command: %s\n", buffer);
  				};

			}

			close(connectfd);
			exit(0);

		}

		else if (childpid < 0){
			perror("fork");
			exit(1);
		}

	}

	return 0;

}