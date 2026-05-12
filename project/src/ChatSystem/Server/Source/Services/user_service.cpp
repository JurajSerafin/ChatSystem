#include "Services/DTOs/user_profile.h"
#include "User/user.h"

#include <Services/user_service.h>
#include <stdexcept>

UserService::UserService(IUserRepository* userRepoObs) : user_repo_obs_(userRepoObs) {
  if (!user_repo_obs_) {
    throw std::invalid_argument("UserService requires IUserRepository");
  }
}

UserProfile UserService::MapToProfile(const User& user) {

  return UserProfile{
    .id = user.GetId().ToString(),
    .tag = user.GetTag().ToString(),
    .login = user.GetLogin(),
    .role = std::string{user.GetRoleStr()}
  };
}

std::vector<UserProfile> UserService::Search(
  const std::string& query,
  const UserId& requestingUserId,
  std::size_t limit,
  std::size_t offset
) {

  // if the requestingUserId is in the result set, and we filter them out we still return
  // the requested amount of items to the client
  const std::vector<User> domain_users = user_repo_obs_->Search(query, limit + 1, offset);

  std::vector<UserProfile> profiles;
  profiles.reserve(limit);

  for (auto&& user : domain_users) {

    if (profiles.size() == limit) {
      break;
    }

    if (user.GetId() == requestingUserId) {
      continue;
    }

    profiles.push_back(MapToProfile(user));
  }

  return profiles;
}

std::optional<UserProfile> UserService::GetById(const UserId& userId) {
  std::optional<User> user = user_repo_obs_->FindById(userId);
  
  if (!user) {
    return std::nullopt;
  }

  return MapToProfile(*user);
}

std::optional<UserProfile> UserService::GetByTag(const tags::UserTag& tag) {
  std::optional<User> user = user_repo_obs_->FindByTag(tag);
  
  if (!user) {
    return std::nullopt;
  }

  return MapToProfile(*user);
}
std::optional<std::string> UserService::GetPublicKey(const UserId& userId) {
  std::optional<User> user = user_repo_obs_->FindById(userId);
  
  if (!user) {
    return std::nullopt;
  }

  return user->GetPublicKey();
}