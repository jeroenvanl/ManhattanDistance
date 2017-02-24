/*COMSM1201 Student Jeroen van Lith*/

/*The extension calculates the manhattan value of the
initial board and gives an estimation of how
long it would take to solve the board :) */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <time.h>


#define SIZE 3
#define DIMENSION 2
#define YDIM 0
#define XDIM 1
#define NUMALLOWEDARGS 2
#define BOARDSIZE SIZE*SIZE
#define LOWERBOUND 0
#define UPPERBOUND SIZE-1
#define MOVES 4 /* North, East, South and West */
#define MAXNUM 8 
#define AMOUNTVALIDCHARS 9
#define BLANK ' '
#define QUICK 5 
#define MEDIUM 12
#define HIGH 20

typedef struct node{
   char board[SIZE][SIZE];
   struct node *parent;
   struct node *next;
   struct node *previous;
}Node;

typedef struct queue{
   Node *start;
   Node *parent;
   Node *current;
}Queue;


/*1. Read and check the file */
void readFile(int *argc, char *filename, char board[SIZE][SIZE]);
void checkCount(int c);
void checkChar(char *c);
void checkDoubles(char board[SIZE][SIZE]);
void checkSolvability(char board[SIZE][SIZE]);
void manhattanCal(char board[SIZE][SIZE]); /* Do the Manhattan Calculation */
void flattenBoard(char board[SIZE][SIZE],char flatboard[BOARDSIZE]);


/*2. Do the 8tile magic */
void findBoard(char initboard[SIZE][SIZE], Queue *q);
Node *allocateNode(char board[SIZE][SIZE], Node *parent, Node *previous);
void findBlank(char board[SIZE][SIZE], int *y, int *x);
int checkWin(char board[SIZE][SIZE]);
int checkBounds(int y,int x,int c);
void swap(char board[SIZE][SIZE], int y, int x, int m);
int checkInList(Node *start, char board[SIZE][SIZE]);
int checkSimilarBoard(char board[SIZE][SIZE],char boardtocheck[SIZE][SIZE]);


/*3. Visualise results and free memory*/
void printSolution(Node *l,char board[SIZE][SIZE]);
void print(char board[SIZE][SIZE]);
void printMessage(int sum);
void freeList(Node *l);


int main(int argc, char **argv)
{

   char board[SIZE][SIZE];
   Queue q;

   readFile(&argc,argv[1],board);
   findBoard(board,&q);
   
   printSolution(q.current,board);
   freeList(q.start);
   
   return 0;
}


void readFile(int *argc, char *filename, char board[SIZE][SIZE])
{
   int i=0,j=0;
   FILE *fp;
	
   checkCount(*argc);
   fp=fopen(filename,"rb");
   for(i=0;i<SIZE;i++){
      for(j=0;j<SIZE;j++){
         board[i][j]=getc(fp);
         checkChar(&board[i][j]);
      }
      while(getc(fp)!='\n'){ /*Skip unneccesary chars*/
      }                            
   }
   checkSolvability(board);
   checkDoubles(board);
   manhattanCal(board);
   fclose(fp);
}


void checkCount(int c)
{
   if(c<NUMALLOWEDARGS){
      printf("Please type in a filename!\n");
      exit(0);
   }
   else if(c>NUMALLOWEDARGS){
      printf("Please type in only 1 filename!\n");
      exit(0);
   }
}


void checkChar(char *c)
{
   char maxnum=MAXNUM+'0'; 
   if(!((*c<=maxnum)||(*c==BLANK))){
      printf("There is an invalid character in your .8tile file\n");
      exit(0);
   }
}


/* This checks for double chars in a 1d array */
void checkDoubles(char board[SIZE][SIZE])
{
   int i,j;
   char flatboard[BOARDSIZE];
   flattenBoard(board,flatboard);
   for(i=0;i<BOARDSIZE-1;i++){
      for(j=i+1;j<BOARDSIZE;j++){
         if(flatboard[i]==flatboard[j]){
            printf("There are double numbers in your .8file\n");
            exit(0);
         }
      }
   }
}


/* This function flattens the 2d board to a 1d array for the checkDoubles*/
void flattenBoard(char board[SIZE][SIZE],char flatboard[BOARDSIZE])
{
   int i,j,k=0;
   for(i=0;i<SIZE;i++){
      for(j=0;j<SIZE;j++){
         flatboard[k++]=board[i][j];
      }
   }
}


/* If there's an odd number of inversions on the initial board,
the board is not solvable, because  */
void checkSolvability(char board[SIZE][SIZE])
{
   int i,j,count=0;
   char flatboard[BOARDSIZE];
   flattenBoard(board,flatboard);
   for(i=0;i<BOARDSIZE-1;i++){
      for(j=i+1;j<BOARDSIZE;j++){
         if((flatboard[i]!=BLANK && flatboard[j]!=BLANK)){
            if(flatboard[i]>flatboard[j]){
               count++;
            }
         }
      }
   }
   if(count%2!=0){
      printf("This board is not solvable, since there's an odd number of inversions\n");
      exit(0);
   }
}


/*Nested loop through parents and four cardinal directions */
void findBoard(char initboard[SIZE][SIZE], Queue *q)
{
   int y,x,m=0;
   char temp[SIZE][SIZE];
   q->start=q->parent=q->current=allocateNode(initboard,NULL,NULL);
   memcpy(temp,initboard,sizeof(char)*BOARDSIZE);
   findBlank(q->parent->board, &y, &x);
   while(!(checkWin(q->current->board))){
      memcpy(temp,q->parent->board,sizeof(char)*BOARDSIZE); 
      if(checkBounds(y,x,m%MOVES)){                   
         swap(temp,y,x,m%MOVES);
         if(!(checkInList(q->current,temp))){ 
            q->current=q->current->next=allocateNode(temp,q->parent,q->current);
         }
      }
      if(m%MOVES==MOVES-1){ /* We've been through N,E,S and W */
         q->parent=q->parent->next;
         findBlank(q->parent->board, &y, &x);
      }
      m++; /* Loop through NESW with the m counter */
   }
}


/* Allocate node with a board and a parent pointer */
Node *allocateNode(char board[SIZE][SIZE], Node *parent, Node *previous)
{
   Node *p;
   p = (Node*)malloc(sizeof(Node));
   if(p==NULL){
      printf("Cannot Allocate Node\n");
      exit(0);
   }
   memcpy(p->board, board,sizeof(char)*BOARDSIZE);
   p->parent=parent;
   p->next=NULL;
   p->previous=previous;
   return p;
}


void manhattanCal(char board[SIZE][SIZE])
{
   int i,j,k=0,sum=0;
   for(i=0;i<SIZE;i++){
      for(j=0;j<SIZE;j++){
         if(board[i][j]!=BLANK){
            sum+=abs(((k)/SIZE)-((int)((board[i][j]-'0'-1)/SIZE)));
            sum+=abs(((k)%SIZE)-((int)((board[i][j]-'0'-1)%SIZE)));
         }
      k++;
      }
   }
   printMessage(sum);
}


void findBlank(char board[SIZE][SIZE], int *y, int *x)
{
   int i=0,j=0,c=0;
   while(board[i][j]!=BLANK){
      c++;
      i=c/SIZE;
      j=c%SIZE;
   }
   *y=i;
   *x=j;
}


int checkWin(char board[SIZE][SIZE])
{
   char winboard[SIZE][SIZE]={{'1','2','3'},{'4','5','6'},{'7','8',' '}};
   int c=0;
   while(winboard[c/SIZE][c%SIZE]==board[c/SIZE][c%SIZE] && c<BOARDSIZE){
      c++;
   }
   if(c==BOARDSIZE){
      return 1;
   }
   return 0;
}


int checkBounds(int y,int x,int c)
{
   int m[MOVES][DIMENSION]={{-1,0},{0,1},{1,0},{0,-1}};
   y+=m[c][YDIM];
   x+=m[c][XDIM];
   if((y>=LOWERBOUND && y<=UPPERBOUND) && (x>=LOWERBOUND && x<=UPPERBOUND)){
      return 1;
   }
   return 0;
}


void swap(char board[SIZE][SIZE], int y, int x, int m) 
{
   int n[MOVES][DIMENSION]={{-1,0},{0,1},{1,0},{0,-1}}; /* N,E,S,W */
   char tmp;
   tmp=board[y][x];
   board[y][x]=board[y+n[m][YDIM]][x+n[m][XDIM]];
   board[y+n[m][YDIM]][x+n[m][XDIM]]=tmp;
}


int checkInList(Node *l, char board[SIZE][SIZE])
{
   do{
      if(checkSimilarBoard(l->board,board)){
         return 1;
      }
      l=l->previous;
   }while(l!=NULL);
   return 0;
}


int checkSimilarBoard(char board[SIZE][SIZE],char boardtocheck[SIZE][SIZE])
{
   int c=0;
   while(board[c/SIZE][c%SIZE]==boardtocheck[c/SIZE][c%SIZE] && c<BOARDSIZE){
      c++;
   }
   if(c==BOARDSIZE){
      return 1;
   }
   return 0;
}


void printSolution(Node *l, char board[SIZE][SIZE])
{
   static int i=0;
   if(!checkSimilarBoard(l->board,board)){
      i++;
      printSolution(l->parent,board); /*Recursion to print in reverse */
      print(l->board);
   }
   else{
      printf("This board took %d moves\n\n\n",i);
      print(l->board);
   }
}



void print(char board[SIZE][SIZE])
{
   int i,j;
   for(i=0;i<SIZE;i++){
      for(j=0;j<SIZE;j++){
         printf("%c",board[i][j]);
      }
      printf("\n");
   }
   printf("\n");
}


void printMessage(int sum)
{
   if(sum<QUICK){
      printf("This one is quicker than the speed of light :D \n"); 
   }
   else if(sum<MEDIUM){
      printf("You might be able to see this message, but its still a quick solution :) \n");
   }
   else if(sum<HIGH){
      printf("According to the manhattan value this is going to take some solid seconds...  :|\n");
   }
   else{
      printf("According to the manhattan value this is take some solid time :(\n");
   }
}


void freeList(Node *l)
{
   Node* tmp;

   while (l!=NULL)
    {
       tmp=l;
       l=l->next;
       free(tmp);
    }
}
