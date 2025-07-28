import os
import pygame
import pygame_menu as ui
from pygame_menu import themes, widgets
from dotenv import load_dotenv
from supabase import create_client, Client

from globals import *
from auth import *
from ui import setup_auth_menu


def setup_client():
  url = os.environ.get("SUPABASE_URL")
  key = os.environ.get("SUPABASE_KEY")

  supabase: Client

  if url is None or key is None:
    raise RuntimeError("SUPABASE_URL and SUPABASE_KEY must be set")
  else:
    supabase = create_client(url, key)

  return supabase


def main():
  load_dotenv()
  pygame.init()
  client = setup_client()

  screen = pygame.display.set_mode(SCREEN_SIZE)
  clock = pygame.time.Clock()
  running = True

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

    if menu.is_enabled():
      menu.draw(screen)
      menu.update(events)

    pygame.display.flip()
    clock.tick(60)

  pygame.quit()


if __name__ == "__main__":
  main()
