/*
 *  expander.h - finds lowest cost path from source to destination node.
 *
 */


#ifndef EXPANDER_H
#define EXPANDER_H


#include "pqueue.h"
#include "graph.h"
#include "path.h"
#include "usage.h"

#define MAX_PATHS 1
#define MAX_DEPTH 1000000

/*
 * Forward declarations
 *
 */
struct _expander;

struct _expnode{
    //internal vars
    struct _expander *expander;
    struct _expnode *parent;
    uint32_t depth;

    //graph vars
    Node *node;
    ArcNode *arcnode;

    //pqueue vars
    double cost;
    size_t position;
};
typedef struct _expnode ExpNode;


struct _expander{
    Graph *graph;
    Usage *usage;
    Node *src_node;
    Node **dst_nodes;
    uint32_t num_dst_nodes;
    pqueue_t *exp_nodes;
};
typedef struct _expander Expander;

typedef enum {
    EXP_OK,
    EXP_MAXDEPTH,
    EXP_ERROR,
} expanderStatus;

Expander *new_expander(Graph *graph, Usage *usage);
void free_expander(Expander *expander);

expanderStatus expand(
    Node *src_node,
    Node **dst_nodes,
    uint32_t num_dst_nodes,
    Path ***paths,
    uint32_t *num_paths,
    Expander *expander);




#endif
