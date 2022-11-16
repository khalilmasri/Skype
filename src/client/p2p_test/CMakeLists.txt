# Sets project name
project(p2p_test)
set(CMAKE_CXX_COMPILER_ID "clang")
set(CMAKE_CXX_COMPILER_VERSION "13.0.1")

if(UNIX AND NOT APPLE)
  set(LINUX TRUE)
endif()

# Ffmpeg
find_path(AVCODEC_INCLUDE_DIR libavcodec/avcodec.h)
find_library(AVCODEC_LIBRARY avcodec)
find_path(AVUTIL_INCLUDE_DIR libavutil/avutil.h)
find_library(AVUTIL_LIBRARY avutil)

# SDL & OpenCV
find_package(SDL2 REQUIRED)
find_package(OpenCV REQUIRED)

# linux flags
if(LINUX)
  set(LINUX_COMPILE FLAGS "-lz -lswresample -lva-drm -lva-x11 -lva -lvdpau -lX11 -llzma  -lmp3lame ")
else()
  set(LINUX_COMPILE FLAGS "")
endif()

# Include all the source code with cpp extension
file(GLOB SERVER_SOURCES    "*.cpp"
                            "../../shared/thread_safe_queue.cpp"
                            "../../shared/*.cpp"
                            "../../shared/data/*.cpp"
                            "../../shared/utils/*.cpp"
                            "../../shared/connection/*.cpp"
                            "../../client/peer_to_peer.cpp"
                            "../../client/av/*.cpp"
                        )

# Define CMAKE_BINARY_DIR for the destination of the binary 
set(CMAKE_BINARY_DIR "../../../bin")

# Define the output to CMAKE_BINARY_DIR declared above
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

# Executes the server the SERVER_SOURCES & COMMON SOURCES
add_executable(${PROJECT_NAME} ${SERVER_SOURCES})

# Define the include path
    
 set(INCLUDE_DIRS   ../../../include/shared     
                    ../../../include/shared/connection 
                    ../../../include/shared/test
                    ../../../include/shared/data 
                    ../../../include/shared/utils 
                    ../../../include/client
                    ../../../include/client/av
                    ../../../include/client/user
                ) 

# SDL & OpenCV Includes
include_directories(${SDL2_INCLUDE_DIRS})
include_directories(${OpenCV_INCLUDE_DIRS})

# Set the include directory path
target_include_directories( ${PROJECT_NAME} PRIVATE  ${INCLUDE_DIRS} ${AVCODEC_INCLUDE_DIR} ${AVUTIL_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE 
                      ${AVCODEC_LIBRARY}
                      ${AVUTIL_LIBRARY}
                      ${SDL2_LIBRARIES}
                      ${OpenCV_LIBS}
                      ${LINUX_COMPILE_FLAGS}
  )


