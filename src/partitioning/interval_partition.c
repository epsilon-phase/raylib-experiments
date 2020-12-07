#include "./interval_partition.h"
#include <stdlib.h>
#include <string.h>
struct interval_cell {
  void *data;
  size_t size, capacity;
};

static void push_cell_back(struct interval_cell *ic, void *data,
                           const struct interval_container *c) {

  if (ic->capacity == ic->size) {
    ic->data = reallocarray(ic->data, ic->size * 2, c->member_size);
    ic->capacity *= 2;
  }
  void *offset = ic->data + (ic->size * c->member_size);
  memcpy(offset, data, c->member_size);
}
static void *find_in_cell(const struct interval_cell *ic,
                          const struct interval_container *c, void *data) {
  void *found = NULL;
  for (size_t i = 0; i < ic->size; i++) {
    void *offset = ic->data + i * c->member_size;
    if (c->comparator(offset, data) == 0) {
      found = offset;
      break;
    }
  }
  return found;
}
static int last_in_cell(void *data, const struct interval_cell *ic,
                        const struct interval_container *c) {
  return ((data - ic->data) / c->member_size) == (ic->size - 1);
}
static void *get_last_cell(const struct interval_cell *ic,
                           const struct interval_container *c) {
  return (c->member_size * (ic->size - 1)) + ic->data;
}
static void remove_from_cell(struct interval_cell *ic, void *data,
                             const struct interval_container *c) {
  void *item = find_in_cell(ic, c, data);
  if (!item)
    return;
  if (c->destructor)
    c->destructor(item);
  // Don't need to move it forwards if it's the last one in the cell
  if (!last_in_cell(data, ic, item)) {
    memcpy(item, get_last_cell(ic, c), c->member_size);
  }
  ic->size--;
}