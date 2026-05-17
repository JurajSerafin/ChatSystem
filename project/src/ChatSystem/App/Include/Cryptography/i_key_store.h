#ifndef I_KEY_STORE_H
#define I_KEY_STORE_H

#include <optional>

struct EncryptedKeyMaterial;

class IKeyStore {
public:
  virtual ~IKeyStore() = default;

  virtual void Store(const EncryptedKeyMaterial& encryptMaterial) = 0;

  virtual std::optional<EncryptedKeyMaterial> Load() = 0;
  
  virtual void Clear() = 0;
};

#endif // I_KEY_STORE_H