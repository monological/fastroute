/*
 ** create_db - takes a graph.csv and writes a condensed binary
 **             version called graph.db.
 **
 **/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>

#include "data.h"
#include "database.h"
#include "util.h"

#include "create_db.h"

void usage(char **argv){
    printf("Usage: %s -c <name_graph.csv> -v\n", argv[0]);
    puts("");
    puts("Creates a graph.db from a graph.csv.");
}

int main(int argc, char *argv[]){
    if(argc < 2){
        usage(argv);
        exit(1);
    }
    bool verbose = false;
    const char *graph_csv = NULL;

    uint32_t opt;
    opterr = 0;
    while((opt = getopt(argc, argv, "c:v")) != -1){
        switch(opt){
            case 'c':
                graph_csv = optarg;
                break;
            case 'v':
                verbose = true;
                break;
            case '?':
                usage(argv);
                if(optopt == 'c'){
                    fprintf(stderr, "Error: switch -%c requires a graph.csv.\n", optopt);
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

    int i, j;
    const char *name;

    Split *csv_sp = create_split(graph_csv, "_");

    if(csv_sp->n != 2){
        usage(argv);
        puts("Error: input file must be in the form <name>_graph.(csv|db).");
        exit(1);
    }

    name = get_split_dup(csv_sp, 0);

    printf("Working on graph %s...\n", graph_csv);

    FILE *fp;
    if((fp = fopen(graph_csv, "r")) == NULL){
        perror("failed to open file.");
        return 1;
    }

    char line_string[4194304];
    bool legend_line = true;

    /* node varibles we'll read into.*/
    uint32_t node_id;
    uint8_t x_coord;
    uint8_t y_coord;
    uint8_t cost;
    uint32_t num_arcnodes;

    uint32_t arc_id;
    bool is_directional;
    bool is_excluded;
    uint32_t src_node_id;
    uint32_t dst_node_id;

    NodeData *node_data;
    ArcData *arc_data;

    char db_name[256];
    sprintf(db_name, "%s.db", name);
    Database *db = create_database(db_name);

    while(fgets(line_string, sizeof(line_string), fp) != NULL){
        if(legend_line){
            line_string[strlen(line_string)-1] = '\0';
            printf("Got csv legend %s.\n", line_string);
            legend_line = false;
            printf("Writing db to disk...\n");
            continue;
        }

        Split *sp = create_split(line_string, ",");
        if(sp->n < 5){
            printf("Error: line %s is missing certain fields.", line_string);
            exit(1);
        }
        node_id = atoi(sp->words[nodeId]);
        x_coord = atoi(sp->words[xCoord]);
        y_coord = atoi(sp->words[yCoord]);
        cost = atoi(sp->words[nodeCost]);
        num_arcnodes = 0;

        if(strcmp(sp->words[sp->n-1], "EOL\n") != 0){
            printf("Error: invalid line '%s'. Buffer is probably too small.\n", line_string);
            exit(1);
        }

        // Find number of arcnodes.
        for(i=arcNodeList;strcmp(sp->words[i],"EOL\n")!=0;i++){
            num_arcnodes++;
        }
        if(verbose){
            printf("Found %i arcnodes for line.\n", num_arcnodes);
        }

        // Malloc the list
        char **arc_strings;
        if((arc_strings = malloc(sizeof(char *)*num_arcnodes)) == NULL){
            perror("malloc");
            exit(1);
        }

        // Copy arnodes into list, w/o commas.
        char *arc_str;
        for(i=arcNodeList, j=0;strcmp(sp->words[i],"EOL\n")!=0;i++,j++){
            arc_str = strdup(sp->words[i]);
            //arc_str[strlen(arc_str)-1] = '\0';
            //arc_str++;
            arc_strings[j] = strdup(arc_str);
        }

        // Create new nodeData and write to db.
        node_data = new_node_data(node_id, x_coord, y_coord, cost, num_arcnodes);
        write_database((const void *)node_data, sizeof(*node_data), db);
        free_node_data(node_data);

        // Create arcData for each, and write to db.
        for(i=0; i<num_arcnodes; i++){
            arc_str = arc_strings[i];
            Split *arc_sp = create_split(arc_str, ":");
            if(arc_sp->n != 4){
                printf("Error: arc split length %i, != 4 for '%s'\n", arc_sp->n, arc_str);
                exit(1);
            }
            arc_id = atoi(arc_sp->words[arcId]);
            is_directional = (bool)atoi(arc_sp->words[isDirectional]);
            is_excluded = (bool)atoi(arc_sp->words[isExcluded]);
            src_node_id = node_id;
            dst_node_id = atoi(arc_sp->words[toNodeId]);
            arc_data = new_arc_data(arc_id, is_directional, is_excluded, src_node_id, dst_node_id);
            write_database((const void *)arc_data, sizeof(*arc_data), db);
            free_arc_data(arc_data);
            free_split(arc_sp);
        }

        free_split(sp);
    }

    close_database(db);

    printf("done!\n");
    fclose(fp);
    return 0;
}


