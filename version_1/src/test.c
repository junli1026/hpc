#include "roomy.h"
#include "RoomyHashTable.h"
#include "Astar.h"


uint64 gFunc(Perm a)
{
	return Lev;
}

uint64 hFunc(Perm state)
{
	uint64 d=dest;
	Perm des;
	stateToPerm(&d, des);

	int i, h=0;
	for(i=0; i<permLen; i++)
		if(des[i]!=state[i])
			h++;
	return h;
}

void genOneState(Perm in, int i, int j, Perm out)
{
	int l;
	for(l=0; l< permLen; l++)
		out[l] = in[l];

	if( (j<0)||(j>8) ) return;
	if(  ((i==2)&&(j==3))||((i==3)&&(j==2))||
		((i==5)&&(j==6))||((i==6)&&(j==5)) )
		 return;

	Elt temp;
	temp = out[i];
	out[i] = out[j];
	out[j] = temp;
}

void generate(Perm in, Perm out[])
{
	uint64 position;
	int i;

	for(i=permLen-1; i>=0; i--)
		if (in[i]==0)
			position=i;

	int nbrsPosition[] = { position-1, position+1, position-3, position+3 };

	for(i=0; i<nbrsNum; i++)
		genOneState(in, position, nbrsPosition[i] , out[i]);
}

int solution(Perm a, Perm b)
{
	uint64 aa[permLen];
	uint64 bb[permLen];
	uint64 aSum=0;
	uint64 bSum=0;
	uint64 i,j;
	for (i=0; i<permLen;i++)
	{
		aa[i]=0;
		bb[i]=0;
	}
	for(i=0; i<permLen; i++)
	{
		for(j=0;j<i;j++)
		{	
			if( (a[j]>a[i])&&(a[i]!=0) )	
				aa[i]++;
			if( (b[j]>b[i])&&(b[i]!=0) )
				bb[i]++;
		}
	} 

	for(i=0; i<permLen;  i++)
	{
		aSum = aSum+aa[i];
		bSum = bSum+bb[i];
	}
//	printf("%lli , %lli\n",aSum, bSum);
	if( ((0==aSum%2)&&(0==bSum%2))|| 
		( (1==aSum%2)&&(1==bSum%2) ))
		return 1;
	else return 0;
	
}	

int main(int argc, char **argv )
{
	Roomy_init(&argc, &argv);
	initAllHT();
	Perm a, b, c;
	a[0]=1;		b[0]=4;
	a[1]=2;		b[1]=0;
	a[2]=3;		b[2]=7;
	a[3]=4;		b[3]=8;
	a[4]=5;		b[4]=2;
	a[5]=6;		b[5]=5;
	a[6]=7;		b[6]=3;
	a[7]=8;		b[7]=6;
	a[8]=0;		b[8]=1;

	c[0]=1;		
	c[1]=2;		
	c[2]=3;		
	c[3]=4;		
	c[4]=5;		
	c[5]=6;		
	c[6]=8;		
	c[7]=0;		
	c[8]=7;	

	uint64 x=1;	
	if(1==solution(a,b))	
		Astar(a, b, &x);	
	else
		 printf("No solution.\n");
	Roomy_finalize();
}
