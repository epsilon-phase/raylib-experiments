#include "./circular_buffer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int contents_correct(const struct CircularBuffer *cb, const char **expected,
                     size_t size);
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
  if (!contents_correct(cb, test_strings, 3))
    printf("Incorrect result of non-reflowing pushing");
  push_value_cb(cb, "Goodbye");
  for (int i = 0; i < cb->capacity; i++)
    printf("%s\n", cb->data[i]);
  if (!contents_correct(cb, (const char *[3]){"Goodbye", "World", "Yes"}, 3)) {
    printf("Overflowed pushing results in correct outcomes\n");
  }
}
int contents_correct(const struct CircularBuffer *cb, const char **expected,
                     size_t size) {
  size_t idx = 0;
  int correct = 1;
  for (idx = 0; idx < cb->capacity && idx < size; idx++) {
    if (expected[idx] == NULL || cb->data[idx] == NULL) {
      correct &= expected[idx] == NULL && cb->data[idx] == NULL;
      continue;
    }
    correct &= strcmp(expected[idx], cb->data[idx]) == 0;
  }
  return correct;
}