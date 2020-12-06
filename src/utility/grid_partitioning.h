#pragma once
#include <stddef.h>
#include <raylib.h>
typedef struct{
  int column,row;
} grid_location;

typedef struct{
  size_t columns,rows;
  float span;
  int max_expected;
  struct {
    void** contents;
    size_t capacity, size;
  } *cells;
}grid_container;
grid_location get_in_grid(const grid_container*,Vector2);
void remove_from_grid(grid_container* grid, Vector2 location, void* data);
