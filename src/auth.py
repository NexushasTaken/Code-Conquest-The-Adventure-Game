from supabase import create_client, Client


def is_signed_in(client: Client):
  return client.auth.get_user() is not None


def sign_in(client: Client, email: str, password: str):
  if is_signed_in(client):
    return "You're already signed in."
  if email == "" or password == "":
    return "Enter email and password"

  client.auth.sign_in_with_password({
      "email": email,
      "password": password,
  })

  return "Signed In"


def sign_in(client: Client, email: str, password: str):
  if email == "" or password == "":
    return "Enter email and password"

  client.auth.sign_in_with_password({
      "email": email,
      "password": password,
  })

  return "Signed up"


def sign_out(client: Client):
  if not is_signed_in(client):
    return "You're not currently sign-in"

  client.auth.sign_out()

  return "Signed out"
