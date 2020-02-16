#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define argLength 100

int main(int argc, char *argv[]) {

	// if no arguments given, exit with error
	if (argc < 2 || strcmp(argv[1], ":") == 0){
		fprintf(stderr, "Error: Program requires at least one argument\n");
		fprintf(stderr, "formatted as ./connect <arg1> : <arg2>\n");
		exit(EXIT_FAILURE);
	}

	// set argCount to at least 1 and char arg1/arg2 for concatenation
	int argCount = 1;
	char *arg1[argLength];
	char *arg2[argLength];
	for (int i = 0; i < argLength; i++){
		arg1[i] = arg2[i] = 0;
	}

	int l = 0; // for adding args to arg2[]

	// for each argument given
	for (int i = 1; i < argc; i++){

		// if 2nd colon reached after second argument, exit with error
		if (argCount == 2 && (strcmp(argv[i], ":") == 0)){
			fprintf(stderr, "Error: Too many colons\n");
			exit(EXIT_FAILURE);
		}

		// search to first colon or to end of line if only one arg given
		if (strcmp(argv[i], ":") == 0 || (argCount != 2 && i == argc - 1)){

			// if second argument is given, set argCount to 2
			if (i < argc - 1){
				argCount = 2;
			}

			// Set arg1 using all arguments up to first colon or to end of line
			for (int j = 1; j <= i; j++){
				if (strcmp(argv[j], ":") != 0){
					arg1[j-1] = argv[j];
				}
			}
		}

		// concatenate arguments to arg2 after first colon parsed
		if (argCount == 2 && strcmp(argv[i], ":") != 0){
			arg2[l] = argv[i];
			l++;
		}
	}

	// If only one argument given
	if (argCount == 1){
		execvp(arg1[0], arg1);
		perror("ecexvp failed");
		exit(EXIT_FAILURE);
	}

	// Else if two arguments given
	else {
		int fd[2];
		int n = pipe (fd);
		if (n == -1){
			fprintf(stderr, "Error: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
		int rdr = fd[0]; 
		int wtr = fd[1];

		// parent
		if (fork ()) {
			close (rdr);
			close(1);
			n = dup(wtr);
			if (n == -1){
				fprintf(stderr, "Error: %s\n", strerror(errno));
				exit(EXIT_FAILURE);
			}
			close(wtr);
			execvp(arg1[0], arg1);
			perror("Parent ecexvp failed");
			exit(EXIT_FAILURE);
		}

		// child
		else {
			close (wtr);
			close (0); 
			n = dup (rdr);
			if (n == -1){
				fprintf(stderr, "Error: %s\n", strerror(errno));
				exit(EXIT_FAILURE);
			}
			close (rdr);
			execvp(arg2[0], arg2);
			perror("Child ecexvp failed");
			exit(EXIT_FAILURE);
		}

	}

	printf("Something went wrong\n");

	exit(EXIT_FAILURE);

}