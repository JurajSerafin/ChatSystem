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

class ClientMessageService : public IClientMessageService {
public:
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

  std::string DecryptIncomingMessage(const nlohmann::json& msgJson, const std::string& msgIdStr);

  nlohmann::json BuildSendMessagePayload(std::string_view ciphertext, const std::vector<CachedUser>& participants, std::string_view aesKey) const;

  static void InsertChatIdToPath(std::string& path, const ChatId& chatId);

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