#include "mftp.h"

// ================
// Helper Functions
// ================

int newSocket(int portNum, char *hostArg){

	int socketfd;

	socketfd = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in servAddr;
	struct hostent* hostEntry;
	struct in_addr **pptr;

	memset( &servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(portNum);

	hostEntry = gethostbyname(hostArg);
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

	return socketfd;

}

void readSocket(int sfd, char *buff){
	int i = 0;
	while(read(sfd, &buff[i], 1) > 0){
		if (buff[i] == '\n'){
			break;
		}
		i++;
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

void splitBuff(char *buff, char *comm, char *arg){

	int wflag = 0;
	int k = 0;
	int j = 0;
	for(int i = 0; buff[i] != '\n'; i++){
		if (k == 0 && isspace(buff[i]) != 0){
			// pass
		}
		else{
			if (isspace(buff[i]) != 0 && wflag == 0) { 
					wflag = 1;
					comm[k] = '\0';
			}
			else {
				if (wflag == 0){
					comm[k] = buff[i];
				}
				else {
					arg[j] = buff[i];
					j++;
				}
			}
			k++;
		}
	}
	comm[k] = '\0';
	arg[j] = '\0';
}

void getTail(char *tail, char *buff){

	int i = 1;
	int j = 0;
	for(i; buff[i] != '\n'; i++){
		tail[j] = buff[i];
		j++;
	}
	tail[j] = '\0';
}

int getDataConn(int sfd, char* dPort, char* buff, char* hostArg){
	int nsfd;
	strcpy(buff, "D\n");
	writeSocket(sfd, buff);
	readSocket(sfd, buff);
	if (buff[0] == 'A'){
		getTail(dPort, buff);
		int dPortNum = atoi(dPort);
		nsfd = newSocket(dPortNum, hostArg);
	}
	else if (buff[0] == 'E'){
		char errMsg[512];
		getTail(errMsg, buff);
		printf("Error from server: %s\n", buff);
		return -1;
	}
	return nsfd;
}

void showMore(int fdIn){
	int cpid = fork();
	// parent
	if (cpid > 0) {
		waitpid(cpid, NULL, 0);
	}
	// child
	else {
		int fd[2];
		int n = pipe (fd);
		int rdr = fd[0]; 
		int wtr = fd[1];

		int childpid = fork();

			// parent
		if (childpid > 0) {
			//waitpid(childpid, NULL, 0);
			close(wtr);
			close(0);
			dup(rdr);
			close(rdr);
			execlp("more", "more", "-20", NULL);
			perror("ls execvp failed");
			exit(1);
		}
		// child
		else {
			close(rdr);
			close(1); // close stdout
			close(0);
			transferData(fdIn, wtr);
			exit(0);
		}
	}
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
bool exitClient(int sfd, char* buff){
	strcpy(buff, "Q\n");
	writeSocket(sfd, buff);
	readSocket(sfd, buff);
	if(buff[0] == 'A'){
		printf("Exiting...\n");
		return true;
	}
	else {
		char errMsg[512];
		getTail(errMsg, buff);
		printf("Error from server: %s\n", errMsg);
		return false;
	}
}

//DONE
void lsClient(char *cwd){
	int cpid = fork();
	if (cpid == -1){
		printf("Error: fork failed\n");
		return;
	}
	// parent
	if (cpid > 0) {
		waitpid(cpid, NULL, 0);
	}
	// child
	else {
		int fd[2];
		int n = pipe (fd);
		int rdr = fd[0]; 
		int wtr = fd[1];

		int childpid = fork();
		if (childpid == -1){
			printf("Error: fork failed\n");
			return;
		}

			// parent
		if (childpid > 0) {
			waitpid(childpid, NULL, 0);
			close(wtr);
			close(0);
			dup(rdr);
			close(rdr);
			execlp("more", "more", "-20", NULL);
			perror("more exec failed");
			exit(1);
		}
		// child
		else {
			close(rdr);
			close(1); // close stdout
			n = dup(wtr);
			close(wtr);

			execlp("ls", "ls", "-l", cwd, NULL);
			perror("ls exec failed");
			exit(1);
		}
	}
}

// DONE
void lsServer(int sfd, char* dPort, char* buff, char* hostArg){
	int nsfd = getDataConn(sfd, dPort, buff, hostArg);
	if (nsfd != -1){
		strcpy(buff, "L\n");
		writeSocket(sfd, buff);
		readSocket(sfd, buff);
		if (buff[0] == 'A'){
			showMore(nsfd);
		}
		else {
			char errMsg[512];
			getTail(errMsg, buff);
			printf("Error from server: %s\n", buff);
		}
	}
	else{
		printf("Data Connection Error from rls");
	}
}

// DONE
void cdClient(char *cwd, char *direct){
	char tempPath[1024];
	strcpy(tempPath, cwd);
	if(realpath(direct, tempPath) == NULL){
		perror("realpath:");
		return;
	}

	struct stat temp_stat;
    stat(tempPath, &temp_stat);

	if (S_ISDIR(temp_stat.st_mode) != 1){
		printf("Error: Not a directory\n");
	}
	else {
		if(access(tempPath, X_OK) == 0){
			strcpy(cwd, tempPath);
			chdir(cwd);
		}
		else
			printf("Error: Access to directory denied\n");
	}
}

// DONE
void cdServer(int sfd, char *buff, char *direct){
	char cdPath[1046];

	strcpy(cdPath, "C");
	strcat(cdPath, direct);
	strcat(cdPath, "\n");

	writeSocket(sfd, cdPath);
	readSocket(sfd, buff);
	if (buff[0] == 'A'){
		printf("Server directory changed to %s\n", direct);
	}
	else {
		char errMsg[512];
		getTail(errMsg, buff);
		printf("Error from server: %s\n", errMsg);
	}
}

// Done
void showC(int sfd, char* dPort, char* buff, char *hostArg, char* arg){
	int nsfd = getDataConn(sfd, dPort, buff, hostArg);
	if (nsfd == -1){
		printf("Data Connection Error is showC\n");
	}
	char argComm[1046];
	strcpy(argComm, "G");
	strcat(argComm, arg);
	strcat(argComm, "\n");
	writeSocket(sfd, argComm);
	readSocket(sfd, buff);
	if (buff[0] == 'A'){
		showMore(nsfd);
	}
	else {
		char errMsg[512];
		getTail(errMsg, buff);
		printf("Error from server: %s\n", errMsg);
	}
	close(nsfd);
}

// DONE
void putC(int sfd, char* dPort, char* buff, char *hostArg, char* arg){
	int fd = open(arg, O_RDONLY);
	if (fd == -1){
		perror("open");
	}
	else {
		int nsfd = getDataConn(sfd, dPort, buff, hostArg);
		if (nsfd == -1){
			printf("Data Connection Error is showC\n");
			exit(1);
		}
		char argComm[1046];
		strcpy(argComm, "P");
		strcat(argComm, arg);
		strcat(argComm, "\n");
		writeSocket(sfd, argComm);
		readSocket(sfd, buff);

		if (buff[0] == 'A'){
			transferData(fd, nsfd);
			printf("File '%s' copied to server\n", arg);
		}
		else {
			char errMsg[512];
			getTail(errMsg, buff);
			printf("Error from server%s\n", errMsg);
		}
		close(nsfd);
	}
	close(fd);
}

// Done
void getC(int sfd, char* dPort, char* buff, char *hostArg, char* arg, char* cwd){
	char relPath[1024];
	strcpy(relPath, arg);
	truncateFname(relPath);
	if(access(relPath, F_OK) == 0){
		printf("Error: File already exists on client\n");
		return;
	}
	int nsfd = getDataConn(sfd, dPort, buff, hostArg);
	char argComm[4096];
	strcpy(argComm, "G");
	strcat(argComm, arg);
	strcat(argComm, "\n");
	writeSocket(sfd, argComm);
	readSocket(sfd, buff);
	if (buff[0] == 'A'){
		int fd = open(relPath, O_WRONLY | O_CREAT);
		if (fd == -1){
			perror("open");
		}
		else {
			transferData(nsfd, fd);
			printf("File '%s' copied from Server\n", relPath);
			close(fd);
			chmod(relPath, S_IRWXU);
		}
	}
	else {
		char errMsg[512];
		getTail(errMsg, buff);
		printf("Error from server: %s\n", errMsg);
	}
	close(nsfd);
}

// =============
// Main Function
// =============

int main(int argc, char *argv[]){

	bool exitFlag = false;

	char buffer[4096];
	char clientWD[4096];
	char dataPort[16];
	char command[2048];
  	char argcom[2048];

	if (argc != 2) {
		fprintf(stderr, "Error: One Server hostname/IP Address argument needed.\n");
		exit(EXIT_FAILURE);
	}

	int socketfd = newSocket(PORT_NUM, argv[1]);

	if(getcwd(clientWD, sizeof(clientWD)) == NULL){
		perror("getcwd");
		exit(1);
	}

	// Process User Commands
	while(exitFlag == false){

		printf("MFTP> ");
  		if (fgets(buffer, 4096, stdin) == NULL){
  			return 0;
  		}
  		else
  			splitBuff(buffer, command, argcom);

  		if (strcmp(command, "exit") == 0){
  			exitFlag = exitClient(socketfd, buffer);
  		}
  		else if (strcmp(command, "ls") == 0){
  			lsClient(clientWD);
  		}
  		else if (strcmp(command, "rls") == 0){
  			lsServer(socketfd, dataPort, buffer, argv[1]);
  		}
  		else if (strcmp(command, "cd") == 0){
  			cdClient(clientWD, argcom);
  		}
  		else if (strcmp(command, "rcd") == 0){
  			cdServer(socketfd, buffer, argcom);
  		}
  		else if (strcmp(command, "show") == 0){
  			showC(socketfd, dataPort, buffer, argv[1], argcom);
  		}
  		else if (strcmp(command, "put") == 0){
  			putC(socketfd, dataPort, buffer, argv[1], argcom);
  		}
  		else if (strcmp(command, "get") == 0){
  			getC(socketfd, dataPort, buffer, argv[1], argcom, clientWD);
  		}
  		else {
  			printf("Improper command\n");
  		}

	}

}