# Skype Server

The server leverages an `PassiveConn` and `ConnectionPoll` to `listen`, `accept` and `receive`, `respond` to multiple client connections. 
Because server has a mere supporting role proving basic user information accross the wire, read and write operations will be sparse and the amount
of data transfered small. For that reason, the server was implemented to run on a single thread to avoid an 
overly complicated design unnecessarily.

The maximum number of connected clients managed by the `ConnectionPoll` may be increased using the `MAX_CONNECTIONS`
macro in `include/shared/connection/connection_poll.hpp`.


## Server Setup

On a ubutu box ensure you have all postgres dev libraries install

        sudo apt-get install libpq-dev postgresql-server-dev-all

It might be necessary to add you postgres version (`psql --verion`) to `libpqxx` CMakeLists.txt in `src/server/vendor/libpqxx/CMakeLists.txt`. 
For postgres v12.11 for example.

           set(PostgreSQL_ADDITIONAL_VERSIONS "12" "12.11") 

## Request

The server uses a `Request` object to pass around information. An `ActiveConn::accept` will create a `Request`
and which will be used to send back a `200 OK` response to the connected client.

           accept  -> | request |  -> respond(request) -> OK 200

After the connection is established the server will loop in `Server::main_loop` listen to messages from all connected clients or
for new connection requests from other clients.

When a message is detected `ActiveConn::receive` will populate the request with the client information such as 
`Request.m_address`, `Request.m_socket` etc and pass that to the `Router`.


          request -> receive(populates info) ->  route -> respond


## Router and Controllers  

The router is responsible for parsing the command and arguments from the client message and route it to the
relevant `Controller`.  So a message like so


            LOGIN khalil 1234

will be routed as follows

            Controllers::login("Khalil 1234", Request req);

The controller in question will add a `Reply` to `Request.data()` depending on the result of the operation.

           // Login was succesfull then
            Request.set_data(new TextData(Reply::get_message(Reply::r_200)));

The `Request`is then passed to `ActiveConn::respond` to respond to the client.


## Database

The server persists user data user [Postgres](https://www.postgresql.org/). Please visit the [postgres/](postgres/) directory
for more information on how to install Postgres locally as well as examples of queries the server using in the implementation.


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
