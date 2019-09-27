#ifndef XOR_LL_H_
#define XOR_LL_H_

#ifndef DEBUG_XOR_LL
#define DEBUG_XOR_LL 0		// change this to view debug information on the XOR linked list
#endif

#include <stdlib.h>
#include <stdio.h>


typedef struct xNode{
	unsigned long address_ptr;		// this address pointer store prev_address XOR next address 
	void* data;
}xNode;

typedef struct xLinkedList{
	xNode* tail;	// this is a pointer to the youngest (most recently added) node on the list
	xNode* head; 	// this is a pointer to the oldest (least recently added) node on the list
}xLinkedList;


void init_xLinkedList(xLinkedList* list);
void add_node(void* data, xLinkedList* list);
void traverse_list(xLinkedList* list);
void delete_head_func(xLinkedList* list);
void delete_tail_func(xLinkedList* list);
void delete_end(xLinkedList* list, int delete_head);
xNode* pop_node_stack(xLinkedList* list);
xNode* pop_node_queue(xLinkedList* list);
void reverse(xLinkedList* list);

#endif
