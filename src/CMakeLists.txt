cmake_minimum_required(VERSION 3.10)
set(TARGET_NAME SNLCompilerGraphic)

set(CMAKE_CXX_STANDARD 11)

include_directories(.)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

file(GLOB_RECURSE SOURCES   "*.cpp")
file(GLOB_RECURSE HEADERS   "*.h")
file(GLOB_RECURSE FORMS     "*.ui")
file(GLOB_RECURSE RESOURCES "*.qrc")
#find_package(PkgConfig REQUIRED)
set(QTPKG Core Gui Widgets)
find_package(Qt5 REQUIRED ${QTPKG})

add_executable(${TARGET_NAME} ${SOURCES} ${HEADERS} ${FORMS} ${RESOURCES})

qt5_use_modules(${TARGET_NAME} ${QTPKG})
