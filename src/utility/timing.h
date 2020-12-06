#pragma once
#include <time.h>

struct timing_variance {
  struct timespec min, max, avg;
  unsigned int samples;
  struct timespec start, end;
};
void start_timing(struct timing_variance *tv);
void end_timing(struct timing_variance *tv);
void init_timing_variance(struct timing_variance *tv);