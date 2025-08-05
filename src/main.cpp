#include <curl/curl.h>
#include <raylib.h>
#include <string>

std::string response;
bool requestDone = false;
CURLM *multi_handle = nullptr;
CURL *easy_handle = nullptr;

size_t WriteCallback(char *ptr, size_t size, size_t nmemb, void *userdata) {
  size_t total = size * nmemb;
  response.append(ptr, total);
  return total;
}

void SetupCurlMulti() {
  curl_global_init(CURL_GLOBAL_ALL);
  easy_handle = curl_easy_init();
  multi_handle = curl_multi_init();

  curl_easy_setopt(easy_handle, CURLOPT_URL, "https://httpbin.org/get");
  curl_easy_setopt(easy_handle, CURLOPT_WRITEFUNCTION, WriteCallback);
  curl_easy_setopt(easy_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
  curl_easy_setopt(easy_handle, CURLOPT_FOLLOWLOCATION, 1L);
  curl_easy_setopt(easy_handle, CURLOPT_SSL_VERIFYPEER, 0L);
  curl_easy_setopt(easy_handle, CURLOPT_SSL_VERIFYHOST, 0L);
  curl_easy_setopt(easy_handle, CURLOPT_TIMEOUT_MS, 5000L);

  curl_multi_add_handle(multi_handle, easy_handle);
}

void CleanupCurlMulti() {
  curl_multi_remove_handle(multi_handle, easy_handle);
  curl_easy_cleanup(easy_handle);
  curl_multi_cleanup(multi_handle);
  curl_global_cleanup();
}

void UpdateCurlMulti() {
  int still_running = 0;
  curl_multi_perform(multi_handle, &still_running);

  if (still_running == 0 && !requestDone) {
    CURLMsg *msg;
    int msgs_left;
    while ((msg = curl_multi_info_read(multi_handle, &msgs_left))) {
      if (msg->msg == CURLMSG_DONE) {
        requestDone = true;
        // Optional: Check result code with msg->data.result
      }
    }
  }
}

int main() {
  InitWindow(800, 600, "libcurl multi interface + raylib (no threads)");
  SetTargetFPS(60);

  SetupCurlMulti();

  while (!WindowShouldClose()) {
    UpdateCurlMulti();

    BeginDrawing();
    ClearBackground(RAYWHITE);

    if (requestDone) {
      DrawText("Request Done!", 20, 20, 20, DARKGREEN);
      DrawText(TextFormat("Response: %.80s", response.c_str()), 20, 60, 14,
               DARKGRAY);
    } else {
      DrawText("Fetching HTTPS (non-blocking, no threads)...", 20, 20, 20,
               DARKBLUE);
    }

    EndDrawing();
  }

  CleanupCurlMulti();
  CloseWindow();
  return 0;
}
