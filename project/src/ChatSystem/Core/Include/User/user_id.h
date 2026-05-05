#ifndef USER_ID_H
#define USER_ID_H

#include "Id/base_id.h"

/**
 * @brief Strongly typed identifier for users.
 *
 * UserId is a type-safe wrapper over BaseId used to uniquely identify users
 * across the system. 
 * 
 * It is a thin wrapper over BaseId providing type safety.
 *
 * Instances are created via BaseId::Generate() or BaseId::FromString().
 */
class UserId : public BaseId<UserId> {
  friend BaseId;

protected:
  /**
   * @brief Constructs a UserId from a string representation.
   *
   * This constructor is protected to enforce controlled creation
   * through BaseId factory methods.
   *
   * @param id Raw identifier string.
   */
  explicit UserId(std::string&& id) : BaseId(std::move(id)) {}
};

#endif // USER_ID_H