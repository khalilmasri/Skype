# Skype Server

The server leverages an `PassiveConn` and `ConnectionPoll` to `listen`, `accept` and `receive`, `respond` to multiple client connections. 
Because server has a mere supporting role proving basic user information accross the wire, read and write operations will be sparse and the amount
of data transfered small. For that reason, the server was implemented to run on a single thread to avoid an 
overly complicated design unnecessarily.

The maximum number of connected clients managed by the `ConnectionPoll` may be increased using the `MAX_CONNECTIONS`
macro in `include/shared/connection/connection_poll.hpp`.


## Server Setup

### Ubuntu

On a ubutu box ensure you have all postgres dev libraries install

        sudo apt-get install libpq-dev postgresql-server-dev-all
        
### Mac OSX

        brew install libpq
        brew install postgresql@14

For any OS might be necessary to add you postgres version (`psql --verion`) to `libpqxx` CMakeLists.txt in `src/server/vendor/libpqxx/CMakeLists.txt`. 
For postgres v12.11 for example.

        set(PostgreSQL_ADDITIONAL_VERSIONS "12" "12.11") 
        
For Mac OSX it may also require specific locations of postgres in the same `~/server/vendor/libpqxx/CMakeLists.txt` if compilier cannot find it. 
        
        e.g brew info postgresql@14
        
        find the correct locations from here of the include and lib directories for example on M1 Silicon Mac it was:
        
        set(PostgreSQL_INCLUDE_DIR "/opt/homebrew/Cellar/postgresql@14/14.5_3/include/postgresql@14")
        set(PostgreSQL_LIBRARY "/opt/homebrew/Cellar/postgresql@14/14.5_3/lib/postgresql@14/libpq.dylib")

        
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

            LOGIN john 1234

will be routed as follows

            Controllers::login("john 1234", Request req);

The controller in question will add a `Reply` to `Request.data()` depending on the result of the operation.

           // Login was succesfull then
            Request.set_data(new TextData(Reply::get_message(Reply::r_200)));

The `Request`is then passed to `ActiveConn::respond` to respond to the client.


## Database

The server persists user data user [Postgres](https://www.postgresql.org/). Please visit the [postgres directory
for more information on how to install Postgres locally as well as examples of queries the server using in the implementation.](https://github.com/khalilmasri/Skype/tree/main/postgres)

## Replies

       200 OK
       201 <data>
       202 Invalid token. Please login
       203 Awaiting
       300 Not OK
       301 User not found
       302 User already exist
       303 Recipient not found
       304 Sender not found
       305 Invalid chat id
       306 Already awaiting. Please HANGUP before connecting
       307 Connection requests not found for this user
       500 Internal server error
       501 Invalid command or wrong number of arguments
       502 Empty argument

## Command token

Apart from `LOGIN` and `CREATE`, All commands **must provide** a token back to the server. Clients
can acquire a token by sending a `LOGIN` command to the server.
      
e.g 

    LOGIN john 1234
    201  Df345rdfdsFgdETfdDFG

without arguments

    LIST Df345rdfdsFgdETfdDFG
    201 ...

with arguments

    PENDING Df345rdfdsFgdETfdDFG 4
    201 ...

## User Commands 

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


### `EXIT`

Logs the current user out if a user is logged in. Server will disconnect upon receiving this command.

    EXIT
    201 Goobye.

## Call commands commands

### `CONNECT`

Requests for an UDP, peer-to-peer audio/video connection to a peer client. **Note: a client can only have
a single connection request at the time.**

    CONNECT <contact_id> <local_ip>

    200 OK
    301 User not found
    306 Already awaiting. Please HANGUP before connecting
    500 Internal server error

e.g
    CONNECT 1 192.168.0.14
    200 OK

### `ACCEPT`

Accepts an UDP, peer-to-peer audio/video connection from a peer client.

    ACCEPT <contact_id> <local_ip>

    201 <peer_ip:peer_port>
    301 User not found
    307 Connection requests not found for this user

e.g

    ACCEPT 2 192.168.0.14
    201 127.0.0.1:4000

Accepting an unexisting `CONNECT` requests

    ACCEPT 1029220
    307 Connection requests not found for this user

### `PING`

After sending a `CONNECT` request the client can ping the server to check if peer client has
accepted its connection request.

    PING 

    201 <ip:port>
    203 Awaiting
    301 User not found
    307 Connection requests not found for this user

Pinging a request that has been accepted

    PING
    201 127.0.0.1:400

Pinging a request still waiting

    PING
    203 Awaiting

### `HANGUP`

Allows the client who sent the `CONNECT` request to cancel it.

    HANGUP 
      
    200 OK
    301 User not found
    307 Connection requests not found for this user

### `REJECT`

Peer clients receiving a `CONNECT` request can `REJECT` it. The client who has made the connection request
will be notify on its next `PING` to the server.

    REJECT <contact_id>
      
    200 OK
    301 User not found
    307 Connection requests not found for this user

## Chat commands

### `SEND`

Sends a chat messages to a contact.  The first argument is the `recipient_id`  and second argument the message.
 
    SEND <contact_id> <message>

    200 OK
    303 Recipient not found
    502 Empty Argument

eg

    SEND 4 Hello world!
    200 OK


    SEND 99 Hello world!
    303 Recipient not found


### `PENDING`

Get pending chat messages from the current user. Client may pass in an **optional** argument specifying a `sender_id` to retrieve pending messages from a specific contact.

    PENDING 
    PENDING <contact_id> 

    201 <data>
    301 User not found
    304 Sender not found


e.g. All pending messages for the current user

    PENDING
    201 0,50,104,147:22:2,2022-08-26,2,1,falseGreat, thanks for asking!22:4,2022-08-27,4,1,falseanother message for you love.22:5,2022-08-27,4,1,falsenothing like chats

Pending messages for the current user from a specific sender id.
           
    PENDING 4
    201 0,54,97:22:4,2022-08-27,4,1,falseanother message for you love.22:5,2022-08-27,4,1,falsenothing like chats

Bad `sender_id` request

    PENDING 3293
    304 Sender not found

We have no control over what the user will input as chat message so we cannot relly delimiter to split the text content sent from the server.
Therefore, `PENDING` responses will contain a header specifying the postion to split each chat message. The response header has a `:` delimiter.

    0,54,97 -> first chat start at position 0, second chat start position 54, end of message at position 97.

Spliting chats

    201 0,54,97:22:4,2022-08-27,4,1,falseanother message for you love.22:5,2022-08-27,4,1,falsenothing like chats
                ^ 0 here                                              ^ 54 here                                  ^ 97 here

Each chat response has its own header specifying where to split between the chat metadata and content because again, there is no way to use a delimiter for user inputed content.

    header   delim     metadata                         content
    22         :       4,2022-08-27,4,1,false           another message for you love.

According to the example above we will split at position 22.

    22:4,2022-08-27,4,1,falseanother message for you love.
                             ^ here


As for the chat metadata, fields are delimited by a `,` and are the following

    id      created_at       sender_id     recipient_id      delivered                     
    4       2022-08-27       4             1                 false                        


### `CHAT`

    CHAT 
    CHAT <contact_id> 

    201 <data>
    301 User not found
    304 Sender not found


Chat retrieves all chat messages from the current users. It follows the same convention as `PENDING` as in you can specify a `sender_id` or not.

All chats

    CHAT
    201 0,50,104,147:22:2,2022-08-26,2,1,falseGreat, thanks for asking!22:4,2022-08-27,4,1,falseanother message for you love.22:5,2022-08-27,4,1,falsenothing like chats

Chat from a specific sender

    CHAT 2
    201 0,50:22:2,2022-08-26,2,1,falseGreat, thanks for asking!


Bad sender id

    CHAT 9390
    304 Sender not found
   

### `DELIVERED`

Informs the server which chats has been received/delivered so I can update the state of a chat message to `delivered = TRUE`.
Accepts a list of chat ids delimited by a `,`.
 
     DELIVERED <id1,id2,id3>

     200 OK
     305 Invalid chat <id>
     502 Empty argument

eg

     DELIVERED 1,2,3
     200 OK

All ids in the list must be correct otherwise none will be updated.

     DELIVERED 1,2,2039
     305 Invalid chat id 2039


`DELIVERED` must contain an argument

     DELIVERED 
     502 Empty argument

