# Sets project name
project(server)

# Include all the source code with cpp extension
file(GLOB SERVER_SOURCES "*.cpp")

# Include sources in shared/connection folder
file(GLOB CONNECTION_SOURCES "../shared/connection/*.cpp")

# include tests
file(GLOB TEST_SOURCES "../shared/connection/*.cpp")

# Define CMAKE_BINARY_DIR for the destination of the binary 
set(CMAKE_BINARY_DIR "../../bin")

# Define the output to CMAKE_BINARY_DIR declared above
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

# Executes the server the SERVER_SOURCES & COMMON SOURCES
add_executable(${PROJECT_NAME} ${SERVER_SOURCES} ${CONNECTION_SOURCES} ${TEST_SOURCES})

#  shared/connection include
set(INCLUDE_CONNECTION ../../include/shared/connection/)
#  text include
set(INCLUDE_TEST ../../include/shared/test/)

set(INCLUDE_SERVER ../../include/server)

# Set the include directory path
target_include_directories( ${PROJECT_NAME} PRIVATE  ${INCLUDE_SERVER} ${INCLUDE_CONNECTION} ${INCLUDE_TEST})


