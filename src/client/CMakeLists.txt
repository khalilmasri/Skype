# Sets project name
project(client)

# Find the packages needed
find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

# Include all the source code with cpp extension
file(GLOB CLIENT_SOURCES "*.cpp" 
"../shared/connection/*.cpp" 
"gui/*.cpp")

# Define CMAKE_BINARY_DIR for the destination of the binary 
set(CMAKE_BINARY_DIR "../../bin")

# Define the output to CMAKE_BINARY_DIR declared above
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

# Executs the client the CLIENT_SOURCES
add_executable(${PROJECT_NAME} ${CLIENT_SOURCES})


# Define the include path
set(INCLUDE_DIRS    ../../include/client 
                    ../../include/client/gui 
                    ../../include/shared     
                    ../../include/shared/test
                    ../../include/shared/connection 
                ) 

# Set the include directory path
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIRS})
include_directories(${DOCTEST_INCLUDE_DIR})

# Setting preferance on OpenGL
set(OpenGL_GL_PREFERENCE LEGACY)

# Linking the libraries
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})
target_link_libraries(${PROJECT_NAME} OpenGL::GL)



    