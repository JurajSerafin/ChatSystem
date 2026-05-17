#ifndef CLIENT_CHAT_SERVICE_H
#define CLIENT_CHAT_SERVICE_H

#include "Networking/i_rest_client.h"
#include "Repositories/i_local_chat_repository.h"
#include "Repositories/i_local_user_repository.h"
#include "Services/i_client_chat_service.h"

class ClientChatService : public IClientChatService {
public:

  explicit ClientChatService(IRestClient* restClientObs, ILocalChatRepository* chatRepoObs);

  std::vector<CachedChat> GetChats(std::size_t limit, std::size_t offset) override;

  CachedChat GetChatById(const ChatId& id) override;

  void CreateChat(const std::vector<UserId>& participantIds) override;

  std::vector<CachedUser> GetParticipants(const ChatId& chatId) override;

private:
  IRestClient* rest_client_obs_;
  ILocalChatRepository* chat_repo_obs_;
  ILocalUserRepository* user_repo_obs_;
};

inline ClientChatService::ClientChatService(IRestClient* restClientObs, ILocalChatRepository* chatRepoObs)
    : rest_client_obs_(restClientObs), chat_repo_obs_(chatRepoObs), user_repo_obs_(nullptr) {}

#endif // CLIENT_CHAT_SERVICE_H
