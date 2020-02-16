#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void handler(int code){
	// pass
}

int main(int argc, char *argv[]){

	if (argc != 2) {
		printf("Error: Single argument required");
		return 0;
	}

	int argLength = strlen(argv[1]);
	int i = 0;

	signal(SIGALRM, handler);

	while(i < argLength) {
		ualarm(100000, 0);
		pause();
		printf("%c", argv[1][i]);
		fflush(stdout);
		i += 1;
	}	

	printf("\n");

	return 0;

}