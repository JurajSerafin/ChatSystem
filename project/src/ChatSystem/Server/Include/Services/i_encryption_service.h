#ifndef I_ENCRYPTION_SERVICE_H
#define I_ENCRYPTION_SERVICE_H

#include <string>

#include <Encryption/key_pair.h>

class IEncryptionService {
public:
    virtual KeyPair GenerateKeyPair() = 0;

    virtual std::string Encrypt(std::string const& plaintext, std::string const& publicKey) = 0;

    virtual std::string Decrypt(std::string const& ciphertext, std::string const& privateKey) = 0;

    virtual std::string Sign(std::string const& data, std::string const& privateKey) = 0;

    virtual bool Verify(std::string const& data,
                        std::string const& signature,
                        std::string const& publicKey) = 0;

    virtual std::string HashPassword(std::string const& password) = 0;
    virtual bool VerifyPassword(std::string const& password, std::string const& hash) = 0;
};

#endif // I_ENCRYPTION_SERVICE_H