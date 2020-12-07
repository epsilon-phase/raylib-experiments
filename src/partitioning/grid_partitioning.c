#include "grid_partitioning.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
struct grid_cell {
  void **data;
  size_t capacity, size;
};
struct grid_iterator_inner {
  // Null terminated list
  struct grid_cell **cells;
  size_t current_data_idx;
  size_t current_cell;
};
static void *grid_inner_next(struct grid_iterator_inner *iter) {
  void *result = NULL;
  do {
    if (!iter->cells[iter->current_cell])
      break;
    struct grid_cell *cell = iter->cells[iter->current_cell];

    if (cell->size <= iter->current_data_idx) {
      iter->current_data_idx = 0;
      iter->current_cell++;
      continue;
    }
    result = cell->data[iter->current_data_idx];
    iter->current_data_idx++;
    break;
  } while (true);
  return result;
}
void *grid_iterator_next(struct grid_iterator gi) {
  return grid_inner_next(gi.inner);
}
void free_grid_iterator(struct grid_iterator gi) {
  free(gi.inner->cells);
  free(gi.inner);
}
void get_whole_ass_grid_iterator(struct grid_iterator *iter,
                                 const grid_container *gc) {
  iter->inner = calloc(1, sizeof(struct grid_iterator));
  iter->inner->cells =
      calloc(gc->columns * gc->rows + 1, sizeof(struct grid_cell **));
  for (int i = 0; i < gc->columns * gc->rows; i++)
    iter->inner->cells[i] = &gc->cells[i];
}
void init_grid_container(grid_container *grid, int columns, int rows,
                         float span, size_t expected_elements) {
  grid->cells = malloc(sizeof(struct grid_cell) * columns * rows);
  grid->span = span;
  grid->rows = rows;
  grid->columns = columns;
  for (unsigned int i = 0; i < columns * rows; i++) {
    grid->cells[i].capacity = expected_elements;
    grid->cells[i].data = calloc(sizeof(void *), expected_elements);
    grid->cells[i].size = 0;
  }
}
size_t get_cell_index(const grid_container *grid, Vector2 v) {
  int column = v.x / grid->span, row = v.y / grid->span;
  int index = grid->columns * column + row;
  return index;
}
bool remove_from_grid(grid_container *grid, Vector2 v, void *data) {
  int index = get_cell_index(grid, v);
  int found = 0;
  struct grid_cell *cell = &grid->cells[index];
  void **i;
  for (int x = 0; x < cell->size; x++) {
    if (cell->data[x] == data) {
      i = &cell->data[x];
      found = 1;
      break;
    }
  }
  if (!found)
    return found;
  while (i < cell->data + cell->size) {
    *i = *(i + 1);
    i++;
  }
  cell->size--;
  return found;
}
bool grid_cell_contains(const grid_container *grid, int column, int row,
                        void *data) {
  size_t cell_index = grid->columns * column + row;
  struct grid_cell *cell = &grid->cells[cell_index];
  for (int i = 0; i < cell->size; i++) {
    if (cell->data[i] == data)
      return true;
  }
  return false;
}
size_t grid_cell_size(const grid_container *grid, int column, int row) {
  return grid->cells[grid->columns * column + row].size;
}
void insert_into_grid(grid_container *grid, Vector2 v, void *data) {
  if (!data)
    return;
  int index = get_cell_index(grid, v);
  struct grid_cell *cell = &grid->cells[index];
  cell->data[cell->size] = data;
  cell->size++;
}

void free_grid_container(grid_container *grid) {
  for (int i = 0; i < grid->columns * grid->rows; i++) {
    free(grid->cells[i].data);
  }
  free(grid->cells);
}