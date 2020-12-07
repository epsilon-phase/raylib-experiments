#include "./timing.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
int main() {
  struct timing_variance tv;
  init_timing_variance(&tv);

  start_timing(&tv);
  sleep(3);
  end_timing(&tv);
  assert(tv.samples == 1);
  assert(timespec_cmp(tv.min, tv.max) == 0);
  print_timing(stdout, &tv);
  start_timing(&tv);
  sleep(1);
  end_timing(&tv);
  assert(tv.samples == 2);
  assert(timespec_cmp(tv.min, tv.max) != 0);
  print_timing(stdout, &tv);
  start_timing(&tv);
  usleep(10000);
  end_timing(&tv);
  print_timing(stdout, &tv);
}