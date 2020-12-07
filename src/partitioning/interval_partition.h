#pragma once
#include <stddef.h>
/***
 * An interval container is a naive way to cluster objects into a
 * a space by location.
 * */
struct interval_container {
  struct interval_cell *cells;
  float span;
  size_t member_size;
  /**
   * Function used to destroy members discarded from the container
   * Can be set to null to indicate that the objects don't need a destructor
   * for whatever reason you like
   **/
  void (*destructor)(void *);
  /**
   * Function used to copy data into the container.
   * Can be set to null to elide such nonsense
   *
   * TODO: Investigate if this should instead take a destination parameter
   * instead of returning a void pointer
   **/
  void *(*copier)(void *);
  /**
   * Comparison function in the vein as strcmp, -1 for smaller, 1 for bigger, 0
   * for equal
   **/
  int (*comparator)(const void *, const void *);
};