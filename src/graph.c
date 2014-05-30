/*
 **  graph.c - api to access the node/arc graph.
 **
 **
 **/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "uthash.h"
#include "data.h"
#include "database.h"
#include "graph.h"
#include "expander.h"

#if 0
int _node_compare(const void *key1, const void *key2){
    register int diff = (int)(*((uint32_t *)key1)) - (int)(*((uint32_t *)key2));
    return diff;
}
#endif


Node * _new_node(NodeData *data, Graph *graph){
    if(data == NULL){
        puts("Error: new_node data given is null.");
        exit(1);
    }
    Node * node;
    if((node = malloc(sizeof(Node))) == NULL){
        perror("malloc");
        exit(1);
    }
    node->graph = graph;
    node->data = data;
    node->arcnodes = calloc(data->num_arcnodes, sizeof(ArcNode*));
    return node;
}


Arc * _new_arc(ArcData *data){
    if(data == NULL){
        puts("Error: new_arc data given is null.");
        exit(1);
    }
    Arc *arc;
    if((arc = malloc(sizeof(Arc))) == NULL){
        perror("malloc");
        exit(1);
    }
    arc->data = data;
    return arc;
}


/*
 ** _new_arcnode - dst_node is not loaded until get_arcs is called.
 **
 */
ArcNode * _new_arcnode(Arc *arc){
    if(arc == NULL){
        puts("Error: new_arcnode arc pointer is null.");
        exit(1);
    }
    ArcNode *arcnode;
    if((arcnode = malloc(sizeof(ArcNode))) == NULL){
        perror("malloc");
        exit(1);
    }
    arcnode->arc = arc;
    arcnode->dst_node = NULL;
    return arcnode;
}


Graph *_new_graph(){
    Graph *graph;
    if((graph = malloc(sizeof(Graph))) == NULL){
        perror("malloc");
        exit(1);
    }
    return graph;
}


Graph *load_graph(Database *db){
    if(db == NULL){
        puts("Error: load graph failed. Db is null.");
        exit(1);
    }
    Graph *graph = _new_graph();
    graph->db = db;
    graph->nodes = NULL;

    int i;
    uint64_t count = 0;
    uint64_t total = 0;
    NodeData *node_data;
    ArcData *arc_data;
    Node *node;
    Arc *arc;
    ArcNode *arcnode;

    puts("Loading graph into memory...");
    fseek(db->fp, 0L, SEEK_SET);
    while(!feof(db->fp)){
        node_data = malloc(sizeof(NodeData));
        count = read_database(node_data, sizeof(NodeData), db);
        total += count;
        if(count == 0){
            continue;
        }
        HASH_FIND_INT(graph->nodes, &(node_data->id), node);
        if(node != NULL){
            printf("Error: node %i already in tree.\n", node_data->id);
            exit(1);
        }
        node = _new_node(node_data, graph);
        HASH_ADD_INT(graph->nodes, data->id, node);
        if(node_data->num_arcnodes > 0){
            for(i=0; i<node_data->num_arcnodes; i++){
                arc_data = malloc(sizeof(ArcData));
                total += read_database((void *)arc_data, sizeof(ArcData), db);
                arc = _new_arc(arc_data);
                arcnode = _new_arcnode(arc);
                node->arcnodes[i] = arcnode;
            }
        }
    }
    printf("Loaded %i nodes.\n", (int)HASH_COUNT(graph->nodes));
    printf("Read %llu bytes from database.\n", (long long unsigned int)total);
    return graph;
}


void close_graph(Graph *graph){
    free(graph);
}


uint32_t get_num_nodes(Graph *graph){
    if(graph == NULL){
        puts("Error: get_node graph given is null.");
        exit(1);
    }
    return (uint32_t)HASH_COUNT(graph->nodes);
}

Node *get_node(uint32_t node_id, Graph *graph){
    if(graph == NULL){
        puts("Error: get_node graph given is null.");
        exit(1);
    }
    Node * node;
    HASH_FIND_INT(graph->nodes, &node_id, node);
    return node;
}

void print_node(Node *node){
    if(node == NULL){
        puts("Error: print_node node given is null.");
        exit(1);
    }
    printf("<node %i - num arcnodes %i>\n", node->data->id, node->data->num_arcnodes);
}

uint32_t get_num_arcnodes(Node *node){
    if(node == NULL){
        puts("Error: get_arcnodes node given is null.");
        exit(1);
    }
    return node->data->num_arcnodes;
}

ArcNode **get_arcnodes(Node *node){
    if(node == NULL){
        puts("Error: get_arcnodes node given is null.");
        exit(1);
    }
    int i;
    Graph *graph = node->graph;
    NodeData *nodedata = node->data;
    ArcNode * arcnode;
    Arc * arc;
    ArcData *arcdata;

    /*
     ** Populate the arcnode's dst_node, by searching for it in set.
     */
    for(i=0; i<nodedata->num_arcnodes; i++){
        arcnode = node->arcnodes[i];
        arc = arcnode->arc;
        arcdata = arc->data;
        if(arcnode->dst_node == NULL){
            if((arcnode->dst_node = get_node(arcdata->dst_node_id, graph)) == NULL){
                printf("Error: could not find node '%i' in graph\n", arcdata->dst_node_id);
                exit(1);
            }
        }
    }
    return node->arcnodes;
}


