#include "raylib.h"
#include <atomic>
#include <chrono>
#include <cstdio>
#include <curl/curl.h>
#include <mutex>
#include <string>
#include <thread>

#include "raylib.h"
#include <cstdio>

const char *WriteCACertToTemp() {
  int size = 0;
  unsigned char *data = LoadFileData("cacert.pem", &size);
  if (!data)
    return nullptr;

  // Write to a known writable path (temporary path on Android)
  const char *tempPath =
      "/data/data/com.raylib.game/cache/cacert.pem"; // Replace with your real
                                                     // package name

  FILE *f = fopen(tempPath, "wb");
  if (!f) {
    UnloadFileData(data);
    return nullptr;
  }

  fwrite(data, 1, size, f);
  fclose(f);
  UnloadFileData(data);
  return tempPath;
}

std::string CurlHTTPSCheck(int &outCode) {
  CURL *curl = curl_easy_init();
  if (!curl) {
    outCode = -1;
    return "Init failed";
  }

  curl_easy_setopt(curl, CURLOPT_URL, "https://example.com");
  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);
  curl_easy_setopt(curl, CURLOPT_CAINFO, WriteCACertToTemp());

  CURLcode res = curl_easy_perform(curl);
  curl_easy_cleanup(curl);

  outCode = res;
  if (res != CURLE_OK) {
    char buf[128];
    snprintf(buf, sizeof(buf), "Error %d: %s", res, curl_easy_strerror(res));
    return std::string(buf);
  }

  return "HTTPS OK";
}

int main() {
  InitWindow(0, 0, "libcurl HTTPS Test (non-blocking)");
  SetTargetFPS(60);

  std::string result = "Checking HTTPS...";
  std::mutex resultMutex;
  std::atomic<bool> running(true);

  std::thread curlThread([&]() {
    while (running) {
      int code = 0;
      std::string msg = CurlHTTPSCheck(code);

      {
        std::lock_guard<std::mutex> lock(resultMutex);
        result = msg;
      }

      if (code == 0)
        break; // Success
      std::this_thread::sleep_for(std::chrono::seconds(3));
    }
  });

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    std::string toDraw;
    {
      std::lock_guard<std::mutex> lock(resultMutex);
      toDraw = result;
    }

    DrawText(toDraw.c_str(), 20, 90, 20, DARKGRAY);
    EndDrawing();
  }

  running = false;
  curlThread.join();
  CloseWindow();
  return 0;
}
