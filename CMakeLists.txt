# Cmake needs a minimum required version
cmake_minimum_required(VERSION 3.10)

# flags
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set project name
project(skype)

# Default dev flags
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Wcast-qual -Wnon-virtual-dtor -Woverloaded-virtual -Wold-style-cast")

set(SERVER_PATH src/server)

# Define the path to server source code
# Add it as a subdirectory
add_subdirectory(${SERVER_PATH})

# Define the path to client source code
set(CLIENT_PATH src/client)

# Add it as a subdirectory
add_subdirectory(${CLIENT_PATH})
