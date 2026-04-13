#ifndef USER_PARAMS_H
#define USER_PARAMS_H

#include <Tags/tag.h>
#include <User/i_user_role.h>
#include <User/user_id.h>
#include <memory>
#include <string>

/**
 * @brief Data transfer object containing user parameters.
 *
 * This struct aggregates all data required to create or update a user.
 * It is typically used as input for validation and business logic.
 */
struct UserParams {
  // Required

  /// Unique identifier of the user.
  UserId id;

  /// User tag.
  Tag tag;

  /// User login name.
  std::string login;

  /// Hashed password.
  std::string password_hash;

  /// Public key associated with the user.
  std::string public_key;

  /// User role defining permissions.
  std::unique_ptr<IUserRole> role;

  // Optional
};

#endif  // USER_PARAMS_H