#include "cpr/api.h"
#include "cpr/session.h"
#include "raygui.h"
#include "raylib.h"
#include <cpr/cpr.h>
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
const int SCREEN_WIDTH = 0;
const int SCREEN_HEIGHT = 0;
#else
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
#endif

using json = nlohmann::json;

namespace supabase {
struct Auth {
  Auth() = default;
  Auth(cpr::Url auth_url, cpr::Header headers) : headers(headers) {
    endpoint = auth_url;
    signup_endpoint = auth_url + "/signup";
    logout_endpoint = auth_url + "/logout";
  }

  void sign_in_anonymously() {
    cpr::Response response = cpr::Post(headers, signup_endpoint, empty_data);
    std::cout << "raw_header: " << response.raw_header << std::endl;
    std::cout << "text: " << response.text << std::endl;
    std::cout << "url: " << response.url << std::endl;
    std::cout << "reason: " << response.reason << std::endl;

    session = json::parse(response.text);
  }

  void sign_out() {
    auto jwt = session["access_token"];
    cpr::Response response = cpr::Post(headers, logout_endpoint, empty_data);
    std::cout << "raw_header: " << response.raw_header << std::endl;
    std::cout << "text: " << response.text << std::endl;
    std::cout << "url: " << response.url << std::endl;
    std::cout << "reason: " << response.reason << std::endl;

    session = json::parse(response.text);
  }

  bool check_auth() { return false; }

private:
  json session;

  cpr::Url endpoint;
  cpr::Url signup_endpoint;
  cpr::Url logout_endpoint;

  cpr::Response response;

  cpr::Body empty_data{"{}"};

  cpr::Header headers;
};

struct Client {
  Client(std::string api_url, std::string api_key)
      : api_url(api_url), api_key(api_key) {
    headers["accept"] = "application/json";
    headers["content-type"] = "application/json;charset=UTF-8";
    headers["apikey"] = api_key;

    auth_url = api_url + "/auth/v1";

    auth = Auth(auth_url, headers);
  }

  Auth auth;

private:
  cpr::Url api_url;
  cpr::Url api_key;
  cpr::Url auth_url;

  cpr::Header headers;
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
