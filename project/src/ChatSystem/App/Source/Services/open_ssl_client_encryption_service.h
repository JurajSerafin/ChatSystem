#ifndef OPEN_SSL_CLIENT_ENCRYPTION_SERVICE_H
#define OPEN_SSL_CLIENT_ENCRYPTION_SERVICE_H

#include "Services/i_client_encryption_service.h"

#include <memory>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <vector>

class OpenSSLClientEncryptionService : public IClientEncryptionService {
public:
  OpenSSLClientEncryptionService() = default;

  KeyPair GenerateKeyPair() override;

  std::string GenerateSymmetricKey() override;

  std::string EncryptSymmetric(std::string_view plaintext, std::string_view key) override;

  std::string DecryptSymmetric(std::string_view ciphertext, std::string_view key) override;

  std::string WrapKey(std::string_view symmetricKey, std::string_view publicKey) override;

  std::string UnwrapKey(std::string_view wrappedKey, std::string_view privateKey) override;

  std::string Sign(std::string_view data, std::string_view privateKey) override;

  bool Verify(std::string_view data, std::string_view signature, std::string_view publicKey) override;

private:
  static std::string Base64Encode(const unsigned char* buffer, std::size_t length);

  static std::vector<unsigned char> Base64Decode(std::string_view input);

  struct BIO_Deleter {
    void operator()(BIO* b) const { 
      BIO_free_all(b); 
    } 
  };

  struct EVP_PKEY_Deleter {
    void operator()(EVP_PKEY* p) const {
      EVP_PKEY_free(p);
    } 
  };

  struct EVP_PKEY_CTX_Deleter {
    void operator()(EVP_PKEY_CTX* ctx) const {
      EVP_PKEY_CTX_free(ctx);
    }
  };

  struct EVP_MD_CTX_Deleter {
    void operator()(EVP_MD_CTX* ctx) const {
      EVP_MD_CTX_free(ctx);
    }
  };

  struct EVP_CIPHER_CTX_Deleter {
    void operator()(EVP_CIPHER_CTX* ctx) const {
      EVP_CIPHER_CTX_free(ctx);
    }
  };

  using BIOPtr = std::unique_ptr<BIO, BIO_Deleter>;
  using EVPPKeyPtr = std::unique_ptr<EVP_PKEY, EVP_PKEY_Deleter>;
  using EVPPKeyCtxPtr = std::unique_ptr<EVP_PKEY_CTX, EVP_PKEY_CTX_Deleter>;
  using EVPMdCtxPtr = std::unique_ptr<EVP_MD_CTX, EVP_MD_CTX_Deleter>;
  using EVPCipherCtxPtr = std::unique_ptr<EVP_CIPHER_CTX, EVP_CIPHER_CTX_Deleter>;

  static EVPPKeyPtr LoadPublicKey(std::string_view pem);

  static EVPPKeyPtr LoadPrivateKey(std::string_view pem);

  static EVPPKeyCtxPtr LoadRSAKeyContext();

  static BIOPtr WritePrivateKeyBio(EVP_PKEY* priKeyObs);

  static BIOPtr WritePublicKeyBio(EVP_PKEY* pubKeyObs);

  static constexpr std::size_t GetMaxBase64StrSize(std::size_t length);

  static int GetSanitizedDecodedTextLength(std::string_view text, int dirtyOutLen);
};

constexpr std::size_t OpenSSLClientEncryptionService::GetMaxBase64StrSize(std::size_t length) {
  return 4 * ((length + 2) / 3) + 1;
}

#endif // OPEN_SSL_CLIENT_ENCRYPTION_SERVICE_H#
