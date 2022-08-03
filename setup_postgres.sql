
CREATE TABLE IF NOT EXISTS contacts (
     id BIGSERIAL NOT NULL PRIMARY KEY,
     user_id INT,
     contact_id INT
);

CREATE TABLE IF NOT EXISTS users (
     id BIGSERIAL NOT NULL PRIMARY KEY,
     username VARCHAR(100) NOT NULL,
     password VARCHAR(100) NOT NULL,
     online BOOLEAN NOT NULL,
     address VARCHAR(100) NOT NULL
);

ALTER TABLE contacts 
     ADD FOREIGN KEY (user_id)
     REFERENCES users (id);
  
ALTER TABLE contacts 
     ADD FOREIGN KEY (contact_id)
     REFERENCES users (id);

-- seed some values to start with
INSERT INTO users(username, password, online, address)
VALUES ('khalil', '1234', FALSE , '123.453.3.1');

INSERT INTO users(username, password, online, address)
VALUES ('mario', '1234', FALSE , '1.453.32.1');

INSERT INTO users(username, password, online, address)
VALUES ('shakira', '1234', FALSE , '53.423.4.1');

INSERT INTO users(username, password, online, address)
VALUES ('dubius', '1234', FALSE , '33.53.3.1');

INSERT INTO contacts(user_id, contact_id)
VALUES (1, 2);

INSERT INTO contacts(user_id, contact_id)
VALUES (1, 3);

INSERT INTO contacts(user_id, contact_id)
VALUES (3, 1);





