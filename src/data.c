/*
 ** data.c - Arc and node data structs and functions.
 **
 **
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "data.h"


NetData *new_net_data(const char *name,
        uint32_t src_node_id,
        uint32_t *dst_node_ids,
        uint32_t num_dst_node_ids
        ){
    if(name == NULL || dst_node_ids == NULL){
        puts("Error: new_net_data name or dst_node_ids is null.");
        exit(1);
    }
    if(num_dst_node_ids == 0){
        puts("Error: new_net_data num_dst_node_ids is zero.");
        exit(1);
    }
    NetData *data;
    if((data = malloc(sizeof(NetData))) == NULL){
        perror("malloc");
        exit(1);
    }
    data->name = strdup(name);
    data->src_node_id = src_node_id;
    if((data->dst_node_ids = malloc(sizeof(uint32_t)*num_dst_node_ids)) == NULL){
        perror("malloc");
        exit(1);
    }
    int i;
    for(i=0; i<num_dst_node_ids; i++){
        data->dst_node_ids[i] = dst_node_ids[i];
    }
    data->num_dst_node_ids = num_dst_node_ids;
    return data;
}

void free_net_data(NetData *data){
    free((char *)data->name);
    free(data->dst_node_ids);
    free(data);
}

NodeData * new_node_data(uint32_t id,
        uint8_t x_coord,
        uint8_t y_coord,
        uint8_t cost,
        uint32_t num_arcnodes
        ){
    NodeData *n;
    if((n = malloc(sizeof(NodeData))) == NULL){
        puts("malloc failed.");
        exit(1);
    }
    n->id = id;
    n->x_coord = x_coord;
    n->y_coord = y_coord;
    n->cost = cost;
    n->num_arcnodes = num_arcnodes;
    return n;
}

void free_node_data(NodeData *data){
    free(data);
}

ArcData * new_arc_data(uint32_t id,
        bool is_directional,
        bool is_excluded,
        uint32_t src_node_id,
        uint32_t dst_node_id
        ){
    ArcData *a;
    if((a = malloc(sizeof(ArcData))) == NULL){
        puts("malloc failed.");
        exit(1);
    }
    a->id = id;
    a->is_directional = is_directional;
    a->is_excluded = is_excluded;
    a->src_node_id = src_node_id;
    a->dst_node_id = dst_node_id;
    return a;
}

void free_arc_data(ArcData *data){
    free(data);
}


