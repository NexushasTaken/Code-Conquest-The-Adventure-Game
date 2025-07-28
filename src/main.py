import os
from dotenv import load_dotenv
from supabase import create_client, Client

import arcade
from arcade import gui
from arcade.gui.experimental.password_input import UIPasswordInput

from auth import sign_in, sign_out, sign_up


WINDOW_WIDTH = 800
WINDOW_HEIGHT = 500
WINDOW_TITLE = "Starting Template"


def setup_client():
  url = os.environ.get("SUPABASE_URL")
  key = os.environ.get("SUPABASE_KEY")

  supabase: Client

  if url is None or key is None:
    raise RuntimeError("SUPABASE_URL and SUPABASE_KEY must be set")
  else:
    supabase = create_client(url, key)

  return supabase


class GameView(arcade.View):

  def __init__(self):
    super().__init__()

    load_dotenv()
    self.client = setup_client()

    self.background_color = arcade.color.DARK_SLATE_GRAY
    self.ui = gui.UIManager()

    self.auth_layout = self.ui.add(gui.UIAnchorLayout())

    self.setup_access_mode_layout()
    self.setup_sign_in_layout()
    self.setup_sign_up_layout()
    self.setup_main_menu_layout()

    self.layouts = [self.access_mode_layout, self.sign_in_layout, self.sign_up_layout, self.main_menu_layout]
    self.auth_layout.add(self.access_mode_layout)


  def hide_layouts(self):
    for layout in self.layouts:
      self.auth_layout.remove(layout)
    pass


  def setup_access_mode_layout(self):
    self.access_mode_layout = gui.UIBoxLayout(space_between=4)
    sign_in_button = gui.UIFlatButton(text="Sign In", height=30)
    sign_up_button = gui.UIFlatButton(text="Sign Up", height=30)
    exit_button = gui.UIFlatButton(text="Exit", height=30)

    @sign_in_button.event("on_click")
    def to_sign_in(event):
      self.hide_layouts()
      self.auth_layout.add(self.sign_in_layout)

    @sign_up_button.event("on_click")
    def to_sign_up(event):
      self.hide_layouts()
      self.auth_layout.add(self.sign_up_layout)

    @exit_button.event("on_click")
    def do_exit(event):
      arcade.exit()

    self.access_mode_layout.add(gui.UILabel(text="Choose Access Layout", height=30))
    self.access_mode_layout.add(sign_in_button)
    self.access_mode_layout.add(sign_up_button)
    self.access_mode_layout.add(exit_button)


  def setup_sign_in_layout(self):
    self.sign_in_layout = gui.UIGridLayout(size_hint=(0, 0), column_count=3, row_count=5, vertical_spacing=4, horizontal_spacing=4)
    input_email = gui.UIInputText(text="example@email.com", height=30, width=200)
    input_password = UIPasswordInput(text="password", height=30, width=200)
    status_label = gui.UILabel(text="", height=30)
    sign_in_button = gui.UIFlatButton(text="Sign In", height=30)
    back_button = gui.UIFlatButton(text="Back", height=30)

    @sign_in_button.event("on_click")
    def do_sign_in(event):
      self.hide_layouts()
      status = sign_in(self.client, input_email.text, input_password.text)
      status_label.text = status
      if status == "Signed in":
        self.hide_layouts()
        self.auth_layout.add(self.main_menu_layout)
        status_label.text = ""
        user = self.client.auth.get_user()
        assert user
        self.signed_in_user_label.text = f"Signed in as {user.user.email}"

    @back_button.event("on_click")
    def to_access_mode(event):
      self.hide_layouts()
      self.auth_layout.add(self.access_mode_layout)

    self.sign_in_layout.add(gui.UILabel(text="Sign In Credentials", height=30), row=0, column_span=2)

    self.sign_in_layout.add(gui.UILabel(text="Email", height=30), row=1, column=0)
    self.sign_in_layout.add(input_email, row=1, column=1)

    self.sign_in_layout.add(gui.UILabel(text="Password", height=30), row=2, column=0)
    self.sign_in_layout.add(input_password, row=2, column=1)

    self.sign_in_layout.add(status_label, row=3, column=0)
    self.sign_in_layout.add(sign_in_button, row=3, column=1)
    self.sign_in_layout.add(back_button, row=4, column_span=2)


  def setup_sign_up_layout(self):
    self.sign_up_layout = gui.UIGridLayout(size_hint=(0, 0), column_count=3, row_count=5, vertical_spacing=4, horizontal_spacing=4)
    input_email = gui.UIInputText(text="example@email.com", height=30, width=200)
    input_password = UIPasswordInput(text="password", height=30, width=200)
    status_label = gui.UILabel(text="", height=30)
    sign_up_button = gui.UIFlatButton(text="Sign Up", height=30)
    back_button = gui.UIFlatButton(text="Back", height=30)

    @sign_up_button.event("on_click")
    def do_sign_up(event):
      status = sign_up(self.client, input_email.text, input_password.text)
      status_label.text = status

    @back_button.event("on_click")
    def to_access_mode(event):
      self.hide_layouts()
      self.auth_layout.add(self.access_mode_layout)
      status_label.text = ""


    self.sign_up_layout.add(gui.UILabel(text="Sign Up Credentials", height=30), row=0, column_span=2)

    self.sign_up_layout.add(gui.UILabel(text="Email", height=30), row=1, column=0)
    self.sign_up_layout.add(input_email, row=1, column=1)

    self.sign_up_layout.add(gui.UILabel(text="Password", height=30), row=2, column=0)
    self.sign_up_layout.add(input_password, row=2, column=1)

    self.sign_up_layout.add(status_label, row=3, column=0)
    self.sign_up_layout.add(sign_up_button, row=3, column=1)
    self.sign_up_layout.add(back_button, row=4, column_span=2)


  def setup_main_menu_layout(self):
    self.main_menu_layout = gui.UIBoxLayout(space_between=4)
    self.signed_in_user_label = gui.UILabel(text="Signed In as <email>", height=30)
    sign_out_button = gui.UIFlatButton(text="Logout", height=30)

    @sign_out_button.event("on_click")
    def do_sign_out(event):
      self.hide_layouts()
      assert sign_out(self.client) == "Signed out"
      self.auth_layout.add(self.access_mode_layout)

    self.main_menu_layout.add(gui.UILabel(text="Main Menu", height=30))
    self.main_menu_layout.add(self.signed_in_user_label)
    self.main_menu_layout.add(sign_out_button)


  def reset(self):
    pass

  def on_draw(self):
    self.clear()
    self.ui.draw()

  def on_update(self, delta_time):
    pass

  def on_show_view(self):
    self.ui.enable()

  def on_hide_view(self):
    self.ui.disable()


def main():
  window = arcade.Window(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE)
  game = GameView()
  window.show_view(game)
  arcade.run()


if __name__ == "__main__":
  main()
