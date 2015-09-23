//
//  hashtable.h
//
//  Created by Daniele Cattaneo on 23/04/15.
//  Copyright (c) 2015 Daniele Cattaneo. All rights reserved.
//

#ifndef __hashtable_h__
#define __hashtable_h__

#include <stdint.h>


typedef int (*fcompare)(void *data1, void *data2);
typedef uint32_t (*fhash)(void *data);

typedef struct hashtable_item_s {
  uint32_t fullhash;
  void *item;
  struct hashtable_item_s *next;
} hashtable_item_t;

typedef struct {
  size_t cbuckets;
  fcompare f;
  fhash h;
  hashtable_item_t *buckets;
} hashtable_t;


hashtable_t *hashtable_make(size_t cbuckets, fcompare f, fhash h);
void hashtable_free (hashtable_t *hashtable);

void hashtable_insert(hashtable_t *ht, void *item);
int hashtable_search(hashtable_t *ht, void *item);
void hashtable_remove(hashtable_t *ht, void *item);
void hashtable_enumerate(hashtable_t *ht, void (*callback)(void *item));


#endif
