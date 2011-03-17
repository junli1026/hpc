#include "roomy.h"
#include "RoomyHashTable.h"

uint64 dest;
uint64 start;
uint64 Lev;


// user define here
#define MIN 0
#define MAX 100
#define permLen 9
#define nbrsNum 4
typedef uint8 Elt;
//typedef Elt Perm[permLen];     
typedef uint64 Perm;

extern uint64 hFunc(Perm a);
extern uint64 gFunc(Perm b);
