#include "roomy.h"
#include "RoomyHashTable.h"

uint64 dest;
uint64 start;
uint64 Lev;



// user define here
#define MIN 0
#define MAX 400
#define permLen 16 
#define nbrsNum 4
typedef uint8 Elt;
typedef Elt Perm[permLen];

extern uint64 hFunc(Perm a);
extern uint64 gFunc(Perm b);
