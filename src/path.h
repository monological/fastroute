/*
 *  path.h - collection of arcs used to route from source to dest node.
 *
 */


#ifndef PATH_H
#define PATH_H

#include "graph.h"

typedef struct {
    Node *src_node;
    Node *dst_node;
    ArcNode **arcnodes;
    uint32_t num_arcnodes;
} Path;

Path * new_path(ArcNode **arcnodes, uint32_t num_arcnodes);
void print_path(Path *path);
uint32_t *get_path_arc_ids(Path *path);
bool is_arcnode_in_path(ArcNode *arcnode, Path *path);
bool is_arcnode_in_paths(ArcNode *arcnode, Path **paths, uint32_t num_paths);

#endif
