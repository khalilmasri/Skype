# Sets project name
project(server)
set(CMAKE_CXX_COMPILER_ID "clang")
set(CMAKE_CXX_COMPILER_VERSION "13.0.1")


# Include all the source code with cpp extension
file(GLOB SERVER_SOURCES    "*.cpp"
                            "../shared/*.cpp"
                            "../shared/data/*.cpp"
                            "../shared/utils/*.cpp"
                            "../shared/connection/*.cpp"
                        )

# Define CMAKE_BINARY_DIR for the destination of the binary 
set(CMAKE_BINARY_DIR "../../bin")


# Define the output to CMAKE_BINARY_DIR declared above
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

# Executes the server the SERVER_SOURCES & COMMON SOURCES
add_executable(${PROJECT_NAME} ${SERVER_SOURCES})

# Define the include path
set(INCLUDE_DIRS    ../../include/server 
                    ../../include/shared     
                    ../../include/shared/test
                    ../../include/shared/connection 
                    ../../include/shared/data 
                    ../../include/shared/utils 
                ) 


add_subdirectory(vendor/libpqxx build-pqxx)
target_link_libraries(${PROJECT_NAME} PRIVATE pqxx)

    
# Set the include directory path
target_include_directories( ${PROJECT_NAME} PRIVATE  ${INCLUDE_DIRS})


