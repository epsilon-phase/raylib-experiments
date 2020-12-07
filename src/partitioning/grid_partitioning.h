#pragma once
/**
 * Provide a generic, non-owning grid partitioned container
 *
 **/
#include <raylib.h>
#include <stdbool.h>
#include <stddef.h>
struct grid_cell;
struct grid_iterator_inner;
typedef struct {
  size_t columns, rows;
  size_t max_expected;
  struct grid_cell *cells;
  float span;
} grid_container;
struct grid_iterator {
  // Opaque pointer (spooky noises)
  struct grid_iterator_inner *inner;
};
void *grid_iterator_next(struct grid_iterator);
void free_grid_iterator(struct grid_iterator);
void get_whole_ass_grid_iterator(struct grid_iterator *,
                                 const grid_container *);
void init_grid_container(grid_container *, int columns, int rows, float span,
                         size_t expected_elements);
bool remove_from_grid(grid_container *grid, Vector2 location, void *data);
bool grid_cell_contains(const grid_container *grid, int row, int column,
                        void *data);
size_t grid_cell_size(const grid_container *grid, int column, int row);
void insert_into_grid(grid_container *grid, Vector2 location, void *data);
void free_grid_container(grid_container *grid);