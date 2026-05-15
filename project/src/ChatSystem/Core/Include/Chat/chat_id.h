#ifndef CHAT_ID_H
#define CHAT_ID_H

#include <Id/base_id.h>

/**
 * @class ChatId
 * 
 * @brief Strongly typed identifier for chats.
 *
 * ChatId represents a unique identifier for a chat entity in the system.
 *
 * It is a thin wrapper over BaseId providing type safety.
 *
 * Instances are created via BaseId::Generate() or BaseId::FromString().
 */
class ChatId : public BaseId<ChatId> {
  friend BaseId;

protected:
  /**
   * @brief Constructs a ChatId from a string representation.
   *
   * This constructor is protected to enforce controlled creation
   * through BaseId factory methods.
   *
   * @param id Raw identifier string.
   */
  explicit ChatId(std::string&& id) : BaseId(std::move(id)) {}
};

#endif // CHAT_ID_H