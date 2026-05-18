#ifndef I_KEY_DERIVATION_SERVICE_H
#define I_KEY_DERIVATION_SERVICE_H

#include <string>
#include <string_view>
#include <vector>

/**
 * @brief Parameters defining the work-factor for a Key Derivation Function.
 */
struct KdfParams {
  /** @brief CPU/Memory cost parameter (e.g., iterations or memory size). */
  int n;

  /** @brief Block size parameter (affects memory hashing). */
  int r;

  /** @brief Parallelization parameter (number of threads/lanes). */
  int p;

  /** @brief The string identifier mapping to the specific KDF algorithm. */
  std::string algorithm_id;
};

/**
 * @brief Service for deriving strong cryptographic keys from user passwords.
 */
class IKeyDerivationService {
public:
  virtual ~IKeyDerivationService() = default;

  /**
   * @brief Generates a cryptographically secure random salt.
   * @return A vector of random bytes.
   */
  virtual std::vector<unsigned char> GenerateSalt() = 0;

  /**
   * @brief Derives a fixed-length cryptographic key from a plaintext password and salt.
   * @param password The plaintext password provided by the user.
   * @param salt The cryptographic salt.
   * @param params The cost factors and parameters for the specific KDF.
   * @return The derived cryptographic key.
   */
  virtual std::string DeriveKey(std::string_view password, const std::vector<unsigned char>& salt, const KdfParams& params) = 0;

  /**
   * @brief Returns the current recommended default parameters for key derivation.
   * @return The default KdfParams struct.
   */
  virtual KdfParams GetDefaultParams() const = 0;

  /**
   * @brief Parses an algorithm identifier string into a structured KdfParams object.
   * @param algId The raw algorithm identification string.
   * @return The parsed KdfParams.
   */
  virtual KdfParams ParseAlgorithmId(std::string_view algId) const = 0;

  /**
   * @brief Checks if the provided algorithm identifier uses outdated parameters requiring an upgrade.
   * @param algId The algorithm identifier string to evaluate.
   * @return True if a rehash is recommended, false otherwise.
   */
  virtual bool NeedsRehash(std::string_view algId) const = 0;
};

#endif // I_KEY_DERIVATION_SERVICE_H