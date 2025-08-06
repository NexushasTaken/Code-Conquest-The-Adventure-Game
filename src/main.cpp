#include "raygui.h"
#include "raylib.h"
#include <curl/curl.h>
#include <string>

#define SOL_BUILD_CXX_MODE 1
#define SOL_ALL_SAFETIES_ON 1
#include "sol/sol.hpp"
#include "lua.hpp"

// #define RAYGUI_IMPLEMENTATION
// #include "raygui.h"

int main() {
#ifdef PLATFORM_DESKTOP
  int SCREEN_WIDTH = 600;
  int SCREEN_HEIGHT = 800;
  InitWindow(SCREEN_HEIGHT, SCREEN_WIDTH, "Hello, World");
#else
  InitWindow(0, 0, "Hello, World");
  int SCREEN_WIDTH = GetScreenWidth();
  int SCREEN_HEIGHT = GetScreenHeight();
#endif

  sol::state lua;

  lua.set_function("print", [] { printf("Hello, World\n"); });
  auto state = lua.script("print()");

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    EndDrawing();
  }
  CloseWindow();
}
