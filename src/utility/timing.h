#pragma once
#include <stdio.h>
#include <time.h>
/**
 * Records the amount of time spent on average in a loop, for example
 **/
struct timing_variance {
  struct timespec min, max, avg;
  unsigned int samples;
  struct timespec start, end;
};
void start_timing(struct timing_variance *tv);
void end_timing(struct timing_variance *tv);
/**
 * Set up a given timing variance for usage, could be zeroed out normally, but
 *it's easier to just give it an init function
 *
 * @param A pointer to the timing variance structure that you want to
 *(re)-initialize
 **/
void init_timing_variance(struct timing_variance *tv);
/**
 * Print the timing_variance information onto the specified file stream
 * @param f The FILE pointer
 * @param tv the pointer to the timing_variance structure
 **/
void print_timing(FILE *f, const struct timing_variance *tv);
/**
 * Regulate a loop or recurring computation by the time that it takes to
 * complete using the timing variant structure
 * @param tv The timing structure
 * @param ts The amount of time that you want the computation to take
 * */
void sleep_timing(const struct timing_variance *tv, struct timespec ts);
/**
 * Compare two timespec structures
 * @param a The first timespec
 * @param b The second timespec
 * @returns 1 for a is greater than b, -1 for a is less than b, and 0 for equal
 **/
int timespec_cmp(struct timespec, struct timespec);
