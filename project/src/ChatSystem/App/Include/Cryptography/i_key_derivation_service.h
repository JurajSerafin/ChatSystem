#ifndef I_KEY_DERIVATION_SERVICE_H
#define I_KEY_DERIVATION_SERVICE_H

#include <string>
#include <string_view>
#include <vector>

struct KdfParams {
  int n;
  int r;
  int p;
  std::string algorithm_id;
};

class IKeyDerivationService {
public:
  virtual ~IKeyDerivationService() = default;

  virtual std::vector<unsigned char> GenerateSalt() = 0;

  virtual std::string DeriveKey(std::string_view password, const std::vector<unsigned char>& salt, const KdfParams& params) = 0;

  virtual KdfParams GetDefaultParams() const = 0;

  virtual KdfParams ParseAlgorithmId(std::string_view algId) const = 0;

  virtual bool NeedsRehash(std::string_view algId) const = 0;
};

#endif // I_KEY_DERIVATION_SERVICE_H