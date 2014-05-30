/*
**  set.c - Interface for inserting, removing, finding an element, which
**          hides the data structure used on the backend.
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
#include "set.h"


#define MAX_SET_SIZE 4294967296

Set *create_set(){
  Set *set;
  if((set = malloc(sizeof(Set))) == NULL){
    perror("malloc");
    exit(1);
  }
  set->elements = NULL;
  set->size = 0;
  return set;
}

void free_set(Set *set){
  free(set->elements);
  free(set);
}

void _realloc(Set *set, uint32_t desired){
  if(set == NULL){
    puts("Error: _realloc set is null.");
    exit(1);
  }
  uint32_t from = set->size;
  if(set->size == 0){
    set->size = pow(2,15);
  }else{
    bool found = false;
    while(!found){
      if(((set->size)*(8)) >= (MAX_SET_SIZE-1)){
        puts("trying to allocate size >= 2^32.");
        exit(1);
      }
      set->size *= 8;
      if(set->size >= desired){
        found = true;
      }
    }
  }
#if DEBUG
  printf("Realloc set from %i to %i\n", from, set->size);
#endif
  if((set->elements = realloc(set->elements, sizeof(void *) * set->size)) == NULL){
    perror("realloc");
    exit(1);
  }
  uint32_t i;
  for(i=from; i<set->size; i++){
    set->elements[i] = NULL;
  }
}

uint32_t _get_key(const void *key, Set *set){
  return (uint32_t)(*((uint32_t*)key));
}

int32_t _check_key(const void *key, Set *set){
  uint32_t k = _get_key(key, set);
  if(k >= (MAX_SET_SIZE-1)){
    return -1;
  }
  if(k < 0){
    return -2;
  }
  if(k >= set->size){
    return -3;
  }
  return 0;
}

void _all_checks(const void *key, Set *set){
  int32_t c;
  if((c = _check_key(key, set)) == -1){
    printf("Error: element idx exceeds max bounds of set.\n");
    exit(1);
  }
  if(c == -2){
    printf("Error: insert_set key is < 0.\n");
    exit(1);
  }
  uint32_t desired = _get_key(key, set);
  if(c == -3){
    _realloc(set, desired);
  } 
}

void *insert_set(const void *key, void *value, Set *set){
  if(key == NULL || value == NULL || set == NULL){
    puts("Error: insert_set key or value or set is null.");
    exit(1);
  }
  _all_checks(key, set);
  uint32_t k = _get_key(key, set);
  if(set->elements[k] != NULL){
    return set->elements[k];
  }
  set->elements[k] = value;
  return NULL;
}

void *remove_set(const void *key, Set *set){
  if(key == NULL || set == NULL){
    puts("Error: remove_set key or set is null.");
    exit(1);
  }
  _all_checks(key, set);
  uint32_t k = _get_key(key, set); 
  void *value;
  value = set->elements[k];
  set->elements[k] = NULL;
  return value;
}

void *find_set(const void *key, Set *set){
  if(key == NULL || set == NULL){
    puts("Error: remove_set key or set is null.");
    exit(1);
  }
  _all_checks(key, set);
  uint32_t k = _get_key(key, set);
  return set->elements[k];
}

uint32_t size_set(Set *set){
  if(set == NULL){
    puts("Error: size_set set is null.");
    exit(1);
  }
  return set->size;
}









