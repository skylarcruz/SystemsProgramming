#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>

int readDirectory(char *d){

	// Open DIR struct from directory path
	DIR *dir = opendir(d);
	if (dir == NULL){
		fprintf(stderr, "Error: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	// declare dirent for read, currFile for full path concatenation
	struct dirent *rd;
	char *currFile;

	// While next file exist and no errors present
	while ((rd = readdir(dir)) != NULL && errno == 0){

		// If not directory dot or symbolic link
		if (strcmp(rd->d_name, ".") != 0 && 
		strcmp(rd->d_name, "..") != 0 &&
		rd->d_type != DT_LNK){

			// Malloc currFile for length of full path and concatenate
			currFile = (char *) malloc(2 + strlen(d) + strlen(rd->d_name));
			strcpy(currFile, d);
			strcat(strcat(currFile, "/"), rd->d_name);

			// If read file is a directory
			if (rd->d_type == DT_DIR){

				// If the directory has execution permissions, sent to readDirectory
				if(access(currFile, X_OK) == 0)
					readDirectory(currFile);
				// If directory does not have execution permissions
				else if (access(currFile, X_OK) == -1){
					// Ignore directory, reset errno
					errno = 0;
				}
			}
			else if (rd->d_type == DT_REG)// If regular file, print full path
				printf("%s\n", currFile);
			free(currFile); // Free malloc'd full path
		}
	}

	// While Loop final error check
	if (errno != 0){
		fprintf(stderr, "Error: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	// close the directory
	closedir(dir);

	// closedir error check
	if (errno != 0){
		fprintf(stderr, "Error: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	return 0;

}

int main(int argc, char *argv[]) {

	// If more than 1 argument, exit program
	if (argc > 2){
		fprintf(stderr, "Error: Too many arguments. Only one optional argument for directory declaration.\n");
		fprintf(stderr, "Exiting program....\n");
		exit(EXIT_FAILURE);
	}

	// Initialize directory char
	char directory[1024];

	// If 1 argument given
	if (argc == 2){

		// Extend to full path if neccesary
		if(realpath(argv[1], directory) == NULL){
			fprintf(stderr, "Error: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}

		// Test access to directory
		if (access(directory, X_OK) == -1){
			fprintf(stderr, "Error: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
	}

	// If no argument given
	else{

		// Get current working directory
		if(getcwd(directory, sizeof(directory)) == NULL){
			fprintf(stderr, "Error: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
	}

	// Enter recursive function
	readDirectory(directory);

	// Final Error Check
	if (errno != 0){
		
		fprintf(stderr, "Error: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);

}