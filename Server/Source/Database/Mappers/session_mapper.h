#ifndef SESSION_MAPPER_H
#define SESSION_MAPPER_H

#include <Session/session.h>

class IRow;

/**
 * @brief Utility class for mapping raw database rows to Session domain entities.
 */
class SessionMapper {
public:
  /**
   * @brief Deserializes an IRow into a Session object.
   * @param row The database row containing session data (token, user_id, expiration, etc.).
   * @return A fully defined Session domain object.
   */
  [[nodiscard]] static Session Map(const IRow& row);
};

#endif // SESSION_MAPPER_H