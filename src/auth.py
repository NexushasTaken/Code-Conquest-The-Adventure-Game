from supabase import Client


def is_signed_in(client: Client) -> bool:
  return client.auth.get_user() is not None


def sign_in(client: Client, email: str, password: str) -> str:
  if is_signed_in(client):
    return "You're already signed in."
  if email == "" or password == "":
    return "Enter email and password"

  try:
    client.auth.sign_in_with_password({
        "email": email,
        "password": password,
    })
  except Exception as e:
    return str(e)

  return "Signed in"


def sign_up(client: Client, email: str, password: str) -> str:
  if email == "" or password == "":
    return "Enter email and password"

  try:
    client.auth.sign_up({
        "email": email,
        "password": password,
    })
  except Exception as e:
    return str(e)

  return "Signed up"


def sign_out(client: Client) -> str:
  if not is_signed_in(client):
    return "You're not currently sign-in"

  try:
    client.auth.sign_out()
  except Exception as e:
    return str(e)

  return "Signed out"

