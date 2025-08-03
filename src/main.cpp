#include "raylib.h"

int main() {
  InitWindow(800, 600, "DVD Bounce");
  SetTargetFPS(60);

  Vector2 pos = { 200, 150 };
  Vector2 speed = { 4, 3 };
  int logoW = 100, logoH = 60;

  while (!WindowShouldClose()) {
    pos.x += speed.x;
    pos.y += speed.y;

    if (pos.x <= 0 || pos.x + logoW >= GetScreenWidth()) speed.x *= -1;
    if (pos.y <= 0 || pos.y + logoH >= GetScreenHeight()) speed.y *= -1;

    BeginDrawing();
    ClearBackground(BLACK);
    DrawRectangle(pos.x, pos.y, logoW, logoH, RED);
    DrawText("DVD", pos.x + 20, pos.y + 20, 20, WHITE);
    EndDrawing();
  }

  CloseWindow();
  return 0;
}

