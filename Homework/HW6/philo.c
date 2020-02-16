#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>

int randomGaussian(int mean, int stddev) {
	double mu = 0.5 + (double) mean;
	double sigma = fabs((double) stddev);
	double f1 = sqrt(-2.0 * log((double) rand() / (double) RAND_MAX));
	double f2 = 2.0 * 3.14159265359 * (double) rand() / (double) RAND_MAX;
	if (rand() & (1 << 5)) 
		return (int) floor(mu + sigma * cos(f2) * f1);
	else            
		return (int) floor(mu + sigma * sin(f2) * f1);
}

int main() {

	// Array of semaphores
	int chopsticks = semget(IPC_PRIVATE, 5, IPC_CREAT | IPC_EXCL  | 0600);
	if (chopsticks == -1){
		fprintf(stderr, "Error: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	pid_t phil[5];

	int i;
	for(i = 0; i < 5; i++){
    	if (semctl(chopsticks, i, SETVAL, 1) == -1) {
			fprintf(stderr, "Error: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
  	}

	for (i = 0; i < 5; i++){
		int pid = fork();
		if (pid == -1){
			fprintf(stderr, "Error: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
		// Children
    	if(pid == 0){
    		srand(getpid()); // Seed random number generator using child pid

    		// Stat counters
			int totalEat = 0;
			int totalThink = 0;
			int cycles = 0;


			// Select chopsticks and prepare sembufs for pickup/release
			struct sembuf pickupChop1[1] = {i, -1, 0};
			struct sembuf releaseChop1[1] = {i, 1, 0};
			struct sembuf pickupChop2[1];
			struct sembuf releaseChop2[1];
			if (i != 4){
				struct sembuf pickupChop2[1] = {i+1, -1, 0};
				struct sembuf releaseChop2[1] = {i+1, 1, 0};
			}
			else{
				struct sembuf pickupChop2[1] = {0, -1, 0};
				struct sembuf releaseChop2[1] = {0, 1, 0};
			}

			// Think/Eat until 100 seconds spent eating
			while (totalEat <= 100) {
				cycles += 1; // increase cycle count

				// determine random think time and sleep
				int think_time = randomGaussian(11, 7);
				if (think_time < 0) think_time = 0;
				printf("Philosopher %d is thinking for %d seconds (%d)\n", i, think_time, totalEat);
				sleep(think_time);
				totalThink += think_time; // increase total think time

				// Pick up chop1
				if (semop(chopsticks, pickupChop1, 1) == -1){
					fprintf(stderr, "Error: %s\n", strerror(errno));
					exit(EXIT_FAILURE);
				}

				// Pick up chop2
				if (semop(chopsticks, pickupChop2, 1) == -1){
					fprintf(stderr, "Error: %s\n", strerror(errno));
					exit(EXIT_FAILURE);
				}

				// Determine random eat time and sleep
				int eat_time = randomGaussian(9, 3);
				if (eat_time < 0) eat_time = 0;
				printf("Philosopher %d is eating for %d seconds (%d) \n", i, eat_time, totalEat);
				sleep(eat_time);
				totalEat += eat_time; // increase total eat time

				// Release chop1
				if (semop(chopsticks, releaseChop1, 1) == -1){
					fprintf(stderr, "Error: %s\n", strerror(errno));
					exit(EXIT_FAILURE);
				}

				// Release chop2
				if (semop(chopsticks, releaseChop2, 1) == -1){
					fprintf(stderr, "Error: %s\n", strerror(errno));
					exit(EXIT_FAILURE);
				}

			}

			// Print results of thinking/eating for each philosopher
			printf("Philosopher %d ate for %d seconds and thought for %d seconds, over %d cycles\n", 
				   i, totalEat, totalThink, cycles);

			exit(i);
    	}
    	// Parent
    	else{
      	phil[i] = pid;
    	}
	}

	// Waits for Children to finish
	int status;
	for(i = 0; i < 5; i++) {
    	if (waitpid(phil[i], &status, 0) == -1){
			fprintf(stderr, "Error: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
  	}

  	// Cleans up semaphore
  	if (semctl(chopsticks, 0, IPC_RMID, 0) == -1){
		fprintf(stderr, "Error: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

  	return 0;

}