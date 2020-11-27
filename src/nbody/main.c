#include "raylib.h"
#include "utility_math.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// An excellent constant, simply,,, the best
#define GRAVITATION_CONSTANT 0.001f
#define MAX_MASS 30
#define VELOCITY_DECAY 0.8f
#define SIZE_SCALING_FACTOR 5.0f
#define MAX_INITIAL_VELOCITY 2
typedef struct {
  Vector2 position, velocity, accelleration;
  float mass;
} mass;

void draw_mass(const mass *m);
void step_mass(mass *, unsigned int n);
mass init_random_mass();
float mass_velocity(const mass *m);
Vector2 mass_reflect(mass *a, mass *b);
float mass_radius(const mass *m);
int main(void) {
  // Initialization
  //--------------------------------------------------------------------------------------
  const int screenWidth = 800;
  const int screenHeight = 450;

  InitWindow(screenWidth, screenHeight, "N-Body");

  SetTargetFPS(60); // Set our game to run at 60 frames-per-second
  //--------------------------------------------------------------------------------------
  const unsigned int body_count = 20;
  mass bodies[body_count];
  for (unsigned int i = 0; i < body_count; i++) {
    bodies[i] = init_random_mass();
  }
  // Main game loop
  while (!WindowShouldClose()) // Detect window close button or ESC key
  {
    // Update
    //----------------------------------------------------------------------------------
    // TODO: Update your variables here
    //----------------------------------------------------------------------------------
    step_mass(bodies, body_count);
    // Draw
    //----------------------------------------------------------------------------------

    BeginDrawing();

    ClearBackground(RAYWHITE);

    for (unsigned int i = 0; i < body_count; i++)
      draw_mass(&bodies[i]);
    EndDrawing();
    //----------------------------------------------------------------------------------
  }

  // De-Initialization
  //--------------------------------------------------------------------------------------
  CloseWindow(); // Close window and OpenGL context
  //--------------------------------------------------------------------------------------

  return 0;
}

void draw_mass(const mass *m) {
  // Draw a blue outline
  DrawCircle(m->position.x, m->position.y, mass_radius(m) + 2.0f, BLUE);
  // Draw the inner bit
  DrawCircle(m->position.x, m->position.y, mass_radius(m), BLACK);
  Vector2 end = v2_add(
      v2_scale(m->accelleration, 20.0 * m->mass / GetFrameTime()), m->position);
  DrawLineEx(m->position, end, 3, RED);
}

void step_mass(mass *m, unsigned int count) {
  unsigned int i = 0;
  // Update velocities first
  for (i = 0; i < count; i++) {
    mass *a = &m[i];
    // Compute the acceleration first
    m[i].accelleration = (Vector2){0.0f, 0.0f};

    for (unsigned int j = 0; j < count; j++) {
      if (i == j)
        continue;
      mass *b = &m[j];
      Vector2 vel_addition = v2_scale(
          v2_sub(b->position, a->position),
          GetFrameTime() * GRAVITATION_CONSTANT * (m[i].mass + m[j].mass) /
              distance(m[i].position, m[j].position));
      a->accelleration =
          v2_add(a->accelleration, v2_scale(vel_addition, 1.0f / m[i].mass));
      Vector2 atmp = mass_reflect(a, b);
      Vector2 btmp = mass_reflect(b, a);
      a->velocity = atmp;
      b->velocity = btmp;
    }
    // Then add it to the velocity
    a->velocity = v2_add(a->velocity, a->accelleration);
  }
  // Then the positions
  for (i = 0; i < count; i++) {
    m[i].position = v2_add(m[i].position, v2_scale(m[i].velocity, 1));

    if ((m[i].position.x < 0.0 && m[i].velocity.x < 0.0) ||
        (m[i].position.x > GetScreenWidth() && m[i].velocity.x > 0.0)) {
      m[i].velocity.x *= -1;
    }
    if ((m[i].position.y < 0 && m[i].velocity.y < 0) ||
        (m[i].position.y > GetScreenHeight() && m[i].velocity.y > 0)) {
      m[i].velocity.y *= -1;
    }
  }
}
mass init_random_mass() {
  mass m;
  m.mass = rand() % MAX_MASS + 1;
  m.position = (Vector2){rand() % GetScreenWidth(), rand() % GetScreenHeight()};
  m.velocity =
      (Vector2){rand_interval(-MAX_INITIAL_VELOCITY, MAX_INITIAL_VELOCITY),
                rand_interval(-MAX_INITIAL_VELOCITY, MAX_INITIAL_VELOCITY)};
  return m;
}
Vector2 mass_reflect(mass *a, mass *b) {
  Vector2 ret = a->velocity;
  if (CheckCollisionCircles(a->position, mass_radius(a), b->position,
                            mass_radius(b))) {
    Vector2 tangent;
    tangent = v2_sub(a->position, b->position);
    tangent = v2_scale(tangent, 1.0f / v2_magnitude(tangent));
    ret = v2_reflect(ret, tangent);
    ret = v2_add(v2_scale(ret, (a->mass - b->mass) / (a->mass + b->mass)),
                 v2_scale(b->velocity, (2.0f * b->mass) / (a->mass + b->mass)));
  }
  return ret;
}
float mass_velocity(const mass *m) {
  return distance((Vector2){0, 0}, m->velocity);
}
float mass_radius(const mass *m) { return logf(m->mass) * SIZE_SCALING_FACTOR; }
