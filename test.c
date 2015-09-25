
#include "hashtable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


hashtable_hash_t mt_hash(void *data) {
  char *x;
  int l;
  
  x = (char*)data;
  l = strlen(x);
  return x[0] + x[1]*0x100 + x[2]*0x10000 + x[3]*0x1000000 + l * 0x100000000;
}


int mt_compare(void *data, void *key) {
  char *x, *y;
  
  x = (char*)data;
  y = (char*)key;
  return strcmp(x, y) == 0;
}


int main(int argc, char *argv[]) {
  hashtable_t *ht;
  char key[80], value[80];
  char *res, *res2;
  
  ht = hashtable_make(0, mt_compare, mt_hash, free);
  for (;;) {
    printf("Mode? 1:query/insert 2:remove 3:enum");
    gets(key);
    if (key[0] == '1') {
    
      for (;;) {
        printf("Key? ");
        gets(key);
        if (key[0] == '\0') break;
        res = (char*)hashtable_search(ht, key);
        if (!res) {
          printf("Not found. Data? ");
          gets(value);
          hashtable_insert(ht, strdup(key), strdup(value));
        } else {
          printf("Value = %s\n", res);
        }
      }
      
    } else if (key[0] == '2') {
    
      for (;;) {
        printf("Key? ");
        gets(key);
        if (key[0] == '\0') break;
        res = (char*)hashtable_search(ht, key);
        if (!res) {
          printf("Not found.\n");
        } else {
          printf("Value = %s\n", res);
        }
        if (hashtable_remove(ht, key)) {
          printf("Removal OK\n");
        } else {
          printf("Removal FAILED\n");
        }
      }
    
    } else if (key[0] == '3') {
    
      hashtable_enum_t *s;
      s = hashtable_enumerate(NULL, ht, (void**)(&res), (void**)(&res2));
      while (s) {
        printf("Key: %s    Value: %s\n", res, res2);
        s = hashtable_enumerate(s, ht, (void**)(&res), (void**)(&res2));
      }
    
    }
  }
}
