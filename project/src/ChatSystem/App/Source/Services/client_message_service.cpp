#include "client_message_service.h"

#include "API/chat_contract.h"
#include "Api/message_contract.h"
#include "client_services_utils.h"
#include "db_model_json_mapper.h"

#include <format>

ClientMessageService::ClientMessageService(
  IRestClient* restClientObs,
  IClientKeyManager* keyManagerObs,
  IClientEncryptionService* cryptoServiceObs,
  IClientChatService* chatServiceObs,
  IClientUserService* userServiceObs,
  ILocalMessageRepository* messageRepoObs,
  ILocalIdentityRepository* identityRepoObs
) : rest_client_obs_(restClientObs),
key_manager_obs_(keyManagerObs),
crypto_service_obs_(cryptoServiceObs),
chat_service_obs_(chatServiceObs),
user_service_obs_(userServiceObs),
message_repo_obs_(messageRepoObs),
identity_repo_obs_(identityRepoObs) {
}

void ClientMessageService::SendMessage(const ChatId& chatId, std::string_view plainText) {
  auto self_identity = identity_repo_obs_->Load();

  if (!self_identity.has_value()) {
    throw std::runtime_error("No authenticated user session active.");
  }

  std::vector<CachedUser> participants = chat_service_obs_->GetParticipants(chatId);

  std::string aes_key = crypto_service_obs_->GenerateSymmetricKey();

  std::string ciphertext = crypto_service_obs_->EncryptSymmetric(plainText, aes_key);

  nlohmann::json request_body = BuildSendMessagePayload(ciphertext, participants, aes_key);

  std::string path = std::string(api::message::routes::kChatMessages);

  InsertChatIdToPath(path, chatId);

  HttpResponse res = rest_client_obs_->Post(path, request_body);

  CachedMessage local_cache_msg = DBModelJsonMapper::ToMessage(res.body, std::string(plainText));

  message_repo_obs_->Upsert(local_cache_msg);
}

std::vector<CachedMessage> ClientMessageService::GetHistory(const ChatId& chatId, std::size_t limit, std::size_t offset) {
  const std::string chat_id_str = chatId.ToString();

  std::vector<CachedMessage> history = message_repo_obs_->FindByChatId(chat_id_str, limit, offset);

  if (!history.empty()) {
    return history;
  }

  std::string path = ConstructGetHistoryQuery(chatId, limit, offset);

  HttpResponse res = rest_client_obs_->Get(path);

  for (auto&& msg_json : res.body) {
    std::string msg_id = msg_json.at(api::message::fields::kId).get<std::string>();

    std::string cleartext = DecryptIncomingMessage(msg_json, msg_id);

    CachedMessage cached_msg = DBModelJsonMapper::ToMessage(msg_json, std::move(cleartext));

    message_repo_obs_->Upsert(cached_msg);

    history.push_back(std::move(cached_msg));
  }

  return history;
}

void ClientMessageService::MarkAsRead(const MessageId& messageId) {
  const std::string msg_id_str = messageId.ToString();

  const std::string path = client::services::utils::ResolveIdRoute(api::message::routes::kReadMessage, msg_id_str, api::message::path_params::kMessageId);

  rest_client_obs_->Post(path);

  message_repo_obs_->MarkAsRead(msg_id_str);
}

std::vector<CachedMessage> ClientMessageService::GetUndelivered() {
  HttpResponse res = rest_client_obs_->Get(api::message::routes::kGetUndelivered);

  std::vector<CachedMessage> fresh_messages;

  for (auto&& msg_json : res.body) {
    std::string msg_id = msg_json.at(api::message::fields::kId).get<std::string>();

    std::string cleartext = DecryptIncomingMessage(msg_json, msg_id);

    CachedMessage cached_msg = DBModelJsonMapper::ToMessage(msg_json, std::move(cleartext));

    message_repo_obs_->SaveForChat(cached_msg);

    fresh_messages.push_back(std::move(cached_msg));
  }

  return fresh_messages;
}

std::vector<CachedMessage> ClientMessageService::SearchMessages(const ChatId& chatId, std::string_view keywords) {
  return message_repo_obs_->Search(chatId.ToString(), keywords);
}

std::string ClientMessageService::DecryptIncomingMessage(const nlohmann::json& msgJson, const std::string& msgIdStr) {

  const std::string key_path = client::services::utils::ResolveIdRoute(api::message::routes::kMessageKey, msgIdStr, api::message::path_params::kMessageId);

  HttpResponse key_res = rest_client_obs_->Get(key_path);

  std::string wrapped_key = key_res.body.at(api::message::fields::kEncryptedKey).get<std::string>();

  std::string self_private_key = key_manager_obs_->GetPrivateKey();

  std::string aes_key = crypto_service_obs_->UnwrapKey(wrapped_key, self_private_key);

  std::string cipher_text = msgJson.at(api::message::fields::kCiphertext).get<std::string>();

  return crypto_service_obs_->DecryptSymmetric(cipher_text, aes_key);
}
nlohmann::json ClientMessageService::BuildSendMessagePayload(
  std::string_view ciphertext,
  const std::vector<CachedUser>& participants,
  std::string_view aesKey) const 
{

  nlohmann::json encrypted_keys_map = nlohmann::json::object();

  for (auto&& user : participants) {
    std::string pub_key = user_service_obs_->GetPublicKey(UserId::Reconstitute(user.id));

    std::string wrapped = crypto_service_obs_->WrapKey(aesKey, pub_key);

    encrypted_keys_map[user.id] = wrapped;
  }

  return  {
    {api::message::fields::kCiphertext, ciphertext},
    {api::message::fields::kEncryptedKeys, encrypted_keys_map},
    {api::message::fields::kType, TextMessageType::TypeString()}
  }; 
}

void ClientMessageService::InsertChatIdToPath(std::string& path, const ChatId& chatId) {
  path = client::services::utils::ResolveIdRoute(
    api::message::routes::kChatMessages,
    chatId.ToString(),
    api::chat::path_params::kChatId
  );
}

std::string ClientMessageService::ConstructGetHistoryQuery(
  const ChatId& chatId,
  std::size_t limit,
  std::size_t offset) {
  return std::format("/chats/{}/messages?limit={}&offset={}", chatId.ToString(), limit, offset);;
}