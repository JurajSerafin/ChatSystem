#ifndef I_CLIENT_ENCRYPTION_SERVICE_H
#define I_CLIENT_ENCRYPTION_SERVICE_H

#include "Cryptography/key_pair.h"
#include <string>
#include <string_view>

/**
 * @brief Low-level cryptographic engine for symmetric/asymmetric encryption and signatures.
 * 
 * Abstracts the underlying cryptographic library, providing primitive operations
 * for the End-to-End Encryption (E2EE) pipeline.
 */
class IClientEncryptionService {
public:
  virtual ~IClientEncryptionService() = default;

  /**
   * @brief Generates a fresh asymmetric public/private key pair.
   * @return A KeyPair instance containing the generated keys.
   */
  virtual KeyPair GenerateKeyPair() = 0;

  /**
   * @brief Generates a cryptographically secure symmetric key.
   * @return A raw byte string representing the symmetric key (e.g., AES-256).
   */
  virtual std::string GenerateSymmetricKey() = 0;

  /**
   * @brief Encrypts plaintext using the provided symmetric key.
   * @param plaintext The unencrypted data.
   * @param key The symmetric key to encrypt with.
   * @return The resulting ciphertext.
   */
  virtual std::string EncryptSymmetric(std::string_view plaintext, std::string_view key) = 0;

  /**
   * @brief Decrypts ciphertext back to plaintext using the provided symmetric key.
   * @param ciphertext The encrypted data.
   * @param key The symmetric key to decrypt with.
   * @return The resulting plaintext.
   */
  virtual std::string DecryptSymmetric(std::string_view ciphertext, std::string_view key) = 0;

  /**
   * @brief Wraps (encrypts) a symmetric key using a recipient's public asymmetric key.
   * @param symmetricKey The payload key to be wrapped.
   * @param publicKey The recipient's public key.
   * @return The asymmetrically encrypted symmetric key.
   */
  virtual std::string WrapKey(std::string_view symmetricKey, std::string_view publicKey) = 0;

  /**
   * @brief Unwraps (decrypts) a symmetric key using the user's private asymmetric key.
   * @param wrappedKey The asymmetrically encrypted symmetric key.
   * @param privateKey The recipient's private key.
   * @return The raw, unencrypted symmetric key.
   */
  virtual std::string UnwrapKey(std::string_view wrappedKey, std::string_view privateKey) = 0;

  /**
   * @brief Generates a cryptographic signature for the given data using a private key.
   * @param data The payload to sign.
   * @param privateKey The signer's private key.
   * @return The cryptographic signature.
   */
  virtual std::string Sign(std::string_view data, std::string_view privateKey) = 0;

  /**
   * @brief Verifies a cryptographic signature against the original data using a public key.
   * @param data The original payload.
   * @param signature The signature to verify.
   * @param publicKey The alleged signer's public key.
   * @return True if the signature is valid, false otherwise.
   */
  virtual bool Verify(std::string_view data, std::string_view signature, std::string_view publicKey) = 0;
};

#endif // I_CLIENT_ENCRYPTION_SERVICE_H