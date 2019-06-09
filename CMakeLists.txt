cmake_minimum_required(VERSION 3.1)
project(ssqtl) # ShadowSocks Core Library

set(SSQTL_VERSION_MAJOR 2)
set(SSQTL_VERSION_MINOR 1)
set(SSQTL_VERSION_PATCH 0)
# Date/SHA1 Build Time Update
set(SSQTL_VERSION_TWEAK "")

option(SSQTL_DBI_BOTAN2 "Enable botan2 download/build/install." ON)
option(SSQTL_AUTOMATIC_QT5 "Enable Qt5 auto search support by xmake." ON)

option(SSQTL_BUILD_TEST "Build shadowsocks Qt5 testing case." ON)
option(SSQTL_BUILD_SSTUI "Build shadowsocks TUI server/client." ON)
option(SSQTL_BUILD_SHARED "Build shared shadowsocks core library." ON)

# xmake setup configuration for mpack
option(SSQTL_ENABLE_CI "Enable CI build." OFF)
option(SSQTL_ENABLE_GCOV "Enable code coverage." ON)
option(SSQTL_ENABLE_ASSERTION "Enable assertion." ON)
if(SSQTL_DBI_BOTAN2)
    option(SSQTL_ENABLE_DEPENDENCY "Enable deps support." ON)
endif()

# cmake xmake modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(xmake)

include(BuildBotan) # botan2 down, extract, build, install and search

# ShadowSocks Qt5 setup by xmake
if(SSQTL_AUTOMATIC_QT5)
    set(AUTOMATIC_QT5 AUTOMATIC)
else()
    if(NOT SSQTL_QT5_STATIC_PREFIX AND NOT SSQTL_QT5_SHARED_PREFIX)
        message(FATAL_ERROR "Qt5 setup error for ShadowSocks Core!")
    endif()
endif()

XmakeQt5SupportSetup(${AUTOMATIC_QT5}
    STATIC_PREFIX ${SSQTL_QT5_STATIC_PREFIX}
    SHARED_PREFIX ${SSQTL_QT5_SHARED_PREFIX}
    FATAL_ERROR_IF_NOT_FOUND
)

# Find Qt5 library
# https://doc.qt.io/qt-5/cmake-manual.html#variable-reference
find_package(Qt5Gui 5.5 REQUIRED)
find_package(Qt5Core 5.5 REQUIRED)
find_package(Qt5Widgets 5.5 REQUIRED)
find_package(Qt5Network 5.5 REQUIRED)

# C++ 14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32 OR APPLE)
    add_definitions(-DFD_SETSIZE=1024)
endif()

# ShadowSocks Core Library Name
set(SSCL_NAME "QtShadowsocks")

add_subdirectory(source/lib)
add_subdirectory(source/tui)

if(SSQTL_BUILD_TEST)
    find_package(Qt5Test)
    if(Qt5Test_FOUND)
        # enable_testing() has to be in the root CMakeLists.txt
        # see https://cmake.org/pipermail/cmake/2010-November/040725.html
        enable_testing()
        add_subdirectory(test)
    endif()
endif()

XmakePrintConfigurationInfo()
