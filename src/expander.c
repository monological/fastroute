/*
 **  expander.c - Find lowest cost path from source to dest node.
 **
 **
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#include "main.h"
#include "data.h"
#include "graph.h"
#include "pqueue.h"
#include "expander.h"
#include "usage.h"


static inline double _distance(ExpNode *from, Node *to){
    register int32_t xdiff = (to->data->x_coord) - (from->node->data->x_coord);
    register int32_t ydiff = (to->data->y_coord) - (from->node->data->y_coord);
    if(xdiff < 0)
        xdiff = -xdiff;
    if(ydiff < 0)
        ydiff = -ydiff;
    xdiff >>= 11;
    ydiff >>= 11;
    return (double)(xdiff + ydiff);
}

double _get_expnode_cost(ExpNode *exp_node){
    if(exp_node == NULL)
        return 0;
    return exp_node->parent->cost +
        exp_node->node->data->cost +
        _distance(exp_node, exp_node->expander->dst_nodes[0]);
}

uint32_t _get_expnode_depth(ExpNode *exp_node){
    if(exp_node == NULL)
        return 0;
    return exp_node->parent->depth + 1;
}

bool _is_expnode_dst(ExpNode *exp_node){
    if(exp_node == NULL){
        puts("Error: is_expnode_dst exp_node is null.");
        exit(1);
    }
    int i;
    for(i=0; i<exp_node->expander->num_dst_nodes; i++){
        if(exp_node->node == exp_node->expander->dst_nodes[i])
            return true;
    }
    return false;
}

ExpNode *_new_expnode(ExpNode *parent, Node *node, Expander *expander){
    ExpNode *exp_node;
    if((exp_node = malloc(sizeof(ExpNode))) == NULL){
        perror("malloc");
        exit(1);
    }
    exp_node->expander = expander;
    exp_node->parent = parent;
    exp_node->node = node;
    if(exp_node->parent == NULL){
        exp_node->cost = exp_node->node->data->cost;
        exp_node->depth = 0;
    }else{
        exp_node->cost = _get_expnode_cost(exp_node);
        exp_node->depth = _get_expnode_depth(exp_node);
    }
    exp_node->arcnode = NULL;

    //This will get set by pq, when inserted.
    exp_node->position = 0;
    return exp_node;
}

void _free_expnode(ExpNode *exp_node){
    free(exp_node);
}

/*
 **  Define all the priority queue callbacks.
 **
 */
static int cmp_priority(double next, double curr){
    return (next > curr);
}

static double get_priority(void *a){
    return (double)((ExpNode *)a)->cost;
}

static void set_priority(void *a, double priority){
    ((ExpNode *)a)->cost = priority;
}

static size_t get_position(void *a){
    return  ((ExpNode *)a)->position;
}

static void set_position(void *a, size_t position){
    ((ExpNode *)a)->position = position;
}

void _init_exp_nodes(Expander *expander){
    if(expander == NULL || expander->graph == NULL){
        puts("Error: _init_exp_nodes expander or graph is null.");
        exit(1);
    }
    expander->exp_nodes = pqueue_init(
            (size_t)get_num_nodes(expander->graph),
            cmp_priority,
            get_priority,
            set_priority,
            get_position,
            set_position);
    if(expander->exp_nodes == NULL){
        puts("Error: failed to malloc priority queue.");
        exit(1);
    }
}

/*
 ** Begin public method implementations.
 **
 */

Expander *new_expander(Graph *graph, Usage *usage){
    if(graph == NULL){
        puts("Error: new_expander graph is null.");
        exit(1);
    }
    Expander *expander;
    if((expander = malloc(sizeof(Expander))) == NULL){
        perror("malloc");
        exit(1);
    }
    expander->graph = graph;
    expander->usage = usage;
    expander->src_node = NULL;
    expander->dst_nodes = NULL;
    expander->num_dst_nodes = 0;
    _init_exp_nodes(expander);
    return expander;
}


void free_expander(Expander *expander){
    pqueue_free(expander->exp_nodes);
    free(expander);
}


void _push(ExpNode *exp_node){
    if(exp_node == NULL){
        puts("Error: _push argument is null.");
        exit(1);
    }
    pqueue_t *pq = exp_node->expander->exp_nodes;
    if(pqueue_insert(pq, exp_node) != 0){
        puts("Error: failed to insert exp node into priority queue.");
        exit(1);
    }
}

ExpNode *_pop(Expander *expander){
    if(expander == NULL || expander->exp_nodes == NULL){
        puts("Error: _pop expander or exp_nodes is null.");
        exit(1);
    }

    pqueue_t *pq = expander->exp_nodes;
    ExpNode *exp_node;
    exp_node = pqueue_pop(pq);
    return exp_node;
}

/*
 **  Find all the arcnodes from the dst back to the
 **  src node.
 **
 */
void _traceback(ExpNode *exp_node, ArcNode **arcnodes, uint32_t num_arcnodes){
    if(exp_node == NULL || num_arcnodes == 0){
        return;
    }
    arcnodes[--num_arcnodes] = exp_node->arcnode;
    _traceback(exp_node->parent, arcnodes, num_arcnodes);
}

inline bool _is_node_in_traceback(ExpNode *current, Node *match){
    if(current == NULL)
        return false;
    if(current->node == match)
        return true;
    return _is_node_in_traceback(current->parent, match);
}

bool _should_push(ExpNode *current, ArcNode *arcnode){
    ArcData *d = arcnode->arc->data;
    //illegal arc check
    if(d->is_excluded)
        return false;
    //used or not
    if(get_usage(arcnode, current->expander->usage) == true)
        return false;
    //loop check
    if(_is_node_in_traceback(current, arcnode->dst_node) == true){
        return false;
    }
    return true;
}

/*
 ** Expand from src to dsts. Malloc num of path pointers,
 ** set num_paths and return with status.
 **
 */
expanderStatus expand(
        Node *src_node,
        Node **dst_nodes,
        uint32_t num_dst_nodes,
        Path ***paths,
        uint32_t *num_paths,
        Expander *expander
        ){
    if(src_node == NULL || dst_nodes == NULL || expander == NULL){
        puts("Error: expand argument is null.");
        exit(1);
    }
    if(num_dst_nodes == 0){
        puts("Error: expand num_dst_nodes is zero.");
        exit(1);
    }
    /*
     **   0[*] --> 1[*] --> 2[*]3[*]4[*]...
     **
     **   Paths has address 0 and is NULL.
     **   We will allocate that and it will
     **   point to 1. 1 will points to array
     **   of path pointers.
     **
     */
    if(*paths != NULL){
        puts("Error: expand paths is not null, but must be.");
        exit(1);
    }
    if(num_paths == NULL){
        puts("Error: expand num_paths is null.");
        exit(1);
    }
    if(*num_paths != 0){
        puts("Error: expand num_paths is not zero.");
        exit(1);
    }

    expander->src_node = src_node;
    if((expander->dst_nodes = calloc(num_dst_nodes, sizeof(Node*))) == NULL){
        perror("calloc");
        exit(1);
    }

    expander->num_dst_nodes = num_dst_nodes;

    int i;
    for(i=0; i<num_dst_nodes; i++){
        expander->dst_nodes[i] = dst_nodes[i];
    }

#if EXP_DEBUG
    printf("Routing from src %i to dest", src_node->data->id);
    for(i=0; i<num_dst_nodes; i++){
        printf(" %i", dst_nodes[i]->data->id);
    }
    puts("");
#endif

    expanderStatus exp_stat = EXP_ERROR;

    ArcNode *arcnode;
    ArcNode **arcnodes;
    ArcNode **found_arcnodes;
    uint32_t depth = 0;
    ExpNode *current, *next;
    current = _new_expnode(NULL, src_node, expander);

    Path *path;
    bool paths_found = false;

    // Start the expansion.
    _push(current);
    current = _pop(expander);
    while(current != NULL){
        depth += 1;
        if(depth > MAX_DEPTH){
            exp_stat = EXP_MAXDEPTH;
            break;
        }
#if EXP_DEBUG
        printf("parent node %i cost:%f depth:%i\n",
                current->node->data->id, current->cost, current->depth);
#endif
        arcnodes = get_arcnodes(current->node);
        for(i=0; i <get_num_arcnodes(current->node); i++){
            arcnode = arcnodes[i];
            if(_should_push(current, arcnode) == false)
                continue;
            next = _new_expnode(current, arcnode->dst_node, expander);
#if EXP_DEBUG
            printf("child node %i cost:%f depth:%i\n",
                    next->node->data->id, next->cost, next->depth);
#endif
            next->arcnode = arcnode;
            if(_is_expnode_dst(next)){
                if((found_arcnodes = calloc(next->depth, sizeof(ArcNode*))) == NULL){
                    perror("calloc");
                    exit(1);
                }
#if EXP_DEBUG
                printf("Found path with depth %i.\n", next->depth);
#endif
                _traceback(next, found_arcnodes, next->depth);
                path = new_path(found_arcnodes, next->depth);
                free(found_arcnodes);
                if(*num_paths == 0){
                    (*num_paths)++;
                    if(((*paths) = calloc((*num_paths), sizeof(Path*))) == NULL){
                        perror("calloc");
                        exit(1);
                    }
                }else{
                    (*num_paths)++;
                    if(((*paths) = realloc((*paths), sizeof(Path*)*(*num_paths))) == NULL){
                        perror("realloc");
                        exit(1);
                    }
                }
                (*paths)[(*num_paths)-1] = path;
                if((*num_paths) >= MAX_PATHS){
                    update_usage_with_paths((*paths), (*num_paths), expander->usage);
                    paths_found = true;
                    exp_stat = EXP_OK;
                    break;
                }
            }
            _push(next);
        }
        if(paths_found)
            break;
        current = _pop(expander);
    }

#if USG_DEBUG
    print_usage(expander->usage);
#endif

    // Return expander to original state.
    while((next = pqueue_pop(expander->exp_nodes)) != NULL){
        if(next == NULL) continue;
        _free_expnode(next);
    }
    pqueue_free(expander->exp_nodes);
    _init_exp_nodes(expander);
    expander->src_node = NULL;
    free(expander->dst_nodes);
    expander->dst_nodes = NULL;

    return exp_stat;
}




















