#include "heap.h"

char HEAP[CAP] = {0};

int allocated_bytes = 0;

address_and_size_t ALLOCATED_BLOCKS[META_CAP] = {0};
int allocated_size = 0;

list_of_address_sizes_t allocated_blocks = { 0 } ;
list_of_address_sizes_t free_blocks = { 
  .count = 1, 
  .all_addresses_and_sizes = {
    [0] = { 
      .begin_address = HEAP, 
      .size = sizeof(HEAP)
    },
  }
  };


bool check_size_less_than_equal(int a, int b) { return a <= b; }
bool check_pointer_within_range(char *left, char *right, char *ptr) {
  return left <= ptr && ptr <= right;
}
void *extend_address(char *left, int length) { return left + length; }
void add_bytes(int *left, int advanced) { *left += advanced; }
void add_to_allocated_array(address_and_size_t curr_meta_data) {
  ALLOCATED_BLOCKS[allocated_size++] = curr_meta_data;
}

int search_address(const list_of_address_sizes_t *list, void *ptr) {

  for (int i = 0; i < list->count; i++) {
    if (ptr == list->all_addresses_and_sizes[i].begin_address) {
      return i;
    }
  }

  return -1;
}

int search_idx_to_insert(const list_of_address_sizes_t *list, int size) {
  for (int i = 0; i < list->count; i++) {
    if (size <= list->all_addresses_and_sizes[i].size) {
      return i;
    }
  }

  return -1;
}

void add_address(int size, void *ptr, list_of_address_sizes_t *list) {
  check_size_less_than_equal(list->count, META_CAP);

  // add to the end of the list
  list->all_addresses_and_sizes[list->count].begin_address = ptr;
  list->all_addresses_and_sizes[list->count].size = size;

  // run on the list
  // maintain sorted by address nature in my meta data blocks

  for (int i = list->count;
       i > 0 && list->all_addresses_and_sizes[i].begin_address <
                    list->all_addresses_and_sizes[i - 1].begin_address;
       --i) {

    const address_and_size_t block_temp = list->all_addresses_and_sizes[i];
    list->all_addresses_and_sizes[i] = list->all_addresses_and_sizes[i - 1];
    list->all_addresses_and_sizes[i - 1] = block_temp;
  }
  // we have one more block now
  ++list->count;
}

void remove_an_address_via_index(int idx, list_of_address_sizes_t *list) {

  check_size_less_than_equal(list->count, idx);

  // removal from sorted list
  for (int i = idx; i < list->count - 1; i++) {
    // shift left  each block
    list->all_addresses_and_sizes[i] = list->all_addresses_and_sizes[i + 1];
  }

  list->count--;
}
void *heap_alloc(int size) {
  if (check_size_less_than_equal(CAP, size)) {
    // cannot proceed
    return NULL;
  }

  int idx = search_idx_to_insert(&free_blocks, size);
  if (idx == -1) {
    return NULL;
  }

  address_and_size_t res = free_blocks.all_addresses_and_sizes[idx];

  remove_an_address_via_index(idx, &free_blocks);

  add_address(size, res.begin_address, &allocated_blocks);
  int tail_size = res.size - size; 
  if (tail_size > 0) {
    add_address(tail_size, extend_address(res.begin_address, size),
               &free_blocks);
  }

  return res.begin_address;

  // // remove_an_address_via_index(idx, &free_blocks);
  // // void *ptr_l = res.begin_address;

  // void *ptr = extend_address(HEAP, allocated_bytes);
  // add_bytes(&allocated_bytes, size);
  // // make a meta struct
  // const address_and_size_t m = {.size = size, .begin_address = ptr};
  // // add to the meta array
  // add_to_allocated_array(m);

  // add_address(size, ptr, &allocated_blocks);
  // return ptr;
}

void block_list_dump(list_of_address_sizes_t *list) {
  printf("block list dump\n");
  printf("\n Blocks (%d):\n", list->count);
  for (int i = 0; i < list->count; i++) {
    printf("meta_idx[%d].size = %d\n", i,
           list->all_addresses_and_sizes[i].size);
    printf("meta_idx[%d].begin_address = %p\n", i,
           list->all_addresses_and_sizes[i].begin_address);
  }
}

void heap_free(void *address) {

  if (address == NULL) {
    return;
  }
  if (!check_pointer_within_range(HEAP, HEAP + CAP, address)) {
    return;
  }

  // find the meta data
  address_and_size_t m = {.size = 0, .begin_address = address};
  int idx = search_address(&allocated_blocks, address);
  if (idx == -1) {
    // cannot proceed
    return;
  }

  // remove from the meta array
  ALLOCATED_BLOCKS[idx] = ALLOCATED_BLOCKS[--allocated_size];

  // remove from the list
  remove_an_address_via_index(idx, &allocated_blocks);

  // add to the free list
  add_address(m.size, m.begin_address, &free_blocks);
}

int main(int argc, const char *argv[]) {

  for (int i = 1; i <= 10; i++) {
    char *buffer = heap_alloc(i);
    if (i % 2 == 0) {
      heap_free(buffer);
    }
  }

  // heap_dump();

  block_list_dump(&allocated_blocks);
  block_list_dump(&free_blocks);

  char *buffer = heap_alloc(10);
  heap_free(buffer);

  return 0;
}
