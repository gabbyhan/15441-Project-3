// Priority Queue using Linked List 
#include <stdio.h> 
#include <stdlib.h> 
  
typedef struct node { 
    int data; 
  
    // lower values indicate higher priority 
    int priority; 
  
    struct node* next; 
  
} Node; 
  
// create a new node 
Node* newNode(int d, int p);
  
// return value at the head
int peek_data(Node** head);

// return priority of the head
int peek_priority(Node **head);

// remove element of highest priority from pq
void pop(Node** head);
  
// push function (maintain that head points to highest priority) 
void push(Node** head, int d, int p);
 
//check if pq is empty 
int isEmpty(Node** head); 
  
