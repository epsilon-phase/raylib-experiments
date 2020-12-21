#include "raylib.h"
#include "utility_math.h"

#include <stdio.h>
typedef struct {
  Vector2 position;
  Vector2 velocity;
  float size;
  float temperature;
  float rotation;
  int sides;
  float mass;
  bool falling_ember, alive;
} Particle;
typedef struct {
  Vector2 position;
  Particle *stuff;
  size_t count;
  bool running;
} emitter;
static float angle_range = 30;
static float max_size = 15;
static float max_speed = 3.0f;
static float min_speed = 1.0f;
static float min_temperature = 4400;
static float max_temperature = 7000;
static float min_mass = 0.75;
static float max_mass = 2;
static const float heat_transfer_coefficient = 0.5f;
/// The Medium temperature
static const float medium_temperature = 275;

Color Blackbody2Rgb(float temp);
void draw_particle(const Particle *restrict p);
void particle_step(Particle *restrict p);
emitter initEmitter(size_t count, Vector2 position);
void draw_emitter(const emitter *restrict e);
void step_emitter(const emitter *restrict e);
Particle initParticle(float x, float y, float size, float temperature);

int main() {
  printf("size of particle: %lu\n", sizeof(Particle));
  // Initialization
  //--------------------------------------------------------------------------------------
  const int screenWidth = 800;
  const int screenHeight = 450;

  InitWindow(screenWidth, screenHeight, "flames2");

  // TODO: Load resources / Initialize variables at this point
  const int particle_count = 1000;
  SetTargetFPS(60);
  float frame = 0;
  //--------------------------------------------------------------------------------------
  emitter center = initEmitter(
      particle_count, (Vector2){GetScreenWidth() / 2, GetScreenHeight()});

  // Main game loop
  while (!WindowShouldClose()) // Detect window close button or ESC key
  {
    center.position.x = GetScreenWidth() / 2 + 40 * cosf(frame / 100);
    step_emitter(&center);
    // Update
    //----------------------------------------------------------------------------------
    // TODO: Update variables / Implement example logic at this point
    //----------------------------------------------------------------------------------

    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();

    ClearBackground(BLACK);

    // TODO: Draw everything that requires to be drawn at this point:

    draw_emitter(&center);
    EndDrawing();
    //----------------------------------------------------------------------------------
    frame += 1;
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
  if (p->alive)
    DrawPoly(p->position, p->sides, p->size, p->rotation * (180 / PI),
             Blackbody2Rgb(p->temperature));
// Debug stuff
#ifdef DEBUG_EMBER
  if (p->falling_ember)
    DrawPolyLines(p->position, p->sides, p->size, p->rotation * (180 / PI),
                  BLUE);
#endif
}
void cool_particle(Particle *restrict p) {
  float Q = heat_transfer_coefficient * (p->size) *
            (p->temperature - medium_temperature);
  Q /= p->mass;
  p->temperature -= Q / (60.0f);
}
void particle_step(Particle *restrict p) {
  if (!p->alive)
    return;
  p->position = v2_add(p->position, v2_scale(p->velocity, 1));

  if (!p->falling_ember) {
    // p->temperature *= random_float_interval(0.96f, 0.98f);

    if (p->velocity.x < 0) {
      p->velocity = v2_rotate(p->velocity, 0.02f);
    } else {
      p->velocity = v2_rotate(p->velocity, -0.02f);
    }
  } else {
    // if (p->velocity.y < 1)
    // p->velocity = v2_rotate(p->velocity, p->velocity.x < 0 ? -0.001 : 0.001);
    p->velocity = v2_add(p->velocity, (Vector2){0, 0.005});
    // p->temperature *= random_float_interval(0.996f, 0.998f);
    if (p->position.y > GetScreenHeight())
      p->temperature = 0;
  }
  p->rotation += random_float_interval(-0.01, 0.01);
  cool_particle(p);

  if (p->temperature < 600) {
    p->alive = false;
    // *p = initParticle(respawn.x, respawn.y,
    //                   random_float_interval(max_size / 4, max_size),
    //                   random_float_interval(4400, 7000));
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
  // Make sure the particle has a chance of being an ember that lasts longer
  // than the rest of the flame
  r.falling_ember = spawn_count % 5000 == 0;
  r.mass = random_float_interval(min_mass, max_mass);
  if (r.falling_ember) {
    r.velocity = v2_scale(r.velocity, 1.2);
    r.temperature *= 2.5;
    r.mass *= 4;
  }
  r.rotation = random_float_interval(0, 2 * PI);
  r.sides = random_interval(3, 7);

  r.alive = true;
  spawn_count++;
  return r;
}
void draw_emitter(const emitter *restrict e) {
  for (Particle *p = e->stuff; p < e->stuff + e->count; p++)
    draw_particle(p);
}
void step_emitter(const emitter *restrict e) {
  for (Particle *p = e->stuff; p < e->stuff + e->count; p++) {
    particle_step(p);
    if (e->running && !p->alive)
      *p = initParticle(e->position.x, e->position.y,
                        random_float_interval(max_size / 4, max_size),
                        random_float_interval(4400, 7700));
  }
}
emitter initEmitter(size_t count, Vector2 position) {
  emitter e;
  e.count = count;
  e.stuff = calloc(count, sizeof(Particle));
  for (int i = 0; i < count; i++)
    e.stuff[i] = initParticle(
        position.x, position.y, random_float_interval(max_size / 4, max_size),
        random_float_interval(min_temperature, max_temperature));
  e.position = position;
  e.running = true;
  return e;
}