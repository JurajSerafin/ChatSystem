#include "client_key_manager.h"
#include "Cryptography/encrypted_key_material.h"
#include <optional>
#include <stdexcept>

ClientKeyManager::ClientKeyManager(
  IClientEncryptionService* cryptoServiceObs,
  std::unique_ptr<IKeyDerivationService> keyDerivationService,
  std::unique_ptr<IKeyStore> keyStore
) : crypto_service_obs_(cryptoServiceObs),
  key_derivation_service_(std::move(keyDerivationService)),
  key_store_(std::move(keyStore)) {}

KeyPair ClientKeyManager::GenerateAndProtectKeyPair(std::string_view password) {
  KeyPair key_pair = crypto_service_obs_->GenerateKeyPair();

  const auto salt = key_derivation_service_->GenerateSalt();

  const auto kdf_params = key_derivation_service_->GetDefaultParams();

  const auto priv_key_encr_key = key_derivation_service_->DeriveKey(password, salt, kdf_params);

  std::string encr_priv_key = crypto_service_obs_->EncryptSymmetric(key_pair.GetPrivateKey(), priv_key_encr_key);

  key_store_->Store({
    .salt = salt,
    .encrypted_key = std::vector<unsigned char>(encr_priv_key.begin(), encr_priv_key.end()),
    .algorithm = kdf_params.algorithm_id
  });

  return key_pair;
}

std::string ClientKeyManager::GetUnlockedPrivateKey(std::string_view password) const {
  const std::optional<EncryptedKeyMaterial> cached_crypto = key_store_->Load();

  if (!cached_crypto.has_value()) {
    throw std::runtime_error{"Failed to load the encrypted key from a cache."};
  }

  const auto kdf_params = key_derivation_service_->ParseAlgorithmId(cached_crypto->algorithm);

  const auto priv_key_encr_key = key_derivation_service_->DeriveKey(password, cached_crypto->salt, kdf_params);

  std::string_view ciphertext_view(
    reinterpret_cast<const char*>(cached_crypto->encrypted_key.data()),
    cached_crypto->encrypted_key.size()
  );

  return crypto_service_obs_->DecryptSymmetric(ciphertext_view, priv_key_encr_key);
}

void ClientKeyManager::DeleteProtectedKeys() {
  key_store_->Clear();
}

