#include "raylib.h"
#include "utility_math.h"
#include <stdio.h>
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
  CENTERWISE_JITTER_2,
  CENTERWISE_JITTER_3,
  CENTERWISE_JITTER_4,
  WALKING_GUARD
} walking = CENTERWISE_JITTER;
static inline void cycle_walking_strategy() {
  walking = (walking + 1) % WALKING_GUARD;
}
static inline const char *get_walking_name() {
  switch (walking) {
  case RANDOMWALK:
    return "Random walk";
  case CENTERWISE_JITTER:

    return "Center nudges";
  case CENTERWISE_JITTER_2:
    return "Center nudges(x2)";
  case CENTERWISE_JITTER_3:
    return "Center nudges(x4)";
  case CENTERWISE_JITTER_4:
    return "Center nudges(x8)";
  default:
    return "FAILURE";
  }
}
static enum {
  FOUR_NEIGHBORHOOD,
  FOUR_DIAGONAL_NEIGHBORHOOD,
  EIGHT_NEIGHBORHOOD,
  NEIGHBOR_GUARD
} neighbor_check = FOUR_NEIGHBORHOOD;
static inline void cycle_neighborhood_check() {
  neighbor_check = (neighbor_check + 1) % NEIGHBOR_GUARD;
}
static inline const char *get_neighbor_check_name() {
  switch (neighbor_check) {
  case FOUR_NEIGHBORHOOD:
    return "4-cardinal";
  case FOUR_DIAGONAL_NEIGHBORHOOD:
    return "4-diagonal";
  case EIGHT_NEIGHBORHOOD:
    return "8-neighborhood";
  default:
    return "FAILURE";
  }
}

static const size_t PARTICLE_COUNT = 10000;
static const int MAX_AGE = 2000;
static const Color colors[] = {BLACK, RED, BLUE, GREEN, PURPLE, DARKGREEN};
static const int color_c = sizeof(colors) / sizeof(Color);
static const int save_frame_min_change = 5;
static bool do_diagonal_movement = false;
static const int max_steps_before_draw = 3;
static bool save_frames = false;

// static RectI stable_bounding_box;
inline Particle init_particle();
inline void init_particles(Particle *restrict p, int count);
bool particle_step(Particle *restrict p, const Particle *restrict others,
                   int count);
inline int particle_is_adjacent(const Particle *restrict a,
                                const Particle *restrict b);
inline void walk_particle(Particle *restrict a);
inline Color get_particle_color(const Particle *restrict p);
inline const char *format_status() {

  return TextFormat("Walking: %s(diagonal: %c)\n"
                    "Neighbor check: %s\n"
                    "FPS: %.1f\n"
                    "Saving frames? %c",
                    get_walking_name(), do_diagonal_movement ? 'Y' : 'N',
                    get_neighbor_check_name(), 1.0f / GetFrameTime(),
                    save_frames ? 'Y' : 'N');
}
int compare_particle(const Particle *restrict a, const Particle *restrict b) {
  return b->stable - a->stable;
}
int main(void) {
  // SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitWindow(425, 425, "Crystal");
  SetTargetFPS(60);
  Particle parts[PARTICLE_COUNT];
  int stable_particles = 0;
  bool draw_all = true, draw_text = true;

  int frames_dumped = 0;
  int images_dumped = 0;
  parts[0] =
      (Particle){GetScreenWidth() / 2, GetScreenHeight() / 2, 1, 0, BLACK, 0};
  init_particles(parts + 1, PARTICLE_COUNT - 1);
  Image thing;
  thing = GenImageColor(425, 425, RAYWHITE);
  int last_stable_particles = 0;
  while (!WindowShouldClose()) {
    bool dump_image = false;
    stable_particles = 0;
    // Step until either a particle stabilizes or the maximum number of steps is
    // reached
    for (int _ = 0, stop = 0; _ < max_steps_before_draw && !stop; _++) {
      for (int i = PARTICLE_COUNT - 1; i >= 0; i--) {
        if (parts[i].stable)
          break;
        int n = particle_step(parts + i, parts, PARTICLE_COUNT);
        stop |= n;
      }
      if (stop) {
        // Move all the stable particles to the front of the array
        qsort(parts, PARTICLE_COUNT, sizeof(Particle), &compare_particle);
      }
    }
    if (IsKeyPressed(KEY_A))
      draw_all = !draw_all;
    if (IsKeyPressed(KEY_T))
      draw_text = !draw_text;
    if (IsKeyPressed(KEY_R)) {
      parts[0].x = GetScreenWidth() / 2;
      parts[0].y = GetScreenHeight() / 2;
      init_particles(parts + 1, PARTICLE_COUNT - 1);
      ImageClearBackground(&thing, RAYWHITE);
      frames_dumped = 0;
      printf("RESET\n");
    }
    if (IsKeyPressed(KEY_D)) {
      do_diagonal_movement = !do_diagonal_movement;
    }
    if (IsKeyPressed(KEY_W)) {
      cycle_walking_strategy();
    }
    if (IsKeyPressed(KEY_N)) {
      cycle_neighborhood_check();
    }
    if (IsKeyPressed(KEY_F)) {
      save_frames = !save_frames;
    }
    if (IsKeyPressed(KEY_O)) {
      dump_image = true;
    }
    BeginDrawing();
    ClearBackground(RAYWHITE);

    for (int i = 0; i < PARTICLE_COUNT; i++) {
      stable_particles += parts[i].stable;
      if (draw_all || parts[i].stable)
        DrawPixel(parts[i].x, parts[i].y, parts[i].col);
      if (parts[i].stable) {
        ImageDrawPixel(&thing, parts[i].x, parts[i].y, parts[i].col);
      }
    }
    if (draw_text && stable_particles != PARTICLE_COUNT) {
      DrawText(TextFormat("Stable Particles %i", stable_particles), 100,
               GetScreenHeight() / 8, 16, BLACK);
      DrawText(format_status(), 100, GetScreenHeight() / 8 + 16, 16, BLACK);
    }
    EndDrawing();
    if (dump_image) {
      dump_image = false;
      TakeScreenshot(TextFormat("Crystal-%03f.png", images_dumped++));
    }
    if (stable_particles - last_stable_particles > save_frame_min_change &&
        save_frames) {
      ExportImage(thing, TextFormat("Frame-%05i.png", frames_dumped++));
      last_stable_particles = stable_particles;
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
  switch (neighbor_check) {
  case FOUR_NEIGHBORHOOD:
    return 1 == (abs(a->x - b->x) + abs(a->y - b->y));
  case FOUR_DIAGONAL_NEIGHBORHOOD:
    return 1 == abs(a->x - b->x) && 1 == abs(a->y - b->y);
  case EIGHT_NEIGHBORHOOD:
    return 1 >= abs(a->x - b->x) && 1 >= abs(a->y - b->y);
  case NEIGHBOR_GUARD:
    fprintf(stderr,
            "NEIGHBOR_GUARD check reached. This should not be permitted\n");
    abort();
  }
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
static inline void random_walk(Particle *restrict p, bool diagonals) {
  switch (random() % (diagonals ? 3 : 2)) {
  case 0:
    p->x += random() % 3 - 1;
    break;
  case 1:
    p->y += random() % 3 - 1;
    break;
  case 2:
    p->x += random() % 3 - 1;
    p->y += random() % 3 - 1;
    break;
  }
}
/// Inverse Chance of getting nudged towards the center, either on x or y
/// plane
static const int center_nudge_chance[] = {60, 30, 15, 7};
static inline void centerwise_jitter_walk(Particle *restrict p,
                                          int centerwise_nudge) {

  if (random() % centerwise_nudge == 1)
    switch (random() % (do_diagonal_movement ? 3 : 2)) {
    case 0:
      p->x += p->x > GetScreenWidth() / 2 ? -1 : 1;
      break;
    case 1:
      p->y += p->y > GetScreenHeight() / 2 ? -1 : 1;
      break;
    case 2:
      p->x += p->x > GetScreenWidth() / 2 ? -1 : 1;
      p->y += p->y > GetScreenHeight() / 2 ? -1 : 1;
      break;
    }
  else
    random_walk(p, do_diagonal_movement);
}
inline void walk_particle(Particle *restrict p) {
  switch (walking) {
  case RANDOMWALK:
    random_walk(p, do_diagonal_movement);
    break;
  case CENTERWISE_JITTER:
  case CENTERWISE_JITTER_2:
  case CENTERWISE_JITTER_3:
  case CENTERWISE_JITTER_4:
    centerwise_jitter_walk(p, center_nudge_chance[walking - CENTERWISE_JITTER]);
    break;
  case WALKING_GUARD:
    abort();
    break;
  }
}
static inline Color color_by_cycle(const Particle *restrict p) {
  return interp_color(colors[(p->cycle) % color_c],
                      colors[(p->cycle + 1) % color_c],
                      p->age / (float)MAX_AGE);
}
inline Color get_particle_color(const Particle *restrict p) {
  Color c;
  c = interp_color(colors[(p->cycle) % color_c],
                   colors[(p->cycle + 1) % color_c], p->age / (float)MAX_AGE);
  return c;
}
bool particle_step(Particle *restrict p, const Particle *restrict others,
                   int count) {
  bool collision = false;
  if (p->stable)
    return false;

  for (int i = 0; i < count; i++) {
    if (!others[i].stable)
      break;
    if (others[i].x == p->x && others[i].y == p->y) {
      collision = true;
      break;
    }
    if (particle_is_adjacent(p, others + i)) {
      p->stable = 1;
      p->col = get_particle_color(p);
      break;
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
  return p->stable;
}