/*
 **  path.c - collection of arcs used to route from source to dest node.
 **
 **
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "graph.h"
#include "path.h"

Path * new_path(ArcNode **arcnodes, uint32_t num_arcnodes){
    if(arcnodes == NULL){
        puts("Error: new_path arcnodes is null.");
        exit(1);
    }
    if(num_arcnodes == 0){
        return NULL;
    }
    Path *path;
    if((path = malloc(sizeof(Path))) == NULL){
        perror("malloc");
        exit(1);
    }
    path->arcnodes = calloc(num_arcnodes, sizeof(ArcNode*));
    int i;
    ArcNode *arcnode;
    for(i=0; i<num_arcnodes; i++){
        arcnode = arcnodes[i];
        if(arcnode == NULL){
            printf("Error: new_path arcnode %i given is null.\n", i);
            exit(1);
        }
        if(i == 0){
            Graph *graph = arcnode->dst_node->graph;
            path->src_node = get_node(arcnode->arc->data->src_node_id, graph);
            if(path->src_node == NULL){
                puts("Error: new_path failed to get src node.");
                exit(1);
            }
        }
        if(i == num_arcnodes-1){
            path->dst_node = arcnode->dst_node;
        }
        path->arcnodes[i] = arcnode;
    }
    path->num_arcnodes = num_arcnodes;
    return path;
}

void print_path(Path *path){
    if(path == NULL){
        puts("Error: print_path path is null.");
        exit(1);
    }
    printf("src %i ->\n", path->src_node->data->id);
    int i;
    for(i=0; i<path->num_arcnodes; i++){
        if(i == (path->num_arcnodes)-1){
            printf("    %i dst\n", path->arcnodes[i]->dst_node->data->id);
        }else{
            printf("    %i ->\n", path->arcnodes[i]->dst_node->data->id);
        }
    }
}


uint32_t *get_path_arc_ids(Path *path){
    if(path == NULL){
        puts("Error: get_path_arc_ids path is null.");
        exit(1);
    }
    uint32_t *arc_ids;
    if((arc_ids = calloc(path->num_arcnodes, sizeof(uint32_t))) == NULL){
        perror("calloc");
        exit(1);
    }
    int i;
    ArcNode *arcnode;
    for(i=0; i<path->num_arcnodes; i++){
        arcnode = path->arcnodes[i];
        arc_ids[i] = arcnode->arc->data->id;
    }
    return arc_ids;
}


bool is_arcnode_in_path(ArcNode *arcnode, Path *path){
    if(path == NULL){
        puts("Error: get_path_arc_ids path is null.");
        exit(1);
    }
    int i;
    for(i=0; i<path->num_arcnodes; i++){
        if(path->arcnodes[i] == arcnode){
            return true;
        }
    }
    return false;
}


bool is_arcnode_in_paths(ArcNode *arcnode, Path **paths, uint32_t num_paths){
    if(arcnode == NULL || paths == NULL ){
        puts("Error: is_arcnode_in_paths arcnode or paths is null.");
        exit(1);
    }
    int i;
    Path *path;
    for(i=0; i<num_paths; i++){
        if((path = paths[i]) == NULL){
            puts("Error: is_arcnode_in_paths path is null");
            exit(1);
        }
        if(is_arcnode_in_path(arcnode, path)){
            return true;
        }
    }
    return false;
}




