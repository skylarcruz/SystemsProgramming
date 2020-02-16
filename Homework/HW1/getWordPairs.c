#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <time.h>

#include "hashTable.h"
#include "getWord.h"

/*  getWordPairs by Skylar Cruz
	===========================
	Program which utilizes a hashtable to count and sort pairs of words 
	from given input files.

	run the program with args <-count> Filename1 <Filename2> <Filename3> ...

	Program will open each file and insert them into the table. Using the table it
	will then sort each pair of words by number of occurrences. The <-count> argument 
	determines how many of the top word pairs will be printed to stdout. If no
	<-count> is given, all word pairs print

	==================================================================
	Program utilizes getWord provided by the instructor, Ben McCammish
*/

int main(int argc, char *argv[]) {

	// initializes clock for performing timing statistics
	clock_t start, end;
    double cpu_time_used;
    start = clock();

    // count for number of items printed after sorting. default of 0: print all data
    int count = 0;

    // initializes a table with the given startSize
    const int startSize = 5;
	Table *hashTable = newTable(startSize);

	// Tests for argument error. Must include arguments
	if (argc < 2){
		fprintf(stderr, "\n\nError: Include arguments in the form of\n");
		fprintf(stderr, "<-count> filename1 <filename2> <filename3> ...\n\n");
	}

    for (int i = 1; i < argc; i++){
    	//if the first argument is <-count>, set count
    	if (i == 1 && argv[1][0] == '-'){
    		count = abs(atoi(argv[1]));

    		// if count is set but no files given in arguments
    		if (argc < 3){
    			fprintf(stderr, "\n\nError: Need files for parsing\n");
				fprintf(stderr, "Include arguments in the form of\n");
				fprintf(stderr, "<-count> filename1 <filename2> <filename3> ...\n\n");
			}
    	}
    	// process file
    	else {

	    	FILE * fh;
	    	fh = fopen(argv[i], "r");

	    	// tests for file open errors
			if (fh == NULL) {
				fprintf(stderr, "\n%s file does not exist or could not be opened.\n", argv[i]);
			}

			// if file can open, process
			else {
				char * FirstWord = getNextWord(fh); // get initial FirstWord
				char * SecondWord = getNextWord(fh); // get initial SecondWord
				char * TwoWordSet; // Final char* after concatenation
				char * space = " "; //Used for concatenating space between words

				do {

					// if FirstWord and SecondWord initialized, process for table insertion
					if (FirstWord != NULL && SecondWord != NULL) {
						// malloc TwoWordSet to size of char* to be concatenated
						TwoWordSet = (char *) malloc(1 + strlen(space) + strlen(FirstWord) + strlen(SecondWord));

						// concatenate chars to TwoWordSet
						strcpy(TwoWordSet, FirstWord);
						strcat(TwoWordSet, space);
						strcat(TwoWordSet, SecondWord);

						// Insert Word Pair into table
						charInsert(hashTable, TwoWordSet);

						// Free and set words for next iteration
						free(FirstWord);
						FirstWord = (char *) malloc(1 + strlen(SecondWord));
						strcpy(FirstWord, SecondWord);
						free(TwoWordSet);
						free(SecondWord);
						SecondWord = getNextWord(fh);
					}

				} while (SecondWord != NULL); // exit when end of file reached
				free(FirstWord); //free last char still in use after loop
				fclose(fh); //close the file
			}
		}
	}

	sortAndPrintHashTable(hashTable, count); // sorts and prints word pairs after processing all files
	freeTable(hashTable); // free table to avoud memory leaks

	// end timer and print program run time
	end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("\nProgram took %f seconds to execute \n", cpu_time_used); 

	return 0;
}