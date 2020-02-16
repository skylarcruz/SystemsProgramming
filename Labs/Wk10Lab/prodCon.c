#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#define BUFFLENGTH 1

char buff[BUFFLENGTH];
int i = 0;
int pCount = 0;
int cCount = 0;
int done = 0;
pthread_mutex_t doTask;

void passChars(){

	char str[] = "Here is a test string.";
	int j = 0;

	do {

		// Lock Buffer for writing
		pthread_mutex_lock(&doTask);
		if (pCount == cCount){
			buff[i] = str[j];
			pCount++;
			j++;
		}

		// Unlock Buffer for Reading
		pthread_mutex_unlock(&doTask);

	} while(str[j] != '\0');

	done = 1;

}

void printChars(){

	while (strlen(buff) == 0){
		// Wait for buffer to increase
	}

	do {

		// Lock Buffer for reading/printing
		pthread_mutex_lock(&doTask);
		if (cCount == pCount - 1){
			printf("%c", buff[i]);
			fflush(stdout);
			cCount++;
			if (i == BUFFLENGTH - 1)
				i = 0;
			else
				i++;
		}

		// Unlock Buffer for writing
		pthread_mutex_unlock(&doTask);

	
	} while (done == 0);

}

int main(){

	printf("\n");

	pthread_t producer, consumer;

	pthread_mutex_init(&doTask, NULL);

	pthread_create(&producer, NULL,(void *) passChars, NULL);
	pthread_create(&consumer, NULL,(void *) printChars, NULL);

	pthread_join(producer, NULL);
	pthread_join(consumer, NULL);

	pthread_mutex_destroy(&doTask);

	printf("\n\n");

}