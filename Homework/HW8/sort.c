#include "sort.h"
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <errno.h>

#define SORT_THRESHOLD      40

typedef struct _sortParams {
    char** array;
    int left;
    int right;
} SortParams;

pthread_mutex_t mutex;
int activeThreads = 0;
int totalThreads = 0;

static int maximumThreads = 1;              /* maximum # of threads to be used */

/* This is an implementation of insert sort, which although it is */
/* n-squared, is faster at sorting short lists than quick sort,   */
/* due to its lack of recursive procedure call overhead.          */

static void insertSort(char** array, int left, int right) {
    int i, j;
    for (i = left + 1; i <= right; i++) {
        char* pivot = array[i];
        j = i - 1;
        while (j >= left && (strcmp(array[j],pivot) > 0)) {
            array[j + 1] = array[j];
            j--;
        }
        array[j + 1] = pivot;
    }
}

/* Recursive quick sort, but with a provision to use */
/* insert sort when the range gets small.            */

static void quickSort(void* p) {
    SortParams* params = (SortParams*) p;
    char** array = params->array;
    int left = params->left;
    int right = params->right;
    int i = left, j = right;
	
    if (j - i > SORT_THRESHOLD) {           /* if the sort range is substantial, use quick sort */

        int m = (i + j) >> 1;               /* pick pivot as median of         */
        char* temp, *pivot;                 /* first, last and middle elements */
        if (strcmp(array[i],array[m]) > 0) {
            temp = array[i]; array[i] = array[m]; array[m] = temp;
        }
        if (strcmp(array[m],array[j]) > 0) {
            temp = array[m]; array[m] = array[j]; array[j] = temp;
            if (strcmp(array[i],array[m]) > 0) {
                temp = array[i]; array[i] = array[m]; array[m] = temp;
            }
        }
        pivot = array[m];

        for (;;) {
            while (strcmp(array[i],pivot) < 0) i++; /* move i down to first element greater than or equal to pivot */
            while (strcmp(array[j],pivot) > 0) j--; /* move j up to first element less than or equal to pivot      */
            if (i < j) {
                char* temp = array[i];      /* if i and j have not passed each other */
                array[i++] = array[j];      /* swap their respective elements and    */
                array[j--] = temp;          /* advance both i and j                  */
            } else if (i == j) {
                i++; j--;
            } else break;                   /* if i > j, this partitioning is done  */
        }
		
        SortParams first;  first.array = array; first.left = left; first.right = j;
        quickSort(&first);                  /* sort the left partition	*/
		
        SortParams second; second.array = array; second.left = i; second.right = right;
        quickSort(&second);                 /* sort the right partition */
				
    } else insertSort(array,i,j);           /* for a small range use insert sort */
}

// New function for quicksorting using threads
static void threadSort(void* p){
    SortParams* params = (SortParams*) p;
    char** array = params->array;
    int left = params->left;
    int right = params->right;
    int i = left, j = right;

    int flag = 0; // Flag if new thread created
    pthread_t sThreadNew; // for initializing a new thread when possible
    
    if (j - i > SORT_THRESHOLD) {           /* if the sort range is substantial, use quick sort */

        int m = (i + j) >> 1;               /* pick pivot as median of         */
        char* temp, *pivot;                 /* first, last and middle elements */
        if (strcmp(array[i],array[m]) > 0) {
            temp = array[i]; array[i] = array[m]; array[m] = temp;
        }
        if (strcmp(array[m],array[j]) > 0) {
            temp = array[m]; array[m] = array[j]; array[j] = temp;
            if (strcmp(array[i],array[m]) > 0) {
                temp = array[i]; array[i] = array[m]; array[m] = temp;
            }
        }
        pivot = array[m];

        for (;;) {
            while (strcmp(array[i],pivot) < 0) i++; /* move i down to first element greater than or equal to pivot */
            while (strcmp(array[j],pivot) > 0) j--; /* move j up to first element less than or equal to pivot      */
            if (i < j) {
                char* temp = array[i];      /* if i and j have not passed each other */
                array[i++] = array[j];      /* swap their respective elements and    */
                array[j--] = temp;          /* advance both i and j                  */
            } else if (i == j) {
                i++; j--;
            } else break;                   /* if i > j, this partitioning is done  */
        }

        SortParams first;  first.array = array; first.left = left; first.right = j;

        // Test if a new thread can be created
        if (activeThreads < maximumThreads && totalThreads < 40){
            
            // Wait for mutex and lock
            if (pthread_mutex_lock(&mutex) != 0){
                fprintf(stderr, "Error: %s\n", strerror(errno));
                exit(EXIT_FAILURE);
            };
            
            // 2nd Test for thread creation
            if (activeThreads < maximumThreads){
                activeThreads++;
                totalThreads++;

                // Unlock mutex
                if (pthread_mutex_unlock(&mutex) != 0){
                    fprintf(stderr, "Error: %s\n", strerror(errno));
                    exit(EXIT_FAILURE);
                };

                // Create new thread
                if (pthread_create(&sThreadNew, NULL, (void *)threadSort, (void *)&first) != 0){
                    fprintf(stderr, "Error: %s\n", strerror(errno));
                    exit(EXIT_FAILURE);
                };
                // Set flag for pthread_join
                flag = 1;
            }
            // if 2nd thread test fails
            else {
                // unlock mutex and recursively call threadsort
                if (pthread_mutex_unlock(&mutex) != 0){
                    fprintf(stderr, "Error: %s\n", strerror(errno));
                    exit(EXIT_FAILURE);
                };
                threadSort(&first);
            }
        }
        // if 1st thread test fails
        else threadSort(&first);

        SortParams second; second.array = array; second.left = i; second.right = right;
        threadSort(&second);
                
    } 
    else insertSort(array,i,j);           /* for a small range use insert sort */

    if (flag == 1){
        // If new thread spawned, wait for child thread to finish
        if (pthread_join(sThreadNew, NULL) != 0){
            fprintf(stderr, "Error: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        activeThreads--;
    }

}

void setSortThreads(int count) {
    maximumThreads = count;
}

void sortThreaded(char** array, unsigned int count) {
    SortParams parameters;
    parameters.array = array; parameters.left = 0; parameters.right = count - 1;

    // Initialize mutex
    if (pthread_mutex_init(&mutex, NULL) != 0){
        fprintf(stderr, "Error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    pthread_t sThread;

    activeThreads++;
    totalThreads++;

    // Create new parent thread with threadSort, which waits for all child threads
    if (pthread_create(&sThread, NULL, (void *)threadSort, (void *)&parameters) != 0){
        fprintf(stderr, "Error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    };

    // Wait for parent thread to finish
    if (pthread_join(sThread, NULL) != 0){
        fprintf(stderr, "Error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    activeThreads--;

    // Free Mutex
    if (pthread_mutex_destroy(&mutex) != 0){
        fprintf(stderr, "Error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    };
}