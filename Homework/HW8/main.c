#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sort.h"
#include "getWord.h"

//#define TEXTFILE "poem.txt"
#define TEXTFILE "mobydick.txt"

//poem size: 80140
//moby dick size: 214872

static int compare (const void * a, const void * b){
	const void **charA = (const void **)a;
 	const void **charB = (const void **)b;

	return strcmp(*charA, *charB);
}


int main() {

	
	// initializes clock for performing timing statistics
	clock_t start, end;
    double cpu_time_used;
    //start = clock();

    char** charArray;
    char** charArray2;
    //charArray = (char **) malloc(80140 * sizeof(char*)); // poem.txt
    charArray = (char **) malloc(214872 * sizeof(char*)); // mobydick.txt
    //charArray2 = (char **) malloc(214872 * sizeof(char*));
    
	FILE * fh;
	fh = fopen(TEXTFILE, "r");

	char * CurrWord = getNextWord(fh);
	int count = 0;

	while(CurrWord != NULL){
		charArray[count] = CurrWord;
		//charArray2[count] = CurrWord;
		count++;
		CurrWord = getNextWord(fh);
	}

	// Run functions here using charArray
	start = clock();
	setSortThreads(5);
	sortThreaded(charArray, count);

	end = clock();
	cpu_time_used = ((double) (end - start));
	printf("Sort took %f time", cpu_time_used);

	// ========================================

	//qsort(charArray2, 214872, sizeof(char*), compare);

	// ========================================

	// int i = 0;
	// while (i < count){
	// 	printf("%s\n",charArray[i]);
	// 	i++;
	// }

	free(charArray);

	return 0;

}