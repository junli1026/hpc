#include "roomy.h"
#include "RoomyHashTable.h"
#include "Astar.h"
#include "RoomyGraph.h"

uint64 gFunc(uint64 a)
{
	return 0;
}

uint64 hFunc(uint64 state)
{
	return 0;
}

RoomyGraph *g;

void createSimpleGraph();
int main(int argc, char **argv )
{
	Roomy_init(&argc, &argv);

	createSimpleGraph();
	initAllHT();
	uint64 a=1;
	uint64 b=9;
	uint64 x=2;	
	Astar(g ,&a, &b, &x);
	Roomy_finalize();
}

void createSimpleGraph()
{
	uint64 one = 1;
	uint64 two = 2;
	uint64 three = 3;
	uint64 four = 4;
	uint64 five = 5;
	
// From our sample graph we know that 1 has the most children (3) so
// maxEdges MUST be at least that number.
	uint64 maxEdges = 3;
// We also know that there are 5 nodes.
	uint64 initialCapacity = 5;
	g = RoomyGraph_make("simple-graph", maxEdges, initialCapacity);

// Add all the nodes
	RoomyGraph_addNode(g, 1);
	RoomyGraph_addNode(g, 2);
	RoomyGraph_addNode(g, 3);
	RoomyGraph_addNode(g, 4);
	RoomyGraph_addNode(g, 5);
	RoomyGraph_addNode(g, 6);	
	RoomyGraph_addNode(g, 7);
	RoomyGraph_addNode(g, 8);
	RoomyGraph_addNode(g, 9);
// Now, add all the edges
	RoomyGraph_addEdge(g, 1, 2);
	RoomyGraph_addEdge(g, 1, 3);
	RoomyGraph_addEdge(g, 1, 4);

	RoomyGraph_addEdge(g, 2, 5);
	RoomyGraph_addEdge(g, 2, 6);

	RoomyGraph_addEdge(g, 3, 7);
	RoomyGraph_addEdge(g, 3, 8);

	RoomyGraph_addEdge(g, 4, 9);

	RoomyGraph_sync(g);
//	uint64 nodeCount = RoomyGraph_nodeCount(g);
//	printf("There are %lli nodes.\n", nodeCount);
	RoomyGraph_print(g);
}
