/*
 **  usage.c - keeps track of arc usage.
 **
 **
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "uthash.h"
#include "graph.h"
#include "usage.h"
#include "main.h"

UsageArc * _new_usage_arc(ArcNode *arcnode){
    UsageArc * ua;
    if((ua = malloc(sizeof(UsageArc))) == NULL){
        perror("malloc");
        exit(1);
    }
    ua->arcnode = arcnode;
    ua->used = false;
    return ua;
}

Usage *new_usage(Graph *graph){
    if(graph == NULL){
        puts("Error: new_usage graph is null.");
        exit(1);
    }
    Usage *usage;
    if((usage = malloc(sizeof(Usage))) == NULL){
        perror("malloc");
        exit(1);
    }
    usage->graph = graph;
    usage->arcs = NULL;

    return usage;
}

void free_usage(Usage *usage){
    free(usage);
}

void mark_usage(bool used, ArcNode *arcnode, Usage *usage){
    if(usage == NULL || arcnode == NULL){
        puts("Error: mark_usage usage or arcnode is null.");
        exit(1);
    }
    UsageArc *usage_arc;
    HASH_FIND_PTR(usage->arcs, &arcnode, usage_arc);
    if(usage_arc == NULL){
        usage_arc = _new_usage_arc(arcnode);
        HASH_ADD_PTR(usage->arcs, arcnode, usage_arc);
    }
    usage_arc->used = used;
    return;
}

bool get_usage(ArcNode *arcnode, Usage *usage){
    if(arcnode == NULL || usage == NULL){
        puts("Error: get_usage arcnode or usage is null.");
        exit(1);
    }
    UsageArc *usage_arc;
    HASH_FIND_PTR(usage->arcs, &arcnode, usage_arc);
    if(usage_arc == NULL){
        usage_arc = _new_usage_arc(arcnode);
        HASH_ADD_PTR(usage->arcs, arcnode, usage_arc);
        usage_arc->used = false;
        return false;
    }
    return usage_arc->used;
}

void reset_usage(Usage *usage){
    if(usage == NULL){
        puts("Error: mark_usage usage is null.");
        exit(1);
    }
    UsageArc *us;
    for(us=usage->arcs; us != NULL; us = us->hh.next){
        us->used = false;
    }
}

void print_usage(Usage *usage){
    if(usage == NULL){
        puts("Error: print_usage usage is null.");
        exit(1);
    }
    uint32_t used = 0;
    UsageArc *us;
    for(us=usage->arcs; us != NULL; us = us->hh.next){
        used += us->used;
    }
    uint32_t total = 0;
    uint32_t i;
    uint32_t num_nodes = 0;
    Node *node;
    num_nodes = get_num_nodes(usage->graph);
    for(i=0; i<num_nodes; i++){
        if((node = get_node(i, usage->graph)) == NULL){
            continue;
        }
        total += get_num_arcnodes(node);
    }
    printf("%i out of %i arcs used.\n", used, total);
}

void update_usage_with_paths(Path **paths, uint32_t num_paths, Usage *usage){
    if(paths == NULL || usage == NULL){
        puts("Error: update_usage_with_paths paths or usage is null.");
        exit(1);
    }
    Path *path;
    ArcNode *arcnode;
    int i,j;
    for(i=0; i<num_paths; i++){
        if((path = paths[i]) == NULL){
            puts("Error: update_usage_with_paths path is null.");
            exit(1);
        }
        for(j=0; j<path->num_arcnodes; j++){
            if((arcnode = path->arcnodes[j]) == NULL){
                puts("Error: update_usage_with_paths arcnode is null.");
                exit(1);
            }
#if USG_DEBUG
            printf("Marking %i:%i:%p as true.\n",i,j,arcnode);
#endif
            mark_usage(true, arcnode, usage);
        }
    }
}











