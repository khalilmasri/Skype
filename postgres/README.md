# Postgres setup

In order to run tests the tests in `src/server/postgres.cpp` you must have [postgres]() installed locally and seed the testing data.
Folow the above steps to make that happen.

## 1. Install postgres

### OSX using brew

         $ brew install postgresql

Then

         $ brew services start postgresql

You can stop postgres 

         $ brew services stop postgresql

and check running services like so

         $ brew services list

### Arch

        $ sudo pacman -S postgresql

Postgres will create a user `postgres` in your system upon installation. Use `su` command to login as the postgres user.
This is important because postgres will create directories when you initialize the database.
You want to do that as the `postgres` user so the postgres has the right permissions to access these directories later.

        $ sudo su - postgres

as the postgres user, initialize the database choosing a directory and exit to your user. 

        $ initdb --locale en_US.UTF-8 -D /var/lib/postgres/data
        $ exit

Start postgres (and check if all is running correctly with `status`)

       $ sudo systemctl start postgresql
       $ sudo systemctl status postgresql

If you want postgres to start on boot 

       $ sudo systemctl enable postgresql


## 3. Create database and setup tables

Create a skype database

      $ createdb skype

Make sure you are in the projects root directory e.g. `cd skype`. Login to the database using `psql`(Postgres client)

      $ psql skype

      psql (14.3)
      Type "help" for help.
      skype=#

You can setup and seed the tables with test data using the provided `postgres/setup_postgres.sql` file 

     skype=# \i postgres/setup_postgres.sql
      CREATE TABLE
      CREATE TABLE
      ALTER TABLE
      ALTER TABLE
      INSERT 0 1
      INSERT 0 1
      INSERT 0 1
      INSERT 0 1
      INSERT 0 1
      INSERT 0 1
      INSERT 0 1
      
That created two tables - `users` and `contacts` Make sure all run well by querying the database.

    skype=# SELECT * FROM users;


If you made a mistake and want to setup again delete the tables with

    skype=# DROP TABLE contacts;
    skype=# DROP TABLE users;

There are also some test queries in the `postgres/` directory. You can ran these files the same way you seeded the database

    skype=# \i postgres/list_user_contacts.sql

## Resetting database with test data

You can reset the database and seed the test data by simply running

     skype=# \i postgres/setup_postgres.sql
