#include "grid_partitioning.h"
#include <math.h>
static size_t location_to_index(const grid_container* grid, grid_location loc){
  return grid->rows * loc.row+loc.column;
}
grid_location get_in_grid(const grid_container *grid,
                          Vector2 pos){
  grid_location result;
  result.column=floor(pos.x/grid->span);
  result.row=floor(pos.y/grid->span);
  return result;
}
void remove_from_grid(grid_container *grid,
                      Vector2 location,
                      void *data) {
  grid_location loc = get_in_grid(grid,location);
  size_t index = location_to_index(grid,loc);
  signed long found_index = -1;
  for(size_t i=0;i<grid->cells[index].size;i++){
    if(grid->cells[index].contents[i]==data){
      found_index=i;
      break;
    }
  }
  if(found_index==-1)
    return;
  for(;found_index<grid->cells[index].size-1;found_index++){
    grid->cells[index].contents[found_index]=grid->cells[index].contents[found_index+1];
  }
  grid->cells[index].contents[grid->cells[index].size-1]=NULL;
}
