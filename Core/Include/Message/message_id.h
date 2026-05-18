#ifndef MESSAGE_ID
#define MESSAGE_ID

#include "Id/base_id.h"

/**
 * @brief Strongly typed identifier for messages.
 *
 * It is a thin wrapper over BaseId providing type safety.
 *
 * Instances are created via BaseId::Generate() or BaseId::FromString().
 */
class MessageId : public BaseId<MessageId> {
  friend BaseId;

protected:
  /**
   * @brief Constructs a MessageId from a string representation.
   *
   * This constructor is protected to enforce controlled creation
   * through BaseId factory methods.
   *
   * @param id Raw identifier string.
   */
  explicit MessageId(std::string&& id) : BaseId(std::move(id)) {}
};

#endif // MESSAGE_ID