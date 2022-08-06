# Skype Server

The server 





## Commands & Replies

If a command is not valid the server will return `501 Invalid command`.

### `CREATE`

Creates a new user and log that user in. 

         CREATE <username> <passwors>

Reply

        200 OK
        300 Not OK


### `LOGIN`

Logins a new user. The user remains logged in `EXIT` is called or the connection is broken.

        LOGIN <username> <password>

Reply

        200 OK
        300 Not OK


### `SEARCH`

Searches accross the entire database for a user and returns the user when found.

        SEARCH <username>

Reply

        201 id:1,username:khalil,password:1234,online:true,ip:127.0.0.1
        301 Not found
        202 Please login

### `ADD`

Adds a contact to the current logged in user making the request. The server identifies the user making the request
by his/her IP address.

        ADD <username>

Reply

        200 OK
        301 Not found
        202 Please login
        500 Internal server error


### `REMOVE`

Removes a contact to the current logged in user making the request. The server identifies the user making the request
by his/her IP address.

        REMOVE <username>

Reply

        200 OK
        301 Not found
        202 Please login
        500 Internal server error


### `AVAILABLE`

Checks if a user is currently online and logged in. Returns `300` when user exists but is not online.

        AVAILABLE <username>

Reply

        201 id:1,username:khalil,password:1234,online:true,ip:127.0.0.1
        301 Not found
        300 No OK

### `LIST`

Lists all current logged in users contacts. Users are delimited by a <space> character.

        201 id:1,username:khalil,password:1234,online:true,ip:127.0.0.1 201 id:2,username:mario,password:,online:false,address:1.453.32.1
        301 Not found
        500 Internal server error


### `PING`
      
Client can ping the service to ensure connection is still valid.

     PING

Reply

    200 OK


### `EXIT`

Logs the current user out if a user is logged in. Server will disconnect upon receiving this command.


    EXIT

    201 Goobye.
