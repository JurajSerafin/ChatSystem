#ifndef ENCRYPTED_KEY_MATERIAL_H
#define ENCRYPTED_KEY_MATERIAL_H

#include <string>
#include <vector>

struct EncryptedKeyMaterial {
  std::vector<unsigned char> salt;

  std::vector<unsigned char> iv;

  std::vector<unsigned char> encrypted_key;

  std::string algorithm;
};

#endif // ENCRYPTED_KEY_MATERIAL_H