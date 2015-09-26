
#include "hashtable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mach/mach_time.h>
#include <assert.h>


int freecnt = 0;
int insertcnt;


hashtable_hash_t mt_hash(void *data) {
  return (hashtable_hash_t)data;
}


int mt_compare(void *data, void *key) {
  return (data == key);
}


void mt_free(void *key) {
  freecnt++;
}


void timeThis(void) {
  static uint64_t stime, etime;
  mach_timebase_info_data_t info;
  uint64_t diff, titm;
  
  if (stime) {
    etime = mach_absolute_time();
    mach_timebase_info(&info);
    diff = (etime - stime) * info.numer / info.denom;
    titm = (diff * 100) / insertcnt;
    printf("Time spent: %lld (per item %lld.%lld) ns\n", diff, titm / 100, titm % 100);
    stime = etime = 0;
  } else {
    stime = mach_absolute_time();
  }
}


int main(int argc, char *argv[]) {
  hashtable_t *ht;
  int cbuckets, i;
  unsigned rand1, rand2;
  hashtable_enum_t *s;

  printf("Test size? ");
  scanf("%d", &insertcnt);
  printf("Bucket count? ");
  scanf("%d", &cbuckets);
  ht = hashtable_make(cbuckets, mt_compare, mt_hash, mt_free, NULL);
  
  printf("Control... ");
  timeThis();
  timeThis();
  
  printf("Insertion... ");
  srand(insertcnt);
  timeThis();
  for (i=0; i<insertcnt; i++) {
    void *a;
    a = (void*)rand();
    hashtable_insert(ht, a, (void*)1);
  }
  timeThis();
  
  printf("Retrieval... ");
  srand(insertcnt);
  timeThis();
  for (i=0; i<insertcnt; i++) {
    void *a;
    a = (void*)rand();
    hashtable_search(ht, a);
  }
  timeThis();
  
  printf("Enumeration... ");
  srand(insertcnt);
  timeThis();
  s = hashtable_enumerate(NULL, ht, (void**)&rand1, NULL);
  while (s) {
    s = hashtable_enumerate(s, ht, (void**)&rand1, NULL);
  }
  timeThis();
  
  printf("Removal... ");
  srand(insertcnt);
  timeThis();
  for (i=0; i<insertcnt; i++) {
    hashtable_remove(ht, (void*)rand());
  }
  timeThis();
  
  if (insertcnt != freecnt) {
    printf("Something's wrong. %d %d\n", insertcnt, freecnt);
  }

  printf("Insertion 2... ");
  srand(insertcnt);
  timeThis();
  for (i=0; i<insertcnt; i++) {
    void *a;
    a = (void*)rand();
    hashtable_insert(ht, a, (void*)1);
  }
  timeThis();
  
  freecnt = 0;
  printf("Free... ");
  timeThis();
  hashtable_free(ht);
  timeThis();
  
  if (insertcnt != freecnt) {
    printf("Something's wrong. %d %d\n", insertcnt, freecnt);
  }
  
  printf("all ok\n");
  return 0;
}

