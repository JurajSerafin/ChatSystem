#include <Message/message_validator.h>

#include <Validation/validation.h>

using namespace validation::rules;

validation::ValidationResult<MessageValidator::kMaxErrors> MessageValidator::Validate(const MessageParams& params) const {
  auto rules =
    (BIND_FIELD(MessageParams, id) | GetIdRule()) &&
    (BIND_FIELD(MessageParams, chat_id) | GetChatIdRule()) &&
    (BIND_FIELD(MessageParams, sender_id) | GetSenderIdRule()) &&
    (BIND_FIELD(MessageParams, content) | GetContentRule()) &&
    (BIND_FIELD(MessageParams, type) | GetTypeRule()) &&
    (BIND_FIELD(MessageParams, created_at) | GetCreatedAtRule());

  return rules(params);
}

constexpr auto MessageValidator::GetIdRule() {
  return IsValid;
}

constexpr auto MessageValidator::GetChatIdRule() {
  return IsValid;
}

constexpr auto MessageValidator::GetSenderIdRule() {
  return IsValid;
}

constexpr auto MessageValidator::GetContentRule() {
  return NotEmpty && MaxLength<kMaxContentLength>;
}

constexpr auto MessageValidator::GetTypeRule() {
  return TrueRule;
}

constexpr auto MessageValidator::GetCreatedAtRule() {
  return TimePointMustBeSet;
}
