#ifndef CLIENT_MESSAGE_SERVICE_H
#define CLIENT_MESSAGE_SERVICE_H

#include "Cryptography/i_client_encryption_service.h"
#include "Cryptography/i_client_key_manager.h"
#include "Networking/i_rest_client.h"
#include "Repositories/i_local_message_repository.h"
#include "Repositories/i_local_identity_repository.h"
#include "Services/i_client_chat_service.h"
#include "Services/i_client_user_service.h"
#include "Services/i_client_message_service.h"

/**
 * @brief Concrete implementation of the client message service.
 * * Orchestrates the complex End-to-End Encryption (E2EE) pipeline, mapping outgoing
 * texts to symmetric keys and safely unwrapping incoming JSON payloads into the local cache.
 */
class ClientMessageService : public IClientMessageService {
public:
  /**
   * @brief Constructs the message service, linking together network, crypto, and database layers.
   */
  explicit ClientMessageService(
    IRestClient* restClientObs,
    IClientKeyManager* keyManagerObs,
    IClientEncryptionService* cryptoServiceObs,
    IClientChatService* chatServiceObs,
    IClientUserService* userServiceObs,
    ILocalMessageRepository* messageRepoObs,
    ILocalIdentityRepository* identityRepoObs
  );

  void SendMessage(const ChatId& chatId, std::string_view plainText) override;
  std::vector<CachedMessage> GetHistory(const ChatId& chatId, std::size_t limit, std::size_t offset) override;
  void MarkAsRead(const MessageId& messageId) override;
  std::vector<CachedMessage> GetUndelivered() override;
  std::vector<CachedMessage> SearchMessages(const ChatId& chatId, std::string_view keywords) override;

private:
  /**
   * @brief Fetches an encrypted message key from the network, unwraps it, and decrypts the ciphertext.
   * @param msgJson The raw JSON payload representing the locked message.
   * @param msgIdStr The specific message ID string used to fetch the wrapped key.
   * @return The fully decrypted plaintext string.
   */
  std::string DecryptIncomingMessage(const nlohmann::json& msgJson, const std::string& msgIdStr);

  /**
   * @brief Packages ciphertext and asymmetrically wrapped keys into a server-ready JSON payload.
   * @param ciphertext The symmetrically encrypted message text.
   * @param participants The list of users in the chat room receiving the message.
   * @param aesKey The single symmetric key to be wrapped for each participant.
   * @return The finalized JSON dictionary payload.
   */
  nlohmann::json BuildSendMessagePayload(std::string_view ciphertext, const std::vector<CachedUser>& participants, std::string_view aesKey) const;

  /**
   * @brief In-place utility to swap the '{id}' template token in a route with a real ChatId.
   */
  static void InsertChatIdToPath(std::string& path, const ChatId& chatId);

  /**
   * @brief Formats a paginated network query string for retrieving message history.
   */
  static std::string ConstructGetHistoryQuery(const ChatId& chatId, std::size_t limit, std::size_t offset);

  IRestClient* rest_client_obs_;
  IClientKeyManager* key_manager_obs_;
  IClientEncryptionService* crypto_service_obs_;
  IClientChatService* chat_service_obs_;
  IClientUserService* user_service_obs_;
  ILocalMessageRepository* message_repo_obs_;
  ILocalIdentityRepository* identity_repo_obs_;
};

#endif // CLIENT_MESSAGE_SERVICE_H