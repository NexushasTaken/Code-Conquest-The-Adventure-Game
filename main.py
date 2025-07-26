import os
import pygame
import pygame_menu as ui
from pygame_menu import themes, widgets
from dotenv import load_dotenv
from supabase import create_client, Client


def setup_client():
  url = os.environ.get("SUPABASE_URL")
  key = os.environ.get("SUPABASE_KEY")

  supabase: Client

  if url is None or key is None:
    raise RuntimeError("SUPABASE_URL and SUPABASE_KEY must be set")
  else:
    supabase = create_client(url, key)

  return supabase


def is_signed_in(client: Client):
  return client.auth.get_user() is not None


def sign_in(client: Client, mail_input: widgets.TextInput, pass_input: widgets.TextInput, menu: ui.Menu):
  email = mail_input.get_value()
  password = pass_input.get_value()
  label = menu.get_widget("status_label")

  if is_signed_in(client):
    if label is not None:
      label.set_title("You're already signed in.")
    return

  if email == "" or password == "":
    if label is not None:
      label.set_title("Enter email and password")
    return

  client.auth.sign_in_with_password({
    "email": email,
    "password": password,
    })

  if label is not None:
    label.set_title("Signed in")


def sign_up(client: Client, mail_input: widgets.TextInput, pass_input: widgets.TextInput, menu: ui.Menu):
  email = mail_input.get_value()
  password = pass_input.get_value()
  label = menu.get_widget("status_label")

  if email == "" or password == "":
    if label is not None:
      label.set_title("Enter email and password")
    return

  client.auth.sign_up({
    "email": email,
    "password": password,
    })

  if label is not None:
    label.set_title("Signed up")


def sign_out(client: Client, menu: ui.Menu):
  label = menu.get_widget("status_label")

  if not is_signed_in(client):
    if label is not None:
      label.set_title("You're not currently sign-in")
    return

  client.auth.sign_out()

  if label is not None:
    label.set_title("Signed out")


def setup_auth_menu(client: Client) -> ui.Menu:
  font_size = 16
  theme = themes.THEME_DARK
  theme.title_font_size = font_size

  menu = ui.Menu(
      height=300,
      width=300,
      title="Menu",
      theme=theme,
      )

  label = menu.add.label("Hello, World", label_id="status_label", font_size=font_size)

  sign_in_mail = menu.add.text_input("Email: ", "example@email.com", font_size=font_size)
  sign_in_pass = menu.add.text_input("Password: ", "password", font_size=font_size, password=False)
  menu.add.button("Sign In", sign_in, client, sign_in_mail, sign_in_pass, menu, font_size=font_size)

  sign_up_mail = menu.add.text_input("Email: ", "example@email.com", font_size=font_size)
  sign_up_pass = menu.add.text_input("Password: ", "password", font_size=font_size, password=False)
  menu.add.button("Sign Up", sign_up, client, sign_up_mail, sign_up_pass, menu, font_size=font_size)

  menu.add.button("Sign Out", sign_out, client, menu, font_size=font_size)
  return menu


def main():
  load_dotenv()
  pygame.init()
  client = setup_client()

  screen = pygame.display.set_mode((800, 600))
  clock = pygame.time.Clock()
  running = True

  font = pygame.Font()
  text_surface = font.render("Hello, World!", True, (0, 0, 0))
  text_rect = text_surface.get_rect(center=(400, 300))

  menu = setup_auth_menu(client)

  while running:
    events = pygame.event.get()
    for event in events:
      if event.type == pygame.QUIT:
        running = False
      elif event.type == pygame.KEYDOWN:
        if event.key == pygame.K_ESCAPE:
          running = False

    screen.fill("white")

    screen.blit(text_surface, text_rect)

    if menu.is_enabled():
      menu.draw(screen)
      menu.update(events)

    pygame.display.flip()
    clock.tick(60)

  pygame.quit()


if __name__ == "__main__":
  main()

