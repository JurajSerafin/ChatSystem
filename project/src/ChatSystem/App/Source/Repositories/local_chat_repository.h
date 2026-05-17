#ifndef LOCAL_CHAT_REPOSITORY_H
#define LOCAL_CHAT_REPOSITORY_H

#include "Database/i_local_database.h"
#include "Repositories/i_local_chat_repository.h"

class LocalChatRepository : public ILocalChatRepository {
public:
  explicit LocalChatRepository(ILocalDatabase* db);

  std::vector<CachedChat> FindAll() override;

  std::optional<CachedChat> FindById(std::string_view chatId) override;

  void Upsert(const CachedChat& chat) override;

  void UpdateLastMessage(std::string_view chatId, std::string_view messageId, int64_t lastActivityMs) override;

  void Delete(std::string_view chatId) override;

  void AddParticipant(std::string_view userId, std::string_view chatId, const UserRoleVariant& role) override;

  std::vector<std::string> GetParticipantIds(std::string_view chatId) override;

  void RemoveParticipant(std::string_view userId, std::string_view chatId) override;

private:
  ILocalDatabase* db_obs_;
};

inline LocalChatRepository::LocalChatRepository(ILocalDatabase* db) : db_obs_(db) {}

#endif // LOCAL_CHAT_REPOSITORY_H