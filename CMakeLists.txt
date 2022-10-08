# Cmake needs a minimum required version
cmake_minimum_required(VERSION 3.10)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# flags
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_BUILD_TYPE Debug) # debug build

# Set project name
project(skype)

# Default dev flags
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pthread")

# Add the option to use more flags
option(CMAKE_EXTRA_FLAGS "Add extra flags" OFF)
if(CMAKE_EXTRA_FLAGS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wpedantic -Wcast-qual -Wnon-virtual-dtor -Woverloaded-virtual -Wold-style-cast")
endif()

option(CMAKE_WARNING_FLAGS "Warning flags enabling" ON)
if(NOT CMAKE_WARNING_FLAGS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wformat=0")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wformat=2")
endif()

message("Enabled flags = ${CMAKE_CXX_FLAGS}")


option(CLIENT "Build only client" OFF)
option(SERVER "Build only server" OFF)

if(NOT CLIENT AND NOT SERVER)
    set(SERVER_PATH src/server)
    add_subdirectory(${SERVER_PATH})
    set(CLIENT_PATH src/client)
    add_subdirectory(${CLIENT_PATH})
    set(SERVER_TESTER_PATH src/server/test_client)
    add_subdirectory(${SERVER_TESTER_PATH})
    set(P2P_TESTER_PATH src/client/p2p_test)
    add_subdirectory(${P2P_TESTER_PATH})


elseif(NOT CLIENT)
    set(SERVER_PATH src/server)
    add_subdirectory(${SERVER_PATH})
    set(SERVER_TESTER_PATH src/server/test_client)
    add_subdirectory(${SERVER_TESTER_PATH})

elseif(NOT SERVER)

    set(CLIENT_PATH src/client)
    add_subdirectory(${CLIENT_PATH})
    set(SERVER_TESTER_PATH src/server/test_client)
    add_subdirectory(${SERVER_TESTER_PATH})
    set(P2P_TESTER_PATH src/client/p2p_test)
    add_subdirectory(${P2P_TESTER_PATH})



endif()

# Copies config.conf default to /home/$USER/.skype.conf
configure_file(config.conf $ENV{HOME}/.skype.conf COPYONLY)
