#include <iostream>
#include <string>
#include "cpr/cpr.h"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

namespace supabase {
struct User {
  User() = default;
  User(cpr::Response response) {
    json body = json::parse(response.text);
    std::cout << body.dump(2) << std::endl;

    json user = body["user"];
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
  std::string id; // 	string	The unique id of the identity of the user.
  std::string aud; // 	string	The audience claim.
  std::string role; // 	string	The role claim used by Postgres to perform Row Level Security (RLS) checks.
  std::string email; // 	string	The user's email address.
  std::string email_confirmed_at; // 	string	The timestamp that the user's email was confirmed. If null, it means that the user's email is not confirmed.
  std::string phone; // 	string	The user's phone number.
  std::string phone_confirmed_at; // 	string	The timestamp that the user's phone was confirmed. If null, it means that the user's phone is not confirmed.
  std::string confirmed_at; // 	string	The timestamp that either the user's email or phone was confirmed. If null, it means that the user does not have a confirmed email address and phone number.
  std::string last_sign_in_at; // 	string	The timestamp that the user last signed in.
  json app_metadata; // 	object	The provider attribute indicates the first provider that the user used to sign up with. The providers attribute indicates the list of providers that the user can use to login with.
  json user_metadata; // 	object	Defaults to the first provider's identity data but can contain additional custom user metadata if specified. Refer to User Identity for more information about the identity object.
  json identities; // 	UserIdentity[]	Contains an object array of identities linked to the user.
  std::string created_at; // 	string	The timestamp that the user was created.
  std::string updated_at; // 	string	The timestamp that the user was last updated.
  bool is_anonymous; // 	boolean	Is true if the user is an anonymous user.
  // clang-format on
};

struct AuthResponse {
  AuthResponse() = default;
  AuthResponse(User user) : user(user) {}
  User user;
};

struct Auth {
  Auth() = default;
  Auth(cpr::Url auth_url, cpr::Header headers) : headers(headers) {
    endpoint = auth_url;
    signup_endpoint = auth_url + "/signup";
    logout_endpoint = auth_url + "/logout";
  }

  AuthResponse sign_in_anonymously() {
    cpr::Response response =
        cpr::Post(headers, signup_endpoint, empty_data,
                  cpr::Ssl(cpr::ssl::CaInfo{"cacert.pem"}));

    user = User{response};
    session = json::parse(response.text);

    std::cout << user.id << std::endl;
    std::cout << user.email << std::endl;
    std::cout << user.role << std::endl;
    std::cout << user.is_anonymous << std::endl;
    std::cout << user.identities << std::endl;

    return AuthResponse(user);
  }

  void sign_out() {
    auto jwt = session["access_token"];
    cpr::Response response = cpr::Post(headers, logout_endpoint, empty_data);
    std::cout << "raw_header: " << response.raw_header << std::endl;
    std::cout << "text: " << response.text << std::endl;
    std::cout << "url: " << response.url << std::endl;
    std::cout << "reason: " << response.reason << std::endl;

    session = json::parse(response.text);

    user = User{};
    session = json{};
  }

  bool check_auth() { return false; }

private:
  User user;
  json session;

  cpr::Url endpoint;
  cpr::Url signup_endpoint;
  cpr::Url logout_endpoint;

  cpr::Response response;

  cpr::Body empty_data{"{}"};

  cpr::Header headers;
};

struct Client {
  Client(std::string api_url, std::string api_key)
      : api_url(api_url), api_key(api_key) {
    headers["accept"] = "application/json";
    headers["content-type"] = "application/json;charset=UTF-8";
    headers["apikey"] = api_key;

    auth_url = api_url + "/auth/v1";

    auth = Auth(auth_url, headers);
  }

  Auth auth;

private:
  cpr::Url api_url;
  cpr::Url api_key;
  cpr::Url auth_url;

  cpr::Header headers;
};
} // namespace supabase
