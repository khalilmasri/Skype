# Sets project name
project(client)

# Include all the source code with cpp extension
file(GLOB CLIENT_SOURCES "*.cpp")

 # shared/connection
file(GLOB SHARED_CONN_SOURCES "../shared/connection/*.cpp")

# include tests
file(GLOB TEST_SOURCES "../shared/connection/*.cpp")

# Define CMAKE_BINARY_DIR for the destination of the binary 
set(CMAKE_BINARY_DIR "../../bin")

# Define the output to CMAKE_BINARY_DIR declared above
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

# Executs the client the CLIENT_SOURCES
add_executable(${PROJECT_NAME} ${CLIENT_SOURCES} ${SHARED_CONN_SOURCES})

# Includes
set(INCLUDE_CONNECTION ../../include/shared/connection/)
set(INCLUDE_TEST ../../include/shared/test/)
set(INCLUDE_CLIENT ../../include/client)
set(INCLUDE_SHARED ../../include/shared)

# Set the include directory path
target_include_directories( ${PROJECT_NAME} PRIVATE ${INCLUDE_CLIENT} ${INCLUDE_CONNECTION} ${INCLUDE_TEST} ${INCLUDE_SHARED})
