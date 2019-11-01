#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct node
{
    char val;
    int freq;
    struct node *left;  // left sub-tree
    struct node *right; // right sub-tree
} TreeNode;

typedef struct listnode
{
  struct listnode * next;
  TreeNode * TN;
} ListNode;

int count_freq(char *, int *);
ListNode *list_make(int *, int *);
ListNode *LN_make(TreeNode* , ListNode* );
TreeNode *TN_make(char , int , TreeNode *, TreeNode *);
TreeNode *tree_make(ListNode *);
int calcTH(TreeNode*);
void calcCode(TreeNode * , int * * );
void calcCodeHelper(TreeNode * , int * * , int * , int );
int ** getCode(TreeNode * , int, int);
int countLN(TreeNode * );
void encode(char * , char * , int ** , int * , int , int , int );
int get_hCode(int *, char , int **, int , int );
void killTree(TreeNode*);



void printPreorder(TreeNode* node);
void printList( ListNode* n) ;

void printList( ListNode* n) 
{ 
    while (n != NULL) { 
        printf(" %d %d ", n->TN->val,n->TN->freq); 
        n = n->next; 
    } 
    printf("\n");
} 

void printPreorder(TreeNode* node) 
{ 
     if (node == NULL) 
          return; 
  
     /* first print data of node */
     printf("%d %d\n", node->val,node->freq);   
  
     /* then recur on left sutree */
     printPreorder(node->left);   
  
     /* now recur on right subtree */
     printPreorder(node->right); 
}     



int main(int argc, char **argv)
{
    int freq[256] = {0};
    unsigned int charCount;
    int leafCount = 0;//number of leaf nodes/unique chars
    int treeH; //tree height
    charCount = count_freq(argv[1], freq);

    ListNode *listHead = list_make(freq, &leafCount);

    //printList( listHead) ;
    //printf("%d\n",charCount);
    

    TreeNode *treeHead = tree_make(listHead);

    //printPreorder(treeHead);

    treeH = calcTH(treeHead);

    int ** hCode = getCode (treeHead, treeH, leafCount);
    char*infile = argv[1];
    char outfile[50];
    strcpy(outfile, infile);
    strcat(outfile, ".huff");
    int tHeight = calcTH(treeHead);
    encode(infile, outfile, hCode,  freq, tHeight, leafCount, charCount);


    int i;
    for (i = 0; i < leafCount; i ++)
    {
      free (hCode[i]);
    }
    free (hCode);

    killTree(treeHead);
    return 0;
}

int count_freq(char *inFile, int *freq){
    int count = 0;
    FILE *fptr = fopen(inFile, "r");
    while (!feof(fptr))
    {
        int curChar = fgetc(fptr);
        if (curChar != EOF)
        {
            count++;
            freq[curChar]++;
        }
    }
    fclose(fptr);
    return count;
}

ListNode *LN_make(TreeNode* curTN, ListNode* nextLN){
    ListNode *newNode = malloc(sizeof(ListNode));
    newNode->next=nextLN;
    newNode->TN=curTN;
    return newNode;
}

ListNode *list_make(int *freq, int *leafCount){
    int i = 0;
    while (freq[i] == 0)
    {   
        i++;
    }
    if (i == 256)
    {
        return NULL;
    }

    TreeNode *tempTN = TN_make(((char)i), freq[i], NULL, NULL);
    ListNode *headLN = LN_make(tempTN,NULL);
    i++;
    (*leafCount)++;
    while (i < 256)
    {
        if (freq[i] != 0)
        {   
            TreeNode *newTN = TN_make(((char)i), freq[i], NULL, NULL);
            ListNode *newLN = LN_make(newTN,NULL);
            if (freq[i] <= ((headLN->TN)->freq))
            {
                newLN->next = headLN;
                headLN = newLN;
            }
            else
            {
                ListNode *tempLN = headLN;
                while ((tempLN->next != NULL) && (freq[i] > (((tempLN->next)->TN)->freq)))
                {
                    tempLN = tempLN->next;
                }
                newLN->next = tempLN->next;
                tempLN->next = newLN;
            }
            (*leafCount)++;
        }
        
        i++;
    }
    return headLN;
}

TreeNode *TN_make(char val, int freq, TreeNode *L, TreeNode *R){
    TreeNode *newNode = malloc(sizeof(TreeNode));
    newNode->left = L;
    newNode->right = R;
    newNode->val = val;
    newNode->freq = freq;
    return newNode;
}

TreeNode *tree_make(ListNode *listHead){   
    TreeNode *treeHead = listHead->TN;
    //printList( listHead) ;
    while(listHead->next!=NULL){
            ListNode*killLN = listHead->next;
            TreeNode*treeL = listHead->TN;
            TreeNode*treeR = killLN->TN;
            //printf("%d %d\n", treeL->val,treeR->val);
            int freqL = treeL->freq;
            int freqR = treeR->freq;
            listHead->TN = TN_make(((char)0), (freqL+freqR), treeL, treeR);
            listHead->next = killLN->next;
            free(killLN);
            if(listHead->next!=NULL && ((freqL+freqR) > (((listHead->next)->TN)->freq))){
                    ListNode*newLN = listHead;
                    ListNode *tempLN = listHead->next;
                    listHead = listHead->next;
                    
                    while ((tempLN->next != NULL) && ((freqL+freqR)> (((tempLN->next)->TN)->freq)))
                    {
                        tempLN = tempLN->next;
                    }
                    newLN->next = tempLN->next;
                    tempLN->next = newLN;

            }
            //printList( listHead) ;
    }
    treeHead = listHead->TN;
    free(listHead);
    return treeHead;
}

//calculate the tree height
int calcTH(TreeNode* TN){  
    if (TN == NULL)   
		return 0;  
    else
    {  
        int LH = calcTH(TN->left);  
        int RH = calcTH(TN->right);  
        if (LH > RH)  
			return(LH + 1);  
        else 
			return(RH + 1);  
    }  
}  

//get huffman code
int ** getCode(TreeNode * tHead, int tHeight, int leafCount){

  	int ** hCodes = malloc(sizeof(int*) * leafCount);

	int i, j;
	for(i = 0; i < leafCount; i++)
	{
		hCodes[i] = malloc(sizeof(int) * (tHeight + 1) );
		for(j = 0; j < (tHeight + 1); j++)
		{
			hCodes[i][j] = -1;
            //printf("%d ",hCodes[i][j]);
		}
	}

    calcCode(tHead, hCodes);
	return hCodes;
}

void calcCode(TreeNode * tHead, int * * hCodes)
{
  int row = 0;
  calcCodeHelper(tHead, hCodes, & row, 1); 
}

void calcCodeHelper(TreeNode * TN, int * * hCodes, int * row, int col)
{
  if (TN == NULL)
    {
      return;
    }

  TreeNode * L = TN -> left;
  TreeNode * R = TN -> right;
  if ((L == NULL) && (R == NULL))
    {
      hCodes[*row][0] = TN -> val;
      (* row) ++;
      return;
    }
  if (L != NULL)
    {
      int temp = countLN(L);
      int i;
      for (i = * row; i < ((* row) + temp); i ++)
	{
	  hCodes[i][col] = 0;
	}
      calcCodeHelper(L, hCodes, row, col + 1);
    }
  if (R != NULL)
    {
      int temp = countLN(R);
      int i;
      for (i = * row; i < ((* row) + temp); i ++)
	{
	  hCodes[i][col] = 1;
	}
      calcCodeHelper(R, hCodes, row, col + 1);
    }    
}

int countLN(TreeNode* treeNode) 
{ 
  if(treeNode == NULL)        
    return 0; 
  if(treeNode->left == NULL && treeNode->right==NULL)       
    return 1;             
  else 
    return countLN(treeNode->left)+ countLN(treeNode->right);       
} 




void encode(char * infile, char * outfile, int ** hCode, int * freq, int tHeight, int leafCount, int charCount)
{ 
 // int row;
   /* for (row = 0; row < tHeight; row ++)
    {
      // print the character
      printf("%c: ", hCode[row][0]);
      int col = 1;
      while (hCode[row][col] != -1)
	{
	  printf("%d ", hCode[row][col]);
	  col ++;
	}
      printf("\n");
    }*/



  FILE * inptr = fopen(infile, "r");
  FILE * outptr = fopen(outfile, "wb");
  int i;
  char curChar = 0;
  char outChar = 0;
  int * curCode = malloc(tHeight * sizeof(int));
	int temp[tHeight+8];
	int codeLeng = 0; //length of huff code of current char
	for(i = 0; i < tHeight+8;i++) {
    temp[i] = -1;
  }
	
	fwrite(freq, sizeof(int), 256, outptr);
  fwrite(&charCount, sizeof(unsigned int), 1, outptr);
 

int tempLeng;
	
	while(!feof(inptr)) 
	{
		curChar = fgetc(inptr);

    
		tempLeng = get_hCode(curCode, curChar, hCode, tHeight, leafCount);
		
		for(i = 0;i < tempLeng; i++) 
		{
      //printf("blablalba\n");
			temp[codeLeng++] = curCode[i];
		}

		while(codeLeng >= 8)
		{
			int tempInt = 0;
			for(i = 0; i < 8; i++)
			{
				tempInt += (1 << (7-i)) * temp[i];
			}
			outChar = tempInt;

			fwrite(&outChar, sizeof(char), 1, outptr); 	

			for(i = 8; i <= codeLeng ; i++){
        temp[i-8] = temp[i];
      }
			codeLeng = codeLeng - 8;

		}		
  }
	

	if(codeLeng > 0)
	{
		for(i = codeLeng ; i < 8; i++)
		{
			temp[i] = 0;
		}
	
		int tempInt = 0;
		for(i = 0; i < 8; i++) 
		{
			tempInt += (1 << (7-i)) * temp[i];
		}
		outChar = tempInt;

		fwrite(&outChar, sizeof(char), 1, outptr);	
	}



	free(curCode);

	fclose(inptr);
	fclose(outptr); 
}








int get_hCode(int *curCode, char curChar, int **hCode, int tHeight, int leafCount)
{
	int i;
  int j = 0;
	for (i = 0; i < tHeight; i++) curCode[i] = -1;
	for (i = 0; i < leafCount; i++)
	{
		if(hCode[i][0] == curChar)
		{
			j = 1;
			while(hCode[i][j] != -1)
			{
				curCode[j-1] = hCode[i][j];
				j++;
			}
			break;
		}
	}
	return j-1;
}



void killTree(TreeNode* TN)  
{  
    if (TN == NULL) return;  

    killTree(TN->left);  
    killTree(TN->right);  

    free(TN);  
}  