#include "People.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

Person* newPerson(char *name, Person *F, Person *M){
	Person *p;
	p = (Person *) malloc(sizeof (Person));
	p->name = name;
	p->father = F;
	p->mother = M;
	p->spouse = NULL;
}

void marryPeople(Person *p1, Person *p2){
	p1->spouse = p2;
	p2->spouse = p1;
}

void divorcePeople(Person *p){
	Person* pSpouse = p->spouse;
	p->spouse = NULL;
	pSpouse->spouse = NULL;
}

void deletePerson(Person *p){
	if (p->spouse != NULL) {
		Person* pSpouse = p->spouse;
		pSpouse->spouse = NULL;
	}
	free(p);
}

// ================== Part 2 ==================

Node* newNode(Person *NodeP){
	Node *n;
	n = (Node *) malloc(sizeof (Node));
	n->p = NodeP;
	n->NodeNext = NULL;
}

void addNodeToList(Node *N, Node *L){
	Node *oldHead = L->NodeNext;
	L->NodeNext = N;
	N->NodeNext = oldHead;
}

void FreeNode(Node *N){
	free(N);
}

void PrintPersonList(Node *L){
	Node *currNode;
	currNode = L->NodeNext;
	while (currNode != NULL) {
		printf("%s", currNode->p->name);
		printf("\r\n");
		currNode = currNode->NodeNext;
	}
}

void CompareEqual(Node *L1, Node *L2){
	Node *currNode1 = L1->NodeNext;
	Node *currNode2 = L2->NodeNext;
	while (currNode1 != NULL || currNode2 != NULL){
		assert(currNode1 == currNode2);
		currNode1 = currNode1->NodeNext;
		currNode2 = currNode2->NodeNext;
	}
	printf("Test Passed: Lists are equal\r\n");
}

void CompareNotEqual(Node *L1, Node *L2){
	Node *currNodeA = L1->NodeNext;
	Node *currNodeB = L2->NodeNext;
	int equal;
	equal = 1;
	while (currNodeA != NULL || currNodeB != NULL){
		if (currNodeA != currNodeB){
			equal = 0;
		}
		currNodeA = currNodeA->NodeNext;
		currNodeB = currNodeB->NodeNext;
	}
	if (equal == 0) {
		printf("Test Passed: Lists are not equal\r\n");
	}
	else {
		printf("Test Failed: Lists are equal\r\n");
	}
}
