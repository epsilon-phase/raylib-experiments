#include "grid_partitioning.h"
#include <math.h>
#include <stdlib.h>
struct grid_cell {
  void **data;
  size_t capacity, size;
};
struct grid_iterator_inner {};
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