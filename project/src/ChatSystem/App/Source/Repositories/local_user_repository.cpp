#include "local_user_repository.h"

std::optional<CachedUser> LocalUserRepository::FindById(std::string_view userId) {
  return db_obs_->LoadUser(userId);
}

std::optional<CachedUser> LocalUserRepository::FindByLoginOrTag(std::string_view login, std::string_view tag) {
  return db_obs_->LoadUserByLoginOrTag(login, tag);
}

void LocalUserRepository::Upsert(const CachedUser& user) {
  db_obs_->UpsertUser(user);
}

void LocalUserRepository::Delete(std::string_view userId) {
  db_obs_->DeleteUser(userId);
}




