/*
 ** route.c - calls expander on list or nets.
 **
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#include "database.h"
#include "graph.h"
#include "expander.h"
#include "route.h"
#include "main.h"

Router *new_router(Expander *expander){
    if(expander == NULL){
        puts("Error: new_router expander is null.");
        exit(1);
    }
    Router *router;
    if((router = malloc(sizeof(Router))) == NULL){
        perror("malloc");
        exit(1);
    }
    router->expander = expander;
    return router;
}

void _add_paths_to_net(Net *net, Path **paths, uint32_t num_paths){
    if(net == NULL){
        puts("Error: _add_paths_to_net net is null.");
        exit(1);
    }
    if(paths == NULL){
        printf("Error: _add_paths_to_net paths is null and num_paths is %i.\n", num_paths);
        exit(1);
    }
    if(num_paths == 0){
        net->paths = NULL;
        net->num_paths = 0;
        return;
    }
    if((net->paths = calloc(num_paths, sizeof(Path*))) == NULL){
        perror("calloc");
        exit(1);
    }
    int i;
    for(i=0; i<num_paths; i++){
        net->paths[i] = paths[i];
    }
    net->num_paths = num_paths;
    return;
}

routerStatus route(Net *net, Router *router){
    if(net == NULL || router == NULL){
        puts("Error: route net or router is null.");
        exit(1);
    }
    Expander *expander = router->expander;
    expanderStatus exp_stat;

    Path **paths = NULL;
    uint32_t num_paths = 0;

    exp_stat = expand(
        net->src_node,
        net->dst_nodes,
        net->num_dst_nodes,
        &paths,
        &num_paths,
        expander);
    if(exp_stat == EXP_MAXDEPTH){
        return RTR_MAXDEPTH;
    }else if(exp_stat == EXP_OK){
        _add_paths_to_net(net, paths, num_paths);
        free(paths);
        return RTR_OK;
    }

    assert(exp_stat == EXP_ERROR);
    return RTR_ERROR;
}

Net *new_net(NetData *data, Node *src_node, Node **dst_nodes, uint32_t num_dst_nodes){
    if(data == NULL || src_node == NULL || dst_nodes == NULL){
        puts("Error: new_net data or src_node or dst_nodes is null.");
        exit(1);
    }
    if(num_dst_nodes == 0){
        printf("Error: new_net net '%s' num_dst_nodes given is zero.\n", data->name);
        exit(1);
    }
    Net *net;
    if((net = malloc(sizeof(Net))) == NULL){
        perror("malloc");
        exit(1);
    }
    net->data = data;
    net->name = strdup(data->name);
    net->src_node = src_node;
    if((net->dst_nodes = calloc(num_dst_nodes, sizeof(Node*))) == NULL){
        perror("malloc");
        exit(1);
    }
    net->num_dst_nodes = num_dst_nodes;
    int i;
    for(i=0; i<num_dst_nodes; i++){
        net->dst_nodes[i] = dst_nodes[i];
    }
    net->paths = NULL;
    net->num_paths = 0;
    return net;
}

void free_net(Net *net){
    free((char *)net->name);
    free(net->dst_nodes);
    if(net->paths != NULL)
        free(net->paths);
    free(net);
}

void print_net(Net *net){
    if(net == NULL){
        puts("Error: print_net net is null");
        exit(1);
    }
    printf("<net %s - %i - %i>\n", net->name, net->src_node->data->id, net->num_dst_nodes);
}

bool is_arcnode_in_net(ArcNode *arcnode, Net *net){
    if(net == NULL){
        puts("Error: is_arcnode_in_net arcnode or net is null");
        exit(1);
    }
    Path *path;
    int i;
    for(i=0; i<net->num_paths; i++){
        if((path = net->paths[i]) == NULL){
            puts("Error: net has path that is null.");
            exit(1);
        }
        if(is_arcnode_in_path(arcnode, path) == true)
            return true;
    }
    return false;
}

