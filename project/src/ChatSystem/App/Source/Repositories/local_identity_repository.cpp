#include "local_identity_repository.h"

void LocalIdentityRepository::Store(const CachedIdentity& identity) {
  db_obs_->SaveIdentity(identity);
}

std::optional<CachedIdentity> LocalIdentityRepository::Load() {
  return db_obs_->LoadIdentity();
}

void LocalIdentityRepository::UpdateSessionToken(std::string_view newToken) {
  auto identity = db_obs_->LoadIdentity();

  if (!identity.has_value()) {
    throw std::runtime_error("Identity is not yet stored in cache, thus its session token cannot be updated.");
  }

  identity->session_token = newToken;

  db_obs_->SaveIdentity(*identity);
}

void LocalIdentityRepository::Clear() {
  db_obs_->ClearIdentity();
}
