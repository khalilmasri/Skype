# Sets project name
project(server)

# Include all the source code with cpp extension
file(GLOB SERVER_SOURCES "*.cpp")
# Include sources in common folder
file(GLOB COMMON_SOURCES "../common/*.cpp")

# Define CMAKE_BINARY_DIR for the destination of the binary 
set(CMAKE_BINARY_DIR "../../bin")

# Define the output to CMAKE_BINARY_DIR declared above
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

# Executes the server the SERVER_SOURCES & COMMON SOURCES
add_executable(${PROJECT_NAME} ${SERVER_SOURCES} ${COMMON_SOURCES})

# Define the include path
set(INCLUDE_COMMON ../../include/common)
set(INCLUDE_SERVER ../../include/server)

# Set the include directory path
target_include_directories( ${PROJECT_NAME} PRIVATE ${INCLUDE_COMMON} ${INCLUDE_SERVER})


