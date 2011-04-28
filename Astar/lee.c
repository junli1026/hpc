#include "roomy.h"
#include "RoomyArray.h"
#include "RoomyList.h"
#include "RoomyHashTable.h"

RoomyHashTable *HT;

int C;
void mapFunc(void *key, void * val)
{
	if((*(uint64*)key)==4 )
		C=1;
}
void lee()
{
	HT = RoomyHashTable_make("HT", sizeof(uint64), sizeof(uint64), 100);
	uint64 a[] ={1,2,3,4};
	uint64 b[]={5,6,7,8};
	int i=0;
	for (i=0; i<4; i++)
		RoomyHashTable_insert(HT, &a[i], &b[i]);

	RoomyHashTable_sync(HT);
	RoomyHashTable_map(HT, mapFunc);
}
int main(int argc , char ** argv)
{
	Roomy_init(&argc, &argv);
	C=0;
	lee();
	Roomy_log("C=%d\n",C);
	Roomy_finalize();
	return 0;
}
