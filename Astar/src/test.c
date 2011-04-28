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

void genOneState_8(Perm in, int i, int j, Perm out)
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


void genOneState_15(Perm in, int i, int j, Perm out)
{
	int l;
	for(l=0; l< permLen; l++)
		out[l] = in[l];

	if( (j<0)||(j>15) ) return;
	if(  ((i==3)&&(j==4))||((i==4)&&(j==3))||
		((i==7)&&(j==8))||((i==8)&&(j==7))||
		((i==11)&&(j==12))||((i==12)&&(j==11)) )
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

	int nbrsPosition[] = { position-1, position+1, position-4, position+4 };

	for(i=0; i<nbrsNum; i++)
		genOneState_15(in, position, nbrsPosition[i] , out[i]);
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
	Perm a, b, c, d;
	a[0]=1;		b[0]=6;
	a[1]=2;		b[1]=1;
	a[2]=3;		b[2]=2;
	a[3]=4;		b[3]=8;
	a[4]=5;		b[4]=5;
	a[5]=6;		b[5]=3;
	a[6]=7;		b[6]=4;
	a[7]=8;		b[7]=11;
	a[8]=9;		b[8]=14;
	a[9]=10;	b[9]=13;
	a[10]=11;	b[10]=10;
	a[11]=12;	b[11]=7;
	a[12]=13;	b[12]=9;
	a[13]=14;	b[13]=0;
	a[14]=15;	b[14]=15;
	a[15]=0;	b[15]=12;

	c[0]=1;		
	c[1]=2;		
	c[2]=0;		
	c[3]=4;		
	c[4]=5;		
	c[5]=6;		
	c[6]=3;		
	c[7]=8;		
	c[8]=9;
	c[9]=10;
	c[10]=7;
	c[11]=11;
	c[12]=13;
	c[13]=14;
	c[14]=15;
	c[15]=12;

	d[0]=0;		
	d[1]=2;		
	d[2]=3;		
	d[3]=4;		
	d[4]=1;		
	d[5]=5;		
	d[6]=7;		
	d[7]=8;		
	d[8]=9;		
	d[9]=6;
	d[10]=10;
	d[11]=11;
	d[12]=13;
	d[13]=14;
	d[14]=15;	
	d[15]=12;

	uint64 x=40;	
	if(0==solution(a,c))	
		Astar(a, b, &x);	
	else
		 printf("No solution.\n");


//	testPopHT();
	Roomy_finalize();
}
