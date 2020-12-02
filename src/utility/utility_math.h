#ifndef UTILITY_MATH_H
#define UTILITY_MATH_H
#include "raylib.h"
#include <math.h>
#include <stdlib.h>
inline float clamp_f(float a, float x, float b) {
  if (a > x)
    return a;
  if (b < x)
    return b;
  return x;
}
inline int clamp_i(int a, int x, int b) { return a > x ? a : (x > b ? b : x); }
inline double clamp_d(double a, double x, double b) {
  return a > x ? a : (x > b ? b : x);
}
inline long double clamp_ld(long double a, long double x, long double b) {
  return a > x ? a : (x > b ? b : x);
}
inline float interpf(float a, float b, float t) {
  return a * (1.0 - t) + b * t;
}
inline double interpd(double a, double b, double t) {
  return a * (1.0 - t) + b * t;
}
inline Color interp_color(Color a, Color b, float t) {
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
inline float hermitef(float a, float b, float m0, float m1, float t) {
  return (2 * powf(t, 3) - 3 * powf(t, 2) + 1) +
         (powf(t, 3) - 2 * powf(t, 2) + t) * m0 +
         (-2 * powf(t, 3) + 3 * powf(t, 2)) * b +
         (powf(t, 3) - powf(t, 2)) * m1;
}
// Get a number between a and b from rand()
inline int rand_interval(int a, int b) { return a + rand() % (b - a); }
inline float wrap_aroundf(float start, float end, float number) {
  if (number < start)
    return end;
  if (number > end)
    return start;
  return number;
}
inline Vector2 wrap_around_v2(Vector2 start, Vector2 end, Vector2 pos) {
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

inline float distance(Vector2 a, Vector2 b) {
  return sqrtf(powf(a.x - b.x, 2) + powf(a.y - b.y, 2));
}
inline Vector2 v2_add(Vector2 a, Vector2 b) {
  return (Vector2){a.x + b.x, a.y + b.y};
}
inline Vector2 v2_sub(Vector2 a, Vector2 b) {
  return (Vector2){a.x - b.x, a.y - b.y};
}
inline float v2_magnitude(Vector2 a) {
  return distance((Vector2){0.0, 0.0}, a);
}
inline Vector2 v2_scale(Vector2 a, float t) {
  return (Vector2){a.x * t, a.y * t};
}
/**
 * Return a normalized vector pointing from a to b
 * @param a The origin point
 * @param b The end point
 * @returns The normalized vector pointing from a to b
 * */
inline Vector2 v2_pointing_to(Vector2 a, Vector2 b) {
  Vector2 c = v2_sub(b, a);
  return v2_scale(c, 1.0f / v2_magnitude(c));
}

inline Vector2 v2_negate(Vector2 a) { return (Vector2){-a.x, -a.y}; }
inline Vector2 v2_interpolate(Vector2 a, Vector2 b, float t) {
  return (Vector2){interpf(a.x, b.x, t), interpf(a.y, b.y, t)};
}
inline float v2_dot(Vector2 a, Vector2 b) { return a.x * b.x + a.y * b.y; }

inline Vector2 v2_reflect(Vector2 velocity, Vector2 normal) {
  Vector2 n =
      v2_add(v2_scale(normal, -2.0f * v2_dot(velocity, normal)), velocity);
  return n;
}
inline Vector2 v2_normalize(Vector2 a) {
  return v2_scale(a, 1.0f / v2_magnitude(a));
}

#endif