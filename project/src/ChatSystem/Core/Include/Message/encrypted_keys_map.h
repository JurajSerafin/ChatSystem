#ifndef ENCRYPTED_KEYS_MAP_H
#define ENCRYPTED_KEYS_MAP_H

#include <User/user_id.h>
#include <string>
#include <unordered_map>

/**
 * @brief Represents a mapping of participants to their specific wrapped cryptographic key.
 *
 * In the E2EE architecture, each participant in a chat (including the sender)
 * receives a copy of the message's symmetric encryption key, encrypted (wrapped)
 * with their specific asymmetric public key.
 */
using EncryptedKeysMap = std::unordered_map<UserId, std::string, BaseId<UserId>::Hasher>;

#endif // ENCRYPTED_KEYS_MAP_H