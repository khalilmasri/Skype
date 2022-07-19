# Cmake needs a minimum required version
cmake_minimum_required(VERSION 3.10)

# Set project name
project(skype)

# Define the path to server source code
set(SERVER_PATH src/server)

# Add it as a subdirectory
add_subdirectory(${SERVER_PATH})

# Define the path to client source code
set(CLIENT_PATH src/client)

# Add it as a subdirectory
add_subdirectory(${CLIENT_PATH})