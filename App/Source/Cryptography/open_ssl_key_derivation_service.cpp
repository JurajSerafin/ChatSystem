#include "open_ssl_key_derivation_service.h"

#include "Cryptography/i_key_derivation_service.h"

#include <format>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <stdexcept>
#include <vector>
#include <limits>

namespace {
  constexpr std::size_t kSaltSize = 16;
  constexpr std::size_t kDerivedKeySize = 32;

  constexpr int kDefaultScryptN = 16384;
  constexpr int kDefaultScryptR = 8;
  constexpr int kDefaultScryptP = 1;
}

std::vector<unsigned char> OpenSSLKeyDerivationService::GenerateSalt() {
  std::vector<unsigned char> salt(kSaltSize);

  if (RAND_bytes(salt.data(), kSaltSize) != 1) {
    throw std::runtime_error("Failed to generate secure salt");
  }

  return salt;
}

bool OpenSSLKeyDerivationService::TryDeriveKey(std::string_view password, const std::vector<unsigned char>& salt, const KdfParams& params, unsigned char* derivedKey) {
  return EVP_PBE_scrypt(password.data(), password.size(),
    salt.data(), salt.size(),
    params.n, params.r, params.p, 0,
    derivedKey, kDerivedKeySize) == 1;
}

std::string OpenSSLKeyDerivationService::DeriveKey(std::string_view password, const std::vector<unsigned char>& salt, const KdfParams& params) {
  unsigned char derived_key[kDerivedKeySize];

  if (!TryDeriveKey(password, salt, params, derived_key)) {
    throw std::runtime_error("scrypt key derivation failed");
  }

  std::string b64_key = Base64Encode(derived_key, kDerivedKeySize);

  OPENSSL_cleanse(derived_key, kDerivedKeySize);

  return b64_key;
}

KdfParams OpenSSLKeyDerivationService::GetDefaultParams() const {
  return {
    kDefaultScryptN,
    kDefaultScryptR,
    kDefaultScryptP,
    std::format("scrypt-{}-{}-{}", kDefaultScryptN, kDefaultScryptR, kDefaultScryptP)
  };
}

bool OpenSSLKeyDerivationService::TryExtractScryptParams(KdfParams& params) {
  return sscanf_s(params.algorithm_id.c_str(), "scrypt-%d-%d-%d", &params.n, &params.r, &params.p) == 3;
}

KdfParams OpenSSLKeyDerivationService::ParseAlgorithmId(std::string_view algId) const {
  KdfParams params{};
  params.algorithm_id = std::string(algId);

  if (!TryExtractScryptParams(params)) {
    throw std::invalid_argument("Invalid or corrupted algorithm identifier");
  }
  return params;
}

bool OpenSSLKeyDerivationService::NeedsRehash(std::string_view algId) const {
  try {
    const KdfParams params = ParseAlgorithmId(algId);

    return params.n < kDefaultScryptN 
      || params.r < kDefaultScryptR
      || params.p < kDefaultScryptP;
  }
  catch (const std::invalid_argument&) {
    return true;
  }
}

std::string OpenSSLKeyDerivationService::Base64Encode(const unsigned char* buffer, std::size_t length) {
  std::vector<char> encoded(GetMaxBase64StrSize(length));

  if (length > std::numeric_limits<int>::max()) {
    throw std::length_error(
      "Length of the data to encode exceeds the maximum value representable by int.");
  }

  const int out_len = EVP_EncodeBlock(reinterpret_cast<unsigned char*>(encoded.data()), buffer, static_cast<int>(length));

  return { encoded.data(), static_cast<std::string::size_type>(out_len) };
}
