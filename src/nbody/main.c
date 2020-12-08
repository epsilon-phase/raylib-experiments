#include "pointer_utils.h"
#include "raylib.h"
#include "timing.h"
#include "utility_math.h"
#include <math.h>
#include <omp.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define min(X, Y) X > Y ? Y : X
#define max(X, Y) X > Y ? X : Y
// An excellent constant, simply,,, the best
const float GRAVITATION_CONSTANT = 0.001f;
static int MAX_SPAWN_MASS = 30;
static float VELOCITY_DECAY = 0.8f;
const float VELOCITY_BELOW_DECAY = 20.0f;
const float SIZE_SCALING_FACTOR = 5.0f;
const float MAX_INITIAL_VELOCITY = 2;
const float MAX_MASS = 1e18f;
static int paint_acceleration = 1;
static float last_frame_time = 1.0f / 60.0f;
static Vector2 screen_bounds = (Vector2){800, 450};
static const Vector2 zero_v = (Vector2){0.0f, 0.0f};
static unsigned int collision_count = 0;
static unsigned int step_count = 0;
static float wall_time = 0.0f;
static pthread_mutex_t copy_mutex;
typedef struct {
  Vector2 position, velocity, accelleration;
  float mass;
} mass;

static struct {
  mass *original;
  mass *copy;
  int count;
  int max;
  int steps;
  bool reset;
  bool change_threads;
} thread_arguments;

void draw_mass(const mass *m);
void step_mass(mass *, unsigned int n, unsigned int steps);
mass init_random_mass();
float mass_speed(const mass *m);
Vector2 mass_reflect(mass *a, mass *b);
float mass_radius(const mass *m);
void absorb_mass(mass *a, mass *b);
void mass_collision(mass *a, mass *b);
void reset_masses(mass *array, int count);
inline Vector2 body_body_acceleration(const mass *, const mass *);
int compare_mass(const void *A, const void *B) {
  const mass *a = A, *b = B;
  return a->mass > b->mass ? -1 : (b->mass == a->mass ? 0 : 1);
}
const mass *bigger_mass(const mass *a, const mass *b);
void *thread_loop(void *);
int main(void) {
  // Initialization
  //--------------------------------------------------------------------------------------
  const int screenWidth = 800;
  const int screenHeight = 450;
  SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);
  InitWindow(screenWidth, screenHeight, "N-Body");

  SetTargetFPS(60); // Set our game to run at 60 frames-per-second
  //--------------------------------------------------------------------------------------
  const unsigned int body_count_o = 1000;
  unsigned int body_count = 300;
  mass bodies[body_count_o];
  for (unsigned int i = 0; i < body_count_o; i++) {
    bodies[i] = init_random_mass();
  }
  bool use_threads = true;
  bool show_help = false;
  bool show_statistics = false;
  int steps_per_tick = 1;
  int threads = omp_get_num_procs();
  thread_arguments.count = body_count;
  thread_arguments.max = body_count_o;
  thread_arguments.original = bodies;
  thread_arguments.change_threads = false;
  pthread_mutex_init(&copy_mutex, NULL);
  pthread_t thread;
  pthread_create(&thread, NULL, &thread_loop, NULL);
  // Main game loop
  while (!WindowShouldClose()) // Detect window close button or ESC key
  {
    // Update
    //----------------------------------------------------------------------------------
    // TODO: Update your variables here
    //----------------------------------------------------------------------------------
    // last_frame_time = GetFrameTime();
    screen_bounds = (Vector2){GetScreenWidth(), GetScreenHeight()};
    if (IsKeyPressed(KEY_R)) {
      reset_masses(bodies, body_count);
      thread_arguments.reset = true;
      collision_count = 0;
      step_count = 0;
      wall_time = 0;
    }
    if (IsKeyDown(KEY_UP)) {
      steps_per_tick++;
    }
    if (IsKeyDown(KEY_DOWN)) {
      steps_per_tick = steps_per_tick > 1 ? steps_per_tick - 1 : 1;
    }
    thread_arguments.steps = steps_per_tick;
    if (IsKeyDown(KEY_RIGHT)) {
      body_count = min(body_count_o, body_count + 1);
      thread_arguments.count = body_count;
    }
    if (IsKeyDown(KEY_LEFT)) {
      body_count = max(1, body_count - 1);
      thread_arguments.count = body_count;
      // VELOCITY_DECAY = min(VELOCITY_DECAY * 1.1, 1.0);
    }
    if (IsKeyPressed(KEY_S)) {
      show_statistics = !show_statistics;
    }
    if (IsKeyPressed(KEY_A)) {
      paint_acceleration = !paint_acceleration;
      printf("PAINTING ACCELERATION %c\n", paint_acceleration ? 'Y' : 'N');
    }

    if (IsKeyDown(KEY_MINUS)) {
      MAX_SPAWN_MASS = max(1, MAX_SPAWN_MASS - 1);
    }
    if (IsKeyDown(KEY_EQUAL))
      MAX_SPAWN_MASS++;
    if (IsKeyPressed(KEY_T)) {
      use_threads = !use_threads;
      thread_arguments.change_threads = true;
    }
    if (IsKeyPressed(KEY_SLASH) || IsKeyPressed(KEY_H)) {
      show_help = !show_help;
    }
    // step_mass(bodies, body_count, steps_per_tick);

    // Draw
    //----------------------------------------------------------------------------------

    BeginDrawing();

    ClearBackground(RAYWHITE);
    pthread_mutex_lock(&copy_mutex);

    for (unsigned int i = 0; i < body_count; i++)
      draw_mass(&bodies[i]);
    pthread_mutex_unlock(&copy_mutex);
    const char *status =
        TextFormat("Body Count: %i Steps per Frame: %i FPS: %0.2f, Threads: "
                   "%s, max mass spawn: %i, "
                   "Press h for help",
                   body_count, steps_per_tick, 1.0f / GetFrameTime(),
                   use_threads ? "Yes" : "No", MAX_SPAWN_MASS);
    int blank_size = MeasureText(status, 16);
    DrawRectangle(50, 50, blank_size, 16, GRAY);
    DrawText(status, 50, 50, 16, BLACK);
    if (show_help) {
      const char *help =
          "Press up and down arrows to increase or decrease steps per tick\n"
          "Press left and right arrows to increase or decrease the number of "
          "bodies shown\n"
          "Press T to toggle threads\n"
          "Press A to toggle showing acceleration vectors\n"
          "Press R to reset with new bodies\n"
          "Press - and = to decrease/increase the maximum spawn mass\n"
          "Press s to toggle the statistics display\n"
          "Press h or ? to show help";
      Vector2 help_size = MeasureTextEx(GetFontDefault(), help, 16, 1.0);
      DrawRectangle(100, 120, help_size.x, help_size.y, GRAY);
      DrawText(help, 100, 120, 16, BLACK);
    }
    if (show_statistics) {
      pthread_mutex_lock(&copy_mutex);
      const mass *m = find_biggest(
          bodies, (const void *(*)(const void *, const void *)) & bigger_mass,
          body_count, sizeof(mass));
      const char *statistics =
          TextFormat("Largest Mass: %.1f\n"
                     "Largest Radius: %.2f\n"
                     "collision count:%i\n"
                     "simulated time: %.2f seconds\n"
                     "Wall time: %.2f seconds\n"
                     "Dilation factor: %.2f",
                     m->mass, mass_radius(m), collision_count,
                     last_frame_time * step_count, wall_time,
                     (last_frame_time * step_count) / wall_time);
      pthread_mutex_unlock(&copy_mutex);
      Vector2 size = MeasureTextEx(GetFontDefault(), statistics, 16, 1.0);
      DrawRectangle(68, 68, size.x, size.y, GRAY);
      DrawText(statistics, 68, 68, 16, BLACK);
    }
    wall_time += GetFrameTime();
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
  Vector2 end = v2_add(
      v2_scale(m->accelleration, 20.0 * mass_radius(m) / last_frame_time),
      m->position);
  Vector2 vend = m->velocity;
  if (v2_magnitude(vend) > mass_radius(m)) {
    vend = v2_scale(v2_normalize(vend), mass_radius(m));
  }
  vend = v2_add(m->position, vend);
  DrawLineEx(vend, m->position, 3, MAGENTA);
  if (paint_acceleration)
    DrawLineEx(m->position, end, 3, RED);
  sprintf(tmp, "%0.1f", m->mass);
  DrawText(tmp, m->position.x - MeasureText(tmp, 12) / 2.0, m->position.y, 12,
           DARKGREEN);
}

void step_mass(mass *m, unsigned int count, unsigned int steps) {
  while (steps > 0) {
    steps--;
    unsigned int i = 0;
// Update velocities first
#pragma omp parallel for schedule(static)
    for (mass *a = m; a < m + count; a++) {
      // Compute the acceleration first
      a->accelleration = (Vector2){0.0f, 0.0f};

      for (mass *b = m; b < m + count; b++) {
        if (a == b)
          continue;

        a->accelleration =
            v2_add(a->accelleration, body_body_acceleration(a, b));
        // if (a->mass > b->mass)
        //   mass_collision(a, b);
        // else if (a->mass < b->mass)
        //   mass_collision(b, a);
      }
      // Then add it to the velocity
      a->velocity = v2_add(a->velocity, a->accelleration);
      // If the speed is too great, then decay it.
      // It's no fun to watch things moving too fast to simulate properly
      // Sure we could simply reduce the velocity, but then that removes all the
      // fun of the acceleration
      if (mass_speed(a) > VELOCITY_BELOW_DECAY) {
        a->velocity = v2_scale(a->velocity, VELOCITY_DECAY);
      }
    }

// Then the positions
#pragma omp parallel for schedule(static)
    for (i = 0; i < count; i++) {
      m[i].position =
          v2_add(m[i].position, v2_scale(m[i].velocity, last_frame_time));
      m[i].position = wrap_around(zero_v, screen_bounds, m[i].position);
    }
    for (mass *a = m; a < m + count; a++) {
      for (mass *b = a + 1; b < m + count; b++) {
        mass_collision(a, b);
      }
    }
    step_count++;
  }
}
mass init_random_mass() {
  mass m;

  m.mass = random_float_interval(1.0, MAX_SPAWN_MASS);
  m.position = (Vector2){random_float_interval(0.0f, screen_bounds.x),
                         random_float_interval(0.0f, screen_bounds.y)};
  m.velocity = (Vector2){
      random_float_interval(-MAX_INITIAL_VELOCITY, MAX_INITIAL_VELOCITY),
      random_float_interval(-MAX_INITIAL_VELOCITY, MAX_INITIAL_VELOCITY)};
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
float mass_radius(const mass *m) {
  float result = logf(m->mass) / logf(3.0) * SIZE_SCALING_FACTOR;
  // So, down facing parabola is something like
  // -x^2+b*x+c
  // (-x-1)*(x-40)
  // -x^2 + 39x + 40

  // return max((-powf(result, 2.0f) + 39 * result + 40) / 10.0f, 10);
  return result > 40 ? 40 - logf(m->mass) / logf(5.0f) : result;
}

void absorb_mass(mass *a, mass *b) {
  Vector2 new_vel = a->velocity;
  float new_mass = a->mass + b->mass;
  // Conservation of energy
  // m1*v1 + m2*v2 = (m1+m2)*v3
  // v3=(m1*v1)/(m1+m2)+(m2*v2)/(m1+m2)
  new_vel = v2_scale(v2_scale(a->velocity, a->mass), 1.0f / new_mass);
  new_vel = v2_add(new_vel,
                   v2_scale(v2_scale(b->velocity, b->mass), 1.0f / new_mass));
  a->velocity = new_vel;
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
    collision_count++;
  }
}
void reset_masses(mass *array, int count) {
  for (int i = 0; i < count; i++)
    array[i] = init_random_mass();
}
Vector2 body_body_acceleration(const mass *a, const mass *b) {
  Vector2 result;
  result = v2_pointing_to(a->position, b->position);
  result = v2_scale(
      result, last_frame_time * GRAVITATION_CONSTANT * (a->mass * b->mass) /
                  (powf(distance(a->position, b->position), 2.0f)));
  result = v2_scale(result, 1.0f / a->mass);
  return result;
}
const mass *bigger_mass(const mass *a, const mass *b) {
  if (a->mass < b->mass)
    return b;
  return a;
}
void reset_all_mass(mass *m, size_t count) {
  for (size_t i = 0; i < count; i++)
    m[i] = init_random_mass();
}
void print_spec(struct timespec ts) {
  printf("%f seconds", ts.tv_sec + ts.tv_nsec / 1.0e9f);
}
/**
 * Get the target loop speed given a number of times per second it should run
 **/
static struct timespec get_multiplier(int n) {
  struct timespec c;
  c.tv_sec = 0;
  c.tv_nsec = ((last_frame_time / n) * 1e9);
  return c;
}
void *thread_loop(void *discard) {
  mass *copy = malloc(sizeof(mass) * thread_arguments.max);
  memcpy(copy, thread_arguments.original, sizeof(mass) * thread_arguments.max);
  struct timing_variance t;
  bool use_threads = true;
  init_timing_variance(&t);
  while (true) {

    if (thread_arguments.reset) {
      reset_all_mass(copy, thread_arguments.count);
      thread_arguments.reset = false;
    }
    if (thread_arguments.change_threads) {
      use_threads = !use_threads;
      omp_set_num_threads(use_threads ? omp_get_num_procs() : 1);
      omp_set_schedule(omp_sched_static,
                       thread_arguments.count / omp_get_num_threads());
      thread_arguments.change_threads = false;
    }
    start_timing(&t);
    step_mass(copy, thread_arguments.count, 1);
    pthread_mutex_lock(&copy_mutex);
    memcpy(thread_arguments.original, copy,
           thread_arguments.max * sizeof(mass));
    pthread_mutex_unlock(&copy_mutex);
    end_timing(&t);
    print_timing(stdout, &t);
    struct timespec c = get_multiplier(thread_arguments.steps);
    printf("Loop speed goal %f\n", c.tv_sec + c.tv_nsec / 1.0e9);
    sleep_timing(&t, c);
  }
  return NULL;
}
