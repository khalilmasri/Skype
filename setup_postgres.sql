CREATE TABLE IF NOT EXISTS users (
    id BIGSERIAL NOT NULL PRIMARY KEY,
     username VARCHAR(100) NOT NULL,
     password VARCHAR(100) NOT NULL,
     online BOOLEAN NOT NULL,
     address VARCHAR(100) NOT NULL
);

-- seed some values to start with

INSERT INTO users(username, password, online, address)
VALUES ('khalil', '1234', FALSE , '123.453.3.1');

INSERT INTO users(username, password, online, address)
VALUES ('mario', '1234', FALSE , '1.453.32.1');

INSERT INTO users(username, password, online, address)
VALUES ('shakira', '1234', FALSE , '53.423.4.1');

INSERT INTO users(username, password, online, address)
VALUES ('dubius', '1234', FALSE , '33.53.3.1');
