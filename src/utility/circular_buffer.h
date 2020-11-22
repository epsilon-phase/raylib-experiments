#pragma once
struct CircularBuffer {
  unsigned int capacity;
  unsigned int used;
  unsigned int push_index;
  void **data;
  // This is what they call "Generic programming" Right?
  void (*destructor)(void *);
  void *(*copier)(void *);
};
/**
 * @param capacity The number of elements allocated to the buffer
 * @param destructor The function used to clean up data as it is
 * replaced/removed NULL makes it do nothing.
 * @param copier The function used to copy the data.
 * If set to null it is assumed that the data is managed by the buffer.
 * */
struct CircularBuffer *allocateCircularBuffer(unsigned int capacity,
                                              void (*destructor)(void *),
                                              void *(*copier)(void *));
void push_value_cb(struct CircularBuffer *cb, void *data);
void pop_value_cb(struct CircularBuffer *cb);
void *last_value_cb(struct CircularBuffer *sb);
void cb_free(struct CircularBuffer *cb);