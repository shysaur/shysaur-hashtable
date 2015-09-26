
#include "hashtable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


hashtable_hash_t mt_hash(void *data) {
  char *x;
  int l, i;
  hashtable_hash_t res;
  
  x = (char*)data;
  l = strlen(x);
  
  if (l >= 4) {
    return x[0] + x[1]*0x100 + x[l-1]*0x10000 + x[l-2]*0x1000000 + l * 0x100000000;
  } else {
    res = l;
    while (l) {
      res = res * 0x100 + x[l-1];
      l--;
    }
    return res+1;
  }
}


int mt_compare(void *data, void *key) {
  char *x, *y;
  
  x = (char*)data;
  y = (char*)key;
  return strcmp(x, y) == 0;
}


int main(int argc, char *argv[]) {
  autoHashtable_t *ht;
  char key[80], value[80];
  char *res, *res2;
  int tmp;
  
  printf("Sizing suggestion? ");
  gets(value);
  sscanf(value, "%d", &tmp);
  ht = autoHashtable_make(tmp, mt_compare, mt_hash, free, free);
  for (;;) {
    printf("Mode? 1:query/insert 2:remove 3:enum ");
    gets(key);
    if (key[0] == '1') {
    
      for (;;) {
        printf("Key? ");
        gets(key);
        if (key[0] == '\0') break;
        res = (char*)autoHashtable_search(ht, key);
        if (!res) {
          printf("Not found. Data? ");
          gets(value);
          autoHashtable_insert(ht, strdup(key), strdup(value));
        } else {
          printf("Value = %s\n", res);
        }
      }
      
    } else if (key[0] == '2') {
    
      for (;;) {
        printf("Key? ");
        gets(key);
        if (key[0] == '\0') break;
        res = (char*)autoHashtable_search(ht, key);
        if (!res) {
          printf("Not found.\n");
        } else {
          printf("Value = %s\n", res);
        }
        if (autoHashtable_remove(ht, key)) {
          printf("Removal OK\n");
        } else {
          printf("Removal FAILED\n");
        }
      }
    
    } else if (key[0] == '3') {
    
      hashtable_enum_t *s;
      s = autoHashtable_enumerate(NULL, ht, (void**)(&res), (void**)(&res2));
      while (s) {
        printf("Key: %s    Value: %s\n", res, res2);
        s = autoHashtable_enumerate(s, ht, (void**)(&res), (void**)(&res2));
      }
    
    }
  }
}

