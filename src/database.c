/*
 ** database.c - functions to read, write, manipulate graph.db
 **
 **
 **/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "database.h"
#include "util.h"

Database *_new_database(){
    Database *db;
    if((db = malloc(sizeof(Database))) == NULL){
        perror("malloc");
        exit(1);
    }
    return db;
}

Database *create_database(const char *name){
    Database *db;
    db = _new_database();
    db->name = strdup(name);

    printf("Creating new database '%s' with 'wb'...\n", db->name);

    if((db->fp = fopen(db->name, "wb")) == NULL){
        perror("open");
        exit(1);
    }
    return db;
}

Database *open_database(const char *file_name){
    Database *db;
    db = _new_database();
    db->name = strdup(file_name);
    printf("Opening database '%s' with 'rb'...\n", db->name);

    if((db->fp = fopen(db->name, "rb")) == NULL){
        perror("open");
        exit(1);
    }

    return db;
}

void close_database(Database *db){
    if(db == NULL){
        puts("Error: handle given is null.");
        exit(1);
    }
    fclose(db->fp);
    free((char *)db->name);
    free(db);
}

size_t read_database(void *data, size_t bytes, Database *db){
    size_t count;
    count = fread(data, 1, bytes, db->fp);
    return count;
}

size_t write_database(const void *data, size_t bytes, Database *db){
    size_t count;
    count = fwrite(data, 1, bytes, db->fp);
    return count;
}

