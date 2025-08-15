#include <cstring>
#include <string>
#include <variant>

#define SOL_BUILD_CXX_MODE 1
#define SOL_ALL_SAFETIES_ON 1
#include "lua.hpp"
#include "sol/sol.hpp"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "raylib.h"

#include "nuklear/raylib-nuklear.h"

#include "supabase.cpp"

#include "env.h"

#ifdef PLATFORM_ANDROID
int SCREEN_WIDTH = 0;
int SCREEN_HEIGHT = 0;
#else
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
#endif

using supabase::Client;

enum Page {
  Authenticate,
  Menu,
  SignIn,
  SignUp,
};

struct GuiContext {
  nk_context *ctx;
  Font font;

  char mail_buffer[256];
  int mail_max = 256;
  int mail_length = 0;

  std::string pass;
  int pass_max = 256;
  bool secret = true;

  std::string label;

  Page page = Authenticate;

  bool do_sign_in = false;
  bool do_sign_up = false;
  bool do_logout = false;

  std::string sign_in_status;
  std::string sign_up_status;
  std::string authenticate_status;

  struct nk_rect auth_win_rect;

  bool request_exit = false;
};

void password_input(nk_context *ctx, std::string &pwd, int max) {
  std::string buf(pwd.size(), '*');
  buf.resize(max, '\0');

  int len = static_cast<int>(pwd.size());
  int old_len = len;

  nk_edit_string(ctx, NK_EDIT_SIMPLE, buf.data(), &len, max, nk_filter_default);

  if (len > old_len) {
    pwd.append(buf, old_len, len - old_len);
    std::cout << std::quoted(pwd) << std::endl;
  } else if (len < old_len) {
    pwd.erase(len);
  }
}

void draw_sign_in_ui(GuiContext &ctx, Client &client) {
  if (nk_begin(ctx.ctx, "Sign In", ctx.auth_win_rect,
               NK_WINDOW_BORDER | NK_WINDOW_SCALABLE |
                   NK_WINDOW_NO_SCROLLBAR)) {
    nk_layout_row_dynamic(ctx.ctx, 0, 1);

    nk_label(ctx.ctx, "Enter your credentials to Sign In", NK_TEXT_CENTERED);
    nk_layout_row_dynamic(ctx.ctx, 0, 2);

    ctx.ctx->style.edit.cursor_size = 1.0;
    nk_label(ctx.ctx, "Email", NK_TEXT_CENTERED);
    nk_edit_string(ctx.ctx, NK_TEXT_EDIT_SINGLE_LINE | NK_EDIT_SIMPLE,
                   ctx.mail_buffer, &ctx.mail_length, ctx.mail_max,
                   nk_filter_default);

    nk_label(ctx.ctx, "Password", NK_TEXT_CENTERED);
    password_input(ctx.ctx, ctx.pass, ctx.pass_max);

    nk_label(ctx.ctx, ctx.sign_in_status.c_str(), NK_TEXT_CENTERED);
    if (nk_button_label(ctx.ctx, "Sign In")) {
      auto sign_up = client.auth.sign_in_email(
          {ctx.mail_buffer, (unsigned long)ctx.mail_length}, ctx.pass);

      if (sign_up.has_error()) {
        ctx.sign_in_status = sign_up.error().msg;
      } else {
        ctx.page = Menu;
      }
    }

    nk_layout_row_dynamic(ctx.ctx, 0, 3);
    nk_spacing(ctx.ctx, 1);
    if (nk_button_label(ctx.ctx, "Back")) {
      ctx.page = Authenticate;
    }
  }
  nk_end(ctx.ctx);
}

void draw_sign_up_ui(GuiContext &ctx, Client &client) {
  if (nk_begin(ctx.ctx, "Sign Up", ctx.auth_win_rect,
               NK_WINDOW_BORDER | NK_WINDOW_SCALABLE |
                   NK_WINDOW_NO_SCROLLBAR)) {
    nk_layout_row_dynamic(ctx.ctx, 0, 1);

    nk_label(ctx.ctx, "Enter your credentials to Sign Up", NK_TEXT_CENTERED);
    nk_layout_row_dynamic(ctx.ctx, 0, 2);

    ctx.ctx->style.edit.cursor_size = 1.0;
    nk_label(ctx.ctx, "Email", NK_TEXT_CENTERED);
    nk_edit_string(ctx.ctx, NK_TEXT_EDIT_SINGLE_LINE | NK_EDIT_SIMPLE,
                   ctx.mail_buffer, &ctx.mail_length, ctx.mail_max,
                   nk_filter_default);

    nk_label(ctx.ctx, "Password", NK_TEXT_CENTERED);
    password_input(ctx.ctx, ctx.pass, ctx.pass_max);

    nk_label(ctx.ctx, ctx.sign_up_status.c_str(), NK_TEXT_CENTERED);
    if (nk_button_label(ctx.ctx, "Sign up")) {
      auto sign_up = client.auth.sign_up_email(
          {ctx.mail_buffer, (unsigned long)ctx.mail_length}, ctx.pass);

      if (sign_up.has_error()) {
        ctx.sign_up_status = sign_up.error().msg;
      } else {
        ctx.page = Menu;
      }
    }

    nk_layout_row_dynamic(ctx.ctx, 0, 3);
    nk_spacing(ctx.ctx, 1);
    if (nk_button_label(ctx.ctx, "Back")) {
      ctx.page = Authenticate;
    }
  }
  nk_end(ctx.ctx);
}

void draw_main_menu_ui(GuiContext &ctx, Client &client) {
  if (nk_begin(ctx.ctx, "Main Menu", ctx.auth_win_rect,
               NK_WINDOW_BORDER | NK_WINDOW_SCALABLE |
                   NK_WINDOW_NO_SCROLLBAR)) {
    nk_layout_row_dynamic(ctx.ctx, 0, 1);

    nk_label(ctx.ctx, "Main Menu", NK_TEXT_CENTERED);

    auto session = client.auth.get_session();
    if (session->user.is_anonymous) {
      nk_label(ctx.ctx, "Signed In as Anonymous", NK_TEXT_CENTERED);
    } else {
      nk_label(ctx.ctx, ("Signed In as " + session->user.email).c_str(),
               NK_TEXT_CENTERED);
    }
    if (nk_button_label(ctx.ctx, "Sign Out")) {
      ctx.page = Authenticate;
      auto sign_out = client.auth.sign_out();
    }
    if (nk_button_label(ctx.ctx, "Quit")) {
      ctx.request_exit = true;
    }
  }
  nk_end(ctx.ctx);
}

void draw_authentication(GuiContext &ctx, Client &client) {
  if (nk_begin(ctx.ctx, "Choose how you want to Sign in", ctx.auth_win_rect,
               NK_WINDOW_BORDER | NK_WINDOW_SCALABLE |
                   NK_WINDOW_NO_SCROLLBAR)) {
    nk_layout_row_dynamic(ctx.ctx, 0, 3);
    nk_spacing(ctx.ctx, 1);
    nk_label(ctx.ctx, "Welcome", NK_TEXT_CENTERED);
    nk_layout_row_dynamic(ctx.ctx, 0, 1);

    if (nk_button_label(ctx.ctx, "Sign in")) {
      ctx.page = SignIn;
    }

    if (nk_button_label(ctx.ctx, "Sign up")) {
      ctx.page = SignUp;
    }

    if (nk_button_label(ctx.ctx, "Sign in as Anonymous")) {
      auto sign_in = client.auth.sign_in_anonymously();

      if (sign_in.has_error()) {
        ctx.authenticate_status = sign_in.error().msg;
      } else {
        ctx.page = Menu;
      }
    }
    if (nk_button_label(ctx.ctx, "Quit")) {
      ctx.request_exit = true;
    }

    nk_label(ctx.ctx, ctx.authenticate_status.c_str(), NK_TEXT_CENTERED);
  }
  nk_end(ctx.ctx);
}

void draw_ui(GuiContext &ctx, Client &client) {
  switch (ctx.page) {
  case Authenticate: {
    draw_authentication(ctx, client);
  } break;
  case SignIn: {
    draw_sign_in_ui(ctx, client);
  } break;
  case SignUp: {
    draw_sign_up_ui(ctx, client);
  } break;
  case Menu: {
    draw_main_menu_ui(ctx, client);
  } break;
  }
}

struct LogWindow {
  std::vector<std::string> lines;

  void Add(const std::string &msg) { lines.push_back(msg); }

  void Draw(struct nk_context *ctx) {
    nk_layout_row_dynamic(ctx, 0, 1);
    for (auto &line : lines) {
      nk_label_wrap(ctx, line.c_str());
    }
  }
};
static LogWindow log_win;
static std::string default_dir;

void DrawFileBrowser(struct nk_context *ctx) {
  nk_layout_row_dynamic(ctx, 25, 1);
  nk_label(ctx, GetWorkingDirectory(), NK_TEXT_CENTERED);
  nk_layout_row_dynamic(ctx, 25, 2);
  if (nk_button_label(ctx, "data")) {
#ifdef PLATFORM_ANDROID
    std::string path = PACKAGE_NAME;
    path = "/data/data/" + path;
    ChangeDirectory(path.c_str());
#else
    ChangeDirectory(GetApplicationDirectory());
#endif
  }
  if (nk_button_label(ctx, "default")) {
    ChangeDirectory(default_dir.c_str());
  }
  nk_layout_row_dynamic(ctx, 25, 1);

  // "Up" directory button ("..")
  if (nk_button_label(ctx, "..")) {
    ChangeDirectory("..");
  }

  // Get files & dirs in currentDir
  FilePathList files = LoadDirectoryFiles(GetWorkingDirectory());

  for (int i = 0; i < files.count; i++) {
    const char *path = files.paths[i];
    if (DirectoryExists(path)) {
      // It's a directory -> button
      const char *dirname = GetFileName(path);
      if (nk_button_label(ctx, dirname)) {
        ChangeDirectory(dirname);
      }
    } else {
      // It's a file -> label
      nk_label(ctx, GetFileName(path), NK_TEXT_LEFT);
    }
  }

  UnloadDirectoryFiles(files);
}

GuiContext create_gui_context() {
  GuiContext ctx;

  ctx.font = LoadFont("fonts/NotoSans-Regular.ttf");
  ctx.ctx = InitNuklearEx(ctx.font, 20);

  ctx.auth_win_rect = nk_rect(SCREEN_WIDTH / 2.0 - SCREEN_WIDTH / 2.0 / 2.0,
                              SCREEN_HEIGHT / 2.0 - SCREEN_HEIGHT / 2.0 / 2.0,
                              SCREEN_WIDTH / 2.0, SCREEN_HEIGHT / 2.0);
  std::strncpy(ctx.mail_buffer, "example@gmail.com", ctx.mail_max);
  ctx.mail_length = std::strlen("example@gmail.com");
  ctx.pass = "password";
  return ctx;
}

void setup_environment() {
  default_dir = GetWorkingDirectory();
#ifdef PLATFORM_DESKTOP
  ChangeDirectory("assets");
#elif PLATFORM_ANDROID
  {
    std::string data_dir = std::string("/data/data/") + PACKAGE_NAME + "/files";
    ChangeDirectory(data_dir.c_str());

    char *data = LoadFileText("cacert.pem");
    SaveFileText("cacert.pem", data);
    log_win.Add(std::string(data));
  }
#endif
}

void save_session(Client &client) {
  auto session_result = client.auth.get_session();
  if (!session_result.has_value()) {
    return;
  }

  auto session = session_result.value();
  if (session.user.is_anonymous) {
    client.auth.sign_out();
    return;
  }

  MakeDirectory(".data");
  SaveFileText(".data/session.json",
               const_cast<char *>(session.as_json().dump(2).c_str()));
}

bool load_session(GuiContext &ctx, Client &client) {
  if (!FileExists(".data/session.json")) {
    return false;
  }

  char *raw_session = LoadFileText(".data/session.json");
  json json_session = json::parse(raw_session);
  auto result = client.auth.load_session(json_session);
  if (result.has_value()) {
    ctx.page = Menu;
  }

  return result.has_value();
}

int main() {
  setup_environment();
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
#ifdef PLATFORM_ANDROID
  SCREEN_WIDTH = GetScreenWidth();
  SCREEN_HEIGHT = GetScreenHeight();
#endif

  GuiContext ctx = create_gui_context();
  load_session(ctx, client);
  bool exit_window = false;

  SetTargetFPS(60);

  std::string label = "Waiting...";

  while (!exit_window) {
    exit_window = WindowShouldClose();
    if (ctx.request_exit) {
      exit_window = true;
    }

    UpdateNuklear(ctx.ctx);

    BeginDrawing();
    ClearBackground(RAYWHITE);

    draw_ui(ctx, client);
    // if (nk_begin(ctx.ctx, "File Browser", nk_rect(10, 10, 400, 580),
    //              NK_WINDOW_BORDER | NK_WINDOW_TITLE | NK_WINDOW_MOVABLE)) {
    //   DrawFileBrowser(ctx.ctx);
    // }
    // nk_end(ctx.ctx);

    // if (nk_begin(ctx.ctx, "Log", nk_rect(400, 10, 380, 580),
    //              NK_WINDOW_BORDER | NK_WINDOW_TITLE | NK_WINDOW_MOVABLE |
    //              NK_WINDOW_SCROLL_AUTO_HIDE | NK_WINDOW_SCALABLE)) {
    //   log_win.Draw(ctx.ctx);
    // }
    // nk_end(ctx.ctx);

    DrawNuklear(ctx.ctx);
    EndDrawing();
  }

  UnloadNuklear(ctx.ctx);

  CloseWindow();

  save_session(client);
  return 0;
}
