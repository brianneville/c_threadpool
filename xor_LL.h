#ifndef XOR_LL_H_
#define XOR_LL_H_

#ifndef DEBUG_XOR_LL
#define DEBUG_XOR_LL 0
#endif

#include <stdlib.h>
#include <stdio.h>


typedef struct xNode{
	unsigned long address_ptr;		// this address pointer store prev_address XOR next address 
	void* data;
}xNode;

typedef struct xLinkedList{
	xNode* tail;
}xLinkedList;


void add_node(void* data, xLinkedList* list){

	xNode* new_xNode = (xNode* )malloc(sizeof(xNode));
	new_xNode->data = data;
	
	unsigned long prev_addr = (unsigned long) list->tail;
	unsigned long next_addr = (unsigned long)(new_xNode);
	new_xNode->address_ptr = prev_addr;    
	
	if(!list->tail){
		if(DEBUG_XOR_LL)printf("adding first xNode\n");
		new_xNode->address_ptr = (unsigned long)NULL;
		list->tail = new_xNode;
	}
	else if(!list->tail->address_ptr){
		if(DEBUG_XOR_LL)printf("adding second xNode\n");     
		list->tail->address_ptr = next_addr; 
	}
	else{
		if(DEBUG_XOR_LL)printf("adding a new xNode\n");
		list->tail->address_ptr = next_addr ^ list->tail->address_ptr;
	}
	
	list->tail = new_xNode;	
}


void traverse_list(xNode* tail){
	//start at tail and traverse in reverse
	xNode* curr = tail;
	xNode* next;
	xNode* prev = NULL;
	printf("Traversing list in reverse\n");
	if(!curr)printf("this list is empty\n");
	while (curr){
		printf ("data is: %p \n", curr->data);
		next = (xNode*)((unsigned long)prev ^ (unsigned long)curr->address_ptr);
		printf ("prev is %p, curr is %p, next is %p\n",prev, curr, next);
		prev = curr;
		curr = next;
	}
	
}

void delete_tail(xLinkedList* list){
	//deletes the xNode for which next is NULL;
	// note this is done from the perspecive of traversing the list in reverse as before
	if (!list->tail){
		//list empty
		if(DEBUG_XOR_LL)printf("list empty. unable to delete\n");
	}	
	else if(!list->tail->address_ptr){	
		if(DEBUG_XOR_LL)printf("list contains only one xNode. This is being deleted\n");     
		list->tail = NULL;
		free(list->tail);
	}else {
		xNode* curr = list->tail;
		xNode* next;
		xNode* prev = NULL;
		if(DEBUG_XOR_LL)printf("list contains more than one xNode. deleting tail\n");
		next = (xNode*)((unsigned long)prev ^ (unsigned long)curr->address_ptr);
		//go to next xNode
		//set address_ptr at next xNode to point to the address of the next xNode it needs to go to
		//prev will be set to NULL
		prev = curr;
		curr = next;
		next->address_ptr = (unsigned long)((unsigned long)prev ^ (unsigned long)curr->address_ptr);
		list->tail = NULL;
		free(list->tail);
		list->tail = next;
	}
}


xNode* pop_node(xLinkedList* list){
	//returns the tail node of the list
	// note this is done from the perspecive of traversing the list in reverse as before
	xNode* temp = list->tail;
	delete_tail(list);
	return temp;		
}


#endif
