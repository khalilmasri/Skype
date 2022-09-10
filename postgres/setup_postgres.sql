DROP TABLE IF EXISTS chats;
DROP TABLE IF EXISTS contacts;
DROP TABLE IF EXISTS tokens;
DROP TABLE IF EXISTS users;

CREATE TABLE IF NOT EXISTS contacts (
     id BIGSERIAL NOT NULL PRIMARY KEY,
     user_id INT,
     contact_id INT
);

CREATE TABLE IF NOT EXISTS users (
     id BIGSERIAL NOT NULL PRIMARY KEY,
     username VARCHAR(100) NOT NULL UNIQUE, -- username must be unique
     password VARCHAR(100) NOT NULL, 
     online BOOLEAN NOT NULL,
     address VARCHAR(100)
);

ALTER TABLE contacts 
     ADD FOREIGN KEY (user_id)
     REFERENCES users (id);
  
ALTER TABLE contacts 
     ADD FOREIGN KEY (contact_id)
     REFERENCES users (id);

INSERT INTO users(username, password, online, address)
VALUES ('john', '1234', FALSE , NULL);

INSERT INTO users(username, password, online, address)
VALUES ('mario', '1234', FALSE , NULL);

INSERT INTO users(username, password, online, address)
VALUES ('shakira', '1234', FALSE , NULL);

INSERT INTO users(username, password, online, address)
VALUES ('marcos', '1234', FALSE , NULL);

INSERT INTO contacts(user_id, contact_id)
VALUES (1, 2);

INSERT INTO contacts(user_id, contact_id)
VALUES (1, 3);

INSERT INTO contacts(user_id, contact_id)
VALUES (3, 1);

INSERT INTO contacts(user_id, contact_id)
VALUES (3, 2);

INSERT INTO contacts(user_id, contact_id)
VALUES (2, 1);

-- TOKENS

CREATE TABLE IF NOT EXISTS tokens (
    id BIGSERIAL NOT NULL PRIMARY KEY,
     created_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP(2),
     user_id INT NOT NULL,
     token VARCHAR(100)
);

ALTER TABLE tokens 
  ADD FOREIGN KEY (user_id)
  REFERENCES users(id);


-- CHATS

CREATE TABLE IF NOT EXISTS chats (
     id BIGSERIAL NOT NULL PRIMARY KEY,
     created_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP(2),
     sender_id INT NOT NULL,
     recipient_id INT NOT NULL,
     delivered BOOLEAN NOT NULL,
     text TEXT
);

ALTER TABLE chats 
  ADD FOREIGN KEY (sender_id)
  REFERENCES users (id);

ALTER TABLE chats 
  ADD FOREIGN KEY (recipient_id)
  REFERENCES users (id);

-- ADDING SOME SLEEP TIME SO THE TIMESTAMPS ARE NOT EQUAL.

INSERT INTO chats(sender_id, recipient_id, text, delivered)
VALUES (1, 3, 'hello who is your day today, friend?', false );

SELECT pg_sleep(1);

INSERT INTO chats(sender_id, recipient_id, text, delivered)
VALUES (1, 3, 'Great, thanks for asking!', false );

SELECT pg_sleep(1);

INSERT INTO chats(sender_id, recipient_id, text, delivered)
VALUES (1, 3, 'Great, thanks for asking!', false );

SELECT pg_sleep(1);

INSERT INTO chats(sender_id, recipient_id, text, delivered)
VALUES (1, 2, 'mario,mario,amrio, my FrIeNd mario', false );

SELECT pg_sleep(1);

INSERT INTO chats(sender_id, recipient_id, text, delivered)
VALUES (1, 2, 'Comme, on; 00mario;;;!', false );

SELECT pg_sleep(1);

INSERT INTO chats(sender_id, recipient_id, text, delivered)
VALUES (1, 2, 'what about luigi? :D!', false );

SELECT pg_sleep(1);

INSERT INTO chats(sender_id, recipient_id, text, delivered)
VALUES (2, 1, 'Sending that message to john?', false );

SELECT pg_sleep(1);

INSERT INTO chats(sender_id, recipient_id, text, delivered)
VALUES (2, 1, 'Shup up johny!!', false );

 SELECT pg_sleep(1);

INSERT INTO chats(sender_id, recipient_id, text, delivered)
VALUES (2, 1, 'time to go the bed :()', false );

SELECT pg_sleep(1);

INSERT INTO chats(sender_id, recipient_id, text, delivered)
VALUES (3, 1, 'Hey John, my name is Mario Prado--', false );

SELECT pg_sleep(1);

INSERT INTO chats(sender_id, recipient_id, text, delivered)
VALUES (3, 1, '#$%^%$###$%^^%#@', false );

SELECT pg_sleep(1);

INSERT INTO chats(sender_id, recipient_id, text, delivered)
VALUES (3, 1, 'Marioleta is my name JJ', false );
