#ifndef MESSAGE_CONTROLLER_H
#define MESSAGE_CONTROLLER_H

#include "Networking/router.h"
#include "Services/Interface/i_auth_service.h"
#include "Services/Interface/i_message_service.h"

#include <nlohmann/json.hpp>
#include <optional>

#include <boost/beast/http.hpp>

/**
 * @brief Presentation layer controller for handling Message routing and retrieval endpoints.
 *
 * Manages the HTTP interface for End-to-End Encrypted (E2EE) messaging. It handles
 * posting new ciphertexts, retrieving message histories via offset pagination, fetching
 * undelivered messages via cursor pagination, and retrieving wrapped AES keys.
 */
class MessageController {
public:
  explicit MessageController(IAuthService* authServiceObs, IMessageService* messageServiceObs);

  /**
   * @brief Persists and routes an E2EE message to a specific chat.
   * @route POST /chats/{id}/messages
   * @body { "ciphertext": "...", "encrypted_keys": {...}, "type": "..." }
   */
  http::response<http::string_body> HandleSendMessage(const http::request<http::string_body>& req, const Router::PathParams& params) const;

  /**
   * @brief Retrieves a paginated history of messages for a specific chat.
   * @route GET /chats/{id}/messages?limit=&offset=
   */
  http::response<http::string_body> HandleGetMessageHistory(const http::request<http::string_body>& req, const Router::PathParams& params) const;

  /**
   * @brief Retrieves the specific encrypted AES key for the requesting user to decrypt a message.
   * @route GET /messages/{id}/key
   */
  http::response<http::string_body> HandleGetMessagePublicKey(const http::request<http::string_body>& req, const Router::PathParams& params) const;

  /**
   * @brief Marks a specific message as read by the requesting user.
   * @route PUT /messages/{id}/read
   */
  http::response<http::string_body> HandleReadMessage(const http::request<http::string_body>& req, const Router::PathParams& params) const;

  /**
   * @brief Fetches offline/undelivered messages using cursor-based pagination.
   * @route GET /messages/undelivered?limit=&afterId=
   */
  http::response<http::string_body> HandleGetUndelivered(const http::request<http::string_body>& req, const Router::PathParams& params) const;

  /**
   * @brief Deletes a message (restricted to the original sender).
   * @route DELETE /messages/{id}
   */
  http::response<http::string_body> HandleDeleteMessage(const http::request<http::string_body>& req, const Router::PathParams& params) const;

  void AddRoutes(Router& router);

private:
  std::optional<UserId> GetAuthenticatedUserId(const http::request<http::string_body>& req) const;

  static void ExtractCursorPagination(const http::request<http::string_body>& req, std::size_t& limitOut, std::optional<MessageId>& afterIdOut);

  static nlohmann::json FormatJsonOutput(const Message& message);

  static nlohmann::json FormatJsonOutput(const std::vector<Message>& messages);

  static EncryptedKeysMap ExtractEncryptedKeys(const nlohmann::json& reqBody);

  static std::string ExtractCiphertext(const nlohmann::json& reqBody);

  IAuthService* auth_service_obs_;
  IMessageService* message_service_obs_;
};

inline MessageController::MessageController(IAuthService* authServiceObs, IMessageService* messageServiceObs)
  : auth_service_obs_(authServiceObs), message_service_obs_(messageServiceObs) {
}

#endif // MESSAGE_CONTROLLER_H