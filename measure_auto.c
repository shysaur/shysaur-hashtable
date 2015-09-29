
#include "hashtable.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#ifdef __APPLE__
#include <mach/mach_time.h>
#elif _WIN32
#include <windows.h>
#endif


int freecnt = 0;
int insertcnt;


hashtable_hash_t mt_hash(void *data) {
  return (hashtable_hash_t)((uintptr_t)data);
}


int mt_compare(void *data, void *key) {
  return (data == key);
}


void mt_free(void *key) {
  freecnt++;
}


void timeThis(void) {
#ifdef __APPLE__
  static uint64_t stime=0, etime;
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
#elif _WIN32
  static LONGLONG stime=0, etime;
  LONGLONG freq;
  uint64_t tmp1, tmp2, diff, titm;
  
  if (stime) {
    QueryPerformanceCounter((LARGE_INTEGER*)&etime);
    QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
    diff = etime - stime;
    tmp1 = (diff * 1000000) / freq;
    tmp2 = ((diff * 1000000) % freq) * 1000 / freq;
    diff = tmp1 * 1000 + tmp2;
    titm = (diff * 100) / insertcnt;
    printf("Time spent: %lld (per item %lld.%lld) ns\n", diff, titm / 100, titm % 100);
    stime = etime = 0;
  } else {
    QueryPerformanceCounter((LARGE_INTEGER*)&stime);
  }
#endif
}


int main(int argc, char *argv[]) {
  autoHashtable_t *ht;
  int cbuckets, i;
  unsigned long long rand1, rand2;
  hashtable_enum_t *s;

  printf("Test size? ");
  scanf("%d", &insertcnt);
  printf("Bucket count? ");
  scanf("%d", &cbuckets);
  ht = autoHashtable_make(cbuckets, mt_compare, mt_hash, mt_free, NULL);
  
  printf("Control... ");
  timeThis();
  timeThis();
  
  
  printf("Insertion... ");
  srand(insertcnt);
  timeThis();
  for (i=0; i<insertcnt; i++) {
    autoHashtable_insert(ht, (void*)((intptr_t)rand()+1), (void*)1);
  }
  timeThis();
  
  
  printf("Retrieval... ");
  srand(insertcnt);
  timeThis();
  for (i=0; i<insertcnt; i++) {
    autoHashtable_search(ht, (void*)((intptr_t)rand()+1));
  }
  timeThis();
  
  
  printf("Enumeration... ");
  srand(insertcnt);
  i=0;
  timeThis();
  s = autoHashtable_enumerate(NULL, ht, (void**)&rand1, NULL);
  while (s) {
    s = autoHashtable_enumerate(s, ht, (void**)&rand1, NULL);
  }
  timeThis();
  
  
  printf("Removal... ");
  srand(insertcnt);
  timeThis();
  for (i=0; i<insertcnt; i++) {
    autoHashtable_remove(ht, (void*)((intptr_t)rand()+1));
  }
  timeThis();
  
  if (insertcnt != freecnt) {
    printf("Something's wrong. %d %d\n", insertcnt, freecnt);
  }


  printf("Insertion 2... ");
  srand(insertcnt);
  timeThis();
  for (i=0; i<insertcnt; i++) {
    autoHashtable_insert(ht, (void*)((intptr_t)rand()+1), (void*)1);
  }
  timeThis();
  
  
  freecnt = 0;
  printf("Free... ");
  timeThis();
  autoHashtable_free(ht);
  timeThis();
  
  if (insertcnt != freecnt) {
    printf("Something's wrong. %d %d\n", insertcnt, freecnt);
  }
  
  
  printf("all ok\n");
  return 0;
}

