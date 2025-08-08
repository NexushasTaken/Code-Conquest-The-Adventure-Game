#include "raygui.h"
#include "raylib.h"
#include <cstdlib>
#include <curl/curl.h>
#include <curl/easy.h>
#include <curl/multi.h>
#include <memory>
#include <string>

#define SOL_BUILD_CXX_MODE 1
#define SOL_ALL_SAFETIES_ON 1
#include "lua.hpp"
#include "sol/sol.hpp"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#define RAYLIB_NUKLEAR_IMPLEMENTATION
#include "nuklear/raylib-nuklear.h"

#include "./utils.cpp"

#include "nlohmann/json.hpp"

using json = nlohmann::json;

namespace supabase {
struct HttpClient {
  HttpClient() { multi_handle = curl_multi_init(); }
  ~HttpClient() { curl_multi_cleanup(multi_handle); }

  void poll() {
    curl_multi_poll(multi_handle, NULL, 0, 0, NULL);
    CURLMsg *msg;
    int msgq;
    msg = curl_multi_info_read(multi_handle, &msgq);
    while (msgq > 0) {
      if (msg->msg == CURLMSG_DONE) {
        curl_multi_remove_handle(multi_handle, msg->easy_handle);
      }
    }
  }

  void add_handle(CURL *curl) {
    assert(curl);
    curl_multi_add_handle(multi_handle, curl);
  }

private:
  CURLM *multi_handle;
};

struct Auth {
  Auth() = default;
  Auth(HttpClient *http, std::string url, curl_slist *headers)
      : http(http), headers(headers) {
    endpoint = url;
    signup_endpoint = url + "/signup";

    curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_HEADER, headers);
  }

  void sign_in_anonymously() {
    http->add_handle(curl);
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "{}");
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, 2);
  }

private:
  std::string endpoint;
  std::string signup_endpoint;

  CURL *curl;

  HttpClient *http;
  curl_slist *headers;
};

struct Client {
  Client(std::string api_url, std::string api_key)
      : api_url(api_url), api_key(api_key) {
    headers = curl_slist_append(headers, "Accept: application/json");
    headers = curl_slist_append(headers,
                                "Content-Type: application/json;charset=UTF-8");
    headers = curl_slist_append(headers, ("apikey: " + api_key).c_str());

    auth_url = api_url + "/auth/v1";

    http = std::make_unique<HttpClient>();

    auth = Auth(http.get(), auth_url, headers);
  }

  ~Client() { curl_slist_free_all(headers); }

  Auth auth;

  void poll() { http->poll(); }

private:
  std::string api_url{};
  std::string api_key{};
  std::string auth_url{};

  curl_slist *headers{};
  std::unique_ptr<HttpClient> http{};
};
} // namespace supabase

using supabase::Client;

int main() {
#ifdef PLATFORM_DESKTOP
  ChangeDirectory("assets");
#endif

  char *json_contents = LoadFileText(".env.json");

  if (!json_contents) {
    TraceLog(LOG_FATAL, ".env.json not found!");
  }

  json env = json::parse(json_contents);
  if (!env.contains("supabaseKey")) {
    TraceLog(LOG_FATAL, "%s", "supabaseKey is required");
  }
  if (!env.contains("supabaseUrl")) {
    TraceLog(LOG_FATAL, "%s", "supabaseUrl is required");
  }
  Client client(env["supabaseKey"], env["supabaseUrl"]);

#ifdef PLATFORM_DESKTOP
  int SCREEN_WIDTH = 600;
  int SCREEN_HEIGHT = 800;
  InitWindow(SCREEN_HEIGHT, SCREEN_WIDTH, "Hello, World");
#else
  InitWindow(0, 0, "Hello, World");
  int SCREEN_WIDTH = GetScreenWidth();
  int SCREEN_HEIGHT = GetScreenHeight();
#endif
  struct nk_context *ctx = InitNuklear(10);

  SetTargetFPS(60);

  GuiSetStyle(DEFAULT, TEXT_SIZE, 20);

  char mail_buffer[256] = {0};
  bool mail_focus = false;
  char pass_buffer[256] = {0};
  bool pass_focus = false;
  bool secret = true;

  while (!WindowShouldClose()) {
    client.poll();
    UpdateNuklear(ctx);

    BeginDrawing();
    ClearBackground(RAYWHITE);

    // init gui state
    enum { EASY, HARD };
    static int op = EASY;
    static float value = 0.6f;
    static int i = 20;

    //nk_init_fixed(&ctx, calloc(1, MAX_MEMORY), MAX_MEMORY, &font);
    if (nk_begin(ctx, "Show", nk_rect(50, 50, 220, 220),
                 NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_CLOSABLE)) {
      // fixed widget pixel width
      nk_layout_row_static(ctx, 30, 80, 1);
      if (nk_button_label(ctx, "button")) {
        // event handling
      }

      // fixed widget window ratio width
      nk_layout_row_dynamic(ctx, 30, 2);
      if (nk_option_label(ctx, "easy", op == EASY))
        op = EASY;
      if (nk_option_label(ctx, "hard", op == HARD))
        op = HARD;

      // custom widget pixel width
      nk_layout_row_begin(ctx, NK_STATIC, 30, 2);
      {
        nk_layout_row_push(ctx, 50);
        nk_label(ctx, "Volume:", NK_TEXT_LEFT);
        nk_layout_row_push(ctx, 110);
        nk_slider_float(ctx, 0, &value, 1.0f, 0.1f);
      }
      nk_layout_row_end(ctx);
    }
    nk_end(ctx);

    DrawNuklear(ctx);
    EndDrawing();
  }

  UnloadNuklear(ctx);

  CloseWindow();

  curl_global_cleanup();
}
