#include "raylib.h"
#include "utility_math.h"
typedef struct {
  int x, y;
  int stable;
  Color col;
  int age;
} Particle;
typedef struct {
  int x1, y1, x2, y2;
} RectI;
static const size_t PARTICLE_COUNT = 3000;
static const int MAX_AGE = 1000;
// static RectI stable_bounding_box;
inline Particle init_particle();
inline void init_particles(Particle *restrict p, int count);
void particle_step(Particle *restrict p, const Particle *restrict others,
                   int count);
inline int particle_is_adjacent(const Particle *restrict a,
                                const Particle *restrict b);
int main(void) {
  // SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitWindow(425, 200, "Crystal");
  // SetTargetFPS(1000);
  Particle parts[PARTICLE_COUNT];
  int stable_particles = 0;
  bool draw_all = true, draw_text = true;
  bool dumped_image = false;
  int images_dumped = 0;
  parts[0] =
      (Particle){GetScreenWidth() / 2, GetScreenHeight() / 2, 1, BLACK, 0};
  init_particles(parts + 1, PARTICLE_COUNT - 1);
  while (!WindowShouldClose()) {
    stable_particles = 0;
#pragma omp parallel for
    for (int i = 0; i < PARTICLE_COUNT; i++) {
      particle_step(parts + i, parts, PARTICLE_COUNT);
    }
    if (IsKeyPressed(KEY_A))
      draw_all = !draw_all;
    if (IsKeyPressed(KEY_T))
      draw_text = !draw_text;
    if (IsKeyPressed(KEY_R)) {
      parts[0].x = GetScreenWidth() / 2;
      parts[0].y = GetScreenHeight() / 2;
      init_particle(parts + 1, PARTICLE_COUNT - 1);
    }

    BeginDrawing();
    ClearBackground(RAYWHITE);

    if (!(IsKeyDown(KEY_S)))
      for (int i = 0; i < PARTICLE_COUNT; i++) {
        stable_particles += parts[i].stable;
        if (draw_all || parts[i].stable)
          DrawPixel(parts[i].x, parts[i].y, parts[i].col);
      }
    if (draw_text && stable_particles != PARTICLE_COUNT)
      DrawText(TextFormat("Stable Particles %i", stable_particles), 100,
               GetScreenHeight() / 4, 16, BLACK);
    EndDrawing();
    if (PARTICLE_COUNT == stable_particles) {
      TakeScreenshot(TextFormat("Crystal-%03i.png", images_dumped++));
      init_particles(parts + 1, PARTICLE_COUNT - 1);
    }
  }
  CloseWindow();
  return 0;
}
inline Particle init_particle() {
  Particle p;
  p.x = random_interval(0, GetScreenWidth());
  p.y = random_interval(0, GetScreenHeight());
  p.stable = 0;
  p.col = BLACK;
  p.age = 0;
  return p;
}
inline void init_particles(Particle *restrict p, int count) {
  for (int i = 0; i < count; i++)
    p[i] = init_particle();
}
inline int particle_is_adjacent(const Particle *restrict a,
                                const Particle *restrict b) {
  return 1 == (abs(a->x - b->x) + abs(a->y - b->y));
}
void particle_step(Particle *restrict p, const Particle *restrict others,
                   int count) {
  bool collision = false;
  if (p->stable)
    return;
  for (int i = 0; i < count; i++) {
    if (others + i == p)
      continue;
    if (!others[i].stable)
      continue;
    if (particle_is_adjacent(p, others + i))
      p->stable = 1;
    if (others[i].x == p->x && others[i].y == p->y) {
      collision = true;
      break;
    }
  }
  if (!p->stable) {
    if (random() % 2) {
      p->x += random() % 3 - 1;
    } else {
      p->y += random() % 3 - 1;
    }
  }
  p->x = wrap_around(0, GetScreenWidth(), p->x);
  p->y = wrap_around(0, GetScreenHeight(), p->y);
  p->age++;
  if ((!collision && p->age > MAX_AGE + random() % 10) || collision)
    *p = init_particle();
}