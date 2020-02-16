#ifndef HASHTABLE_H
#define HASHTABLE_H

/*  HashTable Module by Skylar Cruz
    ===============================
	Hashtable designed to store char* data. The Hashtable is comprised of
	two structure, the Hashtable and Nodes. The Hashtable is designed with
	seperate chaining, each bucket containing a linked list

	Table:
		buckets: an array of pointers
		tSize: number of buckets in the table
		collisions: number of collisions in the table
		nodeCount: total number of Nodes inserted into all buckets

	Node:
		*data: pointer to some char
		count: number of times *data has been inserted into the table
		*next: pointer to the next Node, forms the linked list

	Current functionality is limited with only a few function available for use.

	To use the hashTable:

		1. Initiate a new Table variable using *newTable(int size)
		2. insert char* data into the table with charInsert(Table *T, char *D)
		3. Data can then be sorted and printed to stdout 
		   using sortAndPrintHashTable(Table *T, int c)
		4. When finished utilizing a hashTable, use freeTable(Table *T) 
		   which will free the table and all Nodes as well from memory.

    Hashtable is dynamic. It will resize itself whenever the number of collisions 
    is equal to 5 times the table size. The resized table will have 5 times as many buckets. 

    All other functions are private and are solely utilized by the 4 functions provided for
    other programs making use of the hashTable.

    ============================================================================
	Program utilizes crc64 for hashing provided by the instructor, Ben McCammish.
*/

typedef struct _node {
	char *data;
	int count;
	struct _node *next;
} Node; // Node hashed by data

typedef struct _table {
	Node **buckets;
	int tSize;
	int collisions;
	int nodeCount;
} Table; // List for holding Nodes of same hash

// public functions
Table *newTable(int size);
void freeTable(Table *T);
void charInsert(Table *T, char *D);
void sortAndPrintHashTable(Table *T, int c);

// private functions
static Node *newNode(char *d);
static void freeNode(Node *N);
static unsigned long long hash(char *data, int bucketSize);
static void nodeInsert(Table *T, Node *N, int K);
static char *tableSearch(Table *T, char *D, int K);
static void reHash(Table *T);
static int compare (const void * a, const void * b);

#endif
