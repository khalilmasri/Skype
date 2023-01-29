# Sets project name
project(client)
set(CMAKE_CXX_COMPILER_ID "clang")
set(CMAKE_CXX_COMPILER_VERSION "13.0.1")

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

if(UNIX AND NOT APPLE)
  set(LINUX TRUE)
endif()

# QT6 QT5 specificas
find_package(QT NAMES Qt6 REQUIRED COMPONENTS Widgets Multimedia MultimediaWidgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Multimedia MultimediaWidgets)

# Ffmpeg
find_path(AVCODEC_INCLUDE_DIR libavcodec/avcodec.h)
find_library(AVCODEC_LIBRARY avcodec)
find_path(AVUTIL_INCLUDE_DIR libavutil/avutil.h)
find_library(AVUTIL_LIBRARY avutil)

# SDL & OpenCV
find_package(SDL2 REQUIRED)
find_package(OpenCV REQUIRED)

# Include all the source code with cpp extension
file(GLOB CLIENT_SOURCES    "*.cpp" 
                            "gui/*.cpp"
                            "gui/ui/*.ui"
                            "job/*.cpp"
                            "user/*.cpp"
                            "gui/notification/*.cpp"
                            "av/*.cpp"
                            "../shared/*.cpp"
                            "../shared/data/*.cpp"
                            "../shared/utils/*.cpp"
                            "../shared/connection/*.cpp"               
                        ) 

file(GLOB GUI_HEADERS       "../../include/client/gui/welcome_gui.hpp"
                            "../../include/client/gui/central_gui.hpp"
                            "../../include/client/gui/contact_gui.hpp"
                            "../../include/client/gui/call_gui.hpp"
                            "../../include/client/gui/ring_gui.hpp"
                            "../../include/client/gui/menu_gui.hpp"
                            "../../include/client/program.hpp"
                            "../../include/client/job/job_bus.hpp"
                            "../../include/client/gui/"
                            "../../include/client/gui/notification/notification.hpp"
)

file(GLOB RESOURCES "../../misc/icons/resource.qrc")

# Define CMAKE_BINARY_DIR for the destination of the binary 
set(CMAKE_BINARY_DIR "../../bin")

# Define the output to CMAKE_BINARY_DIR declared above
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(${PROJECT_NAME} 
        MANUAL_FINALIZATION
        ${CLIENT_SOURCES} 
        ${GUI_HEADERS} 
        ${RESOURCES}
    )
else()
    add_executable( ${PROJECT_NAME} 
                    ${CLIENT_SOURCES} 
                    ${GUI_HEADERS} 
                    ${RESOURCES}
                    )
endif()

# Define the include path
set(INCLUDE_DIRS    ../../include/client 
                    ../../include/client/gui
                    ../../include/client/user 
                    ../../include/client/job 
                    ../../include/client/av
                    ../../include/shared     
                    ../../include/shared/test
                    ../../include/shared/connection 
                    ../../include/shared/data 
                    ../../include/shared/utils
                    ../../include/client/gui/notification
                              ) 

# Project includes + ffmpeg
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIRS} ${AVCODEC_INCLUDE_DIR} ${AVUTIL_INCLUDE_DIR} )

#Tests
include_directories(${DOCTEST_INCLUDE_DIR})

# SDL & OpenCV Includes
include_directories(${SDL2_INCLUDE_DIRS})
include_directories(${OpenCV_INCLUDE_DIRS})

# Linking the libraries
if(LINUX)
  target_link_libraries(${PROJECT_NAME} PRIVATE
                        Qt${QT_VERSION_MAJOR}::Widgets
                        Qt${QT_VERSION_MAJOR}::Multimedia 
                        Qt${QT_VERSION_MAJOR}::MultimediaWidgets
                        # AV
                        ${AVCODEC_LIBRARY}
                        ${AVUTIL_LIBRARY}
                        ${SDL2_LIBRARIES}
                        ${OpenCV_LIBS}
                        -lz -lswresample -lva-drm -lva-x11 -lva -lvdpau -lX11 -llzma  -lmp3lame
                      )
else()  
  target_link_libraries(${PROJECT_NAME} PRIVATE
                        Qt${QT_VERSION_MAJOR}::Widgets
                        Qt${QT_VERSION_MAJOR}::Multimedia 
                        Qt${QT_VERSION_MAJOR}::MultimediaWidgets
                        # AV
                        ${AVCODEC_LIBRARY}
                        ${AVUTIL_LIBRARY}
                        ${SDL2_LIBRARIES}
                        ${OpenCV_LIBS}
                      )
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
    # MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    # MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(${PROJECT_NAME})
endif()
