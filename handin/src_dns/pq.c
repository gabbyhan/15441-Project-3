// Priority Queue using Linked List 
#include "pq.h"

// create a new node 
Node* newNode(int d, int p) 
{ 
    Node* temp = (Node*)malloc(sizeof(Node)); 
    temp->data = d; 
    temp->priority = p; 
    temp->next = NULL; 
  
    return temp; 
} 
  
// return value at the head
int peek_data(Node** head) 
{ 
    return (*head)->data; 
} 

// return priority of the head
int peek_priority(Node **head)
{
    return (*head)->priority;
}

// remove element of highest priority from pq
void pop(Node** head) 
{ 
    Node* temp = *head; 
    (*head) = (*head)->next; 
    free(temp); 
} 
  
// push function (maintain that head points to highest priority) 
void push(Node** head, int d, int p) 
{ 
    Node* start = (*head); 
  
    // the node we are adding 
    Node* temp = newNode(d, p); 
  
    // head of list has lesser priority than new node
    // or priority queue is empty and head is null
    if((*head)==NULL || (*head)->priority > p) { 
        // Insert New Node before head 
        temp->next = *head; 
        (*head) = temp; 
    } 
    else { 
        while (start->next != NULL && 
               start->next->priority < p) { 
            start = start->next; 
        } 
        temp->next = start->next; 
        start->next = temp; 
    } 
} 
 
//check if pq is empty 
int isEmpty(Node** head) 
{ 
    return (*head) == NULL; 
} 
  
