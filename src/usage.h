/*
 *  usage.h - keeps track of which arc usage.
 *
 *
 */

#ifndef USAGE_H
#define USAGE_H

#include "graph.h"
#include "path.h"
#include "uthash.h"

typedef struct {
    ArcNode *arcnode;
    bool used;
    UT_hash_handle hh;
} UsageArc;

typedef struct {
    Graph *graph;
    UsageArc *arcs;
} Usage;

Usage *new_usage(Graph *graph);
void free_usage(Usage *usage);

void mark_usage(bool used, ArcNode *arcnode, Usage *usage);
bool get_usage(ArcNode *arcnode, Usage *usage);
void reset_usage(Usage *usage);
void print_usage(Usage *usage);
void update_usage_with_paths(Path **paths, uint32_t num_paths, Usage *usage);

#endif
