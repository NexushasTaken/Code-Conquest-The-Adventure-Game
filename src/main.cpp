//#include "raylib.h"
//#include <future>
//
//int computeSomething(int x) {
//  return 0;
//}
//
//int main() {
//  InitWindow(800, 600, "DVD Bounce");
//  SetTargetFPS(60);
//
//  std::future<int> result = std::async(std::launch::async, computeSomething, 21);
//
//  Vector2 pos = { 200, 150 };
//  Vector2 speed = { 4, 3 };
//  int logoW = 100, logoH = 60;
//
//  while (!WindowShouldClose()) {
//    pos.x += speed.x;
//    pos.y += speed.y;
//
//    if (pos.x <= 0 || pos.x + logoW >= GetScreenWidth()) speed.x *= -1;
//    if (pos.y <= 0 || pos.y + logoH >= GetScreenHeight()) speed.y *= -1;
//
//    BeginDrawing();
//    ClearBackground(BLACK);
//    DrawRectangle(pos.x, pos.y, logoW, logoH, RED);
//    DrawText("DVD", pos.x + 20, pos.y + 20, 20, WHITE);
//    EndDrawing();
//  }
//
//  CloseWindow();
//  return 0;
//}

#include <future>
#include <iostream>
#include <chrono>
#include <thread>

// A function that simulates work
int computeSomething(int x) {
    std::this_thread::sleep_for(std::chrono::seconds(3)); // simulate long task
    return x * 2;
}

int main() {
    // Launch computeSomething asynchronously
    std::future<int> result = std::async(std::launch::async, computeSomething, 21);

    // While waiting, do something else
    while (result.wait_for(std::chrono::milliseconds(0)) != std::future_status::ready) {
        std::cout << "Task still running...\n";
    }

    // When ready, get() retrieves the value (blocks if still running, but now it's done)
    int value = result.get();
    std::cout << "Task finished! Result = " << value << "\n";
}

