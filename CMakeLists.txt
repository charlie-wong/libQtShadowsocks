cmake_minimum_required(VERSION 3.1)
project(SSQT) # ShadowSocks Server/Client

set(SSQT_VERSION_MAJOR 3)
set(SSQT_VERSION_MINOR 0)
set(SSQT_VERSION_PATCH 0)
set(SSQT_VERSION_TWEAK "") # Date/SHA1 Build Time Update

option(SSQT_DBI_BOTAN2 "Enable botan2 download/build/install." OFF)
option(SSQT_DBI_QRENCODE "Enable libqrencode download/build/install" ON)
option(SSQT_AUTOMATIC_QT5 "Enable Qt5 auto search support by xmake." ON)
option(SSQT_WIN_USE_STATIC_GCC_LIBS "Use static GCC for windows build" ON)

option(SSQT_BUILD_TUI "Build shadowsocks TUI server/client." ON)
option(SSQT_BUILD_GUI "Build shadowsocks GUI server/client." ON)

option(SSQT_BUILD_TEST "Build shadowsocks Qt5 testing case." ON)
option(SSQT_BUILD_SHARED "Build shared shadowsocks core library." OFF)

# xmake setup configuration for mpack
option(SSQT_ENABLE_CI "Enable CI build." OFF)
option(SSQT_ENABLE_GCOV "Enable code coverage." ON)
option(SSQT_ENABLE_ASSERTION "Enable assertion." ON)

if(SSQT_DBI_BOTAN2 OR SSQT_DBI_QRENCODE)
    option(SSQT_ENABLE_DEPENDENCY "Enable deps support." ON)
endif()

# cmake xmake modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(xmake.init)
XmakeInit(${XMAKE_VERSION} ${XMAKE_TARBALL_SHA256})

string(REPLACE "v" "" SSQT_VERSION_MMP "${PKG_VERSION}")
add_definitions(-DSSQT_RELEASE_VERSION="${SSQT_RELEASE_VERSION}")

# Down, Extract, Build, Install and Search
include(BuildBotan)
include(BuildQrencode)

# ShadowSocks Qt5 setup by xmake
if(SSQT_AUTOMATIC_QT5)
    set(AUTOMATIC_QT5 AUTOMATIC)
else()
    if(NOT SSQT_QT5_STATIC_PREFIX AND NOT SSQT_QT5_SHARED_PREFIX)
        message(FATAL_ERROR "Qt5 setup error for ShadowSocks!")
    endif()
endif()

XmakeQt5SupportSetup(${AUTOMATIC_QT5}
    STATIC_PREFIX ${SSQT_QT5_STATIC_PREFIX}
    SHARED_PREFIX ${SSQT_QT5_SHARED_PREFIX}
    FATAL_ERROR_IF_NOT_FOUND
)

# C++ 14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32 OR APPLE)
    add_definitions(-DFD_SETSIZE=1024)
endif()

# Find Qt5 library
# https://doc.qt.io/qt-5/cmake-manual.html#variable-reference
find_package(Qt5Gui 5.6 REQUIRED)
find_package(Qt5Core 5.6 REQUIRED)
find_package(Qt5Widgets 5.6 REQUIRED)
find_package(Qt5Network 5.6 REQUIRED)

if(UNIX AND NOT APPLE)
    find_package(Qt5DBus 5.6 REQUIRED)
endif()

#message(STATUS "Qt5Core_VERSION=${Qt5Core_VERSION}")
#message(STATUS "Qt5Core_LIBRARIES=${Qt5Core_LIBRARIES}")
#message(STATUS "Qt5Core_INCLUDE_DIRS=${Qt5Core_INCLUDE_DIRS}")

# Use static shadowSocks core library by default
set(SSQT_CoreLib static_LibQtShadowSocks)
if(SSQT_BUILD_SHARED)
    set(SSQT_CoreLib shared_LibQtShadowSocks)
endif()

add_subdirectory(source)

if(SSQT_BUILD_TEST)
    find_package(Qt5Test)
    if(Qt5Test_FOUND)
        # enable_testing() has to be in the root CMakeLists.txt
        # see https://cmake.org/pipermail/cmake/2010-November/040725.html
        enable_testing()
        add_subdirectory(test)
    endif()
endif()

XmakePrintConfigurationInfo()
