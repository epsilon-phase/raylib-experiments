#pragma once
#include "raylib.h"
#include <math.h>
#include <stdlib.h>
#define RANDOM_MAX ((1L << 31) - 1)
static inline float clamp_f(float a, float x, float b) {
  if (a > x)
    return a;
  if (b < x)
    return b;
  return x;
}
static inline int clamp_i(int a, int x, int b) {
  return a > x ? a : (x > b ? b : x);
}
static inline double clamp_d(double a, double x, double b) {
  return a > x ? a : (x > b ? b : x);
}
static inline long double clamp_ld(long double a, long double x,
                                   long double b) {
  return a > x ? a : (x > b ? b : x);
}
static inline float interpf(float a, float b, float t) {
  return a * (1.0 - t) + b * t;
}
static inline double interpd(double a, double b, double t) {
  return a * (1.0 - t) + b * t;
}
static inline Color interp_color(Color a, Color b, float t) {
  return (Color){(unsigned char)floor(interpf(a.r, b.r, t)),
                 (unsigned char)floor(interpf(a.g, b.g, t)),
                 (unsigned char)floor(interpf(a.b, b.b, t)),
                 (unsigned char)floor(interpf(a.a, b.a, t))};
}
/**
 * Hermite interpolation, for when linear interpolation isn't *smooth* enough
 * for you
 *
 * @param a The starting point
 * @param b The ending point
 * @param m0 The starting tangent
 * @param m1 The ending tangent
 * @param t The position on the interval between 0 and 1
 * @returns The interpolated position
 **/
static inline float hermitef(float a, float b, float m0, float m1, float t) {
  return (2 * powf(t, 3) - 3 * powf(t, 2) + 1) +
         (powf(t, 3) - 2 * powf(t, 2) + t) * m0 +
         (-2 * powf(t, 3) + 3 * powf(t, 2)) * b +
         (powf(t, 3) - powf(t, 2)) * m1;
}
// Get a number between a and b from rand()
static inline int rand_interval(int a, int b) { return a + rand() % (b - a); }
static inline long random_interval(long a, long b) {
  return a + random() % (b - a);
}

static inline float random_float_interval(float a, float b) {
  const long random_max = RANDOM_MAX;
  return (b - a) * (random() / ((float)random_max)) + a;
}
static inline Vector2 random_interval_v2(Vector2 a, Vector2 b) {
  Vector2 r;
  r.x = random_float_interval(a.x, b.x);
  r.y = random_float_interval(a.y, b.y);
  return r;
}
static inline float wrap_aroundf(float start, float end, float number) {
  if (number < start)
    return end;
  if (number > end)
    return start;
  return number;
}
static inline Vector2 wrap_around_v2(Vector2 start, Vector2 end, Vector2 pos) {
  pos.x = wrap_aroundf(start.x, end.x, pos.x);
  pos.y = wrap_aroundf(start.y, end.y, pos.y);
  return pos;
}
#define wrap_around(A, B, T)                                                   \
  _Generic(A, float                                                            \
           : wrap_aroundf, Vector2                                             \
           : wrap_around_v2, default                                           \
           : wrap_aroundf)(A, B, T)
/**
 * Clamp a number between A and B
 * */
#define clamp(A, X, B)                                                         \
  _Generic(X, double                                                           \
           : clamp_d, long double                                              \
           : clamp_ld, float                                                   \
           : clamp_f, default                                                  \
           : clamp_i)(A, X, B)
#define clamp_pos_neg(A, X) clamp(-A, X, A)

#define interp(A, B, T)                                                        \
  _Generic(A, double                                                           \
           : interpd, float                                                    \
           : interpf, Vector2                                                  \
           : v2_interp, Color                                                  \
           : interp_color, default                                             \
           : interpf);

static inline float distance(Vector2 a, Vector2 b) {
  return sqrtf(powf(a.x - b.x, 2) + powf(a.y - b.y, 2));
}
static inline Vector2 v2_add(Vector2 a, Vector2 b) {
  return (Vector2){a.x + b.x, a.y + b.y};
}
static inline Vector2 v2_sub(Vector2 a, Vector2 b) {
  return (Vector2){a.x - b.x, a.y - b.y};
}
static inline float v2_magnitude(Vector2 a) {
  return distance((Vector2){0.0, 0.0}, a);
}
static inline Vector2 v2_scale(Vector2 a, float t) {
  return (Vector2){a.x * t, a.y * t};
}
/**
 * Return a normalized vector pointing from a to b
 * @param a The origin point
 * @param b The end point
 * @returns The normalized vector pointing from a to b
 * */
static inline Vector2 v2_pointing_to(Vector2 a, Vector2 b) {
  Vector2 c = v2_sub(b, a);
  return v2_scale(c, 1.0f / v2_magnitude(c));
}

static inline Vector2 v2_negate(Vector2 a) { return (Vector2){-a.x, -a.y}; }
static inline Vector2 v2_interpolate(Vector2 a, Vector2 b, float t) {
  return (Vector2){interpf(a.x, b.x, t), interpf(a.y, b.y, t)};
}
static inline float v2_dot(Vector2 a, Vector2 b) {
  return a.x * b.x + a.y * b.y;
}

static inline Vector2 v2_reflect(Vector2 velocity, Vector2 normal) {
  Vector2 n =
      v2_add(v2_scale(normal, -2.0f * v2_dot(velocity, normal)), velocity);
  return n;
}
static inline Vector2 v2_normalize(Vector2 a) {
  return v2_scale(a, 1.0f / v2_magnitude(a));
}
static inline float v2_cross(Vector2 a, Vector2 b) {
  return a.x * b.y - b.y * a.x;
}
/**
 * @returns True if there is an intersection. Writes to intersection in this
 * case
 * */
static inline bool v2_line_intersect(Vector2 a, Vector2 b, Vector2 c, Vector2 d,
                                     Vector2 *intersection) {
  Vector2 p = a, r = v2_sub(b, a);
  Vector2 q = c, s = v2_sub(d, c);
  float t = v2_cross(v2_sub(q, p), s) / v2_cross(r, s);
  float u = v2_cross(v2_sub(p, q), r) / v2_cross(s, r);
  if (v2_cross(r, s) == 0 && v2_cross(v2_sub(q, p), r) == 0) {
    // Colinear
    return false;
  }
  if (v2_cross(r, s) == 0 && v2_cross(v2_sub(q, p), r) != 0) {
    // Parallel, non intersecting
    return false;
  }
  if (v2_cross(r, s) != 0) {
    if (u >= 0 && u <= 1.0f && t >= 0.0f && t <= 1.0f) {
      *intersection = v2_add(p, v2_scale(r, t));
      return true;
    } else {
      return false;
    }
  }
}
// Forgive us for the comments here, we haven't worked on this kinda equation in
// long enough that we prefer to do some scratch work to make sure the math is
// right

static inline float v2_line_slope(Vector2 a, Vector2 b) {
  /**
   * y-y0 = m(x-x0)
   *      / (x-x0)
   * (y-y0)/(x-x0)=m
   **/
  return (b.y - a.y) / (b.x - a.x);
}
static inline float v2_line_offset(Vector2 a, Vector2 b) {
  return a.y - v2_line_slope(a, b) * a.x;
}
static inline Vector2 v2_normal_line(Vector2 a, Vector2 b) {
  /**
   * y=m*x+b
   *  - b
   * y-b=m*x
   *    /x
   * (y-b)/x=m
   **/
  Vector2 result;
  float slope = v2_line_slope(a, b);
  /**
   * y-f(x0)=-1/(f'(x0))*(x-x0)
   **/
  result.x = 1;
  result.y = -1 / slope;

  return v2_normalize(result);
}

static inline float degtorad(float deg) { return deg * PI / 180.0f; }
static inline Vector2 v2_get_angled(float angle, float magnitude) {
  return v2_scale((Vector2){cosf(angle), sin(angle)}, magnitude);
}
static inline Vector2 v2_rotate(Vector2 a, float angle) {
  return (Vector2){cosf(angle) * a.x - a.y * sinf(angle),
                   a.x * sinf(angle) + a.y * cosf(angle)};
}
static inline void v2_bezier3(Vector2 a, Vector2 b, Vector2 c, Vector2 *out,
                              size_t segments) {
  float step = 1.0 / (segments-1);
  float t = 0.0f;
  for (int i = 0; i < segments; i++) {
    // b(t)=)(1-t)^2*a+2*(1-t)*b+t^2*c
    out[i] =
        v2_add(v2_scale(a, powf(1 - t, 2)),
               v2_add(v2_scale(b, 2 * (1 - t) * t), v2_scale(c, powf(t, 2))));
    t += step;
  }
}
static inline int factorial(int i) {
  int t = 1;
  for (i = i; i > 0; i--) {
    t *= i;
  }
  return t;
}
static inline int binomial_coefficient(int n, int k) {
  return factorial(n) / (factorial(k) * factorial(n - k));
}
static inline void v2_bezier_n(Vector2 *points, size_t count, Vector2 *output,
                               size_t segments) {
  float step = 1.0 / segments;
  float t = 0;
  for (int s = 0; s < segments; s++) {
    Vector2 current = (Vector2){0, 0};

    for (int i = 0; i < count; i++) {

      current = v2_add(current,
                       v2_scale(points[i], powf(1 - t, count - i) * powf(t, i) *
                                               binomial_coefficient(count, i)));
    }
    output[s] = current;
    t+=step;
  }
}