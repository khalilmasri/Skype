# Sets project name
project(client)
set(CMAKE_CXX_COMPILER_ID "clang")
set(CMAKE_CXX_COMPILER_VERSION "13.0.1")

# Find the packages needed
set(OpenGL_GL_PREFERENCE LEGACY)
find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

# Include all the source code with cpp extension
file(GLOB CLIENT_SOURCES    "*.cpp" 
                            "gui/*.cpp"
                            "gui/base/*.cpp"
                            "user/*.cpp"
                            "../shared/*.cpp"
                            "../shared/data/*.cpp"
                            "../shared/utils/*.cpp"
                            "../shared/connection/*.cpp"
                        ) 

# Define CMAKE_BINARY_DIR for the destination of the binary 
set(CMAKE_BINARY_DIR "../../bin")


# Define the output to CMAKE_BINARY_DIR declared above
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

# Executs the client the CLIENT_SOURCES
add_executable(${PROJECT_NAME} ${CLIENT_SOURCES})


# Define the include path
set(INCLUDE_DIRS    ../../include/client 
                    ../../include/client/gui 
                    ../../include/client/gui/base 
                    ../../include/client/user 
                    ../../include/client/job 
                    ../../include/shared     
                    ../../include/shared/test
                    ../../include/shared/connection 
                    ../../include/shared/data 
                    ../../include/shared/utils 
                ) 

# Set the include directory path
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIRS})
include_directories(${DOCTEST_INCLUDE_DIR})

# Setting preferance on OpenGL

# Linking the libraries
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${OPENGL_gl_LIBRARY})



    
