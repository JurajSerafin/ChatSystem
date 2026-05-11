#include "Infrastructure/OpenSSL/open_ssl_server_encryption_service.h"

#include <limits>
#include <openssl/pem.h>
#include <openssl/rand.h>
#include <stdexcept>
#include <vector>
#include <string_view>

namespace {
  constexpr std::size_t kSaltSize = 16;
  constexpr std::size_t kHashSize = 32;

  constexpr int kScryptHashCost = 16384;
  constexpr int kScryptBlockSize = 8;
  constexpr int kScryptParallelization = 1;
  constexpr int kScryptMaxMem = 0;

  constexpr std::string_view kHashTypeTag = "$scrypt$";
}



OpenSSLServerEncryptionService::EVPPKeyPtr OpenSSLServerEncryptionService::LoadPublicKey(const std::string& pem) {
  const BIOPtr bio(BIO_new_mem_buf(pem.data(), static_cast<int>(pem.size())));
  EVPPKeyPtr public_key(PEM_read_bio_PUBKEY(bio.get(), nullptr, nullptr, nullptr));

  if (!public_key) {
    throw std::runtime_error("Failed to load public key from PEM string");
  }

  return public_key;
}

bool OpenSSLServerEncryptionService::Verify(const std::string& data, const std::string& signatureBase64, const std::string& publicKey) {
  if (data.size() > static_cast<std::size_t>(std::numeric_limits<int>::max())) {
    throw std::length_error("Data exceeds maximum supported OpenSSL buffer size.");
  }

  const EVPPKeyPtr pkey = LoadPublicKey(publicKey);

  const EVPMdCtxPtr mdctx(EVP_MD_CTX_new());

  std::vector<unsigned char> signature;
  try {
    signature = Base64Decode(signatureBase64);
  }
  catch (const std::exception&) {
    return false;
  }

  if (EVP_DigestVerifyInit(mdctx.get(), nullptr, EVP_sha256(), nullptr, pkey.get()) <= 0) {
    return false;
  }

  if (EVP_DigestVerifyUpdate(mdctx.get(), data.data(), data.size()) <= 0) {
    return false;
  }

  return EVP_DigestVerifyFinal(mdctx.get(), signature.data(), signature.size()) == 1;
}



void OpenSSLServerEncryptionService::LoadSalt(unsigned char* saltOut) {
  if (RAND_bytes(saltOut, kSaltSize) != 1) {
    throw std::runtime_error("Failed to generate cryptographic salt");
  }
}

int OpenSSLServerEncryptionService::ComputeScryptHash(const std::string& target, const unsigned char* saltOut, std::size_t saltLen, unsigned char* hashOut) {
  return EVP_PBE_scrypt(
    target.data(), target.size(),
    saltOut, saltLen,
    kScryptHashCost, kScryptBlockSize, kScryptParallelization, kScryptMaxMem,
    hashOut, kHashSize
  );
}

void OpenSSLServerEncryptionService::LoadPasswordHash(const std::string& password, const unsigned char* salt, std::size_t saltLen, unsigned char* hashOut) {
  if (ComputeScryptHash(password, salt, saltLen, hashOut) != 1) {
    throw std::runtime_error("scrypt hashing failed");
  }
}

std::string OpenSSLServerEncryptionService::HashPassword(const std::string& password) {
  unsigned char salt[kSaltSize];
  LoadSalt(salt);

  unsigned char hash[kHashSize];
  LoadPasswordHash(password, salt, kSaltSize, hash);

  return std::string(kHashTypeTag) + Base64Encode(salt, sizeof(salt)) + "$" + Base64Encode(hash, sizeof(hash));
}

bool OpenSSLServerEncryptionService::VerifyPassword(const std::string& password, const std::string& storedHash) {

  if (storedHash.compare(0, kHashTypeTag.length(), kHashTypeTag) != 0) {
    return false;
  }
  std::string payload = storedHash.substr(kHashTypeTag.length());

  const std::size_t delimiter_pos = payload.find('$');
  if (delimiter_pos == std::string::npos) {
    return false;
  }

  try {
    const std::vector<unsigned char> salt = Base64Decode(payload.substr(0, delimiter_pos));
    const std::vector<unsigned char> target_hash = Base64Decode(payload.substr(delimiter_pos + 1));

    unsigned char new_hash[kHashSize];
    if (ComputeScryptHash(password, salt.data(), salt.size(), new_hash) != 1) {
      return false;
    }

    return CRYPTO_memcmp(target_hash.data(), new_hash, sizeof(new_hash)) == 0;
  }
  catch (const std::exception&) {
    return false;
  }
}

bool OpenSSLServerEncryptionService::NeedsRehash(const std::string& hash) const {

  if (hash.compare(0, kHashTypeTag.length(), kHashTypeTag) != 0) {
    return true;
  }

  return false;
}


std::string OpenSSLServerEncryptionService::Base64Encode(const unsigned char* buffer, std::size_t length) {
  if (length > static_cast<std::size_t>(std::numeric_limits<int>::max())) {
    throw std::length_error("Buffer exceeds maximum supported OpenSSL encode size.");
  }

  std::vector<char> encoded(GetMaxBase64StrSize(length));
  const int out_len = EVP_EncodeBlock(reinterpret_cast<unsigned char*>(encoded.data()), buffer, static_cast<int>(length));

  return std::string(encoded.data(), out_len);
}

int OpenSSLServerEncryptionService::GetSanitizedDecodedTextLength(const std::string& text, int dirtyOutLen) {
  constexpr char kPaddingPlaceholder = '=';

  if (text.length() > 0 && text.back() == kPaddingPlaceholder) {
    dirtyOutLen--;
  }
  if (text.length() > 1 && text[text.length() - 2] == kPaddingPlaceholder) {
    dirtyOutLen--;
  }
  return dirtyOutLen;
}

std::vector<unsigned char> OpenSSLServerEncryptionService::Base64Decode(const std::string& input) {
  if (input.empty()) {
    return {};
  }

  if (input.size() > static_cast<std::size_t>(std::numeric_limits<int>::max())) {
    throw std::length_error("Input exceeds maximum supported OpenSSL decode size.");
  }

  std::vector<unsigned char> decoded(input.size());

  int out_len = EVP_DecodeBlock(decoded.data(), reinterpret_cast<const unsigned char*>(input.data()), static_cast<int>(input.size()));

  if (out_len < 0) {
    throw std::runtime_error("Failed to decode Base64 string");
  }

  out_len = GetSanitizedDecodedTextLength(input, out_len);

  if (out_len < 0) {
    return {};
  }

  decoded.resize(static_cast<std::size_t>(out_len));

  return decoded;
}