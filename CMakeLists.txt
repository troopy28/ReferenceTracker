cmake_minimum_required (VERSION 3.8)

project ("ReferenceTracker" VERSION 1.0.0 LANGUAGES CXX)


set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(OpenCV REQUIRED)

INCLUDE_DIRECTORIES( ${OpenCV_INCLUDE_DIRS} )

add_executable (ReferenceTracker "ReferenceTracker.cpp" "ReferenceTracker.h")

target_link_libraries(ReferenceTracker Qt5::Widgets)
target_link_libraries(ReferenceTracker ${OpenCV_LIBS}) 