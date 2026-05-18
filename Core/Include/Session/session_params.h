#ifndef SESSION_PARAMS_H
#define SESSION_PARAMS_H

#include <Session/session_id.h>
#include <User/user_id.h>
#include <chrono>
#include <string>

/**
 * @brief Data transfer object for session creation and validation.
 *
 * SessionParams aggregates all data required to construct a Session.
 * It is typically passed to a validator before being used to create
 * a domain object.
 *
 * Validation rules (e.g. non-empty token, valid timestamps) are enforced
 * externally via an IValidator<SessionParams> implementation.
 */
struct SessionParams {
  // Required

  SessionId id;

  UserId user_id;

  std::string token;

  std::chrono::system_clock::time_point expires_at;

  std::chrono::system_clock::time_point created_at;

  // Optional
};

#endif  // SESSION_PARAMS_H