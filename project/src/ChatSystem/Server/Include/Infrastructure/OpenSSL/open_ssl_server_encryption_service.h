#ifndef OPEN_SSL_ENCRYPTION_SERVICE_H
#define OPEN_SSL_ENCRYPTION_SERVICE_H

#include "Services/Interface/i_server_encryption_service.h"

#include <memory>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <vector>
#include <string>

class OpenSSLServerEncryptionService : public IServerEncryptionService {
public:
  OpenSSLServerEncryptionService() = default;

  std::string HashPassword(const std::string& password) override;

  bool VerifyPassword(const std::string& password, const std::string& hash) override;

  bool NeedsRehash(const std::string& hash) const override;

  bool Verify(const std::string& data, const std::string& signature, const std::string& publicKey) override;

private:
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

  static std::string Base64Encode(const unsigned char* buffer, std::size_t length);
  static std::vector<unsigned char> Base64Decode(const std::string& input);

  static EVPPKeyPtr LoadPublicKey(const std::string& pem);

  static void LoadSalt(unsigned char* saltOut);

  static void LoadPasswordHash(const std::string& password, const unsigned char* salt, std::size_t saltLen, unsigned char* hashOut);

  static int ComputeScryptHash(const std::string& target, const unsigned char* saltOut, std::size_t saltLen, unsigned char* hashOut);

  static int GetSanitizedDecodedTextLength(const std::string& text, int dirtyOutLen);
  static constexpr std::size_t GetMaxBase64StrSize(std::size_t length);
};

constexpr std::size_t OpenSSLServerEncryptionService::GetMaxBase64StrSize(std::size_t length) {
  return 4 * ((length + 2) / 3) + 1;
}

#endif // OPEN_SSL_ENCRYPTION_SERVICE_H