import os
import pygame
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


def main():
  pygame.init()
  client = setup_client()

  screen = pygame.display.set_mode((800, 600))
  clock = pygame.time.Clock()
  running = True

  font = pygame.Font()
  text_surface = font.render("Hello, World!", True, (0, 0, 0))
  text_rect = text_surface.get_rect(center=(400, 300))

  while running:
    for event in pygame.event.get():
      print(pygame.event.event_name(event.type))
      if event.type == pygame.QUIT:
        running = False
      elif event.type == pygame.KEYDOWN:
        if event.key == pygame.K_ESCAPE:
          running = False

    screen.fill("white")

    screen.blit(text_surface, text_rect)

    pygame.display.flip()
    clock.tick(60)

  pygame.quit()


if __name__ == "__main__":
  main()

