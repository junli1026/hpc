#include <stdio.h>
#include "RoomyGraph.h"
#include <assert.h>

#define TRUE 1
#define FALSE 0
#define PASSED 1
#define FAILED 0

int main(int argc, char ** argv) {
  Roomy_init(&argc,&argv);
  
	if(createGraph() &&
		 destroyGraph() && 
		 addNode() &&
		 containsNodeTest() &&
		 addMultipleNodes() &&
		 ableToAddMoreThanInitialSize() &&
		 addingSameNodeDoesntIncreaseCount() &&
		 addEdgeTest()) {
    printf("All tests passed\n");
  } else {
    printf("ERROR OCCURRED... SEE LOGS\n");
  }
	
	Roomy_finalize();
}
int addEdgeTest() {
	RoomyGraph *g = RoomyGraph_make("addEdgeTest", 2, 2);
	uint64 from = 2;
	uint64 to = 4;
	RoomyGraph_addNode(g, from);
	RoomyGraph_addNode(g, to);
	RoomyGraph_addEdge(g, from, to);
	RoomyGraph_sync(g);

//	RoomyGraph_print(g);

	assert(TRUE == RoomyGraph_containsEdge(g, from, to));
	assert(FALSE == RoomyGraph_containsEdge(g, to, from));

	RoomyGraph_destroy(g);
	printf("addEdgeTest completed successfully.\n");
	return PASSED;
}
int addingSameNodeDoesntIncreaseCount() {
	RoomyGraph *g = RoomyGraph_make("multiAdds", 2, 2);
	uint64 node = 653;
	RoomyGraph_addNode(g, node);
	RoomyGraph_sync(g);
	assert(1 == RoomyGraph_nodeCount(g));
	
	RoomyGraph_addNode(g, node);
	RoomyGraph_sync(g);
	assert(1 == RoomyGraph_nodeCount(g));

	uint64 node2 = 1127;
	RoomyGraph_addNode(g, node2);
	RoomyGraph_sync(g);
	assert(2 == RoomyGraph_nodeCount(g));
	
	RoomyGraph_destroy(g);

	printf("addingSameNodeDoesntIncreaseCount completed successfully.\n");
	return PASSED;
}
int ableToAddMoreThanInitialSize() {
	RoomyGraph *g = RoomyGraph_make("increaseCapacity", 2, 2);
	uint64 count = 0;
	while(count < 4) {
		count++;
		RoomyGraph_addNode(g, count);	
	}
	RoomyGraph_sync(g);
	assert(4 == RoomyGraph_nodeCount(g));
	//printf("Contents of RoomyGraph that has been expanded from size 2 to 4...\n");
	//RoomyGraph_print(g);
	RoomyGraph_destroy(g);
	printf("ableToAddMoreThanInitialSize completed successfully.\n");
	return PASSED;
}
// Failing functions should return 1, otherwise 0.
int createGraph() {
  uint64 maxEdges = 10;
  uint64 initialCapacity = 10;
	RoomyGraph *g = RoomyGraph_make("g", maxEdges, initialCapacity);

  assert(NULL != g);
  assert(g->maxEdges == 10);
  assert(NULL != g->graph);

	printf("createGraph completed successfully.\n");
	RoomyGraph_destroy(g);
  return PASSED;
}
int destroyGraph() {
	RoomyGraph *g = RoomyGraph_make("destroy", 1, 1);
	assert(NULL != g);
	RoomyGraph_destroy(g);
	printf("destroyGraph completed successfully.\n");
	return PASSED;
}
int addNode() {
	// Create a very simple RG so we can add a single node to it.
	RoomyGraph *g = RoomyGraph_make("addNode", 2, 2);
	uint64 node = 3;
	RoomyGraph_addNode(g, node);
	RoomyGraph_sync(g);
	assert(TRUE == RoomyGraph_containsNode(g, node));
	RoomyGraph_destroy(g);
	printf("addNode completed successfully.\n");
	return PASSED;
}
int containsNodeTest() {
	RoomyGraph *g = RoomyGraph_make("containsNodeTest", 2, 2);
	uint64 node = 3;
	RoomyGraph_addNode(g, node);
	RoomyGraph_sync(g);
	uint64 notIn = 4;
	assert(TRUE == RoomyGraph_containsNode(g, node));
	assert(FALSE == RoomyGraph_containsNode(g, notIn));
	RoomyGraph_destroy(g);
	printf("containsNodeTest completed successfully.\n");
	return PASSED;
}
int addMultipleNodes() {
	RoomyGraph *g = RoomyGraph_make("multiNode", 2, 4);
	uint64 first = 1;
	uint64 second = 2;
	RoomyGraph_addNode(g, first);
	RoomyGraph_addNode(g, second);
	RoomyGraph_sync(g);
	
	assert(TRUE == RoomyGraph_containsNode(g, first));
	assert(TRUE == RoomyGraph_containsNode(g, second));

	RoomyGraph_destroy(g);
	printf("addMultipleNodes completed successfully.\n");
	return PASSED;
}

