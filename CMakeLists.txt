# Cmake needs a minimum required version
cmake_minimum_required(VERSION 3.10)

# flags
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set project name
project(skype)

# Default dev flags
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

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

# Define the path to server source code
set(SERVER_PATH src/server)

# Add it as a subdirectory
add_subdirectory(${SERVER_PATH})

# Define the path to client source code
set(CLIENT_PATH src/client)

# Add it as a subdirectory
add_subdirectory(${CLIENT_PATH})

# this is a server only used to test the server locally.
set(SERVER_TESTER_PATH src/server/test_client)
add_subdirectory(${SERVER_TESTER_PATH})
