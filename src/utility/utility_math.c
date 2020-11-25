#include "utility_math.h"
#include <math.h>
float clamp_f(float a, float x, float b) {
  if (a > x)
    return a;
  if (b < x)
    return b;
  return x;
}
int clamp_i(int a, int x, int b) { return a > x ? a : (x > b ? b : x); }
double clamp_d(double a, double x, double b) {
  return a > x ? a : (x > b ? b : x);
}
long double clamp_ld(long double a, long double x, long double b) {
  return a > x ? a : (x > b ? b : x);
}
float interpf(float a, float b, float t) { return a * (1.0 - t) + b * t; }
double interpd(double a, double b, double t) { return a * (1.0 - t) + b * t; }
float hermitef(float a, float b, float m0, float m1, float t) {
  return (2 * powf(t, 3) - 3 * powf(t, 2) + 1) +
         (powf(t, 3) - 2 * powf(t, 2) + t) * m0 +
         (-2 * powf(t, 3) + 3 * powf(t, 2)) * b +
         (powf(t, 3) - powf(t, 2)) * m1;
}