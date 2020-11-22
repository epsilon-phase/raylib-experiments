#include "./circular_buffer.h"
#include <stdlib.h>
static unsigned int next_index(struct CircularBuffer *cb) {
  return (cb->push_index + 1) % cb->capacity;
}
static unsigned int previous_index(struct CircularBuffer *cb) {
  return (cb->push_index - 1) % cb->capacity;
}
struct CircularBuffer *allocateCircularBuffer(unsigned int capacity,
                                              void (*destructor)(void *),
                                              void *(*copier)(void *)) {
  struct CircularBuffer *result = calloc(1, sizeof(struct CircularBuffer));
  result->capacity = capacity;
  result->destructor = destructor;
  result->copier = copier;
  result->data = calloc(sizeof(void *), capacity);
  return result;
};
void push_value_cb(struct CircularBuffer *cb, void *data) {
  unsigned int new_index = (cb->push_index + 1) % cb->capacity;
  if (cb->destructor && cb->data[cb->push_index])
    cb->destructor(cb->data[cb->push_index]);
  cb->data[cb->push_index] = cb->copier ? cb->copier(data) : data;
  cb->push_index = new_index;
}
void pop_value_cb(struct CircularBuffer *cb) {
  unsigned int new_index = (cb->push_index - 1) % cb->capacity;
  if (cb->data[new_index] && cb->destructor) {
    cb->destructor(cb->data[new_index]);
  }
  cb->data[new_index] = NULL;
  cb->push_index = new_index;
}
void *last_value_cb(struct CircularBuffer *cb) {
  return cb->data[previous_index(cb)];
}
void cb_free(struct CircularBuffer *cb) {
  unsigned int i = 0;
  if (!cb->destructor) {
    free(cb->data);
    free(cb);

    return;
  }
  for (i = 0; i < cb->capacity; i++)
    if (cb->data[i])
      cb->destructor(cb->data[i]);
  free(cb->data);
  free(cb);
}