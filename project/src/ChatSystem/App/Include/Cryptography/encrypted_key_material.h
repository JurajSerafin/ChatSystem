#ifndef ENCRYPTED_KEY_MATERIAL_H
#define ENCRYPTED_KEY_MATERIAL_H

#include <string>
#include <vector>

/**
 * @brief Represents the securely packaged material needed to unlock a user's private key.
 *
 * This struct groups the ciphertext of the private key along with the exact
 * cryptographic parameters needed to reconstruct the decryption key.
 */
struct EncryptedKeyMaterial {
  /// @brief The cryptographic salt used during the password key derivation.
  std::vector<unsigned char> salt;

  //// @brief The private key ciphertext, protected by the derived password key.
  std::vector<unsigned char> encrypted_key;

  /// @brief The string identifier containing the KDF algorithm and its parameters.
  std::string algorithm;
};

#endif // ENCRYPTED_KEY_MATERIAL_H