//
//  hashtable.c
//
//  Created by Daniele Cattaneo on 23/04/15.
//  Copyright (c) 2015 Daniele Cattaneo. All rights reserved.
//

#include "hashtable.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>


typedef struct hashtable_item_s {
  hashtable_hash_t fullhash;
  void *key;
  void *value;
  struct hashtable_item_s *next;
} hashtable_item_t;

typedef struct hashtable_rootItem_s {
  hashtable_item_t item;
  struct hashtable_rootItem_s *enumPrev;
  struct hashtable_rootItem_s *enumNext;
} hashtable_rootItem_t;

struct hashtable_s {
  size_t cbuckets;
  size_t centries;
  hashtable_fcompare compare;
  hashtable_fhash hash;
  hashtable_ffree free;
  hashtable_rootItem_t *enumHead;
  hashtable_rootItem_t buckets[1];  /* buckets are trailing */
};


hashtable_t *hashtable_make(size_t cbuckets, hashtable_fcompare c, hashtable_fhash h, hashtable_ffree f) {
  hashtable_t *ht;
  
  if (!h || !c)
    return NULL;
  if (cbuckets < 1)
    cbuckets = 127;
  
  ht = calloc(1, sizeof(hashtable_t) + sizeof(hashtable_rootItem_t) * (cbuckets - 1));
  ht->cbuckets = cbuckets;
  ht->compare = c;
  ht->hash = h;
  ht->free = f;
  return ht;
}


void hashtable_free(hashtable_t *ht) {
  hashtable_rootItem_t *thisHead;
  hashtable_item_t *this, *next;
  
  thisHead = ht->enumHead;
  while (thisHead) {
    ht->free(thisHead->item.key);
    ht->free(thisHead->item.value);
    this = thisHead->item.next;
    while (this) {
      next = this->next;
      ht->free(this->key);
      ht->free(this->value);
      free(this);
      this = next;
    }
    thisHead = thisHead->enumNext;
  }
  
  free(ht);
}


void hashtable_insert(hashtable_t *ht, void *key, void *value) {
  hashtable_hash_t hash;
  size_t ph;
  hashtable_rootItem_t *rthis;
  hashtable_item_t *this, *next;
  
  if (!key) return;
  
  hash = ht->hash(key);
  ph = hash % ht->cbuckets;
  rthis = &(ht->buckets[ph]);
  this = &(rthis->item);
  if (this->key) {
    next = calloc(1, sizeof(hashtable_item_t));
    next->next = this->next;
    this->next = next;
    this = next;
  } else {
    rthis->enumPrev = NULL;
    rthis->enumNext = ht->enumHead;
    if (ht->enumHead)
      ht->enumHead->enumPrev = rthis;
    ht->enumHead = rthis;
  }
  
  this->fullhash = hash;
  this->key = key;
  this->value = value;
  ht->centries++;
}


void *hashtable_search(hashtable_t *ht, void *key) {
  hashtable_hash_t hash;
  size_t ph;
  hashtable_item_t *this;
  
  hash = ht->hash(key);
  ph = hash % ht->cbuckets;
  this = &(ht->buckets[ph].item);
  if (!(this->key))
    return NULL;
  
  do {
    if (this->fullhash == hash && ht->compare(this->key, key))
      return this->value;
    
  } while ((this = this->next));
  return NULL;
}


int hashtable_remove(hashtable_t *ht, void *key) {
  hashtable_hash_t hash;
  size_t ph;
  hashtable_rootItem_t *rthis;
  hashtable_item_t *prev, *this, *oldthis;
  
  hash = ht->hash(key);
  ph = hash % ht->cbuckets;
  rthis = &(ht->buckets[ph]);
  this = &(rthis->item);
  if (!(this->key))
    return 0;
  
  prev = NULL;
  do {
    if (this->fullhash == hash && ht->compare(this->key, key)) {
      ht->centries--;
      ht->free(this->key);
      ht->free(this->value);
      
      if (prev) {
        prev->next = this->next;
        free(this);
      } else {
        if (this->next) {
          oldthis = this->next;
          *this = *(this->next);
          free(oldthis);
        } else {
          if (!rthis->enumPrev)
            ht->enumHead = rthis->enumNext;
          else
            rthis->enumPrev->enumNext = rthis->enumNext;
          if (rthis->enumNext)
            rthis->enumNext->enumPrev = rthis->enumPrev;
            
          memset(this, 0, sizeof(hashtable_item_t));
        }
      }
      return 1;
    }
    
    prev = this;
    this = this->next;
  } while (this);
  
  return 0;
}


struct hashtable_enum_s {
  hashtable_t *ht;
  hashtable_rootItem_t *thisHead;
  hashtable_item_t *this;
};

hashtable_enum_t *hashtable_enumerate(hashtable_enum_t *s, hashtable_t *ht, void **key, void **value) {
  if (s == NULL) {
    if (!(ht->enumHead))
      return NULL;
      
    s = calloc(1, sizeof(hashtable_enum_t));
    s->ht = ht;
    s->thisHead = ht->enumHead;
    s->this = &(s->thisHead->item);
  } else if (!key) {
    goto finish;
  } else {
    if (ht)
      assert(ht == s->ht);
    else
      ht = s->ht;
  
    s->this = s->this->next;
    if (!(s->this)) {
      s->thisHead = s->thisHead->enumNext;
      if (!(s->thisHead))
        goto finish;
      s->this = &(s->thisHead->item);
    }
  }
  
  if (key)
    *key = s->this->key;
  if (value)
    *value = s->this->value;
  return s;
  
finish:
  if (key)
    *key = NULL;
  if (value)
    *value = NULL;
  free(s);
  return NULL;
}


void hashtable_enumWithCallback(hashtable_t *ht, void (*callback)(void *key, void *value)) {
  hashtable_enum_t *s;
  void *key, *value;
  
  s = hashtable_enumerate(NULL, ht, &key, &value);
  while (s) {
    callback(key, value);
    s = hashtable_enumerate(s, ht, &key, &value);
  }
}




