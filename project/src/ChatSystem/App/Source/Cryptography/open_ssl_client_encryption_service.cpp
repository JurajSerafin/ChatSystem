#include "open_ssl_client_encryption_service.h"

#include <openssl/pem.h>
#include <openssl/rand.h>
#include <openssl/rsa.h>
#include <stdexcept>
#include <limits>

namespace {
  constexpr std::size_t kAES256KeySize = 32;
  constexpr std::size_t kIVSize = 16;
  constexpr int kRsaKeyGenBits = 2048;
}


KeyPair OpenSSLClientEncryptionService::GenerateKeyPair() {
  const auto ctx = LoadRSAKeyContext();

  if (EVP_PKEY_keygen_init(ctx.get()) <= 0 || EVP_PKEY_CTX_set_rsa_keygen_bits(ctx.get(), kRsaKeyGenBits) <= 0) {
    throw std::runtime_error("Failed to initialize RSA key generation");
  }

  EVP_PKEY* pkey_raw = nullptr;

  if (EVP_PKEY_keygen(ctx.get(), &pkey_raw) <= 0) {
    throw std::runtime_error("Failed to generate RSA key");
  }

  const EVPPKeyPtr pkey(pkey_raw);

  const auto pri_bio = WritePrivateKeyBio(pkey.get());

  const auto pub_bio = WritePublicKeyBio(pkey.get());

  char* pri_data = nullptr;
  const long pri_len = BIO_get_mem_data(pri_bio.get(), &pri_data);

  char* pub_data = nullptr;
  const long pub_len = BIO_get_mem_data(pub_bio.get(), &pub_data);

  return { std::string(pri_data, pri_len), std::string(pub_data, pub_len) };
}

std::string OpenSSLClientEncryptionService::GenerateSymmetricKey() {
  unsigned char aes_key[kAES256KeySize];
  if (RAND_bytes(aes_key, kAES256KeySize) != 1) {
    throw std::runtime_error("Failed to generate random AES key");
  }

  std::string b64_key = Base64Encode(aes_key, kAES256KeySize);

  OPENSSL_cleanse(aes_key, kAES256KeySize);

  return b64_key;
}


std::string OpenSSLClientEncryptionService::EncryptSymmetric(std::string_view plaintext, std::string_view key) {
  auto raw_key = Base64Decode(key);
  if (raw_key.size() != kAES256KeySize) throw std::invalid_argument("Invalid AES key size");

  unsigned char iv[kIVSize];
  if (RAND_bytes(iv, kIVSize) != 1) throw std::runtime_error("Failed to generate IV");

  const EVPCipherCtxPtr aes_ctx(EVP_CIPHER_CTX_new());
  if (EVP_EncryptInit_ex(aes_ctx.get(), EVP_aes_256_cbc(), nullptr, raw_key.data(), iv) != 1) {
    throw std::runtime_error("Failed to initialize AES encryption");
  }

  if (plaintext.size() > static_cast<std::size_t>(std::numeric_limits<int>::max())) {
    throw std::length_error("Plaintext too long");
  }

  std::vector<unsigned char> package(kIVSize + plaintext.size() + EVP_MAX_BLOCK_LENGTH);

  std::copy_n(iv, kIVSize, package.begin());

  int len1 = 0, len2 = 0;

  if (!plaintext.empty()) {
    EVP_EncryptUpdate(
      aes_ctx.get(),
      package.data() + kIVSize,
      &len1,
      reinterpret_cast<const unsigned char*>(plaintext.data()),
      static_cast<int>(plaintext.size())
    );
  }

  EVP_EncryptFinal_ex(aes_ctx.get(), package.data() + kIVSize + len1, &len2);

  package.resize(kIVSize + len1 + len2);

  OPENSSL_cleanse(raw_key.data(), raw_key.size());

  return Base64Encode(package.data(), package.size());
}

std::string OpenSSLClientEncryptionService::DecryptSymmetric(std::string_view ciphertextBase64, std::string_view key) {
  auto raw_key = Base64Decode(key);

  if (raw_key.size() != kAES256KeySize) {
    throw std::invalid_argument("Invalid AES key size");
  }

  auto package = Base64Decode(ciphertextBase64);

  if (package.size() < kIVSize) {
    throw std::runtime_error("Ciphertext too short (missing IV)");
  }

  const unsigned char* iv = package.data();
  const unsigned char* raw_cipher = package.data() + kIVSize;
  const std::size_t cipher_len = package.size() - kIVSize;

  const EVPCipherCtxPtr aes_ctx(EVP_CIPHER_CTX_new());

  if (EVP_DecryptInit_ex(aes_ctx.get(), EVP_aes_256_cbc(), nullptr, raw_key.data(), iv) != 1) {
    throw std::runtime_error("Failed to initialize AES decryption");
  }

  std::vector<unsigned char> plaintext(cipher_len + EVP_MAX_BLOCK_LENGTH);
  int len1 = 0, len2 = 0;

  if (cipher_len > 0) {
    EVP_DecryptUpdate(aes_ctx.get(), plaintext.data(), &len1, raw_cipher, static_cast<int>(cipher_len));
  }

  if (EVP_DecryptFinal_ex(aes_ctx.get(), plaintext.data() + len1, &len2) != 1) {
    throw std::runtime_error("AES decryption failed (Message corrupted or wrong key)");
  }

  OPENSSL_cleanse(raw_key.data(), raw_key.size());

  return { reinterpret_cast<char*>(plaintext.data()), static_cast<std::size_t>(len1) + len2 };
}

std::string OpenSSLClientEncryptionService::WrapKey(std::string_view symmetricKey, std::string_view publicKey) {
  auto raw_sym_key = Base64Decode(symmetricKey);

  const EVPPKeyPtr pkey = LoadPublicKey(publicKey);

  const EVPPKeyCtxPtr rsa_ctx(EVP_PKEY_CTX_new(pkey.get(), nullptr));

  if (EVP_PKEY_encrypt_init(rsa_ctx.get()) <= 0 || EVP_PKEY_CTX_set_rsa_padding(rsa_ctx.get(), RSA_PKCS1_OAEP_PADDING) <= 0) {
    throw std::runtime_error("Failed to initialize RSA key wrapping");
  }

  std::size_t enc_len;

  EVP_PKEY_encrypt(rsa_ctx.get(), nullptr, &enc_len, raw_sym_key.data(), raw_sym_key.size());

  std::vector<unsigned char> enc_key(enc_len);

  if (EVP_PKEY_encrypt(rsa_ctx.get(), enc_key.data(), &enc_len, raw_sym_key.data(), raw_sym_key.size()) <= 0) {
    throw std::runtime_error("RSA key wrapping failed");
  }

  OPENSSL_cleanse(raw_sym_key.data(), raw_sym_key.size());

  return Base64Encode(enc_key.data(), enc_len);
}

std::string OpenSSLClientEncryptionService::UnwrapKey(std::string_view wrappedKey, std::string_view privateKey) {
  auto raw_enc_key = Base64Decode(wrappedKey);

  const EVPPKeyPtr pkey = LoadPrivateKey(privateKey);

  const EVPPKeyCtxPtr rsa_ctx(EVP_PKEY_CTX_new(pkey.get(), nullptr));

  if (EVP_PKEY_decrypt_init(rsa_ctx.get()) <= 0 || EVP_PKEY_CTX_set_rsa_padding(rsa_ctx.get(), RSA_PKCS1_OAEP_PADDING) <= 0) {
    throw std::runtime_error("Failed to initialize RSA key unwrapping");
  }

  std::size_t dec_len;

  EVP_PKEY_decrypt(rsa_ctx.get(), nullptr, &dec_len, raw_enc_key.data(), raw_enc_key.size());

  std::vector<unsigned char> dec_key(dec_len);

  if (EVP_PKEY_decrypt(rsa_ctx.get(), dec_key.data(), &dec_len, raw_enc_key.data(), raw_enc_key.size()) <= 0) {
    throw std::runtime_error("RSA key unwrapping failed (Incorrect Private Key)");
  }

  std::string b64_sym_key = Base64Encode(dec_key.data(), dec_len);

  OPENSSL_cleanse(dec_key.data(), dec_key.size());

  return b64_sym_key;
}


std::string OpenSSLClientEncryptionService::Sign(std::string_view data, std::string_view privateKey) {
  const EVPPKeyPtr pri_key = LoadPrivateKey(privateKey);

  const EVPMdCtxPtr md_ctx(EVP_MD_CTX_new());

  if (EVP_DigestSignInit(md_ctx.get(), nullptr, EVP_sha256(), nullptr, pri_key.get()) <= 0) {
    throw std::runtime_error("Failed to init signature context");
  }

  EVP_DigestSignUpdate(md_ctx.get(), data.data(), data.size());

  std::size_t sig_len;
  EVP_DigestSignFinal(md_ctx.get(), nullptr, &sig_len);

  std::vector<unsigned char> signature(sig_len);
  EVP_DigestSignFinal(md_ctx.get(), signature.data(), &sig_len);

  return Base64Encode(signature.data(), sig_len);
}

bool OpenSSLClientEncryptionService::Verify(std::string_view data, std::string_view signatureBase64, std::string_view publicKey) {
  const EVPPKeyPtr pkey = LoadPublicKey(publicKey);

  const EVPMdCtxPtr mdctx(EVP_MD_CTX_new());

  const std::vector<unsigned char> signature = Base64Decode(signatureBase64);

  if (EVP_DigestVerifyInit(mdctx.get(), nullptr, EVP_sha256(), nullptr, pkey.get()) <= 0) {
    return false;
  }

  EVP_DigestVerifyUpdate(mdctx.get(), data.data(), data.size());

  return EVP_DigestVerifyFinal(mdctx.get(), signature.data(), signature.size()) == 1;
}


OpenSSLClientEncryptionService::EVPPKeyPtr OpenSSLClientEncryptionService::LoadPublicKey(std::string_view pem) {
  const BIOPtr bio(BIO_new_mem_buf(pem.data(), static_cast<int>(pem.size())));

  EVPPKeyPtr public_key(PEM_read_bio_PUBKEY(bio.get(), nullptr, nullptr, nullptr));

  if (!public_key) {
    throw std::runtime_error("Failed to load public key");
  }

  return public_key;
}

OpenSSLClientEncryptionService::EVPPKeyPtr OpenSSLClientEncryptionService::LoadPrivateKey(std::string_view pem) {
  const BIOPtr bio(BIO_new_mem_buf(pem.data(), static_cast<int>(pem.size())));

  EVPPKeyPtr private_key(PEM_read_bio_PrivateKey(bio.get(), nullptr, nullptr, nullptr));

  if (!private_key) {
    throw std::runtime_error("Failed to load private key");
  }

  return private_key;
}

OpenSSLClientEncryptionService::EVPPKeyCtxPtr OpenSSLClientEncryptionService::LoadRSAKeyContext() {
  return EVPPKeyCtxPtr(EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, nullptr));
}

OpenSSLClientEncryptionService::BIOPtr OpenSSLClientEncryptionService::WritePrivateKeyBio(EVP_PKEY* priKeyObs) {
  BIOPtr pri_bio(BIO_new(BIO_s_mem()));

  PEM_write_bio_PrivateKey(pri_bio.get(), priKeyObs, nullptr, nullptr, 0, nullptr, nullptr);

  return pri_bio;
}

OpenSSLClientEncryptionService::BIOPtr OpenSSLClientEncryptionService::WritePublicKeyBio(EVP_PKEY* pubKeyObs) {
  BIOPtr pub_bio(BIO_new(BIO_s_mem()));

  PEM_write_bio_PUBKEY(pub_bio.get(), pubKeyObs);

  return pub_bio;
}

std::string OpenSSLClientEncryptionService::Base64Encode(const unsigned char* buffer, std::size_t length) {
  std::vector<char> encoded(GetMaxBase64StrSize(length));

  if (length > std::numeric_limits<int>::max()) {
    throw std::length_error(
      "Length of the data to encode exceeds the maximum value representable by int.");
  }

  const int out_len = EVP_EncodeBlock(reinterpret_cast<unsigned char*>(encoded.data()), buffer, static_cast<int>(length));

  return { encoded.data(), static_cast<std::string::size_type>(out_len) };
}

int OpenSSLClientEncryptionService::GetSanitizedDecodedTextLength(std::string_view text, int dirtyOutLen) {
  constexpr char kPaddingPlaceholder = '=';

  if (!text.empty() && (text[text.length() - 1] == kPaddingPlaceholder)) {
    dirtyOutLen--;
  }

  if (text.length() > 1 && (text[text.length() - 2] == kPaddingPlaceholder)) {
    dirtyOutLen--;
  }

  return dirtyOutLen;
}

std::vector<unsigned char> OpenSSLClientEncryptionService::Base64Decode(std::string_view input) {
  if (input.empty()) {
    return {};
  }

  std::vector<unsigned char> decoded(input.size());

  int out_len = EVP_DecodeBlock(decoded.data(), reinterpret_cast<const unsigned char*>(input.data()), static_cast<int>(input.size()));

  if (out_len < 0) {
    throw std::runtime_error("Failed to decode Base64 string");
  }

  out_len = GetSanitizedDecodedTextLength(input, out_len);

  decoded.resize(out_len);

  return decoded;
}