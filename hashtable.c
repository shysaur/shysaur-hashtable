//
//  hashtable.c
//  m68ksim
//
//  Created by Daniele Cattaneo on 23/04/15.
//  Copyright (c) 2015 Daniele Cattaneo. All rights reserved.
//

#include "hashtable.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>


hashtable_t *hashtable_make(size_t cbuckets, fcompare f, fhash h) {
  hashtable_t *ht;
  
  if (!h) return NULL;
  
  ht = calloc(1, sizeof(hashtable_t));
  ht->cbuckets = cbuckets;
  ht->f = f;
  ht->h = h;
  ht->buckets = calloc(cbuckets, sizeof(hashtable_item_t));
  return ht;
}


void hashtable_free(hashtable_t *ht) {
  size_t i;
  hashtable_item_t *this, *next;
  
  for (i=0; i<ht->cbuckets; i++) {
    next = ht->buckets[i].next;
    while (next) {
      this = next;
      next = this->next;
      free(this);
    }
  }
  free(ht);
}


void hashtable_insert(hashtable_t *ht, void *item) {
  uint32_t hash;
  size_t ph;
  hashtable_item_t *next, *this;
  
  if (!item) return;
  
  hash = ht->h(item);
  ph = hash % ht->cbuckets;
  this = &(ht->buckets[ph]);
  if (this->item) {
    next = calloc(1, sizeof(hashtable_item_t));
    *next = *this;
  } else
    next = NULL;
  
  this->fullhash = hash;
  this->item = item;
  this->next = next;
}


int hashtable_search(hashtable_t *ht, void *item) {
  uint32_t hash;
  size_t ph;
  hashtable_item_t *this;
  
  hash = ht->h(item);
  ph = hash % ht->cbuckets;
  this = &(ht->buckets[ph]);
  if (!(this->item))
    return 0;
  
  do {
    if (this->item == item || (ht->f && ht->f(this->item, item)))
      return 1;
    
  } while ((this = this->next));
  return 0;
}


void hashtable_remove(hashtable_t *ht, void *item) {
  uint32_t hash;
  size_t ph;
  hashtable_item_t *prev, *this;
  
  hash = ht->h(item);
  ph = hash % ht->cbuckets;
  this = &(ht->buckets[ph]);
  if (!(this->item))
    return;
  
  prev = NULL;
  do {
    if (this->item == item || (ht->f && ht->f(this->item, item))) {
      if (prev) {
        prev->next = this->next;
        free(this);
      } else {
        if (this->next)
          *this = *(this->next);
        else
          memset(this, 0, sizeof(hashtable_item_t));
      }
      return;
    }
    
    prev = this;
    this = this->next;
  } while (this);
}


void hashtable_enumerate(hashtable_t *ht, void (*callback)(void *item)) {
  size_t i;
  hashtable_item_t *this;
  
  for (i=0; i<ht->cbuckets; i++) {
    if (ht->buckets[i].item) {
      this = &(ht->buckets[i]);
      while (this) {
        callback(this->item);
        this = this->next;
      }
    }
  }
}


