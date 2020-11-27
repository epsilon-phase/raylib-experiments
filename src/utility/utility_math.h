#pragma once
#include "raylib.h"
float clamp_f(float a, float x, float b);
int clamp_i(int a, int x, int b);
long double clamp_ld(long double a, long double x, long double b);
double clamp_d(double a, double x, double b);
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

float interpf(float a, float b, float t);
double interpd(double a, double b, double t);
#define interp(A, B, T)                                                        \
  _Generic(T, double                                                           \
           : interpd, float                                                    \
           : interpf, Vector2                                                  \
           : v2_interp, default                                                \
           : interpf);
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
float hermitef(float a, float b, float m0, float m1, float t);

float distance(Vector2 a, Vector2 b);

Vector2 v2_add(Vector2 a, Vector2 b);
Vector2 v2_sub(Vector2 a, Vector2 b);
/**
 * Return a normalized vector pointing from a to b
 * @param a The origin point
 * @param b The end point
 * @returns The normalized vector pointing from a to b
 * */
Vector2 v2_pointing_to(Vector2 a, Vector2 b);
Vector2 v2_scale(Vector2 a, float t);
Vector2 v2_negate(Vector2 a);
Vector2 v2_interpolate(Vector2 a, Vector2 b, float t);
float v2_magnitude(Vector2 a);
Vector2 v2_reflect(Vector2 velocity, Vector2 normal);
// Get a number between a and b from rand()
int rand_interval(int a, int b);