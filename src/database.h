
/*
 * database.h - structures and functions for arc/node manipulation.
 *
 */

#ifndef DATABASE_H
#define DATABASE_H

#include <stdio.h>

typedef struct {
    FILE *fp;
    const char *name;
} Database;

/*
 * Function definitions
 */

Database *create_database(const char *name);
Database *open_database(const char *file_name);
void close_database(Database *db);

size_t read_database(void *data, size_t bytes, Database *db);
size_t write_database(const void *data, size_t bytes, Database *db);

#endif
