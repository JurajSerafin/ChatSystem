#ifndef MESSAGE_VALIDATOR_FOR_H
#define MESSAGE_VALIDATOR_FOR_H

#include "message_id.h"
#include "Types/message_type_variant.h"


#include <Chat/chat_id.h>
#include <User/user_id.h>
#include <Validation/validation.h>
#include <chrono>
#include <concepts>
#include <type_traits>

/**
 * @brief Concept defining the requirements for a Message validator.
 * Extends the basic `ValidatorFor` concept to ensure the validator also provides
 * access to individual field rules. This allows domain objects to validate
 * individual fields during state mutations (e.g., `SetId`) without
 * requiring a full `MessageParams` DTO.
 *
 * @tparam TValidator The validator type being checked.
 * @tparam TParams The parameter DTO type (expected to structurally match MessageParams).
 */
template<typename TValidator, typename TParams>
concept MessageValidatorFor = validation::ValidatorFor<TValidator, TParams>&& requires(TValidator validator, TParams params) {
  // Ensure the params object contains the expected fields
  { params.id } -> std::convertible_to<MessageId>;
  { params.chat_id } -> std::convertible_to<ChatId>;
  { params.sender_id } -> std::convertible_to<UserId>;
  { params.ciphertext } -> std::convertible_to<std::string>;
  { params.type } -> std::convertible_to<MessageTypeVariant>;
  { params.created_at } -> std::convertible_to< std::chrono::system_clock::time_point>;
  { params.encrypted_keys} -> std::convertible_to<std::unordered_map<UserId, std::string>>;

  { validator.GetCiphertextRule()(params.ciphertext).Ok() } -> std::same_as<bool>;
  { validator.GetCreatedAtRule()(params.created_at).Ok() } -> std::same_as<bool>;
  { validator.GetEncryptedKeysRule()(params.encrypted_keys).Ok() } -> std::same_as<bool>;

};


#endif // MESSAGE_VALIDATOR_FOR_H