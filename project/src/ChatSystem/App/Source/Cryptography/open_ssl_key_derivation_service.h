#ifndef OPEN_SSL_KEY_DERIVATION_SERVICE_H
#define OPEN_SSL_KEY_DERIVATION_SERVICE_H

#include "Services/i_key_derivation_service.h"
#include <cstddef>
#include <string_view>
#include <vector>

class OpenSSLKeyDerivationService : public IKeyDerivationService {
public:
  OpenSSLKeyDerivationService() = default;

  std::vector<unsigned char> GenerateSalt() override;

  std::string DeriveKey(std::string_view password, const std::vector<unsigned char>& salt, const KdfParams& params) override;

  KdfParams GetDefaultParams() const override;

  KdfParams ParseAlgorithmId(std::string_view algId) const override;

  bool NeedsRehash(std::string_view algId) const override;

private:
  static std::string Base64Encode(const unsigned char* buffer, std::size_t length);
  static constexpr std::size_t GetMaxBase64StrSize(std::size_t length);

  static bool TryDeriveKey(std::string_view password, const std::vector<unsigned char>& salt, const KdfParams& params, unsigned char* derivedKey);
  
  static bool TryExtractScryptParams(KdfParams& params);
};

constexpr std::size_t OpenSSLKeyDerivationService::GetMaxBase64StrSize(std::size_t length) {
  return 4 * ((length + 2) / 3) + 1;
}


#endif // OPEN_SSL_KEY_DERIVATION_SERVICE_H