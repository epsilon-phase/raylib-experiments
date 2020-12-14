#include "raylib.h"
#include "utility_math.h"

#include <stdio.h>
typedef struct {
  Vector2 position;
  Vector2 velocity;
  float size;
  float temperature;
  int lifetime;
  float rotation;
  int sides;
} Particle;
typedef struct {
  Vector2 position;
  Vector2 direction;
  Particle *stuff;
  size_t count;
} emitter;
static float angle_range = 30;
static float max_size = 15;
static float max_speed = 3.0f;
static float min_speed = 1.0f;
Color Blackbody2Rgb(float temp);
void draw_particle(const Particle *restrict p);
void particle_step(Particle *restrict p, Vector2 respawn);
emitter initEmitter(size_t count, Vector2 position);
void draw_emitter(const emitter *restrict e);
void step_emitter(const emitter *restrict e);
Particle initParticle(float x, float y, float size, float temperature);

int main() {
  // Initialization
  //--------------------------------------------------------------------------------------
  const int screenWidth = 800;
  const int screenHeight = 450;

  InitWindow(screenWidth, screenHeight, "flames2");

  // TODO: Load resources / Initialize variables at this point
  const int particle_count = 1000;
  SetTargetFPS(60);
  //--------------------------------------------------------------------------------------
  Particle particles[particle_count];
  Particle a = {(Vector2){GetScreenWidth() / 2, GetScreenHeight() / 2}, 5,
                6000};
  for (int i = 0; i < particle_count; i++)
    particles[i] =
        initParticle(GetScreenWidth() / 2.0f, GetScreenHeight(),
                     random_float_interval(max_size / 4, max_size), 6000);

  // Main game loop
  while (!WindowShouldClose()) // Detect window close button or ESC key
  {
    for (int i = 0; i < particle_count; i++)
      particle_step(&particles[i],
                    (Vector2){GetScreenWidth() / 2, GetScreenHeight()});
    // Update
    //----------------------------------------------------------------------------------
    // TODO: Update variables / Implement example logic at this point
    //----------------------------------------------------------------------------------

    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();

    ClearBackground(BLACK);

    // TODO: Draw everything that requires to be drawn at this point:

    for (int i = 0; i < particle_count; i++)
      draw_particle(&particles[i]);
    EndDrawing();
    //----------------------------------------------------------------------------------
  }

  // De-Initialization
  //--------------------------------------------------------------------------------------

  // TODO: Unload all loaded resources at this point

  CloseWindow(); // Close window and OpenGL context
  //--------------------------------------------------------------------------------------

  return 0;
}
// Adapted from https://gist.github.com/paulkaplan/5184275
Color Blackbody2Rgb(float kelvin) {
  float temp = kelvin / 100;

  float red, green, blue;

  if (temp <= 66) {

    red = 255;

    green = temp;
    green = 99.4708025861 * logf(green) - 161.1195681661;

    if (temp <= 19) {

      blue = 0;

    } else {

      blue = temp - 10;
      blue = 138.5177312231 * logf(blue) - 305.0447927307;
    }

  } else {

    red = temp - 60;
    red = 329.698727446 * powf(red, -0.1332047592);

    green = temp - 60;
    green = 288.1221695283 * powf(green, -0.0755148492);

    blue = 255;
  }

  return (Color){clamp(0, 255, red), clamp(0, 255, green), clamp(0, 255, blue),
                 255};
}
void draw_particle(const Particle *restrict p) {
#if 0
  //This was fun, but it's kinda unncecessary
  Vector2 a = (Vector2){0, -1.0f}, b = v2_normalize((Vector2){0.25, 0.25}),
          c = v2_normalize((Vector2){-0.25, 0.25});
  a = v2_rotate(a, p->rotation);
  b = v2_rotate(b, p->rotation);
  c = v2_rotate(c, p->rotation);
  a = v2_add(p->position, v2_scale(a, p->size));
  b = v2_add(p->position, v2_scale(b, p->size));
  c = v2_add(p->position, v2_scale(c, p->size));
  DrawTriangle(a, c, b, Blackbody2Rgb(p->temperature));
#endif
  DrawPoly(p->position, p->sides, p->size, p->rotation * (180 / PI),
           Blackbody2Rgb(p->temperature));
}
// TODO: Figure out how to curve it back towards the direction vector in a
// general sense
// MAYBE rodrigues' formula, or its inverse
void particle_step(Particle *restrict p, Vector2 respawn) {
  p->position = v2_add(p->position, v2_scale(p->velocity, 1));
  p->temperature *= random_float_interval(0.96f, 0.98f);
  if (p->velocity.x < 0) {
    p->velocity = v2_rotate(p->velocity, 0.02f);
  } else {
    p->velocity = v2_rotate(p->velocity, -0.02f);
  }
  if (p->temperature < 600) {
    *p = initParticle(respawn.x, respawn.y,
                      random_float_interval(max_size / 4, max_size),
                      random_float_interval(4400, 7000));
  }
}
static int spawn_count = 0;
Particle initParticle(float x, float y, float size, float temperature) {
  Particle r;
  r.position = (Vector2){x, y};
  r.size = size;
  r.temperature = temperature;
  r.velocity =
      v2_rotate((Vector2){0.0, -1.0},
                degtorad(random_float_interval(-angle_range, angle_range)));
  r.velocity =
      v2_scale(r.velocity, random_float_interval(min_speed, max_speed));
  if (spawn_count % 100 == 0) {
    r.velocity = v2_scale(r.velocity, 1.5);
  }
  r.rotation = random_float_interval(0, 2 * PI);
  r.sides = random_interval(3, 7);
  spawn_count++;
  return r;
}
void draw_emitter(const emitter *restrict e) {
  for (Particle *p = e->stuff; p < e->stuff + e->count; p++)
    draw_particle(p);
}
void step_emitter(const emitter *restrict e) {
  for (Particle *p = e->stuff; p < e->stuff + e->count; p++) {
    particle_step(p, e->position);
  }
}
emitter initEmitter(size_t count, Vector2 position) {
  emitter e;
  e.count = count;
  e.stuff = calloc(count, sizeof(Particle));
  e.position = position;
  return e;
}