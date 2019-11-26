#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<ctype.h>
#include "trie.h"
#include "stackType.h"

//MAP MONO ME PINAKA

int TotalWords;  //oles oi lekseis tou keimenou
int TotalUniqueWords;  //oles oi diaforetikes lekseis tou keimenou
int avgdl;   //meso plhthos leksewn se kathe document


int main(int argc,char *argv[]){
	int N = 0, i,id, ofst;
	char **Arr;
	char *line = NULL, ch = 'a';
	size_t len = 0, read;
	FILE *fp;
	char *tok;
	char *defaultFile = "testFile.txt";
	char *docFile;
	
	TotalWords = 0;
	TotalUniqueWords = 0;
	
	/***elegxos twn orismatwn pou dothikan kata thn ektelesh***/
	if( argc == 3 ){ 
		if( strcmp(argv[1], "-i") != 0 ){
			printf("wrong arguments in main\n");
			return -1;
		}
		docFile = malloc( strlen(argv[2])*sizeof(char) );
		strcpy(docFile,argv[2]);
		printf("docFile = %s\n",docFile);
	}
	else if( argc == 5){
		if( strcmp(argv[1], "-i") == 0 ){
			if( strcmp(argv[3], "-k") == 0 ){
				K = atoi( argv[4] );
				if( K == 0 ){
					printf("Wrong arguments\n");
					return -1;
				}
				docFile = malloc( strlen(argv[2])*sizeof(char) );
				strcpy(docFile,argv[2]);	
			}
		}
		else if( strcmp(argv[1], "-k") == 0){
			if( strcmp(argv[3], "-i") == 0 ){
				K = atoi( argv[2] );
				if( K == 0 ){
					printf("Wrong arguments\n");
					return -1;
				}
				docFile = malloc( strlen(argv[4])*sizeof(char) );
				strcpy(docFile,argv[4]);
			}
		}
	}
	else{
		printf("Too few arguments in main\n");
		return -2;
	}
	/****/
	fp = fopen(docFile, "r");
	if( fp == NULL ){
		perror("Error: ");
		return -1;
	}
	
	while(1){      //Prwth sarwsh tou arxeiou gia na vroume to plhthos twn documents se auto
		if( getline(&line, &len,fp) != -1) N++;
		if( feof(fp) ) break;
	}
	free(line);
	fclose(fp);
	printf("Documens: %d\n",N);
	
	fp = fopen(docFile, "r");
	if( fp == NULL ){
		perror("Error: ");
		return -1;
	}

	Arr = malloc(N*sizeof(char*) );  //Desmeush mnhmhs gia to MAP
	len = 0;
	for(i = 0; i < N; i++){//Deuterh sarwsh tou arxeiou kai apothikeush twn documents se enan pinaka
		if( (read = getline(&line, &len,fp)) != -1){
			/////////////////
			if( strcmp(line, "\n") == 0) exit(2);
			ofst = 0;
			while( isspace( *(line+ofst) ) ) ofst++;  //"trwei" ta kena pou uparxoun prin kai meta to id tou document
			
			while( !isspace( *(line+ofst) ) ) ofst++;
			
			while( isspace( *(line+ofst) ) ) ofst++;
			/////////////////
			//printf("i = %d -- ofst = %d\n",i,ofst);
			Arr[i] = malloc( ((read-ofst)+1)*sizeof(char) );
			
			memset(Arr[i], '\0', read-ofst);
			strncpy( Arr[i], line+ofst, read-ofst-1 );
			tok = strtok(line,"\t ");
			id = atoi(tok);
			//printf("Arr[%d] = %s\n",i,Arr[i]);
			if( id != i ){
				printf("Error: Invalid id in document\n");
				for(int j; j <= i; j++) free( Arr[j]);
				free( Arr );
				fclose(fp);
				return -2;
			}
			
		}
		
		if( feof(fp) ) break;
	}
	//printf("\nEktupwsh komvwn mesw pinaka\n\n");
	//for(i = 0; i < N; i++){
		//printf("Epanalhpsh %d: Document = %s\n",i,Arr[i]);
	//}
	
	/*--------------------EISAGWGH TWN LEKSEWN STO TRIE------------------------------*/
	TrieNode *root, *current, *newNode, *checkNode;
	char *tmp, *DelTmp;
	int lineS, flag = 0, newWordFlg = 0, rootFlg = 1;
	lineS = strlen(Arr[0]);

	for(i = 0; i < N; i++){ //Eisagwgh twn leksewn kathe document sto Trie
		tmp = Arr[i];
		while( *tmp != '\0' ){
			
			if( flag == 0 ){//prwto gramma (dhmiourgia rizas)
				root = newTrieNode(*tmp);
				current = root;
				flag = 1;
				tmp = tmp+1;
				continue;
			}
			if( isspace(*tmp) ){ //an vrethei kenos xarakthras shmainei oti exoume nea leksh
				current->final = 1; //ara ara prepei na thesoume ton prohgouno komvo ws termatiko 
									//kai na dhmiourghsoume h na ananewsoume(an h leksh hdh uparxei) thn Posting List
				tmp = tmp + 1;
				newWordFlg = 1;
				//update Posting List
				if( current->PsHead == NULL ){
					PostingListHead *newHeadPL;
					newHeadPL = newPL_Head(i);
					current->PsHead = newHeadPL;
					TotalUniqueWords++;
				}
				else updatePostingList(current->PsHead, i);
				TotalWords++;
				current = root;
				while( isspace( *tmp ) ) tmp = tmp + 1; //an metaksu 2 leksewn uparxoun perissotera kena ta agnooume
				continue;			
			}
			/********************/
			checkNode = current;
			while( checkNode != NULL ){
				if( *tmp == checkNode->letter ){	 //psaxnei na vrei an to gramma pros eisagwgh uparxei hdh  
					if( !isspace( *(tmp+1) ) && *(tmp+1) != '\0' ){
						if( checkNode->same_word != NULL) current = checkNode->same_word;
						else{ //an exoun vrethei koina arxika grammata me uparxousa leksh h opoia einai mikroterh dhmiourgeitai edw neos komvos
							if( newWordFlg == 1) tmp = tmp + 1;
							newNode = newTrieNode(*tmp);
							checkNode->same_word = newNode;
							current = newNode;
							newWordFlg = 0;
						}
					}
					else current = checkNode;
					tmp = tmp + 1;
					break;
				}
				current = checkNode;
				checkNode = checkNode->other_word;
			}
			if(checkNode != NULL) continue;
			/********************/
			newNode = newTrieNode(*tmp); //dhmiourgia neou komvou
			if( newWordFlg == 1 ){ //An to grama pou molis eisaxthike einai gramma neas lekshs to sundeoume me ton deksio deikth(other_word)
				current->other_word = newNode;
				newWordFlg = 0;
			}
			else current->same_word = newNode; //alliws ton sundeoume me ton katw deikth (same_word)
			current = newNode;	
			tmp = tmp + 1;
		}
		current->final = 1;
		if( current->PsHead == NULL ){//update posting list
					PostingListHead *newHeadPL;
					newHeadPL = newPL_Head(i);
					current->PsHead = newHeadPL;
					TotalUniqueWords++;
		}
		else updatePostingList(current->PsHead, i);
		TotalWords++;
		current = root;
		newWordFlg = 1;
		
		
	}
	
	printf("\n------------\n\n");
	//PsListCheck(root);
	/****** TEST GIA ENTOLH /SEARCH ******/
	printf("COMMANDS:\n /search <word1> <word2>... OR\n /tf <id> <word> OR \n /df [<word>] OR\n /exit\n\n");
	size_t chars, Len = 0;
	char *Line, *Tok;
	char *temp;
	char *searchTmp = "/search";
	int words = 0;
	//char termChar = '\0';
	
	chars = getline(&Line, &Len, stdin );
	temp = malloc( (chars)*sizeof(char) );
	strncpy(temp, Line, chars-1);
	memset(temp+chars-1,'\0', 1); 
	free(Line);
	avgdl = TotalWords/N;	
	
	Tok = strtok(temp, "\t ");
	
	if( strcmp( Tok, "/search" ) == 0 ){
		Tok = strtok(NULL, " ");
		searchCommandWithScore(Tok, root, Arr, N);
	}
	else if( strcmp( Tok, "/tf") == 0 ){
		Tok = strtok(NULL, " ");
		tfCommand(Tok, root);
	}
	else if( strcmp( Tok, "/df") == 0 ){
		Tok = strtok( NULL, " ");
		dfCommand(root, Arr, N, Tok );
	}
	else if( strcmp( Tok, "/exit") == 0 ) printf("Now will exit\n");
	else printf("Invalid command\n");
	free(temp);
	
	/*********Test gia thn /tf  ********************/
	
	printf("\n-----------TotalWords = %d----------\n", TotalWords);
	printf("\n-----------TotalUniqueWords = %d----\n", TotalUniqueWords);
	
	//printf("TEST: printing all words\n\n");
	//printAllwords(root);
	/***********************************************/
	TrieDelete(root);	
	
	/*--------------------------------------------------*/
	
	
	//FREE
	for(i = 0; i < N; i++) free(Arr[i]);
	free(Arr);
	fclose(fp);
	return 0;
}
