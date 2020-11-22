#include "./circular_buffer.h"
#include "raylib.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define PARTICLE_MAX_SIZE 12
#define PARTICLE_MAX_SPEED 4.0
typedef struct {
  Vector2 position, velocity;
  int lifetime_remaining;
  float size;
  Color c;
} Particle;
float clamp(float a, float p, float b) {
  if (p < a)
    return a;
  if (p > b)
    return b;
  return p;
}
float linear_interp(float a, float b, float t) { return a * (t - 1) + b * t; }
typedef struct {
  Vector2 position;
  int lifetime_max;
  int particle_count;
  Particle *wheelhouse;
} ParticleEmitter;
const static Color colors[] = {
    {255, 0, 0, 255},  // Red
    {0, 255, 0, 255},  // green
    {0, 0, 255, 255},  // Blue
    {55, 55, 55, 255}, // Darkish gray

};
void draw_particle(const Particle *p);
void particle_step(ParticleEmitter *emitter, Particle *p);
ParticleEmitter *getNewEmitter(int x, int y, int particlecount,
                               int maxlifetime);
void emitter_step(ParticleEmitter *e);
void draw_emitter(const ParticleEmitter *e);
void free_emitter(ParticleEmitter *e);
int main() {
  int screenwidth = 800, screenheight = 450;
  InitWindow(screenwidth, screenheight, "try");
  SetTargetFPS(60);
  ParticleEmitter *e = getNewEmitter(400, 225, 900, 500);
  struct CircularBuffer *emitters =
      allocateCircularBuffer(10, &free_emitter, NULL);
  bool ButtonDown = false;
  while (!WindowShouldClose()) {
    e->position = GetMousePosition();

    if (!ButtonDown && (ButtonDown += IsMouseButtonDown(MOUSE_LEFT_BUTTON))) {
      Vector2 c = GetMousePosition();
      push_value_cb(emitters, getNewEmitter(c.x, c.y, 300, 100));
    } else if (!IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
      ButtonDown = false;
    } else {
      ParticleEmitter *emitter = last_value_cb(emitters);
      emitter->lifetime_max += IsKeyDown(KEY_LEFT_SHIFT) ? 5 : 1;
      if (IsKeyDown(KEY_LEFT_SHIFT)) {
        emitter->wheelhouse = reallocarray(
            emitter->wheelhouse, emitter->particle_count + 5, sizeof(Particle));
        memset(&emitter->wheelhouse[emitter->particle_count], 0,
               5 * sizeof(Particle));
        emitter->particle_count += 5;
      }
    }
    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawText("Whoaaa", 190, 200, 20, LIGHTGRAY);
    draw_emitter(e);
    unsigned int emitter_index = 0;
    for (emitter_index = 0; emitter_index < emitters->capacity;
         emitter_index++) {
      ParticleEmitter *emitter = emitters->data[emitter_index];
      if (emitter) {
        draw_emitter(emitter);
      }
    }
    EndDrawing();
    emitter_step(e);
    for (unsigned int emitter_index = 0; emitter_index < emitters->capacity;
         emitter_index++) {
      ParticleEmitter *emitter = emitters->data[emitter_index];
      if (emitter) {
        emitter_step(emitters->data[emitter_index]);
      }
    }
  }
  free_emitter(e);
  cb_free(emitters);
  return 0;
}
void draw_particle(const Particle *p) {
  // DrawPixel(p->position.x, p->position.y, p->c);
  DrawCircle(p->position.x, p->position.y, p->size, p->c);
}
void particle_step(ParticleEmitter *emitter, Particle *p) {
  if (p->lifetime_remaining <= 0) {
    p->position.x = emitter->position.x;
    p->position.y = emitter->position.y;
    p->velocity = (Vector2){0.0, 0.0};
    p->c = colors[rand() % (sizeof(colors) / sizeof(Color))];
    p->lifetime_remaining = rand() % emitter->lifetime_max;
    p->size = rand() % PARTICLE_MAX_SIZE + 1;
    return;
  }
  p->position.x += p->velocity.x;
  p->position.y += p->velocity.y;

  p->velocity.x += 0.3 * (rand() % 3 - 1);
  p->velocity.y += 0.3 * (rand() % 3 - 1);
  p->velocity.x *= 0.9;
  p->velocity.y *= 0.9;
  p->velocity.x = clamp(-PARTICLE_MAX_SPEED, p->velocity.x, PARTICLE_MAX_SPEED);
  p->velocity.y = clamp(-PARTICLE_MAX_SPEED, p->velocity.y, PARTICLE_MAX_SPEED);
  p->lifetime_remaining--;
  p->size = clamp(1.0,
                  PARTICLE_MAX_SIZE * p->lifetime_remaining /
                      (float)emitter->lifetime_max,
                  PARTICLE_MAX_SIZE);
}
ParticleEmitter *getNewEmitter(int x, int y, int particlecount,
                               int maxlifetime) {
  ParticleEmitter *result = calloc(1, sizeof(ParticleEmitter));
  result->particle_count = particlecount;
  result->position.x = x;
  result->position.y = y;
  result->lifetime_max = maxlifetime;
  result->wheelhouse = calloc(particlecount, sizeof(Particle));
  return result;
}
void emitter_step(ParticleEmitter *e) {
  size_t i;
  for (i = 0; i < e->particle_count; i++) {
    particle_step(e, &e->wheelhouse[i]);
  }
}
void draw_emitter(const ParticleEmitter *e) {
  size_t i;
  DrawCircle(e->position.x, e->position.y, 3.0, BLACK);
  for (i = 0; i < e->particle_count; i++) {
    draw_particle(&e->wheelhouse[i]);
  }
}
void free_emitter(ParticleEmitter *e) {
  free(e->wheelhouse);
  free(e);
}