#pragma once
#include <stdio.h>
#include <time.h>

struct timing_variance {
  struct timespec min, max, avg;
  unsigned int samples;
  struct timespec start, end;
};
void start_timing(struct timing_variance *tv);
void end_timing(struct timing_variance *tv);
void init_timing_variance(struct timing_variance *tv);
void print_timing(FILE *f, const struct timing_variance *tv);
void sleep_timing(const struct timing_variance *tv, struct timespec ts);
int timespec_cmp(struct timespec, struct timespec);