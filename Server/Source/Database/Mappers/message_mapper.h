#ifndef MESSAGE_MAPPER_H
#define MESSAGE_MAPPER_H

#include <Message/message.h>

class IRow;

/**
 * @brief Utility class for mapping raw database rows to Message domain entities.
 */
class MessageMapper {
public:
  /**
   * @brief Deserializes an IRow into a Message object.
   * @param row The database row containing the message fields (id, ciphertext, sender_id, etc.).
   * @return A fully defined Message domain object.
   */
  [[nodiscard]] static Message Map(const IRow& row);
};

#endif // MESSAGE_MAPPER_H