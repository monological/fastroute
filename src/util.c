
/*
 * util.c - utility functions. :)
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "util.h"


/*
 * Split utility function and struct.
 *
 */

Split *create_split(const char *s, const char *delim){
    int num = 0;
    const char *c;
    for(c=s;*c != '\0';c++){
        if(*c == *delim)
            num++;
    }
    Split *sp;
    if((sp = malloc(sizeof(Split))) == NULL){
        perror("malloc");
        exit(1);
    }
    sp->line = strdup(s);
    if((sp->words = calloc(num+1, sizeof(char*))) == NULL){
        perror("calloc");
        exit(1);
    }
    sp->n = 0;
    char *token = strtok(sp->line, delim);
    sp->words[0] = strdup(token);
    sp->n = sp->n + 1;
    while(true){
        if((token = strtok(NULL, delim)) == NULL)
            break;
        sp->words[sp->n] = strdup(token);
        sp->n = sp->n + 1;
    }
    return sp;
}

const char *get_split_dup(Split *sp, int idx){
    if(idx >= sp->n){
        printf("Error: get split index '%i' out of bounds.", idx);
        exit(1);
    }
    return (const char*)strdup(sp->words[idx]);
}

void free_split(Split *sp){
    int i;
    free(sp->line);
    for(i=0;i<sp->n;i++){
        free(sp->words[i]);
    }
    free(sp->words);
    free(sp);
}



