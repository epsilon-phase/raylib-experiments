#pragma once
struct CircularBuffer {
  unsigned int capacity;
  unsigned int used;
  unsigned int push_index;
  void **data;
  // This is what they call "Generic programming" Right?
  void (*destructor)(void *);
};
struct CircularBuffer *allocateCircularBuffer(unsigned int capacity,
                                              void (*destructor)(void *));
void push_value_cb(struct CircularBuffer *cb, void *data);
void pop_value_cb(struct CircularBuffer *cb);