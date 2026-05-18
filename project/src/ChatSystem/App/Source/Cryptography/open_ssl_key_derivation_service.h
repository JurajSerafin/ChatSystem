#ifndef OPEN_SSL_KEY_DERIVATION_SERVICE_H
#define OPEN_SSL_KEY_DERIVATION_SERVICE_H

#include "Cryptography/i_key_derivation_service.h"

#include <string>
#include <string_view>
#include <vector>

/**
 * @brief Concrete implementation of the Key Derivation Service using OpenSSL.
 * * Typically utilizes strong, memory-hard algorithms like Scryptto securely deriving
 *  keys from user passwords, protecting against brute-force attacks.
 */
class OpenSSLKeyDerivationService : public IKeyDerivationService {
public:
  OpenSSLKeyDerivationService() = default;

  std::vector<unsigned char> GenerateSalt() override;

  std::string DeriveKey(std::string_view password, const std::vector<unsigned char>& salt, const KdfParams& params) override;

  KdfParams GetDefaultParams() const override;

  KdfParams ParseAlgorithmId(std::string_view algId) const override;

  bool NeedsRehash(std::string_view algId) const override;

private:
  /** @brief Encodes the derived raw key bytes into a safe Base64 string format. */
  static std::string Base64Encode(const unsigned char* buffer, std::size_t length);

  /** @brief Calculates the maximum required buffer size for Base64 encoding. */
  static constexpr std::size_t GetMaxBase64StrSize(std::size_t length);

  /** * @brief Invokes the internal OpenSSL KDF function (EVP_PBE_scrypt in this case).
   * @return True if derivation succeeded, false on OpenSSL internal error.
   */
  static bool TryDeriveKey(std::string_view password, const std::vector<unsigned char>& salt, const KdfParams& params, unsigned char* derivedKey);

  /** @brief Helper to safely map default or parsed parameters to OpenSSL structures. */
  static bool TryExtractScryptParams(KdfParams& params);
};

constexpr std::size_t OpenSSLKeyDerivationService::GetMaxBase64StrSize(std::size_t length) {
  return 4 * ((length + 2) / 3) + 1;
}

#endif // OPEN_SSL_KEY_DERIVATION_SERVICE_H