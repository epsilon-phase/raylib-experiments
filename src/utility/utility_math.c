#include "utility_math.h"
#include <math.h>
#include <stdlib.h>
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
float distance(Vector2 a, Vector2 b) {
  return sqrtf(powf(a.x - b.x, 2) + powf(a.y - b.y, 2));
}
Vector2 v2_add(Vector2 a, Vector2 b) { return (Vector2){a.x + b.x, a.y + b.y}; }
Vector2 v2_sub(Vector2 a, Vector2 b) { return (Vector2){a.x - b.x, a.y - b.y}; }
Vector2 v2_pointing_to(Vector2 a, Vector2 b) {
  Vector2 c = v2_sub(a, b);
  return v2_scale(c, 1.0f / v2_magnitude(c));
}
Vector2 v2_scale(Vector2 a, float t) { return (Vector2){a.x * t, a.y * t}; }
Vector2 v2_negate(Vector2 a) { return (Vector2){-a.x, -a.y}; }
Vector2 v2_interpolate(Vector2 a, Vector2 b, float t) {
  return (Vector2){interpf(a.x, b.x, t), interpf(a.y, b.y, t)};
}
float v2_dot(Vector2 a, Vector2 b) { return a.x * b.x + a.y * b.y; }
float v2_magnitude(Vector2 a) { return distance((Vector2){0.0, 0.0}, a); }
Vector2 v2_reflect(Vector2 velocity, Vector2 normal) {
  Vector2 n =
      v2_add(v2_scale(normal, -2.0f * v2_dot(velocity, normal)), velocity);
  return n;
}
int rand_interval(int a, int b) { return a + rand() % (b - a); }
float wrap_around(float start, float end, float number) {
  if (number < start)
    return end;
  if (number > end)
    return start;
  return number;
}