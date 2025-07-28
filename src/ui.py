import pygame_menu as ui
from pygame_menu import themes, widgets
from supabase import Client

from globals import *
from auth import *


def sign_in_action(client: Client, menu: ui.Menu):
  email_input = menu.get_widget("sign_in_mail")
  password_input = menu.get_widget("sign_in_pass")
  label = menu.get_widget("status_label")

  assert email_input and password_input and label

  email = email_input.get_value()
  password = password_input.get_value()

  status = sign_in(client, email, password)
  label.set_title(status)


def sign_up_action(client: Client, menu: ui.Menu):
  email_input = menu.get_widget("sign_up_mail")
  password_input = menu.get_widget("sign_up_pass")
  label = menu.get_widget("status_label")

  assert email_input and password_input and label

  email = email_input.get_value()
  password = password_input.get_value()

  status = sign_up(client, email, password)
  label.set_title(status)


def sign_out_action(client: Client, menu: ui.Menu):
  label = menu.get_widget("status_label")
  assert label

  status = sign_out(client)
  label.set_title(status)


def setup_auth_menu(client: Client) -> ui.Menu:
  theme = themes.THEME_DARK
  theme.title_font_size = FONT_SIZE

  menu = ui.Menu(
      width=SCREEN_SIZE[0],
      height=SCREEN_SIZE[1],
      title="Menu",
      theme=theme,
  )

  label = menu.add.label("Authentication Menu",
                         label_id="status_label",
                         font_size=FONT_SIZE)

  sign_in_mail = menu.add.text_input("Email: ",
                                     default="example@email.com",
                                     textinput_id="sign_in_mail",
                                     font_size=FONT_SIZE)
  sign_in_pass = menu.add.text_input("Password: ",
                                     default="password",
                                     textinput_id="sign_in_pass",
                                     font_size=FONT_SIZE,
                                     password=False)
  menu.add.button("Sign In", sign_in_action, client, menu, font_size=FONT_SIZE)

  sign_up_mail = menu.add.text_input("Email: ",
                                     default="example@email.com",
                                     textinput_id="sign_up_mail",
                                     font_size=FONT_SIZE)
  sign_up_pass = menu.add.text_input("Password: ",
                                     default="password",
                                     textinput_id="sign_up_pass",
                                     font_size=FONT_SIZE,
                                     password=False)
  menu.add.button("Sign Up", sign_up_action, client, menu, font_size=FONT_SIZE)

  menu.add.button("Sign Out", sign_out_action, client, menu, font_size=FONT_SIZE)
  return menu
