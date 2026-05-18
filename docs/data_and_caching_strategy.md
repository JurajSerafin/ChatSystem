# Database Schema & Storage Overview

This document details the physical storage layer of the ChatSystem application.

The schemas strictly reflect the **Zero-Knowledge** architecture:

- The Server stores highly relational, encrypted data optimized for horizontal scaling
- The Client stores flattened, decrypted data optimized for instant offline UI rendering


# 1. The Server Schema (PostgreSQL)

The server utilizes PostgreSQL.

The following represents the aggregated state of the database after all Flyway/migration scripts (`V1` through `V4`) have been applied.

---

## 1.1 Structural Schema

```sql
-- Enables native UUID generation
CREATE EXTENSION IF NOT EXISTS "uuid-ossp";

-- Core Identity
CREATE TABLE users (
    id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    login VARCHAR(50) NOT NULL UNIQUE,
    tag VARCHAR(60) NOT NULL UNIQUE,
    password_hash VARCHAR(255) NOT NULL,
    public_key TEXT NOT NULL,
    role VARCHAR(50) NOT NULL,
    created_at TIMESTAMPTZ NOT NULL DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE sessions (
    id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    user_id UUID NOT NULL REFERENCES users(id) ON DELETE CASCADE,
    token TEXT UNIQUE NOT NULL,
    created_at TIMESTAMPTZ NOT NULL DEFAULT CURRENT_TIMESTAMP,
    expires_at TIMESTAMPTZ NOT NULL
);

-- Chat Topology
CREATE TABLE chats (
    id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    name VARCHAR(100),
    type VARCHAR(16) NOT NULL DEFAULT 'direct',
    last_message_id UUID REFERENCES messages(id) ON DELETE SET NULL,
    last_message_timestamp TIMESTAMPTZ,
    created_at TIMESTAMPTZ NOT NULL DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE chat_participants (
    chat_id UUID NOT NULL REFERENCES chats(id) ON DELETE CASCADE,
    user_id UUID NOT NULL REFERENCES users(id) ON DELETE CASCADE,
    joined_at TIMESTAMPTZ NOT NULL DEFAULT CURRENT_TIMESTAMP,
    left_at TIMESTAMP NULL,
    PRIMARY KEY (chat_id, user_id)
);

-- End-to-End Encrypted Messaging
CREATE TABLE messages (
    id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    chat_id UUID NOT NULL REFERENCES chats(id) ON DELETE CASCADE,
    sender_id UUID REFERENCES users(id) ON DELETE SET NULL,
    ciphertext VARCHAR(10000) NOT NULL,
    type VARCHAR(16) NOT NULL,
    created_at TIMESTAMPTZ NOT NULL DEFAULT CURRENT_TIMESTAMP
);

-- 1-to-N Cryptographic Key Wrapping
CREATE TABLE message_keys (
    message_id UUID NOT NULL REFERENCES messages(id) ON DELETE CASCADE,
    user_id UUID NOT NULL REFERENCES users(id) ON DELETE CASCADE,
    encrypted_key TEXT NOT NULL,
    PRIMARY KEY (message_id, user_id)
);

-- Delivery Tracking
CREATE TABLE message_receipts (
    message_id UUID NOT NULL REFERENCES messages(id) ON DELETE CASCADE,
    user_id UUID NOT NULL REFERENCES users(id) ON DELETE CASCADE,
    delivered_at TIMESTAMPTZ,
    read_at TIMESTAMPTZ,
    PRIMARY KEY (message_id, user_id)
);
```

---

## 1.2 Structure & Index Reasoning

### Zero-Knowledge Structure (`messages` vs `message_keys`)

The `messages` table strictly holds the ciphertext.

The AES session key required to decrypt that text is stored in `message_keys`.

Because a group chat requires the same AES key to be wrapped differently for each participant, separating `message_keys` into a 1-to-N mapped table prevents duplicating the massive 10,000-character ciphertext for every user.


### Foreign Key Cascades

`ON DELETE CASCADE` is used extensively.

If a user deletes their account:
- sessions
- memberships
- wrapped keys

are automatically purged by PostgreSQL.

This ensures GDPR compliance and prevents orphaned data.

## The Indexing Strategy

### `idx_sessions_token` (Unique)

**Applied to:**
- `sessions(token)`

**Purpose:**
- Every authenticated API request queries this table through authentication checks

**Architectural Benefit:**
- Prevents authentication from becoming a server bottleneck
- Ensures extremely fast token verification under heavy concurrency


### `idx_users_login` (Unique)

**Applied to:**
- `users(login)`

**Purpose:**
- Enforces global uniqueness for login credentials
- Accelerates:
  - `POST /auth/login`

**Architectural Benefit:**
- Enables instant credential lookup
- Minimizes timing discrepancies during password verification
- Helps reduce timing-attack exposure


### `idx_users_tag` (Unique)

**Applied to:**
- `users(tag)`

**Purpose:**
- Enforces uniqueness for public-facing user handles
- Accelerates:
  - `GET /users/tag/{tag}`
  - `GET /users/search`

**Architectural Benefit:**
- Enables instant user discovery
- Prevents expensive full-table scans across the directory


### `idx_messages_chat_created` (Composite)

**Applied to:**
- `messages(chat_id, created_at DESC)`

**Purpose:**
- Optimizes chronological pagination queries:
  - `LIMIT X`
  - `OFFSET Y`

**Architectural Benefit:**
- Perfectly matches chat-history query execution plans
- Eliminates costly sequential scans on large message histories


### `idx_receipts_user`

**Applied to:**
- `message_receipts(user_id)`

**Purpose:**
- Critical for:
  - `/messages/undelivered`

**Architectural Benefit:**
- Instantly resolves pending messages across all chats
- Enables fast offline-to-online synchronization after WebSocket reconnects


# 2. The Client Cache Schema (SQLite3)

The client database acts as the single source of truth for the User Interface.

Implements:
- Cache-Aside Pattern
- Offline-first rendering


## 2.1 Structural Schema (`Sqlite3LocalDatabase`)

```sql
CREATE TABLE IF NOT EXISTS identity (
    id TEXT PRIMARY KEY,
    login TEXT NOT NULL,
    tag TEXT NOT NULL,
    session_token TEXT
);
CREATE TABLE IF NOT EXISTS cached_users (
    id TEXT PRIMARY KEY,
    login TEXT NOT NULL,
    tag TEXT NOT NULL,
    public_key TEXT NOT NULL,
    cached_at INTEGER NOT NULL,
    is_deleted INTEGER NOT NULL DEFAULT 0
);
CREATE TABLE IF NOT EXISTS cached_chats (
    id TEXT PRIMARY KEY,
    name TEXT, last_message_id TEXT,
    last_activity_at INTEGER NOT NULL,
    cached_at INTEGER NOT NULL,
    is_deleted INTEGER NOT NULL DEFAULT 0
);
CREATE TABLE IF NOT EXISTS cached_chat_participants (
    chat_id TEXT NOT NULL,
    user_id TEXT NOT NULL,
    role TEXT NOT NULL,
    is_deleted INTEGER NOT NULL DEFAULT 0,
    PRIMARY KEY (chat_id, user_id)
);
CREATE TABLE IF NOT EXISTS cached_messages (
    id TEXT PRIMARY KEY,
    chat_id TEXT NOT NULL,
    sender_id TEXT NOT NULL,
    plaintext TEXT NOT NULL, 
    type TEXT NOT NULL,
    created_at INTEGER NOT NULL,
    is_read INTEGER NOT NULL DEFAULT 0,
    is_delivered INTEGER NOT NULL DEFAULT 0,
    is_deleted INTEGER NOT NULL DEFAULT 0
);
CREATE INDEX IF NOT EXISTS idx_cached_messages_chat ON cached_messages(chat_id, created_at DESC);
```

---

## 2.2 Structure & Index Reasoning

### Decrypted State (`plaintext`)

Unlike the server, the client stores actual plaintext messages.

After decryption by `IClientEncryptionService`, plaintext is persisted locally.

**Benefits:**
- instant history loading
- local search support
- avoids expensive decryption on UI thread

---

### Soft Deletes (`is_deleted = 0`)

Direct row deletion can race against GUI data bindings.

**Soft deletion:**
- prevents crashes
- allows graceful UI animations
- enables delayed cleanup


### Unix Timestamps (`INTEGER`)

SQLite lacks native datetime storage.

**Timestamps are stored as:**
- milliseconds since Unix Epoch

**Benefits:**
- aligns with:
  - `std::chrono::system_clock::time_point`
- simplifies serialization


### `idx_cached_messages_chat`

Mirrors server-side pagination index.

**Purpose:**
- Fast paginated local history retrieval
- Efficient upward chat scrolling

**Complexity:**
- approximately `O(log n)`

---

# 3. The Cryptographic Vault (`Sqlite3KeyStore`)

The client's private asymmetric key is the most security-critical asset in the entire E2EE architecture.

It is therefore isolated from the normal cache database into a dedicated cryptographic vault.

---

## 3.1 Structural Schema

```sql
CREATE TABLE IF NOT EXISTS key_store (
    id INTEGER PRIMARY KEY CHECK (id = 1),
    algorithm TEXT NOT NULL,
    encrypted_key BLOB NOT NULL,
    salt BLOB NOT NULL,
    stored_at INTEGER NOT NULL
);
```


## 3.2 Structure Reasoning

### Singleton Constraint (`CHECK (id = 1)`)

The vault is designed to contain exactly one active cryptographic identity.

**Benefits:**
- prevents accidental overwrite
- prevents multi-key corruption
- database enforces integrity automatically

**Any second insert:**
- throws SQL constraint violation


### Binary Safety (`BLOB`)

**Cryptographic outputs:**
- encrypted private keys
- salts
- binary ciphertext

are raw byte arrays:
- `std::vector<uint8_t>`

Storing as `TEXT` risks:
- UTF-8 corruption
- truncation
- encoding issues

`BLOB` guarantees:
- exact bit-perfect restoration into memory
