# UNIX
if(UNIX OR CMAKE_HOST_UNIX)
    # Check UNIX first, MacOS is a kind of UNIX
    option(xmakeI_HOST_OS_SUPPORTED "Host is supported." ON)
    option(HOST_LINUX "Host System: UNIX or Likes." ON)
    set(HOST_SYSTEM_NAME "linux" CACHE INTERNAL "Host OS Name" FORCE)
endif()

# MacOSX
if(APPLE OR CMAKE_HOST_APPLE)
    if(NOT MACOSX_RPATH)
        set(MACOSX_RPATH ON)
    endif()
    option(xmakeI_HOST_OS_SUPPORTED "Host is supported." ON)
    option(HOST_MACOS "Host System: MacOSX." ON)
    set(HOST_SYSTEM_NAME "macos" CACHE INTERNAL "Host OS Name" FORCE)
endif()

# Windows
if(WIN32)
    option(xmakeI_HOST_OS_SUPPORTED "Host is supported." ON)
    option(HOST_WINDOWS "Host System: Windows." ON)
    set(HOST_SYSTEM_NAME "windows" CACHE INTERNAL "Host OS Name" FORCE)
endif()

if(CYGWIN)
    option(xmakeI_HOST_OS_SUPPORTED "Host is supported." ON)
    option(HOST_WINDOWS "Host System: Windows." ON)
    option(HOST_WINDOWS_CYGWIN "Host System: Windows/Cygwin." ON)
    set(HOST_SYSTEM_NAME "windows" CACHE INTERNAL "Host OS Name" FORCE)
endif()

if(CMAKE_HOST_SYSTEM_NAME MATCHES "MSYS")
    option(xmakeI_HOST_OS_SUPPORTED "Host is supported." ON)
    option(HOST_WINDOWS "Host System: Windows." ON)
    option(HOST_WINDOWS_MSYS "Host System: Windows/Msys." ON)
    set(HOST_SYSTEM_NAME "windows" CACHE INTERNAL "Host OS Name" FORCE)
endif()

if(MINGW OR CMAKE_HOST_SYSTEM_NAME MATCHES "MINGW")
    option(xmakeI_HOST_OS_SUPPORTED "Host is supported." ON)
    option(HOST_WINDOWS "Host System: Windows." ON)
    option(HOST_WINDOWS_MINGW "Host System: Windows/MinGW." ON)
    set(HOST_SYSTEM_NAME "windows" CACHE INTERNAL "Host OS Name" FORCE)
endif()

if(HOST_WINDOWS)
    if(NOT HOST_WINDOWS_CYGWIN
       AND NOT HOST_WINDOWS_MSYS
       AND NOT HOST_WINDOWS_MINGW)
        message(FATAL_ERROR "xmake only support windows by cygwin/msys/mingw!")
    endif()
endif()

set(HOST_SYSTEM_VERSION
    "${CMAKE_HOST_SYSTEM_VERSION}" CACHE INTERNAL "Host OS Name" FORCE)

mark_as_advanced(xmakeI_HOST_OS_SUPPORTED)
if(NOT xmakeI_HOST_OS_SUPPORTED)
    set(err_msg "Not unsupported system: ${CMAKE_HOST_SYSTEM}")
    message(FATAL_ERROR "${err_msg}")
endif()

# set HOST_ARCH to the normalized name: x86 or x86_64
# See https://github.com/axr/solar-cmake/blob/master/TargetArch.cmake
include(CheckSymbolExists)

# x86
check_symbol_exists("__i386__"  ""  T_I386)
check_symbol_exists("_M_IX86"   ""  T_M_IX86)
if(T_M_IX86 OR T_I386)
    set(HOST_ARCH "x86")
    option(HOST_ARCH_32 "Host system is 32-bits." ON)
endif()

# x86_64
check_symbol_exists("__amd64__"   ""  T_AMD64)
check_symbol_exists("__x86_64__"  ""  T_X86_64)
check_symbol_exists("_M_AMD64"    ""  T_M_AMD64)

if(T_M_AMD64 OR T_X86_64 OR T_AMD64)
    set(HOST_ARCH "x86_64")
    option(HOST_ARCH_64 "Host system is 64-bits." ON)
endif()

if(NOT HOST_ARCH_32 AND NOT HOST_ARCH_64)
    message(FATAL_ERROR "Unknown host system architecture!")
endif()

include(TestBigEndian)
TEST_BIG_ENDIAN(HOST_BIG_ENDIAN)

include(xmake/CheckHostInfo)
HostNameUserName(HOST_NAME HOST_USER)
HostSystemInfo(HOST_OS_DIST_NAME HOST_OS_DIST_VERSION)
HostSystemTime(${XMAKE}_RELEASE_TIMESTAMP)

string(REGEX MATCH "^([0-9]+)-([0-9]+)-([0-9]+) +([0-9]+):([0-9]+):([0-9]+).*"
    match_result "${${XMAKE}_RELEASE_TIMESTAMP}")
set(${XMAKE}_RELEASE_YEAR "${CMAKE_MATCH_1}")
set(${XMAKE}_RELEASE_MONTH "${CMAKE_MATCH_2}")
set(${XMAKE}_RELEASE_DAY "${CMAKE_MATCH_3}")
