# End-to-End Encryption (E2EE) Protocol

This document details the application's cryptographic architecture.

The foundation is a **Zero-Knowledge Backend**: the server routes messages, authenticates users, and stores ciphertexts, but it physically lacks the cryptographic keys required to read any user's message.


## 1. Core Principles

The Client is the absolute owner of their cryptographic identity. To achieve E2EE while maintaining acceptable performance, we utilize a hybrid encryption model.

### Cryptographic library: OpenSSL

- **Key Derivation:** `scrypt` (Memory-hard password stretching to defeat brute-force).
- **Asymmetric Encryption:** **RSA/Ed25519** (For identity and asymmetric key exchange).
- **Symmetric Encryption:** **AES-256-GCM** (For fast encryption of actual message payloads).


## 2. The Identity Lifecycle

The `IClientKeyManager` is the orchestrator of a user's cryptographic identity during an active session.

### Registration

1. User provides a password.

2. `OpenSSLKeyDerivationService` uses `scrypt` to generate a Master Encryption Key (MEK), derived from the user's password.

3. `IClientEncryptionService` generates a fresh asymmetric key pair.

4. The private key is symmetrically encrypted using the MEK and saved to the local `Sqlite3KeyStore`.

5. The public key is sent to the Server for other users to query.

### Login

1. User enters their password.

2. `OpenSSLKeyDerivationService` derives the MEK again.

3. The `ClientKeyManager` retrieves the encrypted private key from the local `Sqlite3KeyStore`.

4. The MEK is used to decrypt the private key into volatile RAM, keeping it accessible for the duration of the session.

5. On logout, the private key is cleared from memory. Therefore, it never touches the disk in its plaintext form.

---

## 3. The Messaging Flow

To send a message, the Client uses the `IClientEncryptionService` and `IRestClient` before the payload ever reaches the network.

### Sending a Message

When Alice wants to send a message to a chat (e.g., to Bob and Charlie):

1. **Fetch Keys:**
Alice's client queries the Server for the public keys of all participants in the chat.

2. **Generate Session Key:**
Alice's `IClientEncryptionService` generates a random, single-use AES-256 symmetric key.

3. **Encrypt Payload:**
The plaintext message is encrypted using the AES key.

4. **Wrap Keys:**
The AES key itself is asymmetrically encrypted multiple times—once for each participant, using their respective public keys.

5. **Transmit:**
The client builds a payload containing the ciphertext and a map of user IDs to their specifically wrapped AES key. This is sent to the Server via REST, which stores the data.

### Receiving a Message
When Bob receives a notification that Alice sent a message or fetches his history:

1. **Fetch Ciphertext & Wrapped Key:**
Bob's client downloads the encrypted payload and queries the Server specifically for the AES key, encrypted by his public key.

2. **Unwrap Key:**
Bob uses his private key (held in memory by `ClientKeyManager`) to decrypt the wrapped AES key.

3. **Decrypt Payload:**
Bob uses the recovered AES key to decrypt the actual message text.

4. **Cache Plaintext:** The decrypted message is immediately passed to the `Sqlite3LocalDatabase`, so the UI can quickly render it.