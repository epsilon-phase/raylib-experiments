#include "./circular_buffer.h"
#include <stdlib.h>
struct CircularBuffer *allocateCircularBuffer(unsigned int capacity,
                                              void (*destructor)(void *)) {
  struct CircularBuffer *result = calloc(1, sizeof(struct CircularBuffer));
  result->capacity = capacity;
  result->destructor = destructor;
  result->data = calloc(sizeof(void *), capacity);
  return result;
};
void push_value_cb(struct CircularBuffer *cb, void *data) {
  unsigned int new_index = (cb->push_index + 1) % cb->capacity;
  if (cb->destructor && cb->data[cb->push_index])
    cb->destructor(cb->data[cb->push_index]);
  cb->data[cb->push_index] = data;
  cb->push_index = new_index;
}
void pop_value_cb(struct CircularBuffer *cb) {
  if (cb->data[cb->push_index] && cb->destructor) {
    cb->destructor(cb->data[cb->push_index]);
  }
  cb->data[cb->push_index] = NULL;
  cb->push_index = (cb->push_index - 1) % cb->capacity;
}