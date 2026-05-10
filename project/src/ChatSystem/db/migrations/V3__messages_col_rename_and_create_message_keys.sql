ALTER TABLE messages RENAME COLUMN content TO ciphertext;

CREATE TABLE message_keys (
  message_id UUID NOT NULL REFERENCES messages(id) ON DELETE CASCADE,
  user_id UUID NOT NULL REFERENCES users(id) ON DELETE CASCADE,
  encrypted_key TEXT NOT NULL,
  PRIMARY KEY (message_id, user_id)
);