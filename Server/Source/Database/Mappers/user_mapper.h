#ifndef USER_MAPPER_H
#define USER_MAPPER_H

#include <User/user.h>

class IRow;

/**
 * @brief Utility class for mapping raw database rows to User domain entities.
 */
class UserMapper {
public:
  /**
   * @brief Deserializes an IRow into a User object.
   * @param row The database row containing user profile fields (id, login, tag, public_key, etc.).
   * @return A fully defined User domain object.
   */
  [[nodiscard]] static User Map(const IRow& row);
};

#endif // USER_MAPPER_H