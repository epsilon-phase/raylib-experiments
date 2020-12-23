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
#include <string.h>
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
  Vector2 a, b, c;
  float thickness;
} Bezier;
typedef struct {
  Vector2 center;
  float radius;
  float rotation;
  int sides;
} Polygon;
typedef struct {
  Vector2 *vertices;
  size_t count;
  int segments;
  float thickness;
} PolyBezier;
typedef struct {
  enum { LINE, POLYLINE, BEZIER, POLYBEZIER, POLYGON } type;
  union {
    Line l;
    PolyLine pl;
    Bezier bez;
    Polygon poly;
    PolyBezier pb;
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
inputState draw_drawEntry_handle_key(drawEntry *restrict de, int key);
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
    if (IsKeyPressed(KEY_BACKSPACE)) {
      drawEntry_reset(&de);
    }
    if (IsKeyPressed(KEY_B)) {
      de.type = BEZIER;
      drawEntry_reset(&de);
    }
    if (IsKeyPressed(KEY_L)) {
      de.type = LINE;
      drawEntry_reset(&de);
    }
    if(IsKeyPressed(KEY_R))
      v.count=0;
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
void draw_quadratic_bezier(Vector2 a, Vector2 b, Vector2 c, float thickness,
                           Color col, int segments) {
  Vector2 output[30];
  v2_bezier3(a, b, c, output, segments);
  for (int i = 0; i < segments - 1; i++) {
    DrawLineEx(output[i], output[i + 1], thickness, col);
  }
  
}
void draw_drawEntry_Bezier(const drawEntry *restrict de) {
  /*-------
   * States:
   * 0. No drawing
   * 1. No Drawing
   * 2. Drawing to mouse
   * 3. Drawing thickness
   * 4. Drawing
   *-------*/
  switch (de->stage) {
  case 0:
    return;
  case 1:
    DrawCircleV(de->bez.a, 3, BLACK);
    break;
  case 2:
    draw_quadratic_bezier(de->bez.a, de->bez.b, GetMousePosition(), 1, de->col,
                          30);
    break;
  case 3:
    draw_quadratic_bezier(de->bez.a, de->bez.b, de->bez.c,
                          fmax(distance(de->bez.c, GetMousePosition()), 1.0),
                          de->col, 30);
    break;

  case 4:
    default:
    draw_quadratic_bezier(de->bez.a, de->bez.b, de->bez.c, de->bez.thickness,
                          de->col, 30);
  }
}
void draw_drawEntry_Polyline(const drawEntry *restrict de) {
  for (Vector2 *a = de->pl.dots, *b = de->pl.dots + 1;
       b < de->pl.dots + de->pl.count; a++, b++) {
    DrawLineEx(*a, *b, de->pl.thickness, de->col);
  }
}

void draw_drawEntry_PolyBezier(const drawEntry *restrict de) {
  /*----
   *Stages:
   * 0. Uninitialized
   * 1. First Point set
   * 2. Second Point set,
   * 3. Third Point set
   * 4. Continue adding points
   * 5. Done
   *-----*/
  switch (de->stage) {
  case 2:
    draw_quadratic_bezier(de->pb.vertices[0], de->pb.vertices[1],
                          GetMousePosition(), de->pb.thickness, de->col, 20);
    break;
  case 3:
    draw_quadratic_bezier(de->pb.vertices[0], de->pb.vertices[1],
                          de->pb.vertices[2], de->pb.thickness, de->col, 20);
    break;
  case 4:
    break;
  default:
    break;
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
    // DrawLineBezier(de->bez.a, de->bez.b, de->bez.thickness, de->col);
    draw_drawEntry_Bezier(de);
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
inputState bezier_stage(drawEntry *restrict de, Vector2 input) {
  switch (de->stage) {
  case 0:
    de->bez.a = input;
    break;
  case 1:
    de->bez.b = input;
    break;
  case 2:
    de->bez.c = input;
    break;
  case 3:
    de->bez.thickness = fmax(distance(de->bez.c, input), 1);
    break;
  }
  de->stage++;
  return de->stage == 4 ? INPUT_FINISHED : INPUT_ONGOING;
}
inputState drawEntry_input(drawEntry *restrict de, Vector2 pos) {
  switch (de->type) {
  case LINE:
    return line_stage(de, pos);
    break;
  case BEZIER:
    return bezier_stage(de, pos);
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
void drawEntry_reset_bezier(drawEntry *restrict de) { de->bez.thickness = 1; }
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
  if (de->type == POLYLINE)
    memset(&de->pl, 0, sizeof(PolyLine));
}