#ifndef SESSION_ID_H
#define SESSION_ID_H

#include <Id/base_id.h>

/**
 * @brief Strongly typed identifier for sessions.
 *
 * SessionId represents the unique identifier of a user session.
 * 
 * It is a thin wrapper over BaseId providing type safety.
 * 
 * Instances are created via BaseId::Generate() or BaseId::FromString().
 */
class SessionId : public BaseId<SessionId> {
  friend class BaseId;

protected:
  /**
    * @brief Constructs a SessionId from a string representation.
    *
    * This constructor is protected to enforce controlled creation
    * through BaseId factory methods.
    *
    * @param id Raw identifier string.
    */
  explicit SessionId(std::string id) : BaseId{ std::move(id) } {}
};

#endif  // SESSION_ID_H