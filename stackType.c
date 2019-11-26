#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include "stackType.h"
#include "trie.h"

/*Edw einai mia ulopoihsh stoivas me lista kai xrhsimopoieitai gia thn diagrafh twn komvwn tou Trie*/

void push(TrieNode *trieNode, StackHead *StHead, int dir){  //Eisagwgh enos neou komvou sth stoiva
	stackNode *newNode;
	if( StHead->count == 0 ){
		newNode = malloc( sizeof( struct stackNode ) );
		newNode->TrNode = trieNode;
		newNode->Next = NULL;
		newNode->direction = dir;
		StHead->count = 1;
		StHead->start = newNode;
	}
	else{
		stackNode *current;
		current = StHead->start;
		while( current->Next != NULL ){
			current = current->Next;
		}
		
		newNode = malloc( sizeof( struct stackNode ) );
		newNode->TrNode = trieNode;
		newNode->Next = NULL;
		newNode->direction = dir;
		current->Next = newNode;
		StHead->count++;
	}	
}

TrieNode *pop( StackHead * StHead, int *par ){ //Eksagwgh enos komvou sth stoiva
	stackNode *current, *toDel;
	TrieNode *popItem;
	
	if( StHead->count == 0 ){
		printf("----Stack is empty\n");
		return NULL;
	}
	else if( StHead->count == 1 ){
		toDel = StHead->start;
		popItem = toDel->TrNode;
		*par = toDel->direction;
		free(toDel);
		StHead->start = NULL;
		StHead->count--;
		return popItem;
	}
	current = StHead->start;
	while( current->Next != NULL ){
		if( current->Next->Next == NULL ) toDel = current;
		current = current->Next;
	}
	popItem = current->TrNode;
	//current->TrNode = NULL;/////////
	toDel->Next = NULL;
	toDel = current;
	*par = current->direction;
	free(toDel);
	StHead->count--;
	return popItem;
}

int isempty( StackHead* StHead ){ //Epistrefei 1 an h stoiva einai adeia , 0 an den einai adeia kai -1 se periptwsh pou to count einai arnhtiko
	if( StHead->count == 0 ) return 1;
	else if( StHead->count > 0 ) return 0;
	else return -1;
}
