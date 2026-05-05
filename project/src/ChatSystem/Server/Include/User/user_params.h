#ifndef USER_PARAMS_H
#define USER_PARAMS_H

#include <Tags/user_tag.h>
#include <User/i_user_role.h>
#include <User/user_id.h>
#include <memory>
#include <string>
#include <chrono>

/**
 * @brief Data transfer object for user creation and validation.
 *
 * UserParams aggregates all data required to construct or update a User.
 * It is typically passed to a validator before being used to create
 * a domain object.
 *
 * Validation rules (e.g. non-empty fields, length constraints, valid role)
 * are enforced externally via an IValidator<UserParams> implementation.
 */
struct UserParams {
  // Required

  /// Unique identifier of the user.
  UserId id;

  /// User tag.
  tags::UserTag tag;

  /// User login name.
  std::string login;

  /// Hashed password.
  std::string password_hash;

  /// Public key associated with the user).
  std::string public_key;

  /// User role defining permissions.
  std::unique_ptr<IUserRole> role;

  std::chrono::system_clock::time_point created_at;


  // Optional
};

#endif  // USER_PARAMS_H