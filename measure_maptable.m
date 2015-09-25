
#include <Foundation/Foundation.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mach/mach_time.h>
#include <assert.h>


int freecnt = 0;
int insertcnt;


NSUInteger mt_hash(NSMapTable *mt, const void *data) {
  return (unsigned)data;
}


BOOL mt_compare(NSMapTable *mt, const void *data, const void *key) {
  return (data == key);
}


void mt_free(NSMapTable *mt, void *key) {
  if (key != NULL)
    freecnt++;
}


void mt_retain(NSMapTable *mt, void *key) {}


NSString *mt_describe(NSMapTable *mt, const void *key) {
  return @"";
}


const NSMapTableKeyCallBacks mt_keyCallbacks = {
  &mt_hash,
  &mt_compare,
  &mt_retain,
  &mt_free,
  &mt_describe,
  NULL
};

const NSMapTableValueCallBacks mt_valueCallbacks = {
  &mt_retain,
  &mt_retain,
  &mt_describe
};


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
  NSMapTable *ht;
  int cbuckets, i;
  unsigned rand1, rand2;
  NSMapEnumerator s;

  printf("Test size? ");
  scanf("%d", &insertcnt);
  printf("Bucket count? ");
  scanf("%d", &cbuckets);
  ht = NSCreateMapTable(mt_keyCallbacks, mt_valueCallbacks, cbuckets);
  
  printf("Control... ");
  timeThis();
  timeThis();
  
  printf("Insertion... ");
  srand(insertcnt);
  timeThis();
  for (i=0; i<insertcnt; i++) {
    void *a;
    a = (void*)rand();
    NSMapInsert(ht, a, NULL);
  }
  timeThis();
  
  printf("Retrieval... ");
  srand(insertcnt);
  timeThis();
  for (i=0; i<insertcnt; i++) {
    void *a;
    a = (void*)rand();
    NSMapGet(ht, a);
  }
  timeThis();
  
  printf("Enumeration... ");
  srand(insertcnt);
  timeThis(); 
  s = NSEnumerateMapTable(ht);
  while (NSNextMapEnumeratorPair(&s, (void**)&rand1, NULL)) {}
  timeThis();
  
  printf("Removal... ");
  srand(insertcnt);
  timeThis();
  for (i=0; i<insertcnt; i++) {
    NSMapRemove(ht, (void*)rand());
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
    NSMapInsert(ht, a, NULL);
  }
  timeThis();
  
  freecnt = 0;
  printf("Free... ");
  timeThis();
  objc_release(ht);
  timeThis();
  
  if (insertcnt != freecnt) {
    printf("Something's wrong. %d %d\n", insertcnt, freecnt);
  }
  
  printf("all ok\n");
  return 0;
}

