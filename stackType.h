#ifndef STACKTYPE_H
#define STACKTYPE_H


typedef struct stackNode{
	struct TrieNode *TrNode;
	struct stackNode *Next;
	int direction;
	
}stackNode;

typedef struct StackHead{
	struct stackNode *start;
	int count;
}StackHead;

void push(struct TrieNode *, StackHead *, int );
struct TrieNode* pop( StackHead * , int * );
int isempty( StackHead*  );

#endif
