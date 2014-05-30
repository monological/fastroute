/*
 * set.h - Interface for inserting, removing, finding an element, which 
 *         hides the data structure used on the backend.
 *
 */

#ifndef SET_H
#define SET_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
  void **elements;
  uint32_t size;
} Set;


Set *create_set();
void free_set(Set *set);
void *insert_set(const void *key, void *value, Set *set);
void *remove_set(const void *key, Set *set);
void *find_set(const void *key, Set *set);
uint32_t size_set(Set *set);
#endif
