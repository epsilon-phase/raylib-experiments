#include "raylib.h"
#include "utility_math.h"
#include <stddef.h>
typedef struct {
  Color center;
  Vector2 position;
  Vector2 offset;

} Particle;

typedef struct {
  size_t count;
  Particle *array;
} ParticleTrail;
struct ParticleParameters {
  /**
   * The direction vector that will be the basis for movement of the flame.
   *
   * Should be normalized
   **/
  Vector2 initial_delta;
  /**
   * The amount that the width of the trail decays each simulation step
   **/
  float width_decay;
  /**
   * What Color the trail will tend to decay to
   **/
  Color color_decay_to;
};
static float angle_range = 7.0f;
static float max_step_magnitude = 30.0f;
static float min_step_reduction = 0.9999f, max_step_reduction = 0.5f;
ParticleTrail initTrail(size_t segments);
void freeTrail(ParticleTrail *);
void draw_trail(ParticleTrail *restrict pt);
void simulate_trail(ParticleTrail *restrict pt);
int main() {
  // Initialization
  //--------------------------------------------------------------------------------------
  const int screenWidth = 800;
  const int screenHeight = 450;

  InitWindow(screenWidth, screenHeight, "Flame");
  const int flame_count = 8;
  // TODO: Load resources / Initialize variables at this point

  SetTargetFPS(60);
  //--------------------------------------------------------------------------------------
  ParticleTrail pt[flame_count];
  for (int i = 0; i < flame_count; i++) {
    pt[i] = initTrail(10);
    pt[i].array[0] = (Particle){
        ORANGE,
        (Vector2){i * (GetScreenWidth() / flame_count) +
                      GetScreenWidth() / (flame_count * 2),
                  GetScreenHeight()},
        (Vector2){100.0, 0.0},
    };
  }

  // Main game loop
  while (!WindowShouldClose()) // Detect window close button or ESC key
  {
    for (int i = 0; i < flame_count; i++)
      simulate_trail(&pt[i]);
    // Update
    //----------------------------------------------------------------------------------
    // TODO: Update variables / Implement example logic at this point
    //----------------------------------------------------------------------------------

    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();

    ClearBackground(RAYWHITE);

    // TODO: Draw everything that requires to be drawn at this point:
    DrawText("Wiggle!", 190, 200, 20,
             LIGHTGRAY); // Example
    for (int i = 0; i < flame_count; i++)
      draw_trail(&pt[i]);

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
ParticleTrail initTrail(size_t segments) {
  ParticleTrail result;
  result.array = calloc(segments, sizeof(Particle));
  result.count = segments;
  return result;
}
void freeTrail(ParticleTrail *t) {
  free(t->array);
  t->array = NULL;
  t->count = 0;
}
void draw_trail(ParticleTrail *restrict pt) {
  Vector2 a, b, c, d;
  for (int i = 0; i < pt->count - 1; i++) {

    a = pt->array[i].position;
    b = pt->array[i + 1].position;
    c = v2_add(pt->array[i].position, pt->array[i].offset);
    d = v2_add(pt->array[i + 1].position, pt->array[i + 1].offset);
    DrawTriangle(c, b, a, pt->array[i].center);
    DrawTriangle(d, b, c, pt->array[i].center);
    c = v2_sub(pt->array[i].position, pt->array[i].offset);
    d = v2_sub(pt->array[i + 1].position, pt->array[i + 1].offset);
    DrawTriangle(b, c, a, pt->array[i].center);
    DrawTriangle(b, d, c, pt->array[i].center);
    // DrawTriangle(a, b, c, pt->array[i].center);
    // c = v2_sub(pt->array[i].position, pt->array[i].offset);
    // DrawTriangle(b, c, a, pt->array[i].center);
    // c = v2_add(pt->array[i + 1].position, pt->array[i + 1].offset);
    // DrawTriangle(c, b, a, pt->array[i].center);
    // c = v2_sub(pt->array[i + 1].position, pt->array[i + 1].offset);
    // DrawTriangle(b, c, a, pt->array[i].center);
    DrawLineEx(pt->array[i].position, pt->array[i + 1].position, 2, BLACK);
    DrawCircleV(pt->array[i].position, 3.0, BLACK);
  }
}
void step_particle(Particle *restrict a) {
  Vector2 delta = (Vector2){0.0, -1.0};
  delta = v2_rotate(delta,
                    degtorad(random_float_interval(-angle_range, angle_range)));
  a->position =
      v2_add(a->position,
             v2_scale(delta, random_float_interval(max_step_magnitude * 0.25f,
                                                   max_step_magnitude)));
  a->offset = v2_scale(
      a->offset, random_float_interval(min_step_reduction, max_step_reduction));
}
void simulate_trail(ParticleTrail *restrict pt) {
  Particle root = pt->array[0];
  for (int i = 1; i < pt->count; i++) {
    step_particle(&root);
    pt->array[i] = root;
  }
  pt->array[pt->count - 1].offset =
      v2_scale(pt->array[pt->count - 1].offset, 0.25);
}