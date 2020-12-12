/**
 * With apologies to Packbats, her far finer program may be found here:
 * https://packbat.itch.io/rain-gif
 *
 * Which in turn was inspired by this: https://botwiki.org/bot/rain-gif/
 **/
#include "raylib.h"
#include "utility_math.h"
#include <time.h>
typedef struct {
  Vector2 start, pos, motion;
  float speed;
} raindrop;
static const float min_angle = -45, max_angle = 45;
static const float max_magnitude = 15.0f, min_magnitude = 5.0f;
static const float max_speed = 5.0f, min_speed = 3.0f;
static const Color raindrop_colors[] = {DARKBLUE, DARKGRAY, BLUE, BLACK};
static const Color background_colors[] = {RAYWHITE, LIGHTGRAY, GRAY,
                                          (Color){220, 220, 255}};
static const int max_vertical_spawn = 40;
static int background_color = 0;
static int raindrop_color = 0;
static const int raindrop_count = 40;
static void color_cycle(int dir) {
  raindrop_color =
      (raindrop_color + dir) % (sizeof(raindrop_colors) / sizeof(Color));
}
static void cycle_background(int dir) {
  background_color =
      (background_color + dir) % (sizeof(background_colors) / sizeof(Color));
}
inline void draw_raindrop(const raindrop *restrict rd);
inline bool outside_window(const raindrop *restrict rd);
void step_raindrop(raindrop *restrict rd);
raindrop init_raindrop();
int main() {
  const int screenWidth = 450;
  const int screenHeight = 450;

  InitWindow(screenWidth, screenHeight, "Rain");

  srandom(time(NULL));
  SetTargetFPS(60);
  raindrop rain[raindrop_count];
  for (int i = 0; i < raindrop_count; i++)
    rain[i] = init_raindrop();

  while (!WindowShouldClose()) {
    if (IsKeyPressed(KEY_RIGHT)) {
      color_cycle(1);
    }
    if (IsKeyPressed(KEY_LEFT)) {
      color_cycle(-1);
    }
    if (IsKeyPressed(KEY_B)) {
      cycle_background(1);
    }
    if (IsKeyPressed(KEY_R)) {
      for (int i = 0; i < raindrop_count; i++)
        rain[i] = init_raindrop();
    }
    for (int i = 0; i < raindrop_count; i++)
      step_raindrop(&rain[i]);

    BeginDrawing();

    ClearBackground(background_colors[background_color]);

    for (int i = 0; i < raindrop_count; i++)
      draw_raindrop(&rain[i]);
    EndDrawing();
  }

  // De-Initialization
  //--------------------------------------------------------------------------------------

  // TODO: Unload all loaded resources at this point

  CloseWindow(); // Close window and OpenGL context
  //--------------------------------------------------------------------------------------

  return 0;
}
inline void draw_raindrop(const raindrop *restrict rd) {
  DrawLineEx(rd->pos, v2_add(rd->pos, rd->motion), 2,
             raindrop_colors[raindrop_color]);
}
inline bool outside_window(const raindrop *restrict rd) {
  Vector2 start = rd->pos,
          end = v2_add(rd->pos, v2_scale(rd->motion, rd->speed));
  return (start.x < 0 && end.x < 0) ||
         (start.x > GetScreenWidth() && end.x > GetScreenWidth()) ||
         (start.y > GetScreenHeight() && end.y > GetScreenHeight());
}
void reset_raindrop(raindrop *restrict rd) { rd->pos = rd->start; }
raindrop init_raindrop() {
  raindrop rd;
  rd.start = (Vector2){random_float_interval(0, GetScreenWidth()),
                       0.0f - random_float_interval(0.0f, max_vertical_spawn)};
  rd.motion = (Vector2){0, random_float_interval(min_magnitude, max_magnitude)};
  rd.motion = v2_rotate(rd.motion,
                        degtorad(random_float_interval(min_angle, max_angle)));
  rd.speed = random_float_interval(min_speed, max_speed);
  rd.pos = rd.start;
  return rd;
}
void step_raindrop(raindrop *restrict rd) {
  if (outside_window(rd))
    reset_raindrop(rd);
  rd->pos = v2_add(rd->pos, v2_scale(v2_normalize(rd->motion), rd->speed));
}
