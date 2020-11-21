#include "./circular_buffer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main() {
  struct CircularBuffer *cb = allocateCircularBuffer(3, &free);
  const char *test_strings[] = {"Hello", "World", "Yes", NULL};
  int i;
  for (i = 0; test_strings[i]; i++) {
    push_value_cb(cb, strdup(test_strings[i]));
  }
  printf("At index %i\n", cb->push_index);
  for (int i = 0; i < cb->capacity; i++)
    if (cb->data[i])
      printf("%s\n", cb->data[i]);
  push_value_cb(cb, "Goodbye");
  for (int i = 0; i < cb->capacity; i++)
    printf("%s\n", cb->data[i]);
}