#include "./errorcodes.hpp"
#include "../global.hpp"
#include "cpr/error.h"
#include <cassert>
#include <map>

namespace supabase {
ErrorCode::ErrorCode(json error) {
  this->error_code = error.value("error_code", "unknown_error");
  this->msg = error.value("msg", "UNKNOWN_ERROR");

  assert(error_code_enums.find(this->error_code) != error_code_enums.end());
  ErrorCodes error_code = error_code_enums.at(this->error_code);

  this->code = (int)error_code;

  this->supabase_error_code = error_code;
  this->type = Type::Supabase;
};

ErrorCode::ErrorCode(cpr::Error error) {
  this->code = (int)error.code;
  this->error_code = std::to_string(error.code);
  this->msg = error.message;

  this->network_error_code = error.code;
  this->type = Type::Network;
}

std::string ErrorCode::get_description() {
  assert(type != Type::None);

  switch (type) {
  case Type::None:
    return "";
  case Type::Network:
    return std::to_string(network_error_code);
  case Type::Supabase:
    auto res = error_code_descriptions.find(supabase_error_code);
    assert(res != error_code_descriptions.end());
    return res->second;
  }
}

// clang-format off
const std::map<ErrorCodes, std::string> ErrorCode::error_code_descriptions = {
  {ErrorCodes::ANONYMOUS_PROVIDER_DISABLED, "Anonymous sign-ins are disabled."},
  {ErrorCodes::BAD_CODE_VERIFIER, "Returned from the PKCE flow where the provided code verifier does not match the expected one. Indicates a bug in the implementation of the client library."},
  {ErrorCodes::BAD_JSON, "Usually used when the HTTP body of the request is not valid JSON."},
  {ErrorCodes::BAD_JWT, "JWT sent in the Authorization header is not valid."},
  {ErrorCodes::BAD_OAUTH_CALLBACK, "OAuth callback from provider to Auth does not have all the required attributes (state). Indicates an issue with the OAuth provider or client library implementation."},
  {ErrorCodes::BAD_OAUTH_STATE, "OAuth state (data echoed back by the OAuth provider to Supabase Auth) is not in the correct format. Indicates an issue with the OAuth provider integration."},
  {ErrorCodes::CAPTCHA_FAILED, "CAPTCHA challenge could not be verified with the CAPTCHA provider. Check your CAPTCHA integration."},
  {ErrorCodes::CONFLICT, "General database conflict, such as concurrent requests on resources that should not be modified concurrently. Can often occur when you have too many session refresh requests firing off at the same time for a user. Check your app for concurrency issues, and if detected, back off exponentially."},
  {ErrorCodes::EMAIL_ADDRESS_INVALID, "Example and test domains are currently not supported. Use a different email address."},
  {ErrorCodes::EMAIL_ADDRESS_NOT_AUTHORIZED, "Email sending is not allowed for this address as your project is using the default SMTP service. Emails can only be sent to members in your Supabase organization. If you want to send emails to others, set up a custom SMTP provider."},
  {ErrorCodes::EMAIL_CONFLICT_IDENTITY_NOT_DELETABLE, "Unlinking this identity causes the user's account to change to an email address which is already used by another user account. Indicates an issue where the user has two different accounts using different primary email addresses. You may need to migrate user data to one of their accounts in this case."},
  {ErrorCodes::EMAIL_EXISTS, "Email address already exists in the system."},
  {ErrorCodes::EMAIL_NOT_CONFIRMED, "Signing in is not allowed for this user as the email address is not confirmed."},
  {ErrorCodes::EMAIL_PROVIDER_DISABLED, "Signups are disabled for email and password."},
  {ErrorCodes::FLOW_STATE_EXPIRED, "PKCE flow state to which the API request relates has expired. Ask the user to sign in again."},
  {ErrorCodes::FLOW_STATE_NOT_FOUND, "PKCE flow state to which the API request relates no longer exists. Flow states expire after a while and are progressively cleaned up, which can cause this error. Retried requests can cause this error, as the previous request likely destroyed the flow state. Ask the user to sign in again."},
  {ErrorCodes::HOOK_PAYLOAD_INVALID_CONTENT_TYPE, "Payload from Auth does not have a valid Content-Type header."},
  {ErrorCodes::HOOK_PAYLOAD_OVER_SIZE_LIMIT, "Payload from Auth exceeds maximum size limit."},
  {ErrorCodes::HOOK_TIMEOUT, "Unable to reach hook within maximum time allocated."},
  {ErrorCodes::HOOK_TIMEOUT_AFTER_RETRY, "Unable to reach hook after maximum number of retries."},
  {ErrorCodes::IDENTITY_ALREADY_EXISTS, "The identity to which the API relates is already linked to a user."},
  {ErrorCodes::IDENTITY_NOT_FOUND, "Identity to which the API call relates does not exist, such as when an identity is unlinked or deleted."},
  {ErrorCodes::INSUFFICIENT_AAL, "To call this API, the user must have a higher Authenticator Assurance Level. To resolve, ask the user to solve an MFA challenge."},
  {ErrorCodes::INVITE_NOT_FOUND, "Invite is expired or already used."},
  {ErrorCodes::INVALID_CREDENTIALS, "Login credentials or grant type not recognized."},
  {ErrorCodes::MANUAL_LINKING_DISABLED, "Calling the supabase.auth.linkUser() and related APIs is not enabled on the Auth server."},
  {ErrorCodes::MFA_CHALLENGE_EXPIRED, "Responding to an MFA challenge should happen within a fixed time period. Request a new challenge when encountering this error."},
  {ErrorCodes::MFA_FACTOR_NAME_CONFLICT, "MFA factors for a single user should not have the same friendly name."},
  {ErrorCodes::MFA_FACTOR_NOT_FOUND, "MFA factor no longer exists."},
  {ErrorCodes::MFA_IP_ADDRESS_MISMATCH, "The enrollment process for MFA factors must begin and end with the same IP address."},
  {ErrorCodes::MFA_PHONE_ENROLL_NOT_ENABLED, "Enrollment of MFA Phone factors is disabled."},
  {ErrorCodes::MFA_PHONE_VERIFY_NOT_ENABLED, "Login via Phone factors and verification of new Phone factors is disabled."},
  {ErrorCodes::MFA_TOTP_ENROLL_NOT_ENABLED, "Enrollment of MFA TOTP factors is disabled."},
  {ErrorCodes::MFA_TOTP_VERIFY_NOT_ENABLED, "Login via TOTP factors and verification of new TOTP factors is disabled."},
  {ErrorCodes::MFA_VERIFICATION_FAILED, "MFA challenge could not be verified -- wrong TOTP code."},
  {ErrorCodes::MFA_VERIFICATION_REJECTED, "Further MFA verification is rejected. Only returned if the MFA verification attempt hook returns a reject decision."},
  {ErrorCodes::MFA_VERIFIED_FACTOR_EXISTS, "Verified phone factor already exists for a user. Unenroll existing verified phone factor to continue."},
  {ErrorCodes::MFA_WEB_AUTHN_ENROLL_NOT_ENABLED, "Enrollment of MFA Web Authn factors is disabled."},
  {ErrorCodes::MFA_WEB_AUTHN_VERIFY_NOT_ENABLED, "Login via WebAuthn factors and verification of new WebAuthn factors is disabled."},
  {ErrorCodes::NO_AUTHORIZATION, "This HTTP request requires an Authorization header, which is not provided."},
  {ErrorCodes::NOT_ADMIN, "User accessing the API is not admin, i.e. the JWT does not contain a role claim that identifies them as an admin of the Auth server."},
  {ErrorCodes::OAUTH_PROVIDER_NOT_SUPPORTED, "Using an OAuth provider which is disabled on the Auth server."},
  {ErrorCodes::OTP_DISABLED, "Sign in with OTPs (magic link, email OTP) is disabled. Check your server's configuration."},
  {ErrorCodes::OTP_EXPIRED, "OTP code for this sign-in has expired. Ask the user to sign in again."},
  {ErrorCodes::OVER_EMAIL_SEND_RATE_LIMIT, "Too many emails have been sent to this email address. Ask the user to wait a while before trying again."},
  {ErrorCodes::OVER_REQUEST_RATE_LIMIT, "Too many requests have been sent by this client (IP address). Ask the user to try again in a few minutes. Sometimes can indicate a bug in your application that mistakenly sends out too many requests (such as a badly written useEffect React hook)."},
  {ErrorCodes::OVER_SMS_SEND_RATE_LIMIT, "Too many SMS messages have been sent to this phone number. Ask the user to wait a while before trying again."},
  {ErrorCodes::PHONE_EXISTS, "Phone number already exists in the system."},
  {ErrorCodes::PHONE_NOT_CONFIRMED, "Signing in is not allowed for this user as the phone number is not confirmed."},
  {ErrorCodes::PHONE_PROVIDER_DISABLED, "Signups are disabled for phone and password."},
  {ErrorCodes::PROVIDER_DISABLED, "OAuth provider is disabled for use. Check your server's configuration."},
  {ErrorCodes::PROVIDER_EMAIL_NEEDS_VERIFICATION, "Not all OAuth providers verify their user's email address. Supabase Auth requires emails to be verified, so this error is sent out when a verification email is sent after completing the OAuth flow."},
  {ErrorCodes::REAUTHENTICATION_NEEDED, "A user needs to reauthenticate to change their password. Ask the user to reauthenticate by calling the supabase.auth.reauthenticate() API."},
  {ErrorCodes::REAUTHENTICATION_NOT_VALID, "Verifying a reauthentication failed, the code is incorrect. Ask the user to enter a new code."},
  {ErrorCodes::REFRESH_TOKEN_NOT_FOUND, "Session containing the refresh token not found."},
  {ErrorCodes::REFRESH_TOKEN_ALREADY_USED, "Refresh token has been revoked and falls outside the refresh token reuse interval. See the documentation on sessions for further information."},
  {ErrorCodes::REQUEST_TIMEOUT, "Processing the request took too long. Retry the request."},
  {ErrorCodes::SAME_PASSWORD, "A user that is updating their password must use a different password than the one currently used."},
  {ErrorCodes::SAML_ASSERTION_NO_EMAIL, "SAML assertion (user information) was received after sign in, but no email address was found in it, which is required. Check the provider's attribute mapping and/or configuration."},
  {ErrorCodes::SAML_ASSERTION_NO_USER_ID, "SAML assertion (user information) was received after sign in, but a user ID (called NameID) was not found in it, which is required. Check the SAML identity provider's configuration."},
  {ErrorCodes::SAML_ENTITY_ID_MISMATCH, "(Admin API.) Updating the SAML metadata for a SAML identity provider is not possible, as the entity ID in the update does not match the entity ID in the database. This is equivalent to creating a new identity provider, and you should do that instead."},
  {ErrorCodes::SAML_IDP_ALREADY_EXISTS, "(Admin API.) Adding a SAML identity provider that is already added."},
  {ErrorCodes::SAML_IDP_NOT_FOUND, "SAML identity provider not found. Most often returned after IdP-initiated sign-in with an unregistered SAML identity provider in Supabase Auth."},
  {ErrorCodes::SAML_METADATA_FETCH_FAILED, "(Admin API.) Adding or updating a SAML provider failed as its metadata could not be fetched from the provided URL."},
  {ErrorCodes::SAML_PROVIDER_DISABLED, "Using Enterprise SSO with SAML 2.0 is not enabled on the Auth server."},
  {ErrorCodes::SAML_RELAY_STATE_EXPIRED, "SAML relay state is an object that tracks the progress of a supabase.auth.signInWithSSO() request. The SAML identity provider should respond after a fixed amount of time, after which this error is shown. Ask the user to sign in again."},
  {ErrorCodes::SAML_RELAY_STATE_NOT_FOUND, "SAML relay states are progressively cleaned up after they expire, which can cause this error. Ask the user to sign in again."},
  {ErrorCodes::SESSION_EXPIRED, "Session to which the API request relates has expired. This can occur if an inactivity timeout is configured, or the session entry has exceeded the configured timebox value. See the documentation on sessions for more information."},
  {ErrorCodes::SESSION_NOT_FOUND, "Session to which the API request relates no longer exists. This can occur if the user has signed out, or the session entry in the database was deleted in some other way."},
  {ErrorCodes::SIGNUP_DISABLED, "Sign ups (new account creation) are disabled on the server."},
  {ErrorCodes::SINGLE_IDENTITY_NOT_DELETABLE, "Every user must have at least one identity attached to it, so deleting (unlinking) an identity is not allowed if it's the only one for the user."},
  {ErrorCodes::SMS_SEND_FAILED, "Sending an SMS message failed. Check your SMS provider configuration."},
  {ErrorCodes::SSO_DOMAIN_ALREADY_EXISTS, "(Admin API.) Only one SSO domain can be registered per SSO identity provider."},
  {ErrorCodes::SSO_PROVIDER_NOT_FOUND, "SSO provider not found. Check the arguments in supabase.auth.signInWithSSO()."},
  {ErrorCodes::TOO_MANY_ENROLLED_MFA_FACTORS, "A user can only have a fixed number of enrolled MFA factors."},
  {ErrorCodes::UNEXPECTED_AUDIENCE, "(Deprecated feature not available via Supabase client libraries.) The request's X-JWT-AUD claim does not match the JWT's audience."},
  {ErrorCodes::UNEXPECTED_FAILURE, "Auth service is degraded or a bug is present, without a specific reason."},
  {ErrorCodes::USER_ALREADY_EXISTS, "User with this information (email address, phone number) cannot be created again as it already exists."},
  {ErrorCodes::USER_BANNED, "User to which the API request relates has a banned_until property which is still active. No further API requests should be attempted until this field is cleared."},
  {ErrorCodes::USER_NOT_FOUND, "User to which the API request relates no longer exists."},
  {ErrorCodes::USER_SSO_MANAGED, "When a user comes from SSO, certain fields of the user cannot be updated (like email)."},
  {ErrorCodes::VALIDATION_FAILED, "Provided parameters are not in the expected format."},
  {ErrorCodes::WEAK_PASSWORD, "User is signing up or changing their password without meeting the password strength criteria. Use the AuthWeakPasswordError class to access more information about what they need to do to make the password pass."},
  {ErrorCodes::UNKNOWN_ERROR, "UNKNOWN_ERROR"},
};

const std::map<std::string, ErrorCodes> ErrorCode::error_code_enums = {
  {"anonymous_provider_disabled", ErrorCodes::ANONYMOUS_PROVIDER_DISABLED},
  {"bad_code_verifier", ErrorCodes::BAD_CODE_VERIFIER},
  {"bad_json", ErrorCodes::BAD_JSON},
  {"bad_jwt", ErrorCodes::BAD_JWT},
  {"bad_oauth_callback", ErrorCodes::BAD_OAUTH_CALLBACK},
  {"bad_oauth_state", ErrorCodes::BAD_OAUTH_STATE},
  {"captcha_failed", ErrorCodes::CAPTCHA_FAILED},
  {"conflict", ErrorCodes::CONFLICT},
  {"email_address_invalid", ErrorCodes::EMAIL_ADDRESS_INVALID},
  {"email_address_not_authorized", ErrorCodes::EMAIL_ADDRESS_NOT_AUTHORIZED},
  {"email_conflict_identity_not_deletable", ErrorCodes::EMAIL_CONFLICT_IDENTITY_NOT_DELETABLE},
  {"email_exists", ErrorCodes::EMAIL_EXISTS},
  {"email_not_confirmed", ErrorCodes::EMAIL_NOT_CONFIRMED},
  {"email_provider_disabled", ErrorCodes::EMAIL_PROVIDER_DISABLED},
  {"flow_state_expired", ErrorCodes::FLOW_STATE_EXPIRED},
  {"flow_state_not_found", ErrorCodes::FLOW_STATE_NOT_FOUND},
  {"hook_payload_invalid_content_type", ErrorCodes::HOOK_PAYLOAD_INVALID_CONTENT_TYPE},
  {"hook_payload_over_size_limit", ErrorCodes::HOOK_PAYLOAD_OVER_SIZE_LIMIT},
  {"hook_timeout", ErrorCodes::HOOK_TIMEOUT},
  {"hook_timeout_after_retry", ErrorCodes::HOOK_TIMEOUT_AFTER_RETRY},
  {"identity_already_exists", ErrorCodes::IDENTITY_ALREADY_EXISTS},
  {"identity_not_found", ErrorCodes::IDENTITY_NOT_FOUND},
  {"insufficient_aal", ErrorCodes::INSUFFICIENT_AAL},
  {"invite_not_found", ErrorCodes::INVITE_NOT_FOUND},
  {"invalid_credentials", ErrorCodes::INVALID_CREDENTIALS},
  {"manual_linking_disabled", ErrorCodes::MANUAL_LINKING_DISABLED},
  {"mfa_challenge_expired", ErrorCodes::MFA_CHALLENGE_EXPIRED},
  {"mfa_factor_name_conflict", ErrorCodes::MFA_FACTOR_NAME_CONFLICT},
  {"mfa_factor_not_found", ErrorCodes::MFA_FACTOR_NOT_FOUND},
  {"mfa_ip_address_mismatch", ErrorCodes::MFA_IP_ADDRESS_MISMATCH},
  {"mfa_phone_enroll_not_enabled", ErrorCodes::MFA_PHONE_ENROLL_NOT_ENABLED},
  {"mfa_phone_verify_not_enabled", ErrorCodes::MFA_PHONE_VERIFY_NOT_ENABLED},
  {"mfa_totp_enroll_not_enabled", ErrorCodes::MFA_TOTP_ENROLL_NOT_ENABLED},
  {"mfa_totp_verify_not_enabled", ErrorCodes::MFA_TOTP_VERIFY_NOT_ENABLED},
  {"mfa_verification_failed", ErrorCodes::MFA_VERIFICATION_FAILED},
  {"mfa_verification_rejected", ErrorCodes::MFA_VERIFICATION_REJECTED},
  {"mfa_verified_factor_exists", ErrorCodes::MFA_VERIFIED_FACTOR_EXISTS},
  {"mfa_web_authn_enroll_not_enabled", ErrorCodes::MFA_WEB_AUTHN_ENROLL_NOT_ENABLED},
  {"mfa_web_authn_verify_not_enabled", ErrorCodes::MFA_WEB_AUTHN_VERIFY_NOT_ENABLED},
  {"no_authorization", ErrorCodes::NO_AUTHORIZATION},
  {"not_admin", ErrorCodes::NOT_ADMIN},
  {"oauth_provider_not_supported", ErrorCodes::OAUTH_PROVIDER_NOT_SUPPORTED},
  {"otp_disabled", ErrorCodes::OTP_DISABLED},
  {"otp_expired", ErrorCodes::OTP_EXPIRED},
  {"over_email_send_rate_limit", ErrorCodes::OVER_EMAIL_SEND_RATE_LIMIT},
  {"over_request_rate_limit", ErrorCodes::OVER_REQUEST_RATE_LIMIT},
  {"over_sms_send_rate_limit", ErrorCodes::OVER_SMS_SEND_RATE_LIMIT},
  {"phone_exists", ErrorCodes::PHONE_EXISTS},
  {"phone_not_confirmed", ErrorCodes::PHONE_NOT_CONFIRMED},
  {"phone_provider_disabled", ErrorCodes::PHONE_PROVIDER_DISABLED},
  {"provider_disabled", ErrorCodes::PROVIDER_DISABLED},
  {"provider_email_needs_verification", ErrorCodes::PROVIDER_EMAIL_NEEDS_VERIFICATION},
  {"reauthentication_needed", ErrorCodes::REAUTHENTICATION_NEEDED},
  {"reauthentication_not_valid", ErrorCodes::REAUTHENTICATION_NOT_VALID},
  {"refresh_token_not_found", ErrorCodes::REFRESH_TOKEN_NOT_FOUND},
  {"refresh_token_already_used", ErrorCodes::REFRESH_TOKEN_ALREADY_USED},
  {"request_timeout", ErrorCodes::REQUEST_TIMEOUT},
  {"same_password", ErrorCodes::SAME_PASSWORD},
  {"saml_assertion_no_email", ErrorCodes::SAML_ASSERTION_NO_EMAIL},
  {"saml_assertion_no_user_id", ErrorCodes::SAML_ASSERTION_NO_USER_ID},
  {"saml_entity_id_mismatch", ErrorCodes::SAML_ENTITY_ID_MISMATCH},
  {"saml_idp_already_exists", ErrorCodes::SAML_IDP_ALREADY_EXISTS},
  {"saml_idp_not_found", ErrorCodes::SAML_IDP_NOT_FOUND},
  {"saml_metadata_fetch_failed", ErrorCodes::SAML_METADATA_FETCH_FAILED},
  {"saml_provider_disabled", ErrorCodes::SAML_PROVIDER_DISABLED},
  {"saml_relay_state_expired", ErrorCodes::SAML_RELAY_STATE_EXPIRED},
  {"saml_relay_state_not_found", ErrorCodes::SAML_RELAY_STATE_NOT_FOUND},
  {"session_expired", ErrorCodes::SESSION_EXPIRED},
  {"session_not_found", ErrorCodes::SESSION_NOT_FOUND},
  {"signup_disabled", ErrorCodes::SIGNUP_DISABLED},
  {"single_identity_not_deletable", ErrorCodes::SINGLE_IDENTITY_NOT_DELETABLE},
  {"sms_send_failed", ErrorCodes::SMS_SEND_FAILED},
  {"sso_domain_already_exists", ErrorCodes::SSO_DOMAIN_ALREADY_EXISTS},
  {"sso_provider_not_found", ErrorCodes::SSO_PROVIDER_NOT_FOUND},
  {"too_many_enrolled_mfa_factors", ErrorCodes::TOO_MANY_ENROLLED_MFA_FACTORS},
  {"unexpected_audience", ErrorCodes::UNEXPECTED_AUDIENCE},
  {"unexpected_failure", ErrorCodes::UNEXPECTED_FAILURE},
  {"user_already_exists", ErrorCodes::USER_ALREADY_EXISTS},
  {"user_banned", ErrorCodes::USER_BANNED},
  {"user_not_found", ErrorCodes::USER_NOT_FOUND},
  {"user_sso_managed", ErrorCodes::USER_SSO_MANAGED},
  {"validation_failed", ErrorCodes::VALIDATION_FAILED},
  {"weak_password", ErrorCodes::WEAK_PASSWORD},
  {"unknown_error", ErrorCodes::UNKNOWN_ERROR},
};
// clang-format on
} // namespace supabase
