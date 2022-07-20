# Sets project name
project(client)

# Include all the source code with cpp extension
file(GLOB CLIENT_SOURCES "*.cpp")

# Include all the source code with cpp extension
file(GLOB COMMON_SOURCES "../shared/*.cpp")

# Define CMAKE_BINARY_DIR for the destination of the binary 
set(CMAKE_BINARY_DIR "../../bin")

# Define the output to CMAKE_BINARY_DIR declared above
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

# Executs the client the CLIENT_SOURCES
add_executable(${PROJECT_NAME} ${CLIENT_SOURCES} ${COMMON_SOURCES})

# Define the include path
set(INCLUDE_COMMON ../../include/shared)
set(INCLUDE_CLIENT ../../include/client)

# Set the include directory path
target_include_directories( ${PROJECT_NAME} PRIVATE ${INCLUDE_COMMON} ${INCLUDE_CLIENT})
