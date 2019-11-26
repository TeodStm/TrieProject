#include<math.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include<sys/ioctl.h>
#include<unistd.h>
#include "trie.h"
#include "stackType.h"

int K = 10;

/************/

TrieNode* newTrieNode(char ltr){ //Dhmiourgia enos neou komvou tou TRIE kai epistrofh enow deikth se auton
	TrieNode *newNode;
	newNode = malloc( sizeof(struct TrieNode) );
	newNode->letter = ltr;
	newNode->final = 0;
	newNode->same_word = NULL;
	newNode->other_word = NULL;
	newNode->PsHead = NULL;
	return newNode;
}

int destroyTrieNode( TrieNode *toDelNode ){
	if( toDelNode == NULL ){
		printf("Error:Null node was given to delete\n");
		return -1;
	}
	free(toDelNode);
	printf("A Trie node was deleted succesfully!\n");
	return 1;
}

PostingListHead* newPL_Head(int doc_id){ //Dhmiourgia mias kefalhs Posting List (PostingListHead) kai epistrofh enos deikth se authn
	PostingListHead *newPLH;			 //Ektos apo thn kefalh ftiaxnei kai ton prwto komvo ths listas ton opoio kai arxikopoiei
	PostingList *PLnode;
	
	newPLH = malloc( sizeof( struct PostingListHead ) );
	newPLH->mark = 0;
	newPLH-> count = 1;
	newPLH->start = NULL;
	PLnode = malloc( sizeof( struct PostingList ) );
	PLnode->id_doc = doc_id;
	PLnode->freq_on_doc = 1;
	PLnode->next = NULL;
	
	newPLH->start = PLnode;
	return newPLH;
}

void updatePostingList(PostingListHead *PL_head, int docID){ //Ananewsh mias uparxousas Posting List 
	PostingList *currPL;
	int i;
	currPL = PL_head->start;
	
	for( i = 0; i < PL_head->count; i++){ //Se periptwsh pou to docID uparxei hdh sth lista tote apla auksanetai h suxnothta emfanhshs ths lekshs
		if( currPL->id_doc == docID ){
			currPL->freq_on_doc = currPL->freq_on_doc + 1;
			return;
		}
		if( i != PL_head->count - 1 ) currPL = currPL->next;
	}
	PostingList *newPL;   //Diaforetika dhmiourgeitai enas neos komvos PostingList me tis katallhles times
	newPL = malloc( sizeof( struct PostingList) );
	newPL->id_doc = docID;
	newPL->freq_on_doc = 1;
	newPL->next = NULL;
	currPL->next = newPL;
	PL_head->count = PL_head->count + 1;
}

void PostingListPrint(TrieNode *node){
	
	if( node->final != 1 ){
		printf("Not final node\n");
		return;
	}
	
	PostingList *curr;
	curr = node->PsHead->start;
	for(int i = 0; i < node->PsHead->count;  i++){
		printf("[ %d, %d ] , ",curr->id_doc, curr->freq_on_doc);
		curr = curr->next;
	}
	printf("\n");
	
}
/*********** < TF > ***********/
void tfCommand(char *Token, TrieNode *Root){ //Sunarthsh gia thn entolh /tf
	
	TrieNode *curr;
	char *tmp, *toDel;
	int id;

	id = atoi(Token);	
	
	if( id == 0 && strcmp( Token, "0" ) != 0 ){//an den dwthei aritmos san prwto orisma sthn /tf epistrefei lathos
		printf("No document id given on /tf\n");
		return;
	}
	Token = strtok(NULL, "\n");
	if( Token == NULL ){
		printf("No word for search given on /tf\n");
		return;
	}	
	tmp = malloc( strlen(Token)*sizeof(char) );
	if( tmp == NULL ) return;
	
	strcpy(tmp,Token);
	toDel = tmp;
	curr = Root;
	while( curr != NULL ){ //Edw psaxnei na vrei th leksh pou dothike kai an vrethei koitaei thn Posting List auths ths lekshs
		if( curr->letter == *tmp ){
			if( *(tmp+1) == '\0' && curr->final == 1){				
				PostingList *PL;
				PL = curr->PsHead->start;
				for( int i = 0; i < curr->PsHead->count; i++){
					if( id == PL->id_doc){  //An vrethei to document pou zhteitai me th sugkekrimenh leksh emfanizoume th suxnotha ths se auto to document
						printf("%d %s %d\n", id, Token, PL->freq_on_doc);
						free(toDel);
						return;
					}
					PL = PL->next;
				}
				printf("\nWord <%s> not found\n",Token); //An h leksh den uparxei sto sugekrimeno document ektupwnetai antistoixo mhnuma
				free(toDel);
				return;
			}
			curr = curr->same_word;
			tmp = tmp + 1;
			continue;
		}
		curr = curr->other_word;		
	}
	if( curr == NULL ){  //An h leksh den uparxei ektupwnetai antistoixo mhnuma
		printf("\nWord <%s> not found\n",Token);
		free(toDel);
		return;
	}	
}

PostingListHead* searchWordPL( char *word, TrieNode *Root){  //H sunarthsh auth psaxnei mia leksh kai epistrefei thn Posting List auths
															//Se ena loop sugkrinoume to gramma enos komvou me ta grammata ths lekshs
															//kai analoga me tis sugkriseis metakinoumaste katallhla sto Trie
	TrieNode *curr;
	char *tmp, *delTmp;
	tmp = malloc( (strlen(word)+1)*sizeof(char) );
	
	strcpy(tmp,word);
	delTmp = tmp;
	curr = Root;
	while( curr != NULL ){
		if( curr->letter == *tmp ){
			if( ( *(tmp+1) == '\0' || isspace(*(tmp+1)) ) && curr->final == 1){ //Otan ta grammata ths lekshs teleiwsoun kai o trexon komvos tou Trie einai telikos
																				//shmainei oti h leksh vrethike
				free(delTmp);
				return curr->PsHead;
			}
			curr = curr->same_word;
			tmp = tmp + 1;
			continue;
		}
		curr = curr->other_word;		
	}
	
	
	if( curr == NULL ){ //An h leksh den vrethei ektupwnetai antistoixo mhnuma
		printf("Word <%s> not found\n",word);
		//getchar();
		free(delTmp);
		return NULL;
	}
	
}

void Score(double scores_arr[], int N, char **arr, PostingListHead * PLhead){ //H sunarthsh upologizei to score me vash ton tupo pou dinetai 
	int i, nq;
	PostingList *PLnode;
	int DocWords, fq;
	double score, IDF, x, k1 = 1.2, b = 0.75;
	double multToIDF;
	
	nq = PLhead->count;
	x = ( (double)N - (double)nq + 0.5 )/( (double)nq + 0.5);
	IDF = log10(x);
	PLnode = PLhead->start;
	
	while( PLnode != NULL ){
		DocWords = wordsInDoc( arr, N, PLnode->id_doc);
		fq = PLnode->freq_on_doc;
		multToIDF = ( (double)fq*(k1 + 1.0) )/((double)fq + k1*( 1.0 - b + b*((double)DocWords/(double)avgdl) ) );
		score = IDF*multToIDF;
		if( scores_arr[PLnode->id_doc] < 0.0 ) scores_arr[PLnode->id_doc] = 0;
		scores_arr[PLnode->id_doc] = scores_arr[PLnode->id_doc] + score;
		PLnode = PLnode->next;
	}	
	
}

int wordsInDoc(char **arr, int N, int docId){ //H sunarthsh auth metraei poses lekseis uparxoun se ena document kai epistrefei ton arithmo pou vrhke
	int DocWords = 0;
	char *tmp;

	tmp = arr[docId];
	while( *tmp != '\0' ){
		while( !isspace( *tmp ) && *tmp != '\0' ) tmp = tmp + 1;
		if( *tmp == '\0' ) break;
		DocWords++;
		while( isspace( *tmp ) ) tmp = tmp + 1;
	}
	DocWords++;
	return DocWords;
}
/**********< SEARCH > *********/

void searchCommandWithScore(char *Token, TrieNode * Root, char** arr, int N){
	int words = 0;
	PostingListHead* PL_Head;
	int *idsArr;
	PostingList *curr;
	double score;
	double doc_scores[N]; 
	int docIDarr[N], docsFound = 0;
	struct winsize w;
	
	
	for( int i = 0; i < N; i++){
		doc_scores[i] = -1.0; //pinakas pou krataei ta score twn documents
		docIDarr[i] = i; //pinakas pou krataei ta ids twn documents kai xrhsimopoieitai mono gia thn ektupwsh twn taksinomhnenwn(me vash to score) documents
	}
	
	while( Token != NULL && words <= 10 ){
		PL_Head = searchWordPL(Token, Root);
		
		if( PL_Head != NULL ){
			Score(doc_scores, N, arr, PL_Head);  //Enhmerwsh tou pinaka twn score documents
			docsFound = docsFound + PL_Head->count;
		}
		Token = strtok(NULL, "\t ");
		words++;
	}
	
	quickSort( docIDarr, doc_scores, 0, N-1 ); //taksinomhsh twn score apo to megalutero sto mikrotero
	
	ioctl(STDOUT_FILENO,TIOCGWINSZ,&w);
	int currCol = 19; //19 einai to plhthos twn xarakthrwn opu ektupwnetai gia tis plhrofories tou document
	
	for(int i = 0; i < K; i++){
		if( doc_scores[i] < 0.0 ) break; //AN vrethikan ligotera apo K documents me vash to query ektpwnontai osa vrethikan , alliws aktupwnontai ta top K
		char *tmp;
		tmp = arr[ docIDarr[i] ];
		printf("%d.( %3.d)[%.4f] ",i,docIDarr[i],doc_scores[i]);
		while( *tmp != '\0'){
			printf("%c",*tmp);
			if( currCol > w.ws_col ){
				printf("\n");
				currCol = 1;
			}
			tmp = tmp +1;
			currCol++;
			
		}
		currCol = 19;
		printf("\n\n");
	}
	
}

/**************QUICK-SORT AND PARTITION**************/

//H taksinomhsh ginetai me vash ton pinaka me ta score alla enallasontai kai ta stoixeia enos pinaka me ta ids gia na exoume antistoixhsh poio score anoikei se poio document
void quickSort( int A[], double scores[], int left, int right){
	int j;

	if( left < right ){
		j = partition( A, scores, left, right);
		quickSort( A, scores, left, j-1);
		quickSort( A, scores, j+1, right);
	}
}

int partition( int a[], double scores[], int left, int right){
	int i, j, t;
	double pivot, tmp;
	pivot = scores[left];
	i = left;
	j = right + 1;
		
	while(1){
		do ++i; while( scores[i] > pivot && i <= right ); 
		do --j; while( scores[j] < pivot ); 
		if( i >= j ) break;
		tmp = scores[i];
		scores[i] = scores[j];
		scores[j] = tmp;
		
		t = a[i];
		a[i] = a[j];
		a[j] = t;
	}
	tmp = scores[left];
	scores[left] = scores[j];
	scores[j] = tmp;
	
	t = a[left];
	a[left] = a[j];
	a[j] = t;
	return j;
}
/******************* < DF > *********************************/

void dfCommand(TrieNode * Root, char** arr, int N, char *word){
		TrieNode *curr;
		PostingListHead *PLH;
		int i;
		char *temp;
		
		if( word != NULL ){ //Periptwsh pou dwthei mia sugkekrimenh leksh sthn /df
			PLH = searchWordPL(word, Root);
			if( PLH != NULL ){
				printf("%s %d\n",word, PLH->count);
			}
			return;
		}
		for(i = 0; i < N; i++){  //Alliws koitaei gia kathe leksh kai ektupwnei sthn suxnothta emfanishs sto arxeio
				temp = malloc( (strlen(arr[i])+1)*sizeof(char) );
				strcpy(temp, arr[i]);
				char* token = strtok(temp, "\t\f\v\r ");
				
				while( token != NULL){
					PLH = searchWordPL(token, Root);
					if( PLH == NULL ){
						token = strtok(NULL, "\t\f\v\r ");
						continue;
					}
					else{
						if( PLH->mark == 0 ){
							printf("%s %d\n",token, PLH->count);
							PLH->mark = 1;
							//PLH->mark++;
							//if( PLH->mark == PLH->count ) mark = 0;
						}
					}
					token = strtok(NULL, "\t\f\v\r ");
				}
				free(temp);
				temp = NULL;				
		}
}
/*************/

void TrieDelete(TrieNode *Root){  //H sunarthsh auth pernaei apo olous tous komvous tou Trie me skopo thn apodesmeush mnhmhs
	TrieNode *curr, *tmpRoot, *toDel;
	struct StackHead *stackH;			//Xrhsimopoieitai mia stoiva gia na krataei tous komvous tous opoious episkeuthike kai autoi eixan paidia 
	int rightFlag, downFlag, parent;
	
	stackH = malloc( sizeof( struct StackHead ) );
	stackH->count = 0;
	tmpRoot = Root;
	curr = Root;
	
	do{
		
		while( curr->same_word != NULL || curr->other_word != NULL){
			rightFlag = 0;
			downFlag = 0;
			while(curr->other_word != NULL){
				push(curr, stackH, 2); //2 gia deksia
				curr = curr->other_word;
				rightFlag = 1;
			}
			while(curr->same_word != NULL){
				push(curr, stackH, 1); //1 gia aristera
				curr = curr->same_word;
				downFlag = 1;
			}
			
		}
		toDel = curr;	//Otan ftasoume se komvo pou den exei paidia ton diagrafoume kai meta akolouthoume th diadromh pros ta pisw mesw ths stoivas
						//Otan ftanoume se komvo pou exei paidia pame pali se kapoio fullo kai kanoume thn idia diadikasia
		//printf("Node with letter %c will be deleted\n", toDel->letter);
		if( curr->final == 1 ) PostingListDelete(curr->PsHead);
		
		free(toDel);
		toDel = NULL;
		if( isempty( stackH ) == 1 ){
			printf("h stoiva adeiase --eksodos\n\n");
			
			Root = NULL;
			return;
		}
		curr = pop(stackH, &parent);
		
		if( parent == 1 ) curr->same_word = NULL; //OI arithmoi 1 kai 2 xrhsimeuoun sto na kseroume apo poion komvo(gonea) eixame erthei ston trexon komvo
		else if( parent == 2 ) curr->other_word = NULL;
	}while( isempty(stackH) == 0 );
	
	curr->same_word = NULL;
	curr->other_word = NULL;
	curr = NULL;
	//free(curr);
	free(Root);
	Root = NULL;
}

void PostingListDelete( PostingListHead* PLhead ){ //diagrafei thn Posting List 
	PostingList *toDel;
	
	for( int i = 0; i < PLhead->count; i++){
		toDel = PLhead->start;
		PLhead->start = toDel->next;
		toDel->next = NULL;
		free(toDel);		
	}
	free( PLhead );
	PLhead = NULL;
	
}

/***************/

