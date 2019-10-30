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

ListNode *list_make(int *, int *);
ListNode *LN_make(TreeNode* , ListNode* );
TreeNode *TN_make(char , int , TreeNode *, TreeNode *);
TreeNode *tree_make(ListNode *);
void decode(char * , char * );



int main(int argc, char **argv)
{





    char*infile = argv[1];
    char outfile[50];
    strcpy(outfile, infile);
    strcat(outfile, ".unhuff");
    
    
    

    decode(infile, outfile);

    //printPreorder(treeHead);

    
    
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




void decode(char * infile, char * outfile)
{
    int freq[256] = {0};
    unsigned int charCount;
    int leafCount = 0;

    FILE* inptr = fopen(infile, "r");
    FILE* outptr = fopen(outfile, "wb");
    int i = fread(freq, sizeof(int), 256, inptr);
    i = fread(&charCount, sizeof(unsigned int), 1, inptr);


    for (i=0;i<256;i++){
        if(freq[i]!=0){
            leafCount++;
        }
    }
    //printf("%d\n",charCount);
    ListNode *listHead = list_make(freq, &leafCount);

    //printList( listHead) ;
    

    TreeNode *treeHead = tree_make(listHead);
    //printPreorder(treeHead);
	


    char tempChar;
    TreeNode * tempTN = treeHead;
    printf("%d\n", charCount);
    while (charCount > 0)
        {
        
        int tempInt = fread(&tempChar, 1, 1, inptr);
        tempInt = tempChar;
        for(i = 0;i < 8; i++)
		{   
            if(((tempInt & ( 1 << (7-i) )) >> (7-i))){
                tempTN = tempTN->right;
            }
            else if (tempTN->left != NULL)
                {
                tempTN = tempTN -> left;
                }	   

        if(tempTN->left == NULL && tempTN->right == NULL)
			{
				fprintf(outptr, "%c", tempTN -> val);
                tempTN = treeHead;
                charCount = charCount - 1;
			}
        if(charCount <= 0) break;

        }
        }




	fclose(inptr);
	fclose(outptr);
	
}














