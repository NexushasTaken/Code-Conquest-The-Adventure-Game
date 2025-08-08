#include "raygui.h"
#include "raylib.h"
#include <cstdlib>
#include <curl/curl.h>
#include <curl/easy.h>
#include <curl/multi.h>
#include <iomanip>
#include <iostream>
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

#ifdef PLATFORM_ANDROID
const SCREEN_WIDTH = 0;
const SCREEN_HEIGHT = 0;
#else
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
#endif

using json = nlohmann::json;

namespace supabase {
struct HttpClient {
  HttpClient() { multi_handle = curl_multi_init(); }
  ~HttpClient() { curl_multi_cleanup(multi_handle); }

  using read_callback = size_t(char *ptr, size_t size, size_t nitems,
                               void *userdata);

  void update() {
    int remaining_handles = 0;
    CURLMcode res = curl_multi_perform(multi_handle, &remaining_handles);
    if (res != CURLM_OK) {
      TraceLog(LOG_WARNING, "curl_multi_perform: %s", curl_multi_strerror(res));
    }

    if (remaining_handles == 0) {
      return;
    }

    int msgq = 0;
    while (CURLMsg *msg = curl_multi_info_read(multi_handle, &msgq)) {
      char *effective_url = 0;
      CURLcode res = curl_easy_getinfo(msg->easy_handle, CURLINFO_EFFECTIVE_URL,
                                       &effective_url);
      if (res == CURLE_OK) {
        TraceLog(LOG_INFO, "EFFECTIVE_URL", effective_url);
      }

      if (msg->msg != CURLMSG_DONE) {
        continue;
      }

      if (msg->data.result != CURLE_OK) {
        TraceLog(LOG_WARNING, "curl_multi_info_read: %s",
                 curl_easy_strerror(msg->data.result));
      }

      auto code = curl_multi_remove_handle(multi_handle, msg->easy_handle);
      if (code != CURLM_OK) {
        TraceLog(LOG_WARNING, "curl_multi_remove_handle: %s",
                 curl_multi_strerror(code));
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
  Auth(HttpClient *http, std::string auth_url, curl_slist *headers)
      : http(http), headers(headers) {
    endpoint = auth_url;
    signup_endpoint = auth_url + "/signup";

    curl = curl_easy_init();
    if (!curl) {
      TraceLog(LOG_FATAL, "curl_easy_init: Failed to initialize easy_handle");
    }
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
  }
  ~Auth() { curl_easy_cleanup(curl); }

  void sign_in_anonymously() {
    // Reset the CURL handle to avoid stale state
    //if (curl) {
    //  curl_easy_cleanup(curl);
    //}
    //curl = curl_easy_init();
    //if (!curl) {
    //  TraceLog(LOG_ERROR, "curl_easy_init: Failed to initialize easy_handle");
    //  return;
    //}
    auto cb = [](char *ptr, size_t size, size_t nitems, void *userdata) {
      Auth *auth = reinterpret_cast<Auth *>(userdata);
      auth->response.append(ptr, size * nitems);
      std::cout << std::quoted(auth->response) << std::endl;
      return size * nitems;
    };
    curl_easy_reset(curl);
    http->add_handle(curl, cb);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_URL, signup_endpoint.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "{}");
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE_LARGE, 2L);
  }

  bool check_auth() { return false; }

private:
  std::string endpoint;
  std::string signup_endpoint;

  std::string response;
  CURL *curl;

  std::string empty_data = "{}";

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

  void poll() { http->update(); }

private:
  std::string api_url{};
  std::string api_key{};
  std::string auth_url{};

  curl_slist *headers{};
  std::unique_ptr<HttpClient> http{};
};
} // namespace supabase

using supabase::Client;

struct GuiContext {
  nk_context *ctx;

  char mail_buffer[256] = {0};
  bool mail_focus = false;
  char pass_buffer[256] = {0};
  bool pass_focus = false;
  bool secret = true;

  std::string label;
};

void draw_ui(GuiContext &ctx, Client &client) {
  if (nk_begin(ctx.ctx, "Choose how you want to Sign in",
               nk_rect(SCREEN_WIDTH / 2.0 - SCREEN_WIDTH / 2.0 / 2.0,
                       SCREEN_HEIGHT / 2.0 - SCREEN_HEIGHT / 2.0 / 2.0,
                       SCREEN_WIDTH / 2.0, SCREEN_HEIGHT / 2.0),
               NK_WINDOW_BORDER | NK_WINDOW_SCALABLE |
                   NK_WINDOW_NO_SCROLLBAR)) {
    nk_layout_row_dynamic(ctx.ctx, 0, 1);

    if (nk_button_label(ctx.ctx, "Sign in")) {
      ctx.label = "Sign in was clicked!";
    }

    if (nk_button_label(ctx.ctx, "Sign up")) {
      ctx.label = "Sign up was clicked!";
    }

    if (nk_button_label(ctx.ctx, "Sign in as Anonymous")) {
      client.auth.sign_in_anonymously();
      ctx.label = "Anonymous was clicked!";
    }

    nk_label(ctx.ctx, ctx.label.c_str(), NK_TEXT_CENTERED);
  }
  nk_end(ctx.ctx);
}

int main() {
#ifdef PLATFORM_DESKTOP
  ChangeDirectory("assets");
#endif
  TraceLog(LOG_INFO, "%s", curl_version());

  std::string api_key = SUPABASE_KEY;
  std::string api_url = SUPABASE_URL;

  if (api_key.empty()) {
    TraceLog(LOG_FATAL, "%s", "supabaseKey is required");
  }
  if (api_url.empty()) {
    TraceLog(LOG_FATAL, "%s", "supabaseUrl is required");
  }
  curl_global_init(CURL_GLOBAL_DEFAULT);
  Client client(api_url, api_key);

  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Hello, World");

  GuiContext ctx;
  ctx.ctx = InitNuklear(18);

  SetTargetFPS(60);

  std::string label = "Waiting...";

  while (!WindowShouldClose()) {
    client.poll();
    UpdateNuklear(ctx.ctx);

    BeginDrawing();
    ClearBackground(RAYWHITE);

    draw_ui(ctx, client);

    DrawNuklear(ctx.ctx);
    EndDrawing();
  }

  UnloadNuklear(ctx.ctx);

  CloseWindow();

  curl_global_cleanup();
  return 0;
}
