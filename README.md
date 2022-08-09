# Skype

The Skype application is divided between two building blocks - [Client](https://github.com/khalilmasri/Skype/tree/main/src/client) and [server](https://github.com/khalilmasri/Skype/tree/main/src/server).

The client runs on the user machine.  Data connections such as audio, video and chat are peer to peer; clients connect directly to each other with the support of a server that provides information about
other clients in the Skype network. The client has a graphic user interface as well as a backend that responsible for communicating with the server, establishing data connections and data processing.

More details on the [client here](https://github.com/khalilmasri/Skype/tree/main/src/client).

The server runs on a remote computer and is responsible for authenticating users, retrieving a users contacts list, either if they are online or not and their public IP address on the web.
The server is necessary because there is not way for a client to know other clients public IP address when most IP addresses on the web are dynamic and may change from time to time.

More on on the [server here](https://github.com/khalilmasri/Skype/tree/main/src/server).


###  Build System

The build is done using CMake and to facilitate this process we use the script `./build.sh`. This script will
produce and run 3 binaries - ./build/bin/server`,  `./build/bin/client` and `./build/bin/test_client`. 

`test_client` is used to test the communication with a deployed `server`. More in the [server readme](https://github.com/khalilmasri/Skype/tree/main/src/server).

To generate the build files please run

            ./build.sh --gen


To build all above mentioned binaries

            ./build.sh --make


To build and run specific binaries

            ./build.sh --run client
            ./build.sh --run server
            ./build.sh --run test_client

Unit tests will run with the `--test` option

            ./build.sh --test server

Cleaning. Clean all will remove all build files requiring  `./build.sh --gen`.

           ./build.sh --clean
           ./build.sh --clean-bin
           ./build.sh --clean-all

### Share code

The code in `src/shared` and `include/shared` is shared between the client and server.

### Deps

This application depends on the following packages

1. [iamgui](https://github.com/ocornut/imgui)
2. [sdl2] (https://github.com/libsdl-org/SDL)
3. [libpqxx](https://github.com/jtv/libpqxx) (Postgres client)
3. [ffmpeg](https://github.com/FFmpeg/FFmpeg)
