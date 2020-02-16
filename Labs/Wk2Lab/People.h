#ifndef PEOPLE_H_
#define PEOPLE_H_

typedef struct _person{
char *name;
struct _person *father;
struct _person *mother;
struct _person *spouse;
} Person;

Person* newPerson(char *name, Person *F, Person *M);
void marryPeople(Person *p1, Person *p2);
void divorcePeople(Person *p);
void deletePerson(Person *p);

// ================== Part 2 ==================

typedef struct node{
Person *p;
struct node *NodeNext;
} Node;

Node* newNode(Person *NodeP);
void addNodeToList(Node *N, Node *L);
void FreeNode(Node *N);
void PrintPersonList(Node *L);

void CompareEqual(Node *L1, Node *L2);
void CompareNotEqual(Node *L1, Node *L2);

#endif // PEOPLE_H_