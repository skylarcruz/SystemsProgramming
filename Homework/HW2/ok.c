#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#define WORDLENGTH 16
#define DICT "dictionaries/webster"

int ok(int fd, char *word){

	// Set want with extra paces and null terminator
	char want[WORDLENGTH];
	strcpy(want, word);
	int i = strlen(word);
	for (i; i < WORDLENGTH - 1; i++){
		want[i] = ' ';
	}
	want[WORDLENGTH - 1] = '\0';

	char have[WORDLENGTH];
	int bot = 0;
	int top = lseek(fd, 1, SEEK_END);
	int mid;
	int midSet;
	int n;

	// Binary search loop
	while (1){
		if (bot >= top){
			return 0;
		}
		mid = (bot+top)/2; // get mid
		midSet = mid - (mid % (WORDLENGTH)); // set mid to beginning of line
		lseek(fd, midSet, SEEK_SET); // Seek to line in middle of current range
		n = read(fd, have, WORDLENGTH - 1); // read length of word to have
		have[WORDLENGTH - 1] = '\0'; // Set null terminator
		if (n < 0) {
			fprintf(stderr, "Error: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		} else {
			// Compare strings and process binary search
			if (strcmp(want, have) == 0){
				return 1;
			}
			if (strcmp(want, have) < 0){
				top = mid;
			} 
			if (strcmp(want, have) > 0){
				bot = mid + 1;
			}
		}

	}
}

int main(int argc, char *argv[]) {

	// If single word argument is not given, exit on failure
	if (argc != 2){
		fprintf(stderr, "Error: Incorrect number of arguments. Exiting program.\n");
		exit(EXIT_FAILURE);
	}

	char word[WORDLENGTH];

	// set char word to argument, truncating if neccesary
	if (strlen(argv[1]) >= WORDLENGTH){
		for (int i = 0; i < WORDLENGTH - 1; i++){
			word[i] = argv[1][i];
		}
		word[WORDLENGTH - 1] = '\0';
	}
	else {
		strcpy(word, argv[1]);
	}

	// Open file, print errors and exit on failure
	int fd = open(DICT, O_RDONLY, 0);
	if (fd < 0){
		fprintf(stderr, "Error: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	// Test dictionary for existance of argument word
	int result = ok(fd, word);
	if (result == 0){
		printf("no\n");
	}
	else if (result == 1){
		printf("yes\n");
	}

	// Close file
	close(fd);

	// If error present, print error and exit on failure
	if (errno != 0){
		fprintf(stderr, "Error: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);

}