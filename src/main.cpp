#include "raygui.h"
#include "raylib.h"
#include "utils.h"
#include <cstdlib>
#include <curl/curl.h>
#include <curl/easy.h>
#include <curl/multi.h>
#include <exception>
#include <iomanip>
#include <stdexcept>
#include <string>

#define SOL_BUILD_CXX_MODE 1
#define SOL_ALL_SAFETIES_ON 1
#include "lua.hpp"
#include "sol/sol.hpp"

// #define RAYGUI_IMPLEMENTATION
// #include "raygui.h"

#include "./utils.cpp"

#include "nlohmann/json.hpp"

using json = nlohmann::json;

namespace supabase {
struct Api {
  Api() = default;
  Api(std::string supabase_url, std::string supabase_key)
      : supabase_url(supabase_url), supabase_key(supabase_key) {}

  const std::string &key() { return this->supabase_key; }
  const std::string &url() { return this->supabase_url; }

  static Api load(const std::string &filename) {
    char *raw_data = LoadFileText(filename.c_str());
    if (!raw_data) {
      throw std::runtime_error("Failed to load file: " + filename);
    }

    std::string_view content(raw_data);
    std::string url, key;

    auto trim = [](std::string_view sv) {
      while (!sv.empty() && std::isspace(sv.front())) {
        sv.remove_prefix(1);
      }
      while (!sv.empty() && std::isspace(sv.back())) {
        sv.remove_suffix(1);
      }
      return sv;
    };

    while (!content.empty()) {
      auto newline_pos = content.find('\n');
      std::string_view line = (newline_pos == std::string_view::npos)
                                  ? content
                                  : content.substr(0, newline_pos);

      if (newline_pos != std::string_view::npos) {
        content.remove_prefix(newline_pos + 1);
      } else {
        content = {};
      }

      line = trim(line);
      if (line.empty() || line.front() == '#') {
        continue;
      }

      auto eq_pos = line.find('=');
      if (eq_pos == std::string_view::npos) {
        continue;
      }

      auto name = trim(line.substr(0, eq_pos));
      auto value = trim(line.substr(eq_pos + 1));

      if (name == "SUPABASE_URL") {
        url = std::string(value);
      } else if (name == "SUPABASE_KEY") {
        key = std::string(value);
      }
    }

    if (url.empty() && key.empty()) {
      TraceLog(LOG_WARNING, "SUPABASE_KEY and SUPABASE_URL not found!");
    }

    UnloadFileText(raw_data);

    return Api(url, key);
  }

private:
  std::string supabase_url{};
  std::string supabase_key{};
};

struct Client;
struct Auth {
  Auth(const Client &client) : client(client) {
  }
private:
  const Client &client;
};

struct Client {
  Client(Api api) : api(std::move(api)) { init(); }
  ~Client() {
    curl_multi_cleanup(curl_multi);
  }

  void curl_poll() {
  }

private:
  void init() { set_url_endpoints(); init_curl(); }
  void set_url_endpoints() { auth_url.append(api.url() + "/auth/v1"); }
  void init_curl() {
    curl_multi = curl_multi_init();

    headers = curl_slist_append(headers, "Accept: application/json");
    headers = curl_slist_append(headers, "Content-Type: application/json;charset=UTF-8");
    headers = curl_slist_append(headers, ("apikey: " + api.key()).c_str());
  }

  Api api;
  std::string auth_url;

  CURL *curl_multi;
  struct curl_slist *headers;
};
} // namespace supabase

int main() {
  ChangeDirectory("assets");
  auto api = supabase::Api::load(".env");
  curl_global_init(CURL_GLOBAL_DEFAULT);

  CURL *curl = curl_easy_init();

  std::string url = api.url() + "/auth/v1/signup";
  std::string body = "{}";
  std::cout << url << std::endl;
  std::string post_fields = "scope=global";
  struct curl_slist *headers = NULL;
  headers = curl_slist_append(headers, ("apikey: " + api.key()).c_str());
  headers = curl_slist_append(headers,
                              "Content-Type: application/json;charset=UTF-8");
  headers = curl_slist_append(headers, "Accept: application/json");

  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_fields.c_str());
  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
  curl_easy_setopt(curl, CURLOPT_POST, 0);
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
  curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, body.size());

  CURLM *curlm = curl_multi_init();
  curl_multi_add_handle(curlm, curl);
  int still_running = 1;
  while (still_running) {
    auto res = curl_multi_perform(curlm, &still_running);
    std::cout << "Waiting..." << std::endl;
  }
  curl_multi_remove_handle(curlm, curl);
  curl_multi_cleanup(curlm);

  // curl_easy_perform(curl);

  curl_slist_free_all(headers);
  curl_global_cleanup();
  return 0;

#ifdef PLATFORM_DESKTOP
  int SCREEN_WIDTH = 600;
  int SCREEN_HEIGHT = 800;
  InitWindow(SCREEN_HEIGHT, SCREEN_WIDTH, "Hello, World");
  ChangeDirectory("assets");
#else
  InitWindow(0, 0, "Hello, World");
  int SCREEN_WIDTH = GetScreenWidth();
  int SCREEN_HEIGHT = GetScreenHeight();
#endif

  std::cout << std::quoted(api.key());
  std::cout << std::quoted(api.url());

  sol::state lua;

  lua.set_function("print", [] { printf("Hello, World\n"); });
  auto state = lua.script("print()");

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    EndDrawing();
  }

  CloseWindow();

  curl_global_cleanup();
}
