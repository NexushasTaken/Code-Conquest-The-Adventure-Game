#include "cpr/cpr.h"
#include "cpr/payload.h"
#include "cpr/ssl_options.h"
#include "global.hpp"
#include "nlohmann/json.hpp"
#include "raylib.h"
#include <iostream>
#include <optional>
#include <regex.h>
#include <string>

namespace supabase {
struct CprContext {
  CprContext() {
    if (FileExists("cacert.pem")) {
      ssl = cpr::Ssl(cpr::ssl::CaInfo{"cacert.pem"});
    } else {
      ssl = cpr::Ssl(cpr::ssl::MaxTLSVersion{});
    }
  }

  cpr::SslOptions ssl;
};

struct User {
  User() = default;
  User(json user) {
    id = user.value("id", "");
    aud = user.value("aud", "");
    role = user.value("role", "");
    email = user.value("email", "");
    email_confirmed_at = user.value("email_confirmed_at", "");
    phone = user.value("phone", "");
    phone_confirmed_at = user.value("phone_confirmed_at", "");
    confirmed_at = user.value("confirmed_at", "");
    last_sign_in_at = user.value("last_sign_in_at", "");
    app_metadata = user.value("app_metadata", json::object_t{});
    user_metadata = user.value("user_metadata", json::object_t{});
    identities = user.value("identities", json::array_t{});
    created_at = user.value("created_at", "");
    updated_at = user.value("updated_at", "");
    is_anonymous = user.value<bool>("is_anonymous", false);
  }

  // clang-format off
  // https://supabase.com/docs/guides/auth/users#the-user-object
  std::string id;                 // string	The unique id of the identity of the user.
  std::string aud;                // string	The audience claim.
  std::string role;               // string	The role claim used by Postgres to perform Row Level Security (RLS) checks.
  std::string email;              // string	The user's email address.
  std::string email_confirmed_at; // string	The timestamp that the user's email was confirmed. If null, it means that the user's email is not confirmed.
  std::string phone;              // string	The user's phone number.
  std::string phone_confirmed_at; // string	The timestamp that the user's phone was confirmed. If null, it means that the user's phone is not confirmed.
  std::string confirmed_at;       // string	The timestamp that either the user's email or phone was confirmed. If null, it means that the user does not have a confirmed email address and phone number.
  std::string last_sign_in_at;    // string	The timestamp that the user last signed in.
  json app_metadata;              // object	The provider attribute indicates the first provider that the user used to sign up with. The providers attribute indicates the list of providers that the user can use to login with.
  json user_metadata;             // object	Defaults to the first provider's identity data but can contain additional custom user metadata if specified. Refer to User Identity for more information about the identity object.
  json identities;                // UserIdentity[]	Contains an object array of identities linked to the user.
  std::string created_at;         // string	The timestamp that the user was created.
  std::string updated_at;         // string	The timestamp that the user was last updated.
  bool is_anonymous;              // boolean	Is true if the user is an anonymous user.
  // clang-format on
};

struct Session {
  Session() = default;
  Session(json response) : response(response) {
    user = User{response["user"]};

    access_token = response.value("access_token", "");
    refresh_token = response.value("refresh_token", "");
    expires_in = response.value("expires_in", 0);
    expires_at = response.value("expires_at", 0);
    token_type = response.value("token_type", "");
  }

  User user;

  std::string access_token;
  std::string refresh_token;
  int expires_in;
  int expires_at;
  std::string token_type;

private:
  json response;
};

struct AuthResponse {
  AuthResponse() = default;
  AuthResponse(User user, Session session) : user(user), session(session) {}
  User user;
  Session session;
};

struct Auth {
  Auth() = default;
  Auth(CprContext cpr_ctx, cpr::Url auth_url, cpr::Header headers)
      : headers(headers), cpr_ctx(cpr_ctx) {
    endpoint = auth_url;
    signup_endpoint = auth_url + "/signup";
    logout_endpoint = auth_url + "/logout";
  }

  AuthResponse sign_in_anonymously() {
    cpr::Response response =
        cpr::Post(headers, signup_endpoint, empty_data, cpr_ctx.ssl);
    json json_response = json::parse(response.text);

    user = User{json_response["user"]};
    session = Session{json_response};

    std::cout << user.id << std::endl;
    std::cout << user.email << std::endl;
    std::cout << user.role << std::endl;
    std::cout << user.is_anonymous << std::endl;
    std::cout << user.identities << std::endl;

    return AuthResponse(user, session);
  }

  AuthResponse sign_up_email(std::string email, std::string password) {
    cpr::Payload payload = {
        {"email", email},
        {"password", password},
    };
    cpr::Response response =
        cpr::Post(headers, signup_endpoint, payload, cpr_ctx.ssl);
    json json_response = json::parse(response.text);

    user = User{json_response["user"]};
    session = Session{json_response};

    return AuthResponse(user, session);
  }

  void sign_out() {
    auto jwt = session.access_token;
    cpr::Response response =
        cpr::Post(headers, logout_endpoint, empty_data, cpr_ctx.ssl);

    // delete user and session
    user = User{};
    session = Session{};
  }

  bool check_auth() { return false; }

private:
  User user;
  Session session;

  cpr::Url endpoint;
  cpr::Url signup_endpoint;
  cpr::Url logout_endpoint;

  cpr::Response response;

  cpr::Body empty_data = "{}";

  cpr::Header headers;
  CprContext cpr_ctx;
};

struct Client {
  Client(std::string api_url, std::string api_key)
      : api_url(api_url), api_key(api_key) {
    headers["accept"] = "application/json";
    headers["content-type"] = "application/json;charset=UTF-8";
    headers["apikey"] = api_key;

    auth_url = api_url + "/auth/v1";

    auth = Auth(cpr_ctx, auth_url, headers);
  }

  Auth auth;

private:
  cpr::Url api_url;
  cpr::Url api_key;
  cpr::Url auth_url;

  cpr::Header headers;
  CprContext cpr_ctx;
};
} // namespace supabase
