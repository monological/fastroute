/*
 *  route.h - takes nets and routes them!
 *
 */

#ifndef ROUTE_H
#define ROUTE_H

#include <stdint.h>

#include "graph.h"
#include "expander.h"
#include "data.h"

typedef struct {
    NetData *data;
    const char *name;
    Node *src_node;
    Node **dst_nodes;
    uint32_t num_dst_nodes;
    Path **paths;
    uint32_t num_paths;
} Net;


typedef struct {
    Expander *expander;
} Router;

typedef enum {
    RTR_OK,
    RTR_MAXDEPTH,
    RTR_ERROR,
} routerStatus;

Router *new_router(Expander *expander);
void free_router();

routerStatus route(Net *net, Router *router);

Net *new_net(NetData *data, Node *src_node, Node **dst_nodes, uint32_t num_dst_nodes);
void free_net(Net *net);
void print_net(Net *net);
bool is_arcnode_in_net(ArcNode *arcnode, Net *net);

#endif
