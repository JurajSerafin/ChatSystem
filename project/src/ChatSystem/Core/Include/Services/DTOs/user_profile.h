#ifndef USER_PROFILE_H
#define USER_PROFILE_H

#include <string>

/**
 * @brief Data Transfer Object (DTO) representing a public user profile.
 *
 * Flattened to plain strings, so the controller layer can easily serialize
 * it to JSON without knowing about domain's role variant or custom Tag types.
 */
struct UserProfile {
  std::string id;

  std::string tag;

  std::string login;

  std::string role;
};

#endif // USER_PROFILE_H