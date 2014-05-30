
/*
 * util.h - utility functions.
 *
 */


#ifndef UTIL_H
#define UTIL_H

typedef struct {
    char *line;
    char **words;
    int n;
} Split;

Split *create_split(const char *s, const char *delim);
const char *get_split_dup(Split *sp, int idx);
void free_split(Split *sp);



#endif
