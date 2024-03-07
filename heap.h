#ifndef F93A7C14_9069_4121_BC11_011CEFC27AF9
#define F93A7C14_9069_4121_BC11_011CEFC27AF9

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define CAP 640000
#define META_CAP 1024

typedef struct metadata {
  int size;            // how many bytes
  void *begin_address; // start address
} address_and_size_t;

typedef struct list {
  int count;                    // number of items
  address_and_size_t all_addresses_and_sizes[META_CAP]; // blocks blocks blocks
} list_of_address_sizes_t;





#endif /* F93A7C14_9069_4121_BC11_011CEFC27AF9 */
