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
	c[6]=7;		
	c[7]=0;		
	c[8]=8;		
	Astar(a, b,5);	
	Roomy_finalize();
}
