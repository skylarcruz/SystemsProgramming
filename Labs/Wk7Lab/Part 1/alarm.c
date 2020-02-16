#include <signal.h>
#include <stdio.h>
#include <unistd.h>

void handler(int code){
	printf("A\n");
}

void interrupt(int code){
	printf("B\n");
}

int main(int argc, char *argv[]){

	signal(SIGALRM, handler);
	signal(SIGINT, interrupt);
	while(1) {
		alarm(1);
		pause();
	}

	return 0;

}