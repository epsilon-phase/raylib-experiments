#include "./timing.h"

int timespec_cmp(struct timespec a, struct timespec b) {
  if (a.tv_sec > b.tv_sec)
    return 1;
  if (a.tv_sec < b.tv_sec)
    return -1;
  if (a.tv_nsec > b.tv_nsec)
    return 1;
  else if (a.tv_nsec < b.tv_nsec)
    return -1;
  return 0;
}
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
  result.tv_nsec = tv_nsec / (count + 1);
  result.tv_sec = tv_sec / (count + 1);
  return result;
}
static struct timespec timespec_diff(struct timespec a, struct timespec b) {
  struct timespec r;
  r.tv_nsec = a.tv_nsec - b.tv_nsec;
  r.tv_sec = a.tv_sec - b.tv_sec;
  r.tv_nsec = r.tv_nsec < 0 ? 0 : r.tv_nsec;
  r.tv_sec = r.tv_sec < 0 ? 0 : r.tv_sec;
  return r;
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
  tv->samples = 0;
}
static double seconds(struct timespec t) {
  return t.tv_sec + t.tv_nsec / (1.0e9);
}
void print_timing(FILE *f, const struct timing_variance *tv) {
  fprintf(f, "Min: %.3g, Max: %.3g, Average: %.3g, Samples: %i\n",
          seconds(tv->min), seconds(tv->max), seconds(tv->avg), tv->samples);
}
void sleep_timing(const struct timing_variance *tv, struct timespec ts) {
  struct timespec r = timespec_diff(ts, timespec_diff(tv->end, tv->start));
  printf("Sleeping for %f seconds\n", ts.tv_sec + ts.tv_nsec / 1.0e9);
  nanosleep(&r, NULL);
}