# Cmake needs a minimum required version
cmake_minimum_required(VERSION 3.10)

# flags
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set project name
project(skype)

# Default dev flags
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z -Wall -Wextra")
# target_compile_features(${TARGET_NAME} PUBLIC cxx_std_17)
# target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

# Add the option to use more flags
option(CMAKE_EXTRA_FLAGS "Add extra flags" OFF)
if(CMAKE_EXTRA_FLAGS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wpedantic -Wcast-qual -Wnon-virtual-dtor -Woverloaded-virtual -Wold-style-cast")
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
 