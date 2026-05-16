#ifndef I_CLIENT_ENCRYPTION_SERVICE_H
#define I_CLIENT_ENCRYPTION_SERVICE_H

#include "Cryptography/key_pair.h"
#include <string>
#include <string_view>

class IClientEncryptionService {
public:

  virtual KeyPair GenerateKeyPair() = 0;

  virtual std::string GenerateSymmetricKey() = 0;

  virtual std::string EncryptSymmetric(std::string_view plaintext, std::string_view key) = 0;

  virtual std::string DecryptSymmetric(std::string_view ciphertext, std::string_view key) = 0;

  virtual std::string WrapKey(std::string_view symmetricKey, std::string_view publicKey) = 0;

  virtual std::string UnwrapKey(std::string_view wrappedKey, std::string_view privateKey) = 0;

  virtual std::string Sign(std::string_view data, std::string_view privateKey) = 0;

  virtual bool Verify(std::string_view data, std::string_view signature, std::string_view publicKey) = 0;

};

#endif // !I_CLIENT_ENCRYPTION_SERVICE_H
