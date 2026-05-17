#include "client_chat_service.h"

#include "Api/chat_contract.h"
#include "User/Roles/user_role_variant.h"
#include "client_services_utils.h"
#include "db_model_json_mapper.h"

#include <format>

std::vector<CachedChat> ClientChatService::GetChats(std::size_t limit, std::size_t offset) {
  const std::string path = std::format("{}?limit={}&offset={}",
    api::chat::routes::kBase,
    limit,
    offset
  );

  const HttpResponse res = rest_client_obs_->Get(path);

  std::vector<CachedChat> chat_list;

  for (auto&& chat_json : res.body) {

    CachedChat cached_chat = DBModelJsonMapper::ToChat(chat_json);

    chat_repo_obs_->Upsert(cached_chat);

    chat_list.emplace_back(std::move(cached_chat));
  }

  return chat_list;
}

CachedChat ClientChatService::GetChatById(const ChatId& id) {
  return client::services::utils::GetOrFetchById<CachedChat>(
    id,
    chat_repo_obs_,
    rest_client_obs_,
    [](auto&& body) { return DBModelJsonMapper::ToChat(body); },
    api::chat::routes::kById,
    api::chat::path_params::kChatId
  );
}

void ClientChatService::CreateChat(const std::vector<UserId>& participantIds) {
  nlohmann::json json_array = nlohmann::json::array();

  for (auto&& uid : participantIds) {
    json_array.push_back(uid.ToString());
  }

  nlohmann::json request_body = {
    {api::chat::fields::kParticipantIds, json_array}
  };

  const HttpResponse res = rest_client_obs_->Post(api::chat::routes::kBase, request_body);

  CachedChat new_cached_chat = DBModelJsonMapper::ToChat(res.body);
  const std::string chat_id_str = new_cached_chat.id;

  chat_repo_obs_->Upsert(new_cached_chat);

  UserRoleVariant default_role{ RegularUserRole{} };

  for (auto&& uid : participantIds) {
    chat_repo_obs_->AddParticipant(chat_id_str, uid.ToString(), default_role);
  }
}

std::vector<CachedUser> ClientChatService::GetParticipants(const ChatId& chatId) {
  return client::services::utils::GetOrFetchCollection<CachedUser>(
    chatId,
    chat_repo_obs_,
    rest_client_obs_,

    // JSON -> vector Mapper Lambda
    [](const nlohmann::json& body) {
      std::vector<CachedUser> parsed_users;

      for (const auto& item : body) {
        parsed_users.push_back(DBModelJsonMapper::ToUser(item));
      }

      return parsed_users;
    },
   api::chat::routes::kParticipants,
    api::chat::path_params::kChatId,

    // Cache Read Lambda
    [this](std::string_view chatIdStr) {
      std::vector<CachedUser> resolved_profiles;

      std::vector<std::string> raw_ids = chat_repo_obs_->GetParticipantIds(chatIdStr);

      for (const auto& uid_str : raw_ids) {
        if (auto user_opt = user_repo_obs_->FindById(uid_str)) {
          resolved_profiles.push_back(std::move(*user_opt));
        }
      }

      return resolved_profiles;
    },

    // Cache Write Lambda
    [this, &chatId](const std::vector<CachedUser>& fetchedUsers) {
      const std::string& chat_id_str = chatId.ToString();

      UserRoleVariant default_role{};

      for (auto&& user : fetchedUsers) {
        user_repo_obs_->Upsert(user);

        chat_repo_obs_->AddParticipant(chat_id_str, user.id, default_role);
      }
    }
  );
}