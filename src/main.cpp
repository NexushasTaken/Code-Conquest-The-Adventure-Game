#include <cstring>
#include <string>

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

  char mail_buffer[256] = {0};
  int mail_max = 256;
  int mail_length = 0;
  char pass_buffer[256] = {0};
  int pass_max = 256;
  int pass_length = 0;
  bool secret = true;

  std::string label;

  Page page = Authenticate;

  bool do_sign_in = false;
  bool do_sign_up = false;
  bool do_logout = false;

  struct nk_rect auth_win_rect;
};

void password_input(nk_context *ctx, char *pwd_buf, int *len, int max) {
  char *buf = new char[max];
  memset(buf, '*', *len);
  buf[*len] = 0;

  int old_len = *len;
  nk_edit_string(ctx, NK_EDIT_SIMPLE, buf, len, max, nk_filter_default);

  if (old_len < *len) {
    memcpy(&pwd_buf[old_len], &buf[old_len], (nk_size)(*len - old_len));
  } else if (old_len > *len) {
    pwd_buf[*len] = 0;
  }
  delete[] buf;
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
    password_input(ctx.ctx, ctx.pass_buffer, &ctx.pass_length, ctx.pass_max);

    nk_label(ctx.ctx, "", NK_TEXT_CENTERED);
    if (nk_button_label(ctx.ctx, "Sign In")) {
      ctx.label = "Sign up was clicked!";
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
    password_input(ctx.ctx, ctx.pass_buffer, &ctx.pass_length, ctx.pass_max);

    nk_label(ctx.ctx, "", NK_TEXT_CENTERED);
    if (nk_button_label(ctx.ctx, "Sign up")) {
      client.auth.sign_up_email({ctx.mail_buffer, (unsigned long)ctx.mail_length},
                                {ctx.pass_buffer, (unsigned long)ctx.pass_length});
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

    nk_label(ctx.ctx, "Authentication status", NK_TEXT_CENTERED);
    if (nk_button_label(ctx.ctx, "Sign Out")) {
      ctx.page = Authenticate;
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
      ctx.label = "Sign in was clicked!";
    }

    if (nk_button_label(ctx.ctx, "Sign up")) {
      ctx.page = SignUp;
      ctx.label = "Sign up was clicked!";
    }

    if (nk_button_label(ctx.ctx, "Sign in as Anonymous")) {
      client.auth.sign_in_anonymously();
      ctx.label = "Signed in successfully";
    }

    nk_label(ctx.ctx, ctx.label.c_str(), NK_TEXT_CENTERED);
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

  SetTargetFPS(60);

  std::string label = "Waiting...";

  while (!WindowShouldClose()) {
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

  curl_global_cleanup();
  return 0;
}
