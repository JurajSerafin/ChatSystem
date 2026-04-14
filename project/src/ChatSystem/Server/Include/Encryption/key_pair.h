#ifndef KEY_PAIR_H
#define KEY_PAIR_H

#include <string>

/**
 * @brief Represents a cryptographic key pair.
 *
 * This class stores a public/private key pair used for authentication
 * and encryption-related operations.
 *
 * Instances are immutable after construction except for move operations.
 */
class KeyPair {
private:
  std::string private_key_;

  std::string public_key_;

public:
  /**
   * @brief Deleted default constructor.
   *
   * A KeyPair must always be explicitly initialized with valid keys.
   */
  KeyPair() = delete;

  /**
   * @brief Constructs a KeyPair from private and public keys.
   *
   * @param privateKey Private key.
   * @param publicKey Public key.
   */
  KeyPair(std::string&& privateKey, std::string&& publicKey) : private_key_(std::move(privateKey)), public_key_(std::move(publicKey)) {}

  /**
   * @brief Copy constructor.
   */
  KeyPair(const KeyPair& other) = default;

  /**
   * @brief Move constructor.
   */
  KeyPair(KeyPair&& other) = default;

  /**
   * @brief Deleted copy assignment operator.
   *
   * KeyPair is immutable after creation and cannot be reassigned via copy.
   */
  KeyPair& operator=(const KeyPair& other) = delete;

  /**
   * @brief Deleted move assignment operator.
   *
   * Prevents reassignment to preserve immutability guarantees.
   */
  KeyPair& operator=(KeyPair&& other) = delete;

  /**
   * @brief Destructor.
   */
  ~KeyPair() = default;

  /**
   * @brief Compares two key pairs for equality.
   *
   * Only the public key is used for comparison.
   *
   * @param other KeyPair to compare against.
   * @return true if public keys are equal, false otherwise.
   */
  bool operator==(const KeyPair& other) const;

  /**
   * @brief Retrieves the public key.
   *
   * @return Constant reference to the public key string.
   */
  [[nodiscard]] const std::string& GetPublicKey() const;

};

inline bool KeyPair::operator==(const KeyPair& other) const {
  return this->public_key_ == other.public_key_;
}

inline const std::string& KeyPair::GetPublicKey() const {
  return public_key_;
}

#endif // KEY_PAIR_H