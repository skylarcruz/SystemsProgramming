#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "People.h"

int main(){

	// ================== Part 1 ==================

	Person *H = newPerson("Husband", NULL, NULL);
	assert(H->name == "Husband");
	printf("Test 1: Husband exists - passed\r\n");
	
	Person *W = newPerson("Wife", NULL, NULL);
	assert(W->name == "Wife");
	printf("Test 2: Wife exists - passed\r\n");
	
	Person *C = newPerson("Child" , H, W);
	assert(C->name == "Child");
	assert(C->father == H);
	assert(C->mother == W);
	printf("Test 3: Child exists and has a mother and father - passed\r\n");
	
	marryPeople(H, W);
	assert(H->spouse == W);
	assert(W->spouse == H);
	printf("Test 4: Husband and Wife have Been Married - passed\r\n");

	divorcePeople(H);
	assert(H->spouse == NULL);
	assert(W->spouse == NULL);
	printf("Test 5: Husband and Wife have Been Divorced - passed\r\n");

	deletePerson(C);
	deletePerson(H);
	deletePerson(W);

	// ================== Part 2 ==================

	Person *p1 = newPerson("p1", NULL, NULL);
	Person *p2 = newPerson("p2", NULL, NULL);
	Person *p3 = newPerson("p3", NULL, NULL);

	printf("test1\r\n");

	Node *n1 = newNode(p1);
	Node *n2 = newNode(p2);
	Node *n3 = newNode(p3);

	Node *n4 = newNode(p3);
	Node *n5 = newNode(p2);
	Node *n6 = newNode(p1);

	printf("test2\r\n");

	Node *List1 = newNode(NULL);
	Node *List2 = newNode(NULL);
	Node *List3 = newNode(NULL);

	printf("test3\r\n");

	addNodeToList(n1, List1);
	addNodeToList(n2, List1);
	addNodeToList(n3, List1);

	addNodeToList(n1, List2);
	addNodeToList(n2, List2);
	addNodeToList(n3, List2);

	addNodeToList(n4, List3);
	addNodeToList(n5, List3);
	addNodeToList(n6, List3);

	printf("test4\r\n");

	//PrintPersonList(List1);
	//PrintPersonList(List2);

	CompareEqual(List1, List2);

	//PrintPersonList(List3);

	CompareNotEqual(List1, List3);

	deletePerson(p1);
	deletePerson(p2);
	deletePerson(p3);

	FreeNode(n1);
	FreeNode(n2);
	FreeNode(n3);
	FreeNode(n4);
	FreeNode(n5);
	FreeNode(n6);
	FreeNode(List1);
	FreeNode(List2);
	FreeNode(List3);

	printf("test5\r\n");

	return 0;

}