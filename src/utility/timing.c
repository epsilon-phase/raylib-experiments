#include "./timing.h"
static struct timespec min(struct timespec a, struct timespec b) {
  if (a.tv_sec > b.tv_sec)
    return b;
  if (a.tv_sec < b.tv_sec)
    return a;
  if (a.tv_nsec > b.tv_nsec)
    return b;
  return a;
}
static struct timespec max(struct timespec a, struct timespec b) {
  if (a.tv_sec > b.tv_sec)
    return a;
  if (a.tv_sec < b.tv_sec)
    return b;
  if (a.tv_nsec > b.tv_nsec)
    return a;
  return b;
}
static struct timespec avg(struct timespec a, struct timespec b, int count) {
  long long tv_nsec = a.tv_nsec * count + b.tv_nsec;
  long long tv_sec = a.tv_sec * count + b.tv_sec;
  struct timespec result;
  result.tv_nsec = tv_nsec / count;
  result.tv_sec = tv_sec / count;
  return result;
}
void start_timing(struct timing_variance *tv) {
  clock_gettime(CLOCK_MONOTONIC_RAW, &tv->start);
}
void end_timing(struct timing_variance *tv) {
  struct timespec diff;
  clock_gettime(CLOCK_MONOTONIC_RAW, &tv->end);
  diff.tv_nsec = tv->end.tv_nsec - tv->start.tv_nsec;
  diff.tv_sec = tv->end.tv_sec - tv->start.tv_sec;
  tv->min = min(tv->min, diff);
  tv->max = max(tv->max, diff);
  tv->samples++;
  tv->avg = avg(tv->avg, diff, tv->samples);
}

void init_timing_variance(struct timing_variance *tv) {
  tv->max = (struct timespec){0, 0};
  tv->min = (struct timespec){10000000, 10000000};
  tv->avg = (struct timespec){0, 0};
  tv->samples = 1;
}