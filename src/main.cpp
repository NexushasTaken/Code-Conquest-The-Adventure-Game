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

#include "nlohmann/json.hpp"

#include "env.h"

using json = nlohmann::json;

namespace supabase {
struct HttpClient {
  HttpClient() { multi_handle = curl_multi_init(); }
  ~HttpClient() { curl_multi_cleanup(multi_handle); }

  using read_callback = size_t(char *ptr, size_t size, size_t nitems,
                               void *userdata);

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

  void add_handle(CURL *curl, read_callback cb) {
    assert(curl);
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, cb);
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
    auto cb = [](char *ptr, size_t size, size_t nitems, void *userdata) {
      Auth *auth = reinterpret_cast<Auth *>(userdata);
      auth->response.append(ptr, size * nitems);
      std::cout << std::quoted(auth->response) << std::endl;
      return size * nitems;
    };
    http->add_handle(curl, cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "{}");
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, 2);
  }

private:
  std::string endpoint;
  std::string signup_endpoint;

  std::string response;
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

  std::string api_key = SUPABASE_KEY;
  std::string api_url = SUPABASE_URL;

  if (api_key.empty()) {
    TraceLog(LOG_FATAL, "%s", "supabaseKey is required");
  }
  if (api_url.empty()) {
    TraceLog(LOG_FATAL, "%s", "supabaseUrl is required");
  }
  Client client(api_key, api_url);

#ifdef PLATFORM_DESKTOP
  int SCREEN_WIDTH = 800;
  int SCREEN_HEIGHT = 600;
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Hello, World");
#else
  InitWindow(0, 0, "Hello, World");
  int SCREEN_WIDTH = GetScreenWidth();
  int SCREEN_HEIGHT = GetScreenHeight();
#endif
  struct nk_context *ctx = InitNuklear(40);

  SetTargetFPS(60);

  GuiSetStyle(DEFAULT, TEXT_SIZE, 40);

  char mail_buffer[256] = {0};
  bool mail_focus = false;
  char pass_buffer[256] = {0};
  bool pass_focus = false;
  bool secret = true;

  std::string label = "Waiting...";

  while (!WindowShouldClose()) {
    //client.poll();
    UpdateNuklear(ctx);

    BeginDrawing();
    ClearBackground(RAYWHITE);

    if (nk_begin(ctx, "Login as Anonymous", nk_rect(20, 20, SCREEN_WIDTH - 40, SCREEN_HEIGHT - 40),
                 NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_CLOSABLE | NK_WINDOW_SCALABLE | NK_WINDOW_NO_SCROLLBAR)) {
      nk_layout_row_dynamic(ctx, 0, 1);

      nk_label(ctx, "Login as Anonymous", NK_TEXT_CENTERED);

      nk_layout_row_dynamic(ctx, 0, 2);
      if (nk_button_label(ctx, "Load .env.json")) {
        //label = "Login was clicked!";
        label = LoadFileText("env.json");
      }
      if (nk_button_label(ctx, "Load hello_world.txt")) {
        //label = "Back was clicked!";
        label = LoadFileText("hello_world.txt");
      }

      nk_layout_row_dynamic(ctx, 0, 1);
      nk_label(ctx, label.c_str(), NK_TEXT_CENTERED);
      nk_label(ctx, GetApplicationDirectory(), NK_TEXT_CENTERED);

    }
    nk_end(ctx);

    DrawNuklear(ctx);
    EndDrawing();
  }

  UnloadNuklear(ctx);

  CloseWindow();

  curl_global_cleanup();
}
