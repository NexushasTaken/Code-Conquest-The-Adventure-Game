from kivy.app import App
from kivy.uix.screenmanager import ScreenManager, Screen, NoTransition
from kivy.uix.boxlayout import BoxLayout
from kivy.uix.label import Label
from kivy.uix.button import Button
from kivy.uix.textinput import TextInput

from dotenv import load_dotenv
from supabase import create_client

from auth import sign_in, sign_up, sign_in_anonymously, sign_out

import os


def setup_client():
  load_dotenv()
  url = os.environ.get("SUPABASE_URL")
  key = os.environ.get("SUPABASE_KEY")
  if url is None or key is None:
    raise RuntimeError("SUPABASE_URL and SUPABASE_KEY must be set")
  return create_client(url, key)


class AccessModeScreen(Screen):

  def __init__(self, client, **kwargs):
    super().__init__(**kwargs)
    self.client = client
    layout = BoxLayout(orientation='vertical',
                       spacing=10,
                       padding=20,
                       size_hint=(None, None),
                       width=400,
                       height=350,
                       pos_hint={
                           'center_x': 0.5,
                           'center_y': 0.5
                       })
    layout.add_widget(Label(text="Choose Access Mode", size_hint_y=None, height=40))
    btn_sign_in = Button(text="Sign In", size_hint_y=None, height=40)
    btn_sign_up = Button(text="Sign Up", size_hint_y=None, height=40)
    btn_guest = Button(text="Sign In as Guest", size_hint_y=None, height=40)
    btn_exit = Button(text="Exit", size_hint_y=None, height=40)

    def go_sign_in(instance):
      self.manager.current = "sign_in"

    def go_sign_up(instance):
      self.manager.current = "sign_up"

    btn_sign_in.bind(on_release=go_sign_in)
    btn_sign_up.bind(on_release=go_sign_up)
    btn_guest.bind(on_release=self.sign_in_guest)
    btn_exit.bind(on_release=lambda _: App.get_running_app().stop())
    layout.add_widget(btn_sign_in)
    layout.add_widget(btn_sign_up)
    layout.add_widget(btn_guest)
    layout.add_widget(btn_exit)
    self.add_widget(layout)

  def sign_in_guest(self, *_):
    sign_in_anonymously(self.client)
    self.manager.get_screen(
        "main_menu").signed_in_user_label.text = "Signed in as Guest"
    self.manager.current = "main_menu"


class SignInScreen(Screen):

  def __init__(self, client, **kwargs):
    super().__init__(**kwargs)
    self.client = client
    layout = BoxLayout(orientation='vertical',
                       spacing=10,
                       padding=20,
                       size_hint=(None, None),
                       width=400,
                       height=350,
                       pos_hint={
                           'center_x': 0.5,
                           'center_y': 0.5
                       })
    layout.add_widget(Label(text="Sign In Credentials", size_hint_y=None, height=40))
    self.input_email = TextInput(text="example@email.com",
                                 hint_text="Email",
                                 size_hint_y=None,
                                 height=40)
    self.input_password = TextInput(text="password",
                                    hint_text="Password",
                                    password=True,
                                    size_hint_y=None,
                                    height=40)
    self.status = Label(text="", size_hint_y=None, height=30)
    btn_sign_in = Button(text="Sign In", size_hint_y=None, height=40)
    btn_back = Button(text="Back", size_hint_y=None, height=40)
    btn_sign_in.bind(on_release=self.do_sign_in)

    def go_access_mode(_):
      self.manager.current = "access_mode"

    btn_back.bind(on_release=go_access_mode)
    layout.add_widget(self.input_email)
    layout.add_widget(self.input_password)
    layout.add_widget(self.status)
    layout.add_widget(btn_sign_in)
    layout.add_widget(btn_back)
    self.add_widget(layout)

  def do_sign_in(self, *_):
    status = sign_in(self.client, self.input_email.text,
                     self.input_password.text)
    self.status.text = status
    if status == "Signed in":
      user = self.client.auth.get_user()
      if user:
        self.manager.get_screen(
            "main_menu"
        ).signed_in_user_label.text = f"Signed in as {user.user.email}"
      self.manager.current = "main_menu"


class SignUpScreen(Screen):

  def __init__(self, client, **kwargs):
    super().__init__(**kwargs)
    self.client = client
    layout = BoxLayout(orientation='vertical',
                       spacing=10,
                       padding=20,
                       size_hint=(None, None),
                       width=400,
                       height=350,
                       pos_hint={
                           'center_x': 0.5,
                           'center_y': 0.5
                       })
    layout.add_widget(
        Label(text="Sign Up Credentials", size_hint_y=None, height=40))
    self.input_email = TextInput(text="example@email.com",
                                 hint_text="Email",
                                 size_hint_y=None,
                                 height=40)
    self.input_password = TextInput(text="password",
                                    hint_text="Password",
                                    password=True,
                                    size_hint_y=None,
                                    height=40)
    self.status = Label(text="", size_hint_y=None, height=30)
    btn_sign_up = Button(text="Sign Up", size_hint_y=None, height=40)
    btn_back = Button(text="Back", size_hint_y=None, height=40)
    btn_sign_up.bind(on_release=self.do_sign_up)

    def go_access_mode(_):
      self.manager.current = "access_mode"

    btn_back.bind(on_release=go_access_mode)
    layout.add_widget(self.input_email)
    layout.add_widget(self.input_password)
    layout.add_widget(self.status)
    layout.add_widget(btn_sign_up)
    layout.add_widget(btn_back)
    self.add_widget(layout)

  def do_sign_up(self, *_):
    status = sign_up(self.client, self.input_email.text,
                     self.input_password.text)
    self.status.text = status


class MainMenuScreen(Screen):

  def __init__(self, client, **kwargs):
    super().__init__(**kwargs)
    self.client = client
    layout = BoxLayout(orientation='vertical',
                       spacing=10,
                       padding=20,
                       size_hint=(None, None),
                       width=400,
                       height=350,
                       pos_hint={
                           'center_x': 0.5,
                           'center_y': 0.5
                       })
    layout.add_widget(Label(text="Main Menu", size_hint_y=None, height=40))
    self.signed_in_user_label = Label(text="Signed In as <email>",
                                      size_hint_y=None,
                                      height=30)
    btn_logout = Button(text="Logout", size_hint_y=None, height=40)
    btn_logout.bind(on_release=self.do_sign_out)
    layout.add_widget(self.signed_in_user_label)
    layout.add_widget(btn_logout)
    self.add_widget(layout)

  def do_sign_out(self, *_):
    status = sign_out(self.client)
    self.manager.current = "access_mode"


class CodeConquestApp(App):

  def build(self):
    client = setup_client()
    sm = ScreenManager(transition=NoTransition())
    sm.add_widget(AccessModeScreen(client, name="access_mode"))
    sm.add_widget(SignInScreen(client, name="sign_in"))
    sm.add_widget(SignUpScreen(client, name="sign_up"))
    sm.add_widget(MainMenuScreen(client, name="main_menu"))
    sm.current = "access_mode"
    return sm


if __name__ == "__main__":
  CodeConquestApp().run()
