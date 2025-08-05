#include "raylib.h"
#include "raygui.h"
#include <curl/curl.h>
#include <string>

// #define RAYGUI_IMPLEMENTATION
// #include "raygui.h"

#define CLAY_IMPLEMENTATION
#include "clay.h"
#include "./clay_renderer_raylib.c"

void HandleClayErrors(Clay_ErrorData errorData) {
  // See the Clay_ErrorData struct for more information
  printf("%s", errorData.errorText.chars);
  switch (errorData.errorType) {
    // etc
  }
}

int main() {
#ifdef PLATFORM_DESKTOP
  int SCREEN_WIDTH = 800;
  int SCREEN_HEIGHT = 600;
  InitWindow(SCREEN_HEIGHT, SCREEN_WIDTH, "Hello, World");
#else
  InitWindow(0, 0, "Hello, World");
  int SCREEN_WIDTH = GetScreenWidth();
  int SCREEN_HEIGHT = GetScreenHeight();
#endif

  auto font = GetFontDefault();

  auto min_mem_size = Clay_MinMemorySize();
  Clay_Arena arena =
      Clay_CreateArenaWithCapacityAndMemory(min_mem_size, malloc(min_mem_size));
  Clay_Initialize(arena, {SCREEN_WIDTH, SCREEN_HEIGHT}, (Clay_ErrorHandler){HandleClayErrors});
  Clay_SetMeasureTextFunction(Raylib_MeasureText, &font);

  while (!WindowShouldClose()) {
    Clay_SetLayoutDimensions({GetScreenWidth(), GetScreenHeight()});

    BeginDrawing();
    ClearBackground(RAYWHITE);

    Clay_BeginLayout();

    CLAY({
      .layout = {
        .sizing = {
          .width = CLAY_SIZING_GROW(0),
          .height = CLAY_SIZING_GROW(0),
        },
        .childAlignment = {
          .x = CLAY_ALIGN_X_CENTER,
          .y = CLAY_ALIGN_Y_CENTER,
        },
        .layoutDirection = CLAY_TOP_TO_BOTTOM,
      },
      .backgroundColor = {43, 41, 51, 255},
    }) {

      CLAY({
        .layout = {
          .sizing = {
            .width = CLAY_SIZING_FIT(0),
            .height = CLAY_SIZING_FIT(0),
          },
          .padding = {
            .left = 8,
            .right = 8,
            .top = 8,
            .bottom = 8,
          },
          .childGap = 16,
          .childAlignment = {
            .x = CLAY_ALIGN_X_CENTER,
            .y = CLAY_ALIGN_Y_TOP,
          },
        },
        .backgroundColor = {63, 61, 81, 255},
      }) {

        CLAY({
          .layout = {
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
          }
        }) {
          CLAY_TEXT(CLAY_STRING("Email"), CLAY_TEXT_CONFIG({.textColor = {200, 200, 200, 255}, .fontSize = 40, .letterSpacing = 4}));
          CLAY_TEXT(CLAY_STRING("Password"), CLAY_TEXT_CONFIG({.textColor = {200, 200, 200, 255}, .fontSize = 40, .letterSpacing = 4}));
        }
        CLAY({
          .layout = {
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
          }
        }) {
          CLAY_TEXT(CLAY_STRING("your-email"), CLAY_TEXT_CONFIG({.textColor = {200, 200, 200, 255}, .fontSize = 40, .letterSpacing = 4}));
          CLAY({
            .id = CLAY_ID("input/password"),
          }) {
            CLAY_TEXT(CLAY_STRING("your-password"), CLAY_TEXT_CONFIG({.textColor = {200, 200, 200, 255}, .fontSize = 40, .letterSpacing = 4}));
            static int index = 0;
            CLAY({
              .id = CLAY_ID("input/password/caret"),
              .layout = {
                .sizing = {
                  .width = CLAY_SIZING_FIXED(4),
                  .height = CLAY_SIZING_GROW(0),
                },
              },
              .backgroundColor = {10, 10, 10, 255},
              .floating = {
                .attachTo = CLAY_ATTACH_TO_PARENT,
              },
            }){}
          }
        }
      }
    }

    auto render_cmd_array = Clay_EndLayout();
    Clay_Raylib_Render(render_cmd_array, &font);

    EndDrawing();
  }
  CloseWindow();
}

