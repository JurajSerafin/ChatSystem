CREATE EXTENSION IF NOT EXISTS "uuid-ossp";

CREATE TABLE users (
 id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
 login VARCHAR(50) NOT NULL,
 tag VARCHAR(60) UNIQUE NOT NULL,
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

CREATE TABLE chats (
 id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
 name VARCHAR(100),
 created_at TIMESTAMPTZ NOT NULL DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE chat_participants (
 chat_id UUID NOT NULL REFERENCES chats(id) ON DELETE CASCADE,
 user_id UUID NOT NULL REFERENCES users(id) ON DELETE CASCADE,
 joined_at TIMESTAMPTZ NOT NULL DEFAULT CURRENT_TIMESTAMP,
 PRIMARY KEY (chat_id, user_id)
);

CREATE TABLE messages (
 id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
 chat_id UUID NOT NULL REFERENCES chats(id) ON DELETE CASCADE,
 sender_id UUID REFERENCES users(id) ON DELETE SET NULL,
 content VARCHAR(10000) NOT NULL,
 type VARCHAR(16) NOT NULL,
 created_at TIMESTAMPTZ NOT NULL DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE message_receipts (
 message_id UUID NOT NULL REFERENCES messages(id) ON DELETE CASCADE,
 user_id UUID NOT NULL REFERENCES users(id) ON DELETE CASCADE,
 delivered_at TIMESTAMPTZ,
 read_at TIMESTAMPTZ,
 PRIMARY KEY (message_id, user_id)
);

CREATE UNIQUE INDEX idx_sessions_token ON sessions(token);

CREATE UNIQUE INDEX idx_users_login ON users(login);

CREATE UNIQUE INDEX idx_users_tag ON users(tag);

CREATE INDEX idx_messages_chat_created ON messages(chat_id, created_at DESC);

CREATE INDEX idx_receipts_user ON message_receipts(user_id);
