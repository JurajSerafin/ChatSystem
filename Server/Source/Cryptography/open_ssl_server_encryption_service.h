#ifndef OPEN_SSL_ENCRYPTION_SERVICE_H
#define OPEN_SSL_ENCRYPTION_SERVICE_H

#include "Cryptography/i_server_encryption_service.h"

#include <memory>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <vector>
#include <string>

/**
 * @brief Concrete implementation of the server-side encryption service using OpenSSL.
 * * Handles secure password hashing (via Scrypt) for user authentication and validates
 * cryptographic signatures.
 * * @note Crucially, this service lacks any symmetric decryption capabilities,
 * strictly enforcing the backend's Zero-Knowledge E2EE architecture.
 */
class OpenSSLServerEncryptionService : public IServerEncryptionService {
public:
  OpenSSLServerEncryptionService() = default;

  std::string HashPassword(const std::string& password) override;

  bool VerifyPassword(const std::string& password, const std::string& hash) override;

  bool NeedsRehash(const std::string& hash) const override;

  bool Verify(const std::string& data, const std::string& signature, const std::string& publicKey) override;

private:
  // OpenSSL Custom Deleters for Smart Pointers

  struct BIO_Deleter {
    void operator()(BIO* b) const { BIO_free_all(b); }
  };

  struct EVP_PKEY_Deleter {
    void operator()(EVP_PKEY* p) const { EVP_PKEY_free(p); }
  };

  struct EVP_MD_CTX_Deleter {
    void operator()(EVP_MD_CTX* ctx) const { EVP_MD_CTX_free(ctx); }
  };

  using BIOPtr = std::unique_ptr<BIO, BIO_Deleter>;
  using EVPPKeyPtr = std::unique_ptr<EVP_PKEY, EVP_PKEY_Deleter>;
  using EVPMdCtxPtr = std::unique_ptr<EVP_MD_CTX, EVP_MD_CTX_Deleter>;


  /** @brief Encodes a raw byte buffer into a safe Base64 string for database storage. */
  static std::string Base64Encode(const unsigned char* buffer, std::size_t length);

  /** @brief Decodes a Base64 string obtained from database back into a raw byte vector. */
  static std::vector<unsigned char> Base64Decode(const std::string& input);

  /** @brief Parses a PEM-formatted public key string into an OpenSSL EVP_PKEY object. */
  static EVPPKeyPtr LoadPublicKey(const std::string& pem);

  /** @brief Generates a cryptographically secure random salt for password hashing. */
  static void LoadSalt(unsigned char* saltOut);

  /** @brief Combines a password and salt, executing the KDF to produce the final hash. */
  static void LoadPasswordHash(const std::string& password, const unsigned char* salt, std::size_t saltLen, unsigned char* hashOut);

  /** * @brief Internal helper to execute the memory-hard Scrypt Key Derivation Function.
   * @return 1 on success, 0 on OpenSSL internal error.
   */
  static int ComputeScryptHash(const std::string& target, const unsigned char* saltOut, std::size_t saltLen, unsigned char* hashOut);

  /** @brief Trims padding and handles dirty trailing bytes resulting from OpenSSL decoding operations. */
  static int GetSanitizedDecodedTextLength(const std::string& text, int dirtyOutLen);

  /** @brief Calculates the maximum required buffer size for Base64 encoding. */
  static constexpr std::size_t GetMaxBase64StrSize(std::size_t length);
};

constexpr std::size_t OpenSSLServerEncryptionService::GetMaxBase64StrSize(std::size_t length) {
  return 4 * ((length + 2) / 3) + 1;
}

#endif // OPEN_SSL_ENCRYPTION_SERVICE_H