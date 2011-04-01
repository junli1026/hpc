#include "roomy.h"
#include "RoomyArray.h"
#include "RoomyList.h"
#include "RoomyHashTable.h"

#include "Astar.h"
#include "RoomyGraph.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <malloc.h>

RoomyHashTable *openHT;
RoomyHashTable *closeHT;
RoomyHashTable *curLevHT;
RoomyHashTable *nexLevHT;
RoomyHashTable *pathHT;
RoomyGraph *g;

typedef struct{
	uint64 dist;
	uint64 father;
}Node;

int WeGetIt=0; 
uint64 mapDist( uint64 state )
{
	uint64 dist = gFunc(state) + hFunc(state);
	return dist;
}

/*************************************************************************
init all the three hashtable
**************************************************************************/
void initAllHT()
{
	openHT = RoomyHashTable_make("openHT", sizeof(uint64), sizeof(Node), 1000);
	if(openHT==NULL) printf("Initialize openHT ERROR!!\n");

	closeHT = RoomyHashTable_make("closeHT", sizeof(uint64), sizeof(Node), 1000);
	if(closeHT==NULL) printf("Initialize closeHT ERROR!!\n");

	curLevHT = RoomyHashTable_make("curLevHT", sizeof(uint64), sizeof(Node), 500);
	if(curLevHT==NULL) printf("Initialize curLevHT ERROR!!\n");

	nexLevHT = RoomyHashTable_make("nexLevHT", sizeof(uint64), sizeof(Node), 500);
	if(nexLevHT==NULL) printf("Initialize nexLevHT ERROR!!\n");

	pathHT = RoomyHashTable_make("pathHT", sizeof(uint64), sizeof(Node), 1000);
	if(pathHT==NULL) printf("Initialize nexLevHT ERROR!!\n");
}

/**************************************************************************
push a state/dist into the HashTable
**************************************************************************/
void push(void *key, void *oldVal, void *passedVal, void * newVal)
{
	if(oldVal == NULL)
		*(Node*)newVal = *(Node*)passedVal;
	else
	{
		if( (*(Node*)oldVal).dist >  (*(Node*)passedVal).dist )
		{	
		//	printf("Old key is replaced by the new key.\n");
			*(Node*)newVal = *(Node*)passedVal;
		}
		else *(Node*)newVal = *(Node*)oldVal;
	}
}

void beginPushHT(void * rht)
{
	RoomyHashTable_registerUpdateFunc(rht, push, sizeof(Node));
}

void pushHT(void *rht, void *state, void *node)
{
//	uint64 d = mapDist(*(uint64*)state);
	RoomyHashTable_update(rht,state,node,push);
}

void endPushHT(void *rht)
{
	RoomyHashTable_sync(rht);
}


/***********************************************************************
pop out the state/dist whose dist is between min and max. 
:kept in curLevHT
************************************************************************/
uint64 count[MAX];
uint64 rangeLow;
uint64 rangeUp;
void mapCount(void *key, void *val)
{
	uint64 index= (*(Node*)val).dist;
	count[index]++;
}
void getRange(uint64 *low, uint64 *up, uint64 step)
{
	uint64 i;
	*low=MIN;
	*up=MAX;
	for(i=0; i<MAX; i++)
		count[i]=0;
	RoomyHashTable_map( openHT , mapCount );

//	for (i=0; i<MAX; i++)
//		printf("count[%lli]:%lli\n",i, count[i]);

	for(i=0; i<MAX; i++)
	{
		if( count[i] > 0 )
		{
			*low= i;
			break;
		}
	}
	
	uint64 temp=0;
	for(i= *low; i<MAX; i++ )
	{
		if( temp >= step )
		{	
			*up = i-1;
			break;
		}
	 	else 
			temp=count[i]+ temp;
				
	}
}

void pop(void *key, void *val)
{
	uint64 state = *(uint64*)key;
	uint64 distance = (*(Node*)val).dist;
	
	if((distance >= rangeLow )&&(distance <= rangeUp))
	{
		pushHT(curLevHT,key,val);
		RoomyHashTable_remove(openHT,key);
	}	
}

void popHT(void *rht, uint64 step)
{
	uint64 low, up;
	getRange(&low, &up, step);
	rangeLow= low;
	rangeUp= up;
	
	beginPushHT(curLevHT);
	RoomyHashTable_map( (RoomyHashTable*)rht , pop);
	endPushHT(curLevHT);
	RoomyHashTable_sync(openHT);

}

/*****************************************************************
print out an HT
******************************************************************/
void print(void *key, void *val)
{
 	printf("state:%10lli  perm:",*(uint64*)key);
	printf("  dist:%10lli", (*(Node*)val).dist );
	printf("  father:%10lli\n", (*(Node*)val).father );
}

void printHT(void *rht)
{
	RoomyHashTable_map(rht, print);
}

void printAll()
{
	printf("\n\n\n\n/********openHT*********/\n");
	printHT(openHT);
	printf("\n/********curLevHT********/\n");
	printHT(curLevHT);
	printf("\n/********nexLevHT********/\n");
	printHT(nexLevHT);
	printf("\n/********closeHT*********/\n");
	printHT(closeHT);
	printf("\n/********pathHT*********/\n");
	printHT(pathHT);
}
/************************************************************************
update nexLevHT, if one state exist in curLevHT and nexLevHT, we need to
compare the dist .
*************************************************************************/
void pushInNex(void *key, void *val, void *passedVal)
{
	if(val!=NULL) //exist in closeHT
	{
		if ( (*(Node*)passedVal).dist < (*(Node*)val).dist ) 
		{	
			*(uint64*)val = *(uint64*)passedVal;
			//chang father too!! add here!
		}
		else
			RoomyHashTable_remove(nexLevHT,key);	
	}
}

void updateNexLev(void *key, void *val)
{
	RoomyHashTable_access(closeHT,key,val,pushInNex);
}

void updateNexLevHT()
{
	RoomyHashTable_registerAccessFunc(closeHT, pushInNex, sizeof(uint64));
	RoomyHashTable_map(nexLevHT, updateNexLev);	
	if(RoomyHashTable_isSynced(closeHT)==0)
		RoomyHashTable_sync(closeHT);
	if(RoomyHashTable_isSynced(nexLevHT)==0)
		RoomyHashTable_sync(nexLevHT);
}

/************************************************************************
add nexLevHT into openHT
*************************************************************************/
void add(void * key, void *val)
{
	pushHT(openHT,key,val);
}

void addNexLevToOpen()
{
	beginPushHT(openHT);
	RoomyHashTable_map(nexLevHT, add);
	endPushHT(openHT);
}

/************************************************************************
setpathHT and show the path
*************************************************************************/
void passAll(void *key, void *val)
{
	RoomyHashTable_insert(pathHT,key,val);
}

void addToPathFrom(RoomyHashTable *rht)
{
	RoomyHashTable_map(rht, passAll);
}
void setPath()
{
	addToPathFrom(curLevHT);
	addToPathFrom(openHT);
	addToPathFrom(closeHT);
	RoomyHashTable_sync(pathHT);
}

uint64 tempAsPassedVal;
void accessShow(void *key, void * val, void * passedVal)
{
	tempAsPassedVal = (*(Node*)val).father;
}

void showPath()
{
	uint64 stack[Lev-1];
	uint64 size=0; //size always <=Lev-1 .
	stack[size]=dest;
	while ( stack[size]!=start )
	{
		RoomyHashTable_registerAccessFunc(pathHT,accessShow,0);
		RoomyHashTable_access(pathHT, &stack[size], NULL, accessShow );
		RoomyHashTable_sync(pathHT);
		size++;
		stack[size]=tempAsPassedVal;
	}

	printf("Path:%lli steps.\n",size);
	int i;
	for (i=0; i<size; i++)
		printf("%lli\n",stack[size-i]);
		
	printf("%lli\n",dest);
}
/************************************************************************
genetrate the nexLevHT
*************************************************************************/
void childrenFunction(void *key, void *val, void * passedVal)
{
	uint64* array = (uint64*)val;
//	printf("father:%lli\n", parent );
	uint64 i;
	if(0 != array[0])
	{	
		Node node[array[0]];
		for(i=1; i<=array[0]; i++)
			if( array[i]!=(*(uint64*)key) )
			{
				node[i].father = *(uint64*)key;
				node[i].dist = mapDist(array[i]);
				pushHT(nexLevHT,&array[i], &node[i]);
			}
	}
}	

void genNBRS(void * key, void * val)
{
	pushHT(closeHT,key,val);

	if ((*(uint64*)key)==dest)
	{
		WeGetIt = 1;
		printf("\n**********Notice**********\nWe got it !!!!\n");
		printf("**********End*************\n");
	}

	uint64 parent = *(uint64*)key;
	
	RoomyHashTable_access(g->graph, &parent, NULL, childrenFunction); 
	
}

void genNexLevHT()
{	
	Lev++;
	beginPushHT(closeHT);
	beginPushHT(nexLevHT);

	RoomyHashTable_registerAccessFunc(g->graph, childrenFunction, 0);		RoomyHashTable_map(curLevHT, genNBRS);
	RoomyHashTable_sync(g->graph);	

	endPushHT(nexLevHT);
	endPushHT(closeHT);
}


/******************************************************************************
Astar
*******************************************************************************/
void Astar(RoomyGraph * temp, uint64 * a, uint64 * b, uint64 * popNum)
{

	g=temp;	
	start = *a;
	dest = *b;
	uint64 step= *popNum;
	
	Node d;
	d.dist = mapDist(start);
	d.father = 0;
	
	printf("start node: %lli\n", start);
	printf("goal node: %lli\n", dest);

	beginPushHT(openHT);
		pushHT(openHT,&start,&d);
	endPushHT(openHT);	
	
	while(WeGetIt==0)
	{
		curLevHT = RoomyHashTable_make("curLevHT",sizeof(uint64),sizeof(Node),1000);
		nexLevHT = RoomyHashTable_make("nexLevHT",sizeof(uint64),sizeof(Node),1000);

	//	printAll();
		popHT(openHT,step);
	//	printAll();
		genNexLevHT();
		printAll();
		/****** possible we get it*****/
		if(WeGetIt==1)
		{
			RoomyHashTable_destroy(nexLevHT);
			setPath();
		//	printAll();
		}
		else
		{
			printf("Lev%lli: %lli nodes.\n",Lev,RoomyHashTable_size(curLevHT));
			updateNexLevHT();	
		//	printAll();
	 		addNexLevToOpen();
		//	printAll();
			RoomyHashTable_destroy(curLevHT);
			RoomyHashTable_destroy(nexLevHT);
		}	
	/*		
		if (0==RoomyHashTable_size(openHT));
		{
			printf("sorry, no solutoin!\n");
			break;
		}
	*/
	}
	
	if(1==WeGetIt)
	{	
		printf("Depth:%lli\n", Lev-1);
		showPath();	
	}
	RoomyHashTable_destroy(pathHT);
}

