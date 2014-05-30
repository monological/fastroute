/*
 * graph.h - api to access the arc/node graph.
 *
 */

#ifndef GRAPH_H
#define GRAPH_H

#include <stdint.h>
#include <stdbool.h>

#include "uthash.h"
#include "data.h"
#include "database.h"

/*
 * Forward declare structs.
 *
 */
struct _node;
struct _arcnode;
struct _graph;


/*
 * Simply encapsulates the data. May add more properties later.
 *
 */
typedef struct {
    ArcData *data;
} Arc;


/*
 * ArcNode encapsulates the arc, and has a handle to the
 * dst_node, which only gets loaded when get_arcs is called.
 */
struct _arcnode{
    Arc *arc;
    struct _node *dst_node;
};


/*
 * Node->arcnode->dst_node only gets loaded when get_arcnodes
 * is called, for obvious reasons.
 *
 */
struct _node {
    struct _graph *graph;
    NodeData *data;
    struct _arcnode **arcnodes;
    UT_hash_handle hh;
};


/*
 * Graph holds all nodes. Arcnodes for nodes are initially
 * not loaded.
 *
 */
struct _graph{
    Database *db;
    //Set *nodes;
    //This is the handle to the hash
    struct _node *nodes;
};

/*
 * Put the typedefs here because of circular reference
 * in structs.
 *
 */
typedef struct _arcnode ArcNode;
typedef struct _node Node;
typedef struct _graph Graph;


Graph *load_graph(Database *db);
void close_graph(Graph *graph);

uint32_t get_num_nodes(Graph *graph);
Node *get_node(uint32_t node_id, Graph *graph);
void print_node(Node *node);

/*
 * Note: get_arcnodes will do a set lookup to find
 * each arnode's arc->dst_node based on the node id.
 * This should only be called by the expander when
 * absolutetly needed.
 *
 */
uint32_t get_num_arcnodes(Node *node);
ArcNode **get_arcnodes(Node *node);












#endif
