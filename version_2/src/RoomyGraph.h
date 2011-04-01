#include <RoomyHashTable.h>
#include <roomy.h>

/*
RoomyGraphs consist of Nodes and Edges linking two nodes.
Nodes are stores as uint64 to minimize space. Considering,
any given node can have many edges (up to maxEdges, to be
exact) we want to reduce the amount of unnecessary, redundant
space. So allowing a Node to be of some type occupying more
than sizeof(uint64) bytes, would be a waste because there will
be many copies of it around. To map your data structures to
Nodes (which are basically ids) you could have a RoomyHashTable
containing this mapping.
*/
typedef struct {
  uint64 maxEdges; /* The maximum number of edges that leave a Node
maxEdges is a requirement due to the fact that RoomyHashTable cannot
contain values with undermined size and, threfore, increasing size.
Ideally, the Value of the RHT should be a collection of undermined
size that can grow indefintely, but this is not possible.
We should also consider using a [Node -> RoomyArray[Node]]. This
will guarrentee that Edges will not take up all available RAM.*/
  
  RoomyHashTable* graph; /* NodeType -> List[NodeType]
You can think of this as the collection of all edges come out of
the Node as the Key.*/

}RoomyGraph;

/*
Constructs a RoomyGraph where each Node consists of bytesPerElt bytes
with a maximum outgoing edge count of maxEdges.
*/
RoomyGraph* RoomyGraph_make(char* name, uint64 maxEdges, uint64 initialCapacity);

/*
Destroys the RoomyGraph and frees up memory allocated for g.
*/
void RoomyGraph_destroy(RoomyGraph *g);

/* Adds the node to the RoomyGraph (RG).
NOTE: This is a delayed operation therefore you must call RoomyGraph_sync before
the node is ensured to be added to the RG */
void RoomyGraph_addNode(RoomyGraph *g, uint64 node);

/* Completes all delayed operations */
void RoomyGraph_sync(RoomyGraph *g);

/* Returns 1 if the RoomyGraph contains a node equal to the provided node, or 0
if no node can be found.
NOTE: It is recommended to ensure that the RoomyGraph has been sync'd before
calling this function. */
int RoomyGraph_containsNode(RoomyGraph *g, uint64 node);

/* Prints the contents of the RoomyGraph to the console */
void RoomyGraph_print(RoomyGraph *g);

/* Returns the number of nodes in the RoomyGraph */
uint64 RoomyGraph_nodeCount(RoomyGraph *g);

/* Adds an directed edge originating at from and terminating at to. It is
assumed that the nodes have already been added. There is no check to
ensure this, though.
NOTE: this is a delayed operation */
void RoomyGraph_addEdge(RoomyGraph *g, uint64 from, uint64 to);

/* Returns 1 if an edge originating at from and terminating at to is contained
within the RoomyGraph */
int RoomyGraph_containsEdge(RoomyGraph *g, uint64 from, uint64 to);
typedef struct {
uint64 count;
uint64 *child;
} Children;
Children* RoomyGraph_getChildren(RoomyGraph *g, uint64 parent);


