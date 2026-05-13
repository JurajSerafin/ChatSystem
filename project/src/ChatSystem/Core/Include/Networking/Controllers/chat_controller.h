#ifndef CHAT_CONTROLLER_H
#define CHAT_CONTROLLER_H

#include "Networking/router.h"
#include "Services/i_auth_service.h"
#include "Services/i_chat_service.h"
#include "Session/session_validator.h"
#include "User/user_validator.h"
#include <nlohmann/json.hpp>

#include <string_view>
#include <optional>
#include <boost/beast/http.hpp>


/**
 * @brief Presentation layer controller for handling all Chat-related REST API endpoints.
 *
 * The ChatController acts as the translation boundary between the raw HTTP network
 * traffic and the application's core business logic. It is strictly responsible for:
 * 1. Extracting and validating Bearer authentication tokens.
 * 2. Parsing incoming JSON bodies, URL path parameters, and query parameters.
 * 3. Delegating the actual business operations to the underlying services.
 * 4. Serializing C++ domain entities (like Chat and UserProfile) back into
 * standardized JSON HTTP responses.
 */
class ChatController {
public:
  explicit ChatController(IAuthService* authServiceObs, IChatService* chatServiceObs);

  // POST /chats
  http::response<http::string_body> HandleCreateChat(const http::request<http::string_body>& req, const Router::PathParams& params) const;

  // GET /chats?limit=&offset=
  http::response<http::string_body> HandleGetChats(const http::request<http::string_body>& req, const Router::PathParams& params) const;

  // GET /chats/{id}
  http::response<http::string_body> HandleGetChatById(const http::request<http::string_body>& req, const Router::PathParams& params) const;

  // GET /chats/{id}/participants
  http::response<http::string_body> HandleGetParticipants(const http::request<http::string_body>& req, const Router::PathParams& params) const;

  // POST /chats/{id}/participants
  http::response<http::string_body> HandleAddParticipant(const http::request<http::string_body>& req, const Router::PathParams& params) const;

  // DELETE /chats/{id}/participants/{user_id}
  http::response<http::string_body> HandleRemoveParticipant(const http::request<http::string_body>& req, const Router::PathParams& params) const;

  // DELETE /chats/{id}
  http::response<http::string_body> HandleDeleteChat(const http::request<http::string_body>& req, const Router::PathParams& params) const;

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