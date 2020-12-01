#include "raylib.h"
#include "utility_math.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#define min(X, Y) X > Y ? Y : X
#define max(X, Y) X > Y ? X : Y
// An excellent constant, simply,,, the best
const float GRAVITATION_CONSTANT = 0.001f;
const int MAX_SPAWN_MASS = 30;
static float VELOCITY_DECAY = 0.8f;
const float VELOCITY_BELOW_DECAY = 20.0f;
const float SIZE_SCALING_FACTOR = 5.0f;
const float MAX_INITIAL_VELOCITY = 2;
const float MAX_MASS = 3000000;
static int paint_acceleration = 1;
typedef struct {
  Vector2 position, velocity, accelleration;
  float mass;
} mass;

void draw_mass(const mass *m);
void step_mass(mass *, unsigned int n);
mass init_random_mass();
float mass_speed(const mass *m);
Vector2 mass_reflect(mass *a, mass *b);
float mass_radius(const mass *m);
void absorb_mass(mass *a, mass *b);
void mass_collision(mass *a, mass *b);
void reset_masses(mass *array, int count);
int sortalgo(const void *A, const void *B) {
  const mass *a = A, *b = B;
  return a->mass > b->mass ? -1 : (b->mass == a->mass ? 0 : 1);
}
int main(void) {
  // Initialization
  //--------------------------------------------------------------------------------------
  const int screenWidth = 800;
  const int screenHeight = 450;
  SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);
  InitWindow(screenWidth, screenHeight, "N-Body");

  SetTargetFPS(60); // Set our game to run at 60 frames-per-second
  //--------------------------------------------------------------------------------------
  const unsigned int body_count_o = 300;
  unsigned int body_count = body_count_o;
  mass bodies[body_count];
  for (unsigned int i = 0; i < body_count; i++) {
    bodies[i] = init_random_mass();
  }
  int steps_per_tick = 1;
  // Main game loop
  while (!WindowShouldClose()) // Detect window close button or ESC key
  {
    // Update
    //----------------------------------------------------------------------------------
    // TODO: Update your variables here
    //----------------------------------------------------------------------------------
    if (IsKeyPressed(KEY_R))
      reset_masses(bodies, body_count);
    if (IsKeyDown(KEY_UP)) {
      steps_per_tick++;
    }
    if (IsKeyDown(KEY_LEFT)) {
      body_count = min(body_count_o, body_count + 1);
      // VELOCITY_DECAY *= 0.999;
    }
    if (IsKeyDown(KEY_RIGHT)) {
      body_count = max(1, body_count - 1);

      qsort(bodies, body_count, sizeof(mass), sortalgo);
      // VELOCITY_DECAY = min(VELOCITY_DECAY * 1.1, 1.0);
    }
    if (IsKeyPressed(KEY_A)) {
      paint_acceleration = !paint_acceleration;
      printf("PAINTING ACCELERATION %c\n", paint_acceleration ? 'Y' : 'N');
    }
    if (IsKeyDown(KEY_DOWN)) {
      steps_per_tick = steps_per_tick > 1 ? steps_per_tick - 1 : 1;
    }
    for (int i = 0; i < steps_per_tick; i++)
      step_mass(bodies, body_count);

    // Draw
    //----------------------------------------------------------------------------------

    BeginDrawing();

    ClearBackground(RAYWHITE);

    for (unsigned int i = 0; i < body_count; i++)
      draw_mass(&bodies[i]);
    EndDrawing();
    // printf("%f\n", 1.0 / GetFrameTime());
    //----------------------------------------------------------------------------------
  }

  // De-Initialization
  //--------------------------------------------------------------------------------------
  CloseWindow(); // Close window and OpenGL context
  //--------------------------------------------------------------------------------------

  return 0;
}

void draw_mass(const mass *m) {
  char tmp[20];
  // Draw a blue outline
  DrawCircle(m->position.x, m->position.y, mass_radius(m) + 2.0f, BLUE);
  // Draw the inner bit
  DrawCircle(m->position.x, m->position.y, mass_radius(m), BLACK);
  Vector2 end =
      v2_add(v2_scale(m->accelleration, 20.0 * mass_radius(m) / GetFrameTime()),
             m->position);

  DrawLineEx(v2_add(m->position, m->velocity), m->position, 3, MAGENTA);
  if (paint_acceleration)
    DrawLineEx(m->position, end, 3, RED);
  sprintf(tmp, "%0.1f", m->mass);
  DrawText(tmp, m->position.x - MeasureText(tmp, 12) / 2.0, m->position.y, 12,
           DARKGREEN);
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
          GetFrameTime() * GRAVITATION_CONSTANT * (m[i].mass * m[j].mass) /
              powf(distance(m[i].position, m[j].position), 2.0f));
      a->accelleration =
          v2_add(a->accelleration, v2_scale(vel_addition, 1.0f / m[i].mass));
      mass_collision(a, b);
    }
    // Then add it to the velocity
    a->velocity = v2_add(a->velocity, a->accelleration);
    if (mass_speed(a) > VELOCITY_BELOW_DECAY) {
      a->velocity = v2_scale(a->velocity, VELOCITY_DECAY);
    }
  }

  // Then the positions
  for (i = 0; i < count; i++) {
    m[i].position =
        v2_add(m[i].position, v2_scale(m[i].velocity, GetFrameTime()));
    m[i].position.x = wrap_around(0, GetScreenWidth(), m[i].position.x);
    m[i].position.y = wrap_around(0, GetScreenHeight(), m[i].position.y);
    // if ((m[i].position.x < 0.0 && m[i].velocity.x < 0.0) ||
    //     (m[i].position.x > GetScreenWidth() && m[i].velocity.x > 0.0)) {
    //   m[i].velocity.x *= -1;
    //   m[i].velocity.x *= 1.01;
    // }
    // if ((m[i].position.y < 0 && m[i].velocity.y < 0) ||
    //     (m[i].position.y > GetScreenHeight() && m[i].velocity.y > 0)) {
    //   m[i].velocity.y *= -1;
    //   m[i].velocity.y *= 1.01;
    // }
  }
}
mass init_random_mass() {
  mass m;

  m.mass = rand() % MAX_SPAWN_MASS + 1;
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
float mass_speed(const mass *m) {
  return distance((Vector2){0, 0}, m->velocity);
}
float mass_radius(const mass *m) { return logf(m->mass) * SIZE_SCALING_FACTOR; }

void absorb_mass(mass *a, mass *b) {
  Vector2 new_vel = a->velocity;
  new_vel = v2_add(v2_scale(a->velocity, 1 / (a->mass + b->mass)),
                   v2_scale(b->velocity, 1.0f / (b->mass + a->mass)));
  // new_vel = v2_scale(
  //     v2_add(v2_scale(a->velocity, a->mass), v2_scale(b->velocity, b->mass)),
  //     1.0f / (a->mass + b->mass));
  a->accelleration =
      v2_add(v2_scale(a->accelleration, a->mass / (a->mass + b->mass)),
             v2_scale(b->accelleration, b->mass / (a->mass + b->mass)));
  a->position = v2_scale(
      v2_add(v2_scale(a->position, a->mass), v2_scale(b->position, b->mass)),
      1.0f / (a->mass + b->mass));
  a->mass += b->mass;
  if (a->mass > MAX_MASS)
    *a = init_random_mass();
  *b = init_random_mass();
}
void mass_collision(mass *a, mass *b) {
  if (CheckCollisionCircles(a->position, mass_radius(a), b->position,
                            mass_radius(b))) {
    absorb_mass(a, b);
  }
}
void reset_masses(mass *array, int count) {
  for (int i = 0; i < count; i++)
    array[i] = init_random_mass();
}