#ifndef CHAT_CONTROLLER_H
#define CHAT_CONTROLLER_H

#include "Networking/router.h"
#include "Services/i_auth_service.h"
#include "Services/i_chat_service.h"
#include "Session/session_validator.h"
#include "User/user_validator.h"

#include <nlohmann/json.hpp>
#include <optional>

#include <boost/beast/http.hpp>

/**
 * @brief Presentation layer controller for Chat room management and participant tracking endpoints.
 *
 * Exposes the HTTP interface for creating and deleting End-to-End Encrypted chat rooms,
 * retrieving paginated lists of a user's active chats, and managing chat participants
 * (adding, removing, and listing members).
 */
class ChatController {
public:
  explicit ChatController(IAuthService* authServiceObs, IChatService* chatServiceObs);

  /**
   * @brief Creates a new chat room with the specified participants.
   * @route POST /chats
   * @body { "participant_ids": ["uuid-1", "uuid-2"] }
   */
  http::response<http::string_body> HandleCreateChat(const http::request<http::string_body>& req, const Router::PathParams& params) const;

  /**
   * @brief Retrieves a paginated list of all chats the authenticated user is a member of.
   * @route GET /chats?limit=&offset=
   */
  http::response<http::string_body> HandleGetChats(const http::request<http::string_body>& req, const Router::PathParams& params) const;

  /**
   * @brief Retrieves the metadata and details of a specific chat by its ID.
   * @route GET /chats/{id}
   */
  http::response<http::string_body> HandleGetChatById(const http::request<http::string_body>& req, const Router::PathParams& params) const;

  /**
   * @brief Fetches the list of user profiles for everyone currently in the chat.
   * @route GET /chats/{id}/participants
   */
  http::response<http::string_body> HandleGetParticipants(const http::request<http::string_body>& req, const Router::PathParams& params) const;

  /**
   * @brief Adds a new user to an existing chat.
   * @route POST /chats/{id}/participants
   * @body { "user_id": "uuid-to-add" }
   */
  http::response<http::string_body> HandleAddParticipant(const http::request<http::string_body>& req, const Router::PathParams& params) const;

  /**
   * @brief Removes a user from a chat (or allows a user to leave the chat).
   * @route DELETE /chats/{id}/participants/{user_id}
   */
  http::response<http::string_body> HandleRemoveParticipant(const http::request<http::string_body>& req, const Router::PathParams& params) const;

  /**
   * @brief Deletes a chat entirely for all users.
   * @route DELETE /chats/{id}
   */
  http::response<http::string_body> HandleDeleteChat(const http::request<http::string_body>& req, const Router::PathParams& params) const;

  void AddRoutes(Router& router);

private:
  std::optional<UserId> GetAuthenticatedUserId(const http::request<http::string_body>& req) const;

  static std::vector<UserId> ExtractParticipantIds(const nlohmann::json& reqBody);

  static nlohmann::json FormatJsonOutput(const Chat& chat);

  static nlohmann::json FormatJsonOutput(const std::vector<Chat>& chats);

  static nlohmann::json FormatJsonOutput(const std::vector<User>& participants);

  IAuthService* auth_service_obs_;
  IChatService* chat_service_obs_;

  UserValidator user_validator_;
  SessionValidator session_validator_;
};

inline ChatController::ChatController(IAuthService* authServiceObs, IChatService* chatServiceObs)
  : auth_service_obs_(authServiceObs), chat_service_obs_(chatServiceObs) {
}

#endif // CHAT_CONTROLLER_H