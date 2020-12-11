#include "raylib.h"
#include "utility_math.h"
typedef struct {
  int x, y;
  int stable;
  short cycle;
  Color col;
  int age;
} Particle;
typedef struct {
  int x1, y1, x2, y2;
} RectI;
static enum {
  RANDOMWALK,
  CENTERWISE_JITTER,
  WALKING_GUARD
} walking = CENTERWISE_JITTER;
static const size_t PARTICLE_COUNT = 6000;
static const int MAX_AGE = 2000;
static const Color colors[] = {BLACK, RED, BLUE, GREEN, PURPLE, DARKGREEN};
static const int color_c = sizeof(colors) / sizeof(Color);
// static RectI stable_bounding_box;
inline Particle init_particle();
inline void init_particles(Particle *restrict p, int count);
void particle_step(Particle *restrict p, const Particle *restrict others,
                   int count);
inline int particle_is_adjacent(const Particle *restrict a,
                                const Particle *restrict b);
inline void walk_particle(Particle *restrict a);
inline Color get_particle_color(const Particle *restrict p);
int main(void) {
  // SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitWindow(425, 425, "Crystal");
  // SetTargetFPS(1000);
  Particle parts[PARTICLE_COUNT];
  int stable_particles = 0;
  bool draw_all = true, draw_text = true;
  bool dumped_image = false;
  int images_dumped = 0;
  parts[0] =
      (Particle){GetScreenWidth() / 2, GetScreenHeight() / 2, 1, 0, BLACK, 0};
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
      init_particles(parts + 1, PARTICLE_COUNT - 1);
      printf("RESET\n");
    }
    if (IsKeyPressed(KEY_W)) {
      walking = (walking + 1) % WALKING_GUARD;
    }

    BeginDrawing();
    ClearBackground(RAYWHITE);

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
  p.cycle = 0;
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
static inline void move_to_border(Particle *restrict p) {
  int x, y;
  switch (random() % 4) {
    // The Vertical borders
  case 0:
    x = 0;
    y = random() % GetScreenHeight();
    break;
  case 3:
    x = GetScreenWidth();
    y = random() % GetScreenHeight();
    break;
  case 1:
    x = random() % GetScreenWidth();
    y = 0;
    break;
  case 2:
    y = GetScreenHeight();
    x = random() % GetScreenWidth();
    break;
  }
  p->x = x;
  p->y = y;
}
static inline void centerwise_jitter_walk(Particle *restrict p) {
  /// Inverse Chance of getting nudged towards the center, either on x or y
  /// plane
  const int centerwise_nudge_chance = 60;
  if (random() % 2) {
    if (random() % centerwise_nudge_chance == 1)
      p->x += p->x > GetScreenWidth() / 2 ? -1 : 1;
    else
      p->x += random() % 3 - 1;
  } else {
    if (random() % centerwise_nudge_chance == 1)
      p->y += p->y > GetScreenHeight() / 2 ? -1 : 1;
    else
      p->y += random() % 3 - 1;
  }
}
inline void walk_particle(Particle *restrict p) {
  switch (walking) {
  case RANDOMWALK:

    if (random() % 2) {
      p->x += random() % 3 - 1;
    } else {
      p->y += random() % 3 - 1;
    }
    break;
  case CENTERWISE_JITTER:
    centerwise_jitter_walk(p);
    break;
  case WALKING_GUARD:
    abort();
    break;
  }
}
inline Color get_particle_color(const Particle *restrict p) {
  Color c;
  c = interp_color(colors[(p->cycle) % color_c],
                   colors[(p->cycle + 1) % color_c], p->age / (float)MAX_AGE);
  return c;
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

    if (others[i].x == p->x && others[i].y == p->y) {
      collision = true;
      break;
    }
    if (particle_is_adjacent(p, others + i)) {
      p->stable = 1;
      p->col = get_particle_color(p);
    }
  }
  if (!p->stable) {
    walk_particle(p);
  }
  p->x = wrap_around(0, GetScreenWidth(), p->x);
  p->y = wrap_around(0, GetScreenHeight(), p->y);
  p->age++;
  if (collision) {
    move_to_border(p);
    p->stable = 0;
  }
  if (p->age > MAX_AGE) {
    p->cycle++;
    p->age = 0;
  }
}