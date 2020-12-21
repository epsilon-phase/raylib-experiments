/**
 * Things we want to do here:
 *
 * 1. Make polyline possible
 * 2. Make polygon possible
 * 3. Make Bezier possible
 * 4. Color selection
 * 5. Thiccness selection
 * 6. Raster Output
 * 7. Undo
 * 8. Erase
 * 9. SVG output(Should be *kinda* easy)
 * */
#include "utility_math.h"
#include <raylib.h>
#include <stdlib.h>
typedef struct {
  Vector2 a, b;
  float thickness;
} Line;
typedef struct {
  Vector2 *dots;
  size_t count;
  float thickness;
} PolyLine;
typedef struct {
  Vector2 a, b;
  float thickness;
} Bezier;
typedef struct {
  Vector2 center;
  float radius;
  float rotation;
  int sides;
} Polygon;
typedef struct {
  enum { LINE, POLYLINE, BEZIER, POLYGON } type;
  union {
    Line l;
    PolyLine pl;
    Bezier bez;
    Polygon poly;
  };
  Color col;
  int stage;
} drawEntry;
struct Vectors {
  drawEntry *stuff;
  size_t count;
  size_t capacity;
};

typedef enum { INPUT_ONGOING, INPUT_FINISHED } inputState;
inputState drawEntry_input(drawEntry *restrict de, Vector2 pos);
void drawEntry_reset(drawEntry *restrict de);
void drawEntry_motion(drawEntry *restrict de, Vector2 pos);
void draw_drawEntry(const drawEntry *restrict de);
void drawVectors(const struct Vectors *restrict v);
void Vectors_add_drawEntry(struct Vectors *restrict v, drawEntry *restrict de);
int main() {
  // Initialization
  //--------------------------------------------------------------------------------------
  const int screenWidth = 800;
  const int screenHeight = 450;

  InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");

  // TODO: Load resources / Initialize variables at this point

  SetTargetFPS(60);
  //--------------------------------------------------------------------------------------
  drawEntry de;
  de.type = LINE;
  de.col = BLACK;
  de.stage = 0;
  de.l.thickness = 1;
  struct Vectors v;
  v.capacity = 10;
  v.stuff = calloc(v.capacity, sizeof(drawEntry));
  v.count = 0;
  // Main game loop
  while (!WindowShouldClose()) // Detect window close button or ESC key
  {
    // Update
    //----------------------------------------------------------------------------------
    // TODO: Update variables / Implement example logic at this point
    //----------------------------------------------------------------------------------

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
      if (drawEntry_input(&de, GetMousePosition()) == INPUT_FINISHED) {
        Vectors_add_drawEntry(&v, &de);
        drawEntry_reset(&de);
      }

    } else {
      drawEntry_motion(&de, GetMousePosition());
    }

    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();

    ClearBackground(RAYWHITE);

    // TODO: Draw everything that requires to be drawn at this point:

    // DrawText("Congrats! You created your first window!", 190, 200, 20,
    // LIGHTGRAY); // Example
    drawVectors(&v);
    draw_drawEntry(&de);

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

void draw_drawEntry_Line(const drawEntry *restrict de) {
  switch (de->stage) {
  case 0:
    return;
  case 1:
    DrawLineEx(de->l.a, GetMousePosition(), de->l.thickness, de->col);
    break;
  case 2: {
    Vector2 cursor = GetMousePosition();
    float dist = fmax(distance(de->l.b, cursor), 1);

    DrawLineEx(de->l.a, de->l.b, dist, de->col);
    printf("end(%f,%f), cursor (%f,%f) distance %f\n", de->l.b.x, de->l.b.y,
           cursor.x, cursor.y, dist);
  } break;
  case 3:
    DrawLineEx(de->l.a, de->l.b, de->l.thickness, de->col);
  }
}
void draw_drawEntry_Polyline(const drawEntry *restrict de) {
  for (Vector2 *a = de->pl.dots, *b = de->pl.dots + 1;
       b < de->pl.dots + de->pl.count; a++, b++) {
    DrawLineEx(*a, *b, de->pl.thickness, de->col);
  }
}
void draw_drawEntry(const drawEntry *restrict de) {
  switch (de->type) {
  case LINE:
    draw_drawEntry_Line(de);
    break;
  case POLYLINE:
    draw_drawEntry_Polyline(de);
    break;
  case BEZIER:
    DrawLineBezier(de->bez.a, de->bez.b, de->bez.thickness, de->col);
    break;
  case POLYGON:
    DrawPoly(de->poly.center, de->poly.sides, de->poly.radius,
             de->poly.rotation, de->col);
    break;
  }
}
inputState line_stage(drawEntry *restrict de, Vector2 input) {
  switch (de->stage) {
  case 0:
    de->l.a = input;
    break;
  case 1:
    de->l.b = input;
    break;
  case 2:
    de->l.thickness = fmax(distance(de->l.b, input), 1);
    printf("thickness %f\n", de->l.thickness);
    break;
  }
  de->stage = (de->stage + 1);
  return de->stage == 3 ? INPUT_FINISHED : INPUT_ONGOING;
}
inputState drawEntry_input(drawEntry *restrict de, Vector2 pos) {
  switch (de->type) {
  case LINE:
    return line_stage(de, pos);
    break;
  default:
    abort();
  }
}
void line_motion(drawEntry *restrict de, Vector2 pos) { de->l.b = pos; }
void drawEntry_motion(drawEntry *restrict de, Vector2 pos) {
  switch (de->type) {
  case LINE:
    // line_motion(de, pos);
    break;
  }
}
void drawEntry_reset_line(drawEntry *restrict de) { de->l.thickness = 1; }
void drawEntry_reset(drawEntry *restrict de) {
  de->stage = 0;
  switch (de->type) {
  case LINE:
    drawEntry_reset_line(de);
    break;
  default:
    break;
  }
}
void drawVectors(const struct Vectors *restrict v) {
  for (const drawEntry *de = v->stuff; de < v->stuff + v->count; de++)
    draw_drawEntry(de);
}

void Vectors_add_drawEntry(struct Vectors *restrict v, drawEntry *restrict de) {
  if (v->capacity == v->count) {
    v->stuff = reallocarray(v->stuff, v->capacity * 2, sizeof(drawEntry));
    v->capacity *= 2;
  }
  v->stuff[v->count] = *de;
  v->count++;
}