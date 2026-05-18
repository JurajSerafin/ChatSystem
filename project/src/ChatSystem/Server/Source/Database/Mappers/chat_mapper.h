#ifndef CHAT_MAPPER_H
#define CHAT_MAPPER_H

#include <Chat/chat.h>
#include <User/user_id.h>
#include <vector>

class IRow;

/**
 * @brief Utility class for mapping raw database rows to Chat domain entities.
 */
class ChatMapper {
public:
  /**
   * @brief Deserializes an IRow and its participant list into a domain Chat object.
   * @param row The database row containing the core chat metadata (id, created_at, etc.).
   * @param participants The fetched list of user IDs participating in the chat.
   * @return A fully defined Chat domain object.
   */
  [[nodiscard]] static Chat Map(const IRow& row, std::vector<UserId> participants);
};

#endif // CHAT_MAPPER_H