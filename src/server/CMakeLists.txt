# Sets project name
project(server)

# Include all the source code with cpp extension
file(GLOB SERVER_SOURCES "*.cpp")

# Define CMAKE_BINARY_DIR for the destination of the binary 
set(CMAKE_BINARY_DIR "../../bin")

# Define the output to CMAKE_BINARY_DIR declared above
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

# Executs the server the SERVER_SOURCES
add_executable(${PROJECT_NAME} ${SERVER_SOURCES})

# Define the include path
set(INCLUDE_COMMON ../../include/common)
set(INCLUDE_SERVER ../../include/server)

# Set the include directory path
target_include_directories( ${PROJECT_NAME} PRIVATE ${INCLUDE_COMMON} ${INCLUDE_SERVER})


