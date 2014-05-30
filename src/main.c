/*
 **  main.c - it all starts here.
 **
 **
 **
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>

#include "data.h"
#include "database.h"
#include "graph.h"
#include "expander.h"
#include "usage.h"
#include "route.h"
#include "util.h"
#include "main.h"

void usage(char **argv){
    printf("Usage: %s -d <database> -u <unrouted.csv> -r <routed.csv>\n", argv[0]);
}

int main(int argc, char *argv[]){

    const char *database_name = NULL;
    const char *unrouted_csv = NULL;
    const char *routed_csv = NULL;

    uint32_t opt;

    if(argc < 4){
        usage(argv);
        exit(1);
    }

    opterr = 0;
    while((opt = getopt(argc, argv, "d:u:r:")) != -1){
        switch(opt){
            case 'd':
                database_name = optarg;
                break;
            case 'u':
                unrouted_csv = optarg;
                break;
            case 'r':
                routed_csv = optarg;
                break;
            case '?':
                usage(argv);
                if(optopt == 'd'){
                    fprintf(stderr, "Error: switch -%c requires a database.\n", optopt);
                }else if(optopt == 'u'){
                    fprintf(stderr, "Error: switch -%c requires an unrouted.csv.\n", optopt);
                }else if(optopt == 'r'){
                    fprintf(stderr, "Error: switch -%c requires a routed.csv.\n", optopt);
                }else{
                    fprintf(stderr, "Error: unknown switch given -%c.\n", optopt);
                }
                exit(1);
                break;
            default:
                usage(argv);
                exit(1);
        }
    }

    if(strcmp(unrouted_csv, routed_csv) == 0){
        puts("Error: files unrouted.csv and routed.csv cannot be the same file.");
        exit(1);
    }

    // parse the unrouted csv and get all the nets.
    printf("Parsing nets in '%s'...\n", unrouted_csv);
    FILE *fp;
    if((fp = fopen(unrouted_csv, "r")) == NULL){
        perror("open");
        exit(1);
    }

    int i, j;
    bool legend_line = true;
    uint32_t num_net_datas = 0;
    uint32_t max_net_datas = 20000;
    NetData **net_datas;
    if((net_datas = calloc(max_net_datas, sizeof(NetData*))) == NULL){
        perror("calloc");
        exit(1);
    }
    NetData *net_data;
    const char *name;
    uint32_t src_node_id;
    uint32_t num_dst_node_ids;
    uint32_t *dst_node_ids;
    char line_string[4194304];

    while(fgets(line_string, sizeof(line_string), fp) != NULL){
        line_string[strlen(line_string)-1] = '\0';
        if(legend_line){
            printf("Got csv legend %s.\n", line_string);
            legend_line = false;
            continue;
        }
        Split *sp = create_split(line_string, ",");
        if(sp->n < 5){
            printf("Error: line '%s' is missing certain fields.\n", line_string);
            exit(1);
        }
        name = get_split_dup(sp, netName);
        src_node_id = atoi(get_split_dup(sp, netSrcNodeId));
        num_dst_node_ids = atoi(get_split_dup(sp, netNumDstNodeIds));
        if((dst_node_ids = malloc(sizeof(uint32_t) * num_dst_node_ids)) == NULL){
            perror("malloc");
            exit(1);
        }
        if(strcmp(sp->words[sp->n-1], "EOL") != 0){
            printf("Error: invalid line '%s'. Buffer is probably too small.\n", line_string);
            exit(1);
        }
        for(i=0; i<num_dst_node_ids; i++){
            dst_node_ids[i] = atoi(get_split_dup(sp, netDstNodeIds+i));
        }
        if(strcmp(sp->words[netDstNodeIds+i], "EOL") != 0){
            printf("Error: invalid line '%s'. No EOL found.\n", line_string);
            exit(1);
        }
        net_data = new_net_data(name, src_node_id, dst_node_ids, num_dst_node_ids);
        if(net_data == NULL){
            puts("Error: failed to malloc net_data.");
            exit(1);
        }
        net_datas[num_net_datas] = net_data;
        if(++num_net_datas >= max_net_datas){
            max_net_datas *= 2;
#if DEBUG
            printf("Reallocating nets array from %i to %i.\n", max_net_datas-(max_net_datas/2), max_net_datas);
#endif
            if((net_datas = realloc(net_datas, sizeof(NetData*)*(max_net_datas))) == NULL){
                perror("realoc");
                exit(1);
            }
        }
        free((char *)name);
        free(dst_node_ids);
        free_split(sp);
    }
    fclose(fp);
    printf("Read in %i nets.\n", num_net_datas);

#if 0
    for(i=0; i<num_net_datas; i++){
        net_data = net_datas[i];
        printf("net %s %i %i", net_data->name, net_data->src_node_id, net_data->num_dst_node_ids);
        for(j=0;j<net_data->num_dst_node_ids;j++){
            printf(" %i", net_data->dst_node_ids[j]);
        }
        puts("");
    }
    exit(1);
#endif

    Database *db;
    db = open_database(database_name);

    Graph *graph;
    graph = load_graph(db);

    Usage *usg;
    usg = new_usage(graph);
    Expander *expander = new_expander(graph, usg);

    puts("Preparing nets for routing...");
    Net *net;
    Net **nets = calloc(num_net_datas, sizeof(Net*));
    Node *src_node;
    Node **dst_nodes;
    for(i=0; i<num_net_datas; i++){
        net_data = net_datas[i];
        if(net_data == NULL){
            puts("Error: net_data is null.");
            exit(1);
        }
        if((src_node = get_node(net_data->src_node_id, graph)) == NULL){
            printf("Error: could not find src node %i for net '%s' in graph.\n",
                    net_data->src_node_id, net_data->name);
            exit(1);
        }
        if((dst_nodes = calloc(net_data->num_dst_node_ids, sizeof(Node*))) == NULL){
            perror("calloc");
            exit(1);
        }
        for(j=0; j<net_data->num_dst_node_ids; j++){
            if((dst_nodes[j] = get_node(net_data->dst_node_ids[j], graph)) == NULL){
                printf("Error: could not find dst node %i for net '%s' in graph.\n",
                        net_data->dst_node_ids[j], net_data->name);
                exit(1);
            }
        }
        nets[i] = new_net(net_data, src_node, dst_nodes, net_data->num_dst_node_ids);
        free(dst_nodes);
    }

    routerStatus rtr_stat;
    Router *router = new_router(expander);

    time_t start, end;
    puts("Routing nets...");
    start = time(NULL);
    if(start == (time_t)-1) {
        fprintf(stderr, "Error: 'time()'\n");
        exit(1);
    }
    for(i=0; i<num_net_datas; i++){
        net = nets[i];
#if DEBUG
        printf("Routing net '%s'...\n", net->name);
#endif
        rtr_stat = route(net, router);
        if((i % 2000) == 0){
            printf("Done with %i of %i.\n", i+1, num_net_datas);
        }
        if(rtr_stat == RTR_OK){
#if DEBUG
            for(j=0; j<net->num_paths; j++){
                print_path(net->paths[j]);
            }
#endif
        }else if(rtr_stat == RTR_MAXDEPTH){
            printf("Error: expander exceeded max depth for net '%s' %i of %i.\n", net->name, i, num_net_datas);
        }else{
            printf("Error: failed to route net '%s' %i of %i.\n", net->name, i, num_net_datas);
        }
    }
    end = time(NULL);
    printf("Finished routing in %.2f seconds.\n", difftime(end,start));
    print_usage(usg);


    printf("Dumping paths to '%s'.\n", routed_csv);
    if((fp = fopen(routed_csv, "w")) == NULL){
        perror("failed to open routed csv file.");
        return 1;
    }
    fprintf(fp, "net_name,num_arcs,arc_id1,EOL\n");
    int k;
    Path *path;
    uint32_t num = 0;
    uint32_t *arc_ids;
    for(i=0; i<num_net_datas; i++){
        net = nets[i];
        num = 0;
        fprintf(fp, "%s,", strdup(net->name));
        for(j=0; j<net->num_paths; j++){
            path = net->paths[j];
            num += path->num_arcnodes;
        }
        fprintf(fp, "%i,", num);
        for(j=0; j<net->num_paths; j++){
            path = net->paths[j];
            arc_ids = get_path_arc_ids(path);
            for(k=0; k<path->num_arcnodes; k++){
                fprintf(fp, "%i,", arc_ids[k]);
            }
        }
        fprintf(fp, "EOL\n");
    }
    fclose(fp);

    //clean up and quit
    for(i=0; i<num_net_datas; i++)
        free_net(nets[i]);
    free(nets);
    free_expander(expander);
    free_usage(usg);
    close_graph(graph);
    close_database(db);
    puts("done!");
    return 0;
}













