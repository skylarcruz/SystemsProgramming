#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

pthread_mutex_t chopstick[5];

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

void *thinkEat(int n) {

	srand(n);

	int totalEat = 0;
	int totalThink = 0;
	int cycles = 0;

	while (totalEat <= 100) {
		cycles += 1; // increase cycle count

		// determine random think time and sleep
		int think_time = randomGaussian(11, 7);
		if (think_time < 0) think_time = 0;
		printf("Philosopher %d is thinking for %d seconds (%d)\n", n, think_time, totalEat);
		sleep(think_time);
		totalThink += think_time; // increase total think time

		if(pthread_mutex_lock(&chopstick[n]) != 0){
			fprintf(stderr, "Error: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
		if (n != 4){
			if(pthread_mutex_lock(&chopstick[n+1]) != 0){
				fprintf(stderr, "Error: %s\n", strerror(errno));
				exit(EXIT_FAILURE);
			}
		}
		else{
			if(pthread_mutex_lock(&chopstick[0]) != 0){
				fprintf(stderr, "Error: %s\n", strerror(errno));
				exit(EXIT_FAILURE);
			}
		}

		// Determine random eat time and sleep
		int eat_time = randomGaussian(9, 3);
		if (eat_time < 0) eat_time = 0;
		printf("Philosopher %d is eating for %d seconds (%d) \n", n, eat_time, totalEat);
		sleep(eat_time);
		totalEat += eat_time; // increase total eat time

		if (pthread_mutex_unlock(&chopstick[n]) != 0){
			fprintf(stderr, "Error: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
		if (n != 4){
			if (pthread_mutex_unlock(&chopstick[n+1]) != 0) {
				fprintf(stderr, "Error: %s\n", strerror(errno));
				exit(EXIT_FAILURE);
			}
		}
		else{
			if (pthread_mutex_unlock(&chopstick[0]) != 0){
				fprintf(stderr, "Error: %s\n", strerror(errno));
				exit(EXIT_FAILURE);
			}
		}
	}

	// Print results of thinking/eating for each philosopher
	printf("Philosopher %d ate for %d seconds and thought for %d seconds, over %d cycles\n", 
		   n, totalEat, totalThink, cycles);

	int *status;
	pthread_exit(status);

}

int main() {

	pthread_t phil[5];

	for (int i = 0; i < 5; i++){
		if(pthread_mutex_init(&chopstick[i], NULL) != 0){
			fprintf(stderr, "Error: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
	}

	for (int i = 0; i < 5; i++){
		if(pthread_create(&phil[i], NULL, (void *)thinkEat, (void *)i) != 0){
			fprintf(stderr, "Error: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
	}

	for (int i = 0; i < 5; i++){
		if(pthread_join(phil[i], NULL) != 0){
			fprintf(stderr, "Error: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
	}

	for (int i = 0; i < 5; i++){
		if (pthread_mutex_destroy(&chopstick[i]) != 0){
			fprintf(stderr, "Error: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
	}
	
  	return 0;

}