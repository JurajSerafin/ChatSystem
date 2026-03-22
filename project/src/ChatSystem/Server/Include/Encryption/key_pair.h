#ifndef KEY_PAIR_H
#define KEY_PAIR_H

#include <string>

class KeyPair {
private:
    std::string private_key_;
    std::string public_key_;

public:
    KeyPair() = delete;

    KeyPair(std::string&& privateKey, std::string&& publicKey)
        : private_key_(std::move(privateKey)), public_key_(std::move(publicKey)) {}

    KeyPair(const KeyPair& other) = default;

    KeyPair(KeyPair&& other) = default;

    KeyPair& operator=(const KeyPair& other) = delete;

    KeyPair& operator=(KeyPair&& other) = delete;

    ~KeyPair() = default;

    bool operator==(const KeyPair& other) const;

    [[nodiscard]] const std::string& GetPublicKey() const;
    [[nodiscard]] const std::string& GetPrivateKey() const;
};

inline bool KeyPair::operator==(const KeyPair& other) const {
    return this->public_key_ == other.public_key_;
}

inline const std::string& KeyPair::GetPublicKey() const { 
    return public_key_; 
}

inline const std::string& KeyPair::GetPrivateKey() const { return private_key_; }

#endif // KEY_PAIR_H
