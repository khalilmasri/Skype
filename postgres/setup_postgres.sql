
DROP TABLE IF EXISTS chats;
DROP TABLE IF EXISTS contacts;
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
     address VARCHAR(100) UNIQUE -- IP must be unique
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
VALUES ('marcos', '1234', FALSE , NULL );

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


CREATE TABLE IF NOT EXISTS chats (
     id BIGSERIAL NOT NULL PRIMARY KEY,
     created_at DATE NOT NULL DEFAULT CURRENT_DATE,
     sender_id INT NOT NULL,
     recipient_id INT NOT NULL,
     delivered BOOLEAN NOT NULL,
     text TEXT
);

ALTER TABLE chats 
  ADD FOREIGN KEY (recipient_id)
  REFERENCES users (id);


INSERT INTO chats(sender_id, recipient_id, delivered, text)
VALUES (1, 2, false, 'hello who is your day today, friend?');

INSERT INTO chats(sender_id, recipient_id, delivered, text)
VALUES (2, 1, false, 'Great, thanks for asking!');
