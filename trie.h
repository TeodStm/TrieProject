#ifndef TRIE_H
#define TRIE_H


extern int TotalWords;
extern int avgdl;
extern int K;

typedef struct PostingList{
	int id_doc;
	int freq_on_doc;
	struct PostingList *next;
}PostingList;

typedef struct PostingListHead{
	int count;
	int mark;
	PostingList *start;
}PostingListHead;

typedef struct TrieNode{
	char letter;
	struct TrieNode *other_word;
	struct TrieNode *same_word;
	struct PostingListHead *PsHead;
	int final; 
}TrieNode;


TrieNode* newTrieNode(char);
int destroyTrieNode( TrieNode* );

void updatePostingList(PostingListHead* , int);
PostingListHead* newPL_Head(int );

void searchCommandWithScore(char*, TrieNode*, char**, int);
void Score(double scores_arr[], int N, char **arr, PostingListHead * PLhead);
int wordsInDoc(char **arr, int N, int docId);

void tfCommand(char*, TrieNode*);

PostingListHead* searchWordPL( char *, TrieNode *);

void dfCommand(TrieNode*, char** , int, char* );
void TrieDelete(TrieNode *Root);
void PostingListDelete( PostingListHead* PLhead );

void quickSort( int A[], double scores[], int left, int right);
int partition( int a[], double scores[], int left, int right);
#endif
