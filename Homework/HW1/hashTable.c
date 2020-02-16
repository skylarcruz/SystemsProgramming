#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hashTable.h"
#include "crc64.h"

// Table Constructor
Table *newTable(int size) {
	Table *newT = malloc(sizeof(Table)); //allocate memory for new Table
	newT->collisions = 0; //set collision count to 0
	newT->tSize = size; //set arraysize
	newT->nodeCount = 0;
	newT->buckets = calloc(newT->tSize, sizeof(Node*)); //allocate memor for bucket pointers
	for (int i = 0; i < size; i++){
		newT->buckets[i] = NULL; //set bucket pointers to NULL
	}
	return newT;
}

// Free an entire table including all nodes in linked List
void freeTable(Table *T){

	// find and free Nodes
	for (int i = 0; i < T->tSize; i++){
		Node *currNode = T->buckets[i];
		Node *fNode;
		while (currNode != NULL) {
			fNode = currNode;
			currNode = currNode->next;
			freeNode(fNode);
		}
	}

	// free Table
	free(T->buckets);
	free(T);
}

// Insert a char into a Hash table
void charInsert(Table *T, char *D){
	int key = hash(D, T->tSize); // get Hashkey

	// if data doesn't exist, add to beginning of list
	if (tableSearch(T, D, key) == NULL) {
		Node *newTableNode = newNode(D);
		nodeInsert(T, newTableNode, key); // Insert Node in table bucket
	}

	// if too many collisions, rehash table (avg. 5 collissions per bucket)
	if (T->collisions > (5 * T->tSize)){
		reHash(T);
	}
}

// Sorts Hashtable by total number of char* occurences 
// and prints out c number of top occuring Nodes
void sortAndPrintHashTable(Table *T, int c) {
	int i = 0;
	int j = 0;
	if (c == 0){ // if c is not given as argument, print all Nodes
		c = T->nodeCount;
	}
	Node *currNode;
	Node *nArray[T->nodeCount]; // set array to number of Nodes in Table

	//Place all nodes in Table in Array
	for (i; i < T->tSize; i++) {
		currNode = T->buckets[i];
		if (currNode != NULL) {
			while (currNode != NULL) {
				nArray[j] = currNode;
				j += 1;
				currNode = currNode->next;
			}
		}
	}

	// qsort Nodes in array by occurence count using compare
	qsort(nArray, T->nodeCount, sizeof(Node*), compare);

	// print c number of top occuring char*
	for (i = 0; i < c; i++){
		printf("%10d %s\n", nArray[i]->count, nArray[i]->data);
	}
}

// Node constructor, initializes with given data, 1 count, and null pointer
static Node *newNode(char *d){
	Node *newN = malloc(sizeof(Node)); // allocate memory for new Node
	newN->data = strdup(d);
	newN->count = 1;
	newN->next = NULL;
	return newN;
}

// Free an individual Node
static void freeNode(Node *N){
	free(N->data);
	free(N);
}

// Given a data item, returns a hash key
static unsigned long long hash(char *data, int bucketSize) {
	return crc64(data) % bucketSize;
}

// Given a Node and it's hashkey, insert into the table at beginning of bucket
static void nodeInsert(Table *T, Node *N, int K){
		Node *firstNode = T->buckets[K];

		//if bucket not Null
		if (firstNode != NULL){
			T->collisions += 1; // increment collisions
			N->next = firstNode; //set new node pointer to first node in bucket
		}

		//else set new pointer to NULL
		else{
			N->next = NULL;
		}
		T->buckets[K] = N; //set bucket pointer to new node
		T->nodeCount += 1;
}

// Search table for existence of data
static char *tableSearch(Table *T, char *D, int K){
	Node *searchNode = T->buckets[K]; //set node pointer to beginning of bucket

	while (searchNode != NULL){ // while the node exists
		if (strcmp(searchNode->data, D) == 0){ // if the node data matches the search data
			//T->collisions += 1; // increment table collisions
			searchNode->count += 1; //increment Node occurrence count
			return searchNode->data;
		}
		searchNode = searchNode->next; // go to next node in linked list
	}
	return NULL;
}

// rehash table when collisions go over load
static void reHash(Table *T){
	int newSize = T->tSize * 5;
	Table *temp = newTable(newSize);
	int i = 0;
	int key;
	Node *currNode;;
	Node *nextNode;

	// find all nodes in original table, find new key, insert into new table
	for (i; i < T->tSize; i++) {
		currNode = T->buckets[i];
		if (currNode != NULL) {
			while (currNode != NULL) {
				nextNode = currNode->next;
				key = hash(currNode->data, newSize);
				nodeInsert(temp, currNode, key);
				currNode = nextNode;
			}
		}
	}

	free(T->buckets); //free old pointer array
	T->buckets = temp->buckets; //set table pointer array to new pointer array
	T->collisions = temp->collisions; //set table collisions to new collision count
	T->tSize = newSize; //set table size to new size
	T->nodeCount = temp->nodeCount;
	free(temp); //free the temporary table
}

// compare function for qsort
static int compare (const void * a, const void * b){
	Node *nodeA = *(Node **)a;
  	Node *nodeB = *(Node **)b;

	return ( nodeB->count - nodeA->count );
}
