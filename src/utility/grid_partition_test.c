#include "./grid_partitioning.h"
#include "raylib.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
Vector2 v2(float a, float b) { return (Vector2){a, b}; }
int main() {
  grid_container gc;
  init_grid_container(&gc, 2, 2, 10.f, 5);
  int data[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  // (0,0),(0,0),(0,0),(1,1),(1,1),(1,1),(1,0),(1,0),(1,0),(0,1)
  Vector2 position[10] = {v2(1, 1),   v2(1, 1),   v2(1, 1),  v2(11, 11),
                          v2(11, 11), v2(11, 11), v2(11, 0), v2(11, 0),
                          v2(11, 0),  v2(0, 11)};
  for (int i = 0; i < 10; i++)
    insert_into_grid(&gc, position[i], &data[i]);
  printf("(0,0): %li\n", grid_cell_size(&gc, 0, 0));
  printf("(1,1): %li\n", grid_cell_size(&gc, 1, 1));
  printf("(1,0): %li\n", grid_cell_size(&gc, 1, 0));
  printf("(0,1): %li\n", grid_cell_size(&gc, 0, 1));
  assert(grid_cell_size(&gc, 0, 0) == 3);
  assert(grid_cell_size(&gc, 1, 1) == 3);
  struct grid_iterator iter;
  get_whole_ass_grid_iterator(&iter, &gc);
  for (int *i = grid_iterator_next(iter); i != NULL;
       i = grid_iterator_next(iter)) {
    printf("%i\n", *i);
  }
  free_grid_iterator(iter);
  remove_from_grid(&gc, position[0], &data[0]);
  assert(grid_cell_size(&gc, 0, 0) == 2);
}