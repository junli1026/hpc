#include "roomy.h"
#include "RoomyArray.h"
#include "RoomyList.h"
#include "RoomyHashTable.h"

#include "Astar.h"

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

typedef struct{
	uint64 dist;
	uint64 father;
}Node;

int WeGetIt;

// Return the identity permutation in the argument out.
void getIdentPerm(Perm out) {
    int i;
    for(i = 0; i < permLen; i++)
        out[i] = i;
}

// Swap elements i and j of permutation p.
void swap(Perm p, Elt i, Elt j) {
    Elt tmp = p[i];
    p[i] = p[j];
    p[j] = tmp;
}

// Copy permutation from to permutation to.
void copyPerm(Perm to, Perm from) {
    memcpy(to, from, permLen * sizeof(Elt));
}

// Reverse the first k elements of the given permutation.
void revPrefix(Perm in, int k, Perm out) {
    copyPerm(out, in);
    int i;
    for(i = 0; i < k/2; i++) {
        swap(out, k-i-1, i);
    }
}

void invertPerm(Perm p, Perm out) {
    int i;
    for(i=0; i<permLen; i++)
        out[p[i]] = i;
}

uint64 rankRecur(Elt n, Perm p, Perm r) {
    if(n == 1) return 0;
    Elt s = p[n-1];
    swap(p, n-1, r[n-1]);
    swap(r, s, n-1);
    return s + n * rankRecur(n-1, p, r);
}

void unrankRecur(Elt n, uint64 r, Perm p) {
    if(n > 0) {
        swap(p, n-1, r%n);
        unrankRecur(n-1, r/n, p);
    }

}

void permToState( Perm in, uint64 *out)
{
	Perm p2;
    	copyPerm(p2, in);
    	Perm r;
    	invertPerm(p2, r);
    	*(uint64*)out = rankRecur(permLen, p2, r);
}

void stateToPerm(uint64 * in,  Perm out)
{
	uint64 r = *(uint64*)in;
	getIdentPerm(out);
    	unrankRecur(permLen, r, out);
}

void printPerm(uint64 in)
{
	Perm a;
	stateToPerm(&in, a);
	int i;
/*
	uint64 res=0;
	uint64 temp=1;
	for(i=0; i<permLen; i++)
	{
		res += temp*a[permLen-i-1];
		temp = temp*10;
	}
	for(i=0; i<permLen;i++)
		Roomy_log("%lli\n",res);
*/
	for(i=0; i<permLen; i++)
		Roomy_log("%lli\n", a[i]);
	Roomy_log("\n\n\n");
}

/************************************************************************
mapDist
*************************************************************************/
uint64 mapDist( Perm state )
{
	uint64 dist = gFunc(state) + hFunc(state);
	return dist;
}

/*************************************************************************
init all the three hashtable
**************************************************************************/
uint8 predFunc(void *key, void * value);
void initAllHT()
{
	openHT = RoomyHashTable_make("openHT", sizeof(uint64), sizeof(Node), 1000);
//	RoomyHashTable_attachPredicate(openHT, predFunc);
	if(openHT==NULL) printf("Initialize openHT ERROR!!\n");

	closeHT = RoomyHashTable_make("closeHT", sizeof(uint64), sizeof(Node), 1000);
	if(closeHT==NULL) printf("Initialize closeHT ERROR!!\n");

	pathHT = RoomyHashTable_make("pathHT", sizeof(uint64), sizeof(Node), 1000);
	if(pathHT==NULL) printf("Initialize nexLevHT ERROR!!\n");

	curLevHT = RoomyHashTable_make("curLevHT", sizeof(uint64),sizeof(Node),100);
	RoomyHashTable_attachPredicate(curLevHT, predFunc);
	nexLevHT = RoomyHashTable_make("nexLevHT", sizeof(uint64),sizeof(Node),100);
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
	if(0==RoomyHashTable_isSynced(rht))
		RoomyHashTable_sync(rht);
}

/**********************************************************************

***********************************************************************/
void mergeValAndAns(void *ans, void * key, void *val)
{
	uint64* dist = (uint64*)ans;
	if ( *dist > ((Node*)val)->dist )
		*dist = ((Node*)val)->dist;
}

void mergeAnsAndAns(void * inOut, void * in)
{
	if (  (*(uint64*)inOut) > (*(uint64*)in)  )
		*(uint64*)inOut= *(uint64*)in;
//	Roomy_log("%lli\n",*(uint64*)inOut);
}	

uint64 popMin(RoomyHashTable * rht)
{
	uint64 ans=MAX;
	RoomyHashTable_reduce(rht, &ans, sizeof(uint64), mergeValAndAns, mergeAnsAndAns);
	return ans;
}

/***********************************************************************
pop out the state/dist whose dist is between min and max. 
kept in curLevHT
************************************************************************/
/*
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

//	rangeLow= 0;
//	rangeUp = 100;	
	beginPushHT(curLevHT);
	RoomyHashTable_map( (RoomyHashTable*)rht , pop);	
	endPushHT(curLevHT);
	RoomyHashTable_sync(openHT);
}
*/

uint64 minDist;
void pop(void* key, void* val)
{
	if ( ((Node*)val)->dist == minDist )
	{
		pushHT(curLevHT, key, val);
	//	Roomy_log("satus:  %lli\n",*(uint64*)key);
		RoomyHashTable_remove(openHT,key);
	}
}

void popHT()
{
	minDist = popMin(openHT);
	beginPushHT(curLevHT);
	RoomyHashTable_map(openHT, pop);
	endPushHT(curLevHT);
	RoomyHashTable_sync(openHT);
}

/*****************************************************************
print out an HT
******************************************************************/
void print(void *key, void *val)
{
// 	Roomy_logAny("\nstate:%10lli   dist:%5lli   father:%10lli\n",
//		*(uint64*)key, ((Node*)val)->dist, ((Node*)val)->father );	

//for pathHT
 	Roomy_logAny("	%lli -> %lli\n", ((Node*)val)->father, *(uint64*)key );	
}
void printHT(void *rht)
{
	if(rht!=NULL)
		RoomyHashTable_map(rht, print);
	else
		Roomy_log("Error in print.\n");
}

void printAll()
{
	Roomy_log("/********openHT*********/\n");
	printHT(openHT);
	Roomy_log("\n");
	Roomy_log("/********curLevHT********/\n");
	printHT(curLevHT);
	Roomy_log("\n");
	Roomy_log("/********nexLevHT********/\n");
	printHT(nexLevHT);
	Roomy_log("\n");
	Roomy_log("/********closeHT*********/\n");
	printHT(closeHT);
	Roomy_log("\n");
	Roomy_log("/********pathHT*********/\n");
	printHT(pathHT);
	Roomy_log("\n");
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
i*************************************************************************/
void add(void * key, void *val)
{
	pushHT(openHT,key,val);
}

void addNexLevToOpen()
{
	beginPushHT(openHT);
	RoomyHashTable_map(nexLevHT, add);
	endPushHT(openHT);
	//Roomy_log("open size:%lli\n",RoomyHashTable_size(openHT));
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
	if((rht!=NULL)&&(0!=RoomyHashTable_size(rht)))
		RoomyHashTable_map(rht, passAll);
}
void setPath()
{	
//	Roomy_log("curHT size:%lli\n",RoomyHashTable_size(curLevHT));
//	Roomy_log("nexHT size:%lli\n",RoomyHashTable_size(nexLevHT));
//	Roomy_log("openHT size:%lli\n",RoomyHashTable_size(openHT));
//	Roomy_log("closeHT size:%lli\n",RoomyHashTable_size(closeHT));

	addToPathFrom(curLevHT);
//	addToPathFrom(openHT);
	addToPathFrom(closeHT);
	RoomyHashTable_sync(pathHT);
	
//	Roomy_log("pathHT size:%lli\n",RoomyHashTable_size(pathHT));
}

RoomyArray * beginning;

void setBeginning(uint64 i, void * oldValIn, void * passedVal, void * newValOut){
	*((uint64 *)newValOut) = *((uint64 *)passedVal);
//	Roomy_logAny("In update:%lli\n",*((uint64 *)newValOut));
}

//uint64 tempAsPassedVal = 1234;
void accessShow(void *key, void * val, void * passedVal)
{
	uint64 tempAsPassedVal = (*(Node*)val).father;
//	Roomy_logAny("In access:%lli\n",tempAsPassedVal);
	RoomyArray_update(beginning, 0, &tempAsPassedVal, setBeginning);
}

void mergeElemValAndAns(void * ansInOut, uint64 i, void * val){
	*((uint64 *)ansInOut) = *((uint64 *)val);
//	Roomy_logAny("val: %lli\n",*((uint64 *)ansInOut));
}

void mergeElemAnsAndAns(void * ansInOut, void * ansIn){
//	Roomy_logAny("ansInOut: %lli\n",*((uint64 *)ansInOut));
//	Roomy_logAny("ansIn: %lli\n",*((uint64 *)ansIn));
	if (*((uint64 *)ansInOut) == 0){
		*((uint64 *)ansInOut) = *((uint64 *)ansIn);
	}
	//Roomy_logAny("ansInOut: %lli\n",*((uint64 *)ansInOut));
	//Roomy_logAny("ansIn: %lli\n",*((uint64 *)ansIn));
}

void showPath()
{
	RoomyHashTable_registerAccessFunc(pathHT,accessShow,0);
	beginning = RoomyArray_makeBytes("begin",sizeof(uint64),1);
	RoomyArray_registerUpdateFunc(beginning,setBeginning,sizeof(uint64));
	uint64 current_elem = dest;
	uint64 count_steps = 0;
	while ( current_elem!=start )
	{
		RoomyHashTable_access(pathHT, &current_elem, NULL, accessShow );
		RoomyHashTable_sync(pathHT);
		RoomyArray_sync(beginning);
		current_elem = 0;
		RoomyArray_reduce(beginning, &current_elem, sizeof(uint64), mergeElemValAndAns, mergeElemAnsAndAns);
		Roomy_log("Path: %lli\n",current_elem);
		count_steps ++;
	}
	Roomy_log("Path steps: %lli\n",count_steps);
/*
	Roomy_log("Path:%lli steps.\n",size);
	int i;
	for (i=0; i<size+1; i++)
	{  
		Roomy_log("%lli\n",stack[size-i]);
	}
*/
}
/************************************************************************
genetrate the nexLevHT
*************************************************************************/
void genNBRS(void * key, void * val)
{
	pushHT(closeHT,key,val);
	int i;
	Perm in;
	Perm out[nbrsNum];
	uint64 state[nbrsNum];
	Node nbrNode[nbrsNum];

	stateToPerm((uint64*)key, in);
	generate(in, out);	
		
	for(i=0; i<nbrsNum; i++)
	{
		permToState(out[i],&state[i]);
		(nbrNode[i]).dist = mapDist(out[i]) ;
		(nbrNode[i]).father = *(uint64*)key;
		if( state[i]!=(*(uint64*)key) )
			pushHT(nexLevHT,&state[i], &nbrNode[i]);
	}
	
}

void genNexLevHT()
{	
	Lev++;
	beginPushHT(closeHT);
	beginPushHT(nexLevHT);
	RoomyHashTable_map(curLevHT, genNBRS);	
	endPushHT(nexLevHT);
	endPushHT(closeHT);
}

void mapCleanNex(void * key, void * val)
{
	RoomyHashTable_remove(nexLevHT, key);
}
void mapCleanCur(void *key, void* val )
{
	RoomyHashTable_remove(curLevHT, key);
}
void cleanHT()
{
	RoomyHashTable_map(curLevHT, mapCleanCur);
	RoomyHashTable_sync(curLevHT);
	RoomyHashTable_map(nexLevHT, mapCleanNex);
	RoomyHashTable_sync(nexLevHT);

//	Roomy_log("curHT size:%lli\n",RoomyHashTable_size(curLevHT));
//	Roomy_log("nexHT size:%lli\n",RoomyHashTable_size(nexLevHT));
}

uint8 predFunc(void *key, void *val)
{
	if( (*(uint64*)key) == dest)
		return 1;
	else 
		return 0;
}

/******************************************************************************
Astar
*******************************************************************************/
void Astar(Perm ST, Perm Dest, uint64 * popNum)
{
	uint64 step= *popNum;
	permToState(ST, &start);
	permToState(Dest, &dest);
	Roomy_log("start:%lli\n",start);
	Roomy_log("dest:%lli\n\n",dest);
	Node d;
	d.dist = mapDist(ST);
	d.father = 0;

	beginPushHT(openHT);
	pushHT(openHT,&start,&d );
	endPushHT(openHT);
	WeGetIt = 0;
	uint64 numCount;
	while(0==WeGetIt)
	{
	/*	
		RoomyHashTable * cur = RoomyHashTable_make("cur", sizeof(uint64), sizeof(Node), 100);	
		RoomyHashTable_attachPredicate(cur, predFunc);

		RoomyHashTable * nex = RoomyHashTable_make("nex", sizeof(uint64), sizeof(Node), 100);	
		curLevHT = cur;
		nexLevHT = nex;
	*/
		numCount = 0;
		popHT();	
		numCount = RoomyHashTable_predicateCount(curLevHT, predFunc);
		if ( numCount > 0 )
		{
			WeGetIt = 1;
			Roomy_log("We find the goal.\n");
			break;
		}

		genNexLevHT();	
		Roomy_log("Lev%lli: %lli nodes.  min f(x): %lli\n",Lev,RoomyHashTable_size(curLevHT), minDist);					
		updateNexLevHT();
 		addNexLevToOpen();
	//	RoomyHashTable_destroy(cur);
	//	RoomyHashTable_destroy(nex);
		cleanHT();
	
	//	Roomy_log("OPEN size: %lli\n",RoomyHashTable_size(openHT));
		if(0==RoomyHashTable_size(openHT))	
		{
			Roomy_log("Sorry, no solution.\n");
			break;
		}
	
	}
	if(1==WeGetIt)
	{
	//	Roomy_log("here\n");
		Roomy_log("Depth:%lli\n", Lev-1);
		setPath();
	//	printHT(pathHT);
		showPath();		
	}
//	Roomy_log("nodes in OPEN: %lli\n",RoomyHashTable_size(openHT)+RoomyHashTable_size(curLevHT));
	Roomy_log("We popolate %lli nodes\n",RoomyHashTable_size(closeHT)+RoomyHashTable_size(openHT));
}

/********************************************************************
test
*********************************************************************/
void testSet()
{
/*	uint64 size=N;
	uint64 *array, *index;
	array = (uint64*)malloc(size*sizeof(uint64));
	index = (uint64*)malloc(size*sizeof(uint64));
uint64 i;
	for(i=0; i<size; i++)
	{
		index[i]=i;
		array[i]=10+i;
	}
	
	beginPushHT(openHT);
//	for(i=0; i<size; i++)
//		pushHT(openHT,&index[i],&array[i]);
	endPushHT(openHT);

	free(array);
	free(index);
*/
	Node a;
	a.father =234;
	a.dist=15;

	
	beginPushHT(openHT);
		pushHT(openHT,&start,&a);
	endPushHT(openHT);

	printAll(openHT);

	popHT(openHT, 10 );
	printAll();
		
	genNexLevHT();
	printAll();

	updateNexLevHT();
	printAll();

	addNexLevToOpen();	
	printAll();

}

void testPopHT()
{
	//RoomyHashTable_attachPredicate(openHT, predFunc);
	RoomyHashTable * temp = RoomyHashTable_make("temp", sizeof(uint64), sizeof(uint64), 100);	
	RoomyHashTable_attachPredicate(temp, predFunc);
	Node a[10];
	uint64 b[10];
	uint64 i;
	for (i=0; i<10; i++)
	{
		a[i].father=i;
		a[i].dist=10;
		b[i]=i;
	}
	a[5].dist=1234;
	dest =13;
	b[3] = dest;

	beginPushHT(openHT);
	for(i=0;i<10; i++)
		pushHT(openHT,&b[i], &a[i]);
	endPushHT(openHT);
/*
	uint64 count= RoomyHashTable_predicateCount(openHT, predFunc);
	Roomy_log("count: %lli\n",count);
*/
	printHT(openHT);
	uint64 sum = popMin(openHT);
	Roomy_log("result: %lli\n",sum);

/*
	
	beginPushHT(temp);
	for (i=0; i<10; i++)
		pushHT(temp,&b[i], &a[i]);
	endPushHT(temp);
	uint64 count= RoomyHashTable_predicateCount(temp, predFunc);
	Roomy_log("count: %lli\n",count);
*/
//	printAll();

//	popHT(openHT,2);
//	printAll();
}
