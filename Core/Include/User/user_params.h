#ifndef USER_PARAMS_H
#define USER_PARAMS_H

#include <Tags/user_tag.h>
#include <User/user_id.h>
#include <chrono>
#include <string>

/**
 * @brief DTO for user creation and validation.
 *
 * UserParams aggregates all data required to construct or update a User.
 */
struct UserParams {
  // Required

  UserId id;

  tags::UserTag tag;

  std::string login;

  std::string password_hash;

  std::string public_key;

  UserRoleVariant role;

  std::chrono::system_clock::time_point created_at;


  // Optional
};

#endif  // USER_PARAMS_H