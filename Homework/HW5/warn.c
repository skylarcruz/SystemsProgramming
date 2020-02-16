#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <signal.h>
#include <ctype.h>

// Ugly global variable
char buffer[512];
int sflag;

void childHandler(int sig){
	// pass
	sflag = 0;
}

void parentHandler(int sig){
	// pass
	sflag = 1;
}

void alarmSig(int sig){
	printf("%s", buffer);
	sflag = 2;
}

int main() {

	char wString[512]; // user input string
	int delay; // delay for alarm
	char dString[4]; // store delay digits from user

	// Initialize alarm timer
	struct itimerval timeSet;

	// Initialize pipe
	int fd[2];
	int n = pipe (fd);
	if (n == -1){
		fprintf(stderr, "Error: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	int rdr = fd[0]; 
	int wtr = fd[1];

	int childPid = fork();
	if(childPid) { // child process
		if (close(wtr) == -1){
			fprintf(stderr, "Error: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
		if (close(0) == -1){
			fprintf(stderr, "Error: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}

		// set child to ignore SIGINT
		if (signal(SIGINT, SIG_IGN) == SIG_ERR){
			fprintf(stderr, "Error: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}

		// set SIGFPE for child to childHandler
		if (signal(SIGFPE, childHandler) == SIG_ERR){
			fprintf(stderr, "Error: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
		
		// set alarm function
		if (signal(SIGALRM, alarmSig) == SIG_ERR){
			fprintf(stderr, "Error: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}

		while(1){
			pause(); // wait for SIGFPE from Parent
			read(rdr, buffer, 512);

			// Parse digits, if any
			if (isdigit(buffer[0])){
				int i = 0;
				while (isdigit(buffer[i])){
					dString[i] = buffer[i];
					i++;
					if (i > 4) {
						fprintf(stderr, "Error: Delay time too long.\n");
						fprintf(stderr, "Must not exceed 4 digits.\n");
						exit(EXIT_FAILURE);
					}
				}

				// Check for space after digits
				int spacer;
				if (buffer[i] == ' ')
					spacer = 1;
				else
					spacer = 0;

				// Remove digits and space if it exists
				int j = i;
				while (i < strlen(buffer) - spacer){
					buffer[i - j] = buffer[i + spacer];
					i++;
				}
				buffer[i - j] = '\0';

				// set delay from input string
				delay = atoi(dString);
			}

			// if no digits, set delay to default
			else {
				delay = 5;
			}

			// check for exit
			if (strcmp(buffer, "exit\n") == 0){
				close(rdr);
				printf("Child is exiting...\n");
				return 0;
			}

			// set itimerval
			timeSet.it_value.tv_sec = delay;
			timeSet.it_value.tv_usec = 0;
			timeSet.it_interval = timeSet.it_value;
			if ((setitimer(ITIMER_REAL, &timeSet, NULL)) == -1) {
			    perror("error calling setitimer()");
			    exit(1);
			}

			// Loop through each alarm until next interrupt passes SIGFPE
			do{
				pause();
			} while (sflag != 0);

			alarm(0);
		}
	} else { // parent process

		int flag = 0;

		// close pipe read end
		if (close(rdr) == -1){
			fprintf(stderr, "Error: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}

		// set parent to ignore SIGFPE
		if (signal(SIGFPE, SIG_IGN) == SIG_ERR){
			fprintf(stderr, "Error: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}

		// set parent for interrupt signal
		if (signal(SIGINT, parentHandler) == SIG_ERR){
			fprintf(stderr, "Error: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}

		printf("Send interrupt to begin.\n");
		do {
			pause(); // pause until interrupt
			if (flag == 1)
				kill(childPid, SIGFPE);
			flag = 1;
			printf("Interrupt received, enter new message: ");
			fgets(wString,512,stdin);
			write(wtr, wString, 512);
			kill(childPid, SIGFPE);
		} while (strcmp(wString, "exit\n") != 0);
		if (close(wtr) == -1){
			fprintf(stderr, "Error: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
	}

	return 0;

}