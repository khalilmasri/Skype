# Sets project name
project(client)
set(CMAKE_CXX_COMPILER_ID "clang")
set(CMAKE_CXX_COMPILER_VERSION "13.0.1")

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# QT6 QT5 specificas
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

# Include all the source code with cpp extension
file(GLOB CLIENT_SOURCES    "*.cpp" 
                            "gui/*.cpp"
                            "gui/ui/*.ui"
                            "job/*.cpp"
                            "user/*.cpp"
                            "gui/notification/*.cpp"
                            "../shared/*.cpp"
                            "../shared/data/*.cpp"
                            "../shared/utils/*.cpp"
                            "../shared/connection/*.cpp"               
                        ) 

file(GLOB GUI_HEADERS       "../../include/client/gui/welcome_gui.hpp"
                            "../../include/client/gui/chat_gui.hpp"
                            "../../include/client/gui/contact_gui.hpp"
                            "../../include/client/program.hpp"
                            "../../include/client/job/job_bus.hpp"
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
                    ../../include/shared     
                    ../../include/shared/test
                    ../../include/shared/connection 
                    ../../include/shared/data 
                    ../../include/shared/utils
                    ../../include/client/gui/notification
                ) 

# Set the include directory path
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIRS})
include_directories(${DOCTEST_INCLUDE_DIR})

# Setting preferance on OpenGL

# Linking the libraries
target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

set_target_properties(${PROJECT_NAME} PROPERTIES
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(${PROJECT_NAME})
endif()
