#ifndef I_ENCRYPTION_SERVICE_H
#define I_ENCRYPTION_SERVICE_H

#include <string>

#include <Encryption/key_pair.h>

class IEncryptionService {
public:
    virtual KeyPair GenerateKeyPair() = 0;

    virtual std::string Encrypt(const std::string& plaintext, const std::string& publicKey) = 0;

    virtual std::string Decrypt(const std::string& ciphertext, const std::string& privateKey) = 0;

    virtual std::string Sign(const std::string& data, const std::string& privateKey) = 0;

    virtual bool Verify(const std::string& data, const std::string& signature, const std::string& publicKey) = 0;

    virtual std::string HashPassword(const std::string& password) = 0;
    virtual bool VerifyPassword(const std::string& password, const std::string& hash) = 0;
};

#endif // I_ENCRYPTION_SERVICE_H