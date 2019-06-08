# xmake git repo is https://github.com/gkide/xmake
# xmakeI_* is for xmake Internal Variable, Function, Macro
cmake_minimum_required(VERSION 2.8.12)
include(xmake/PreventInTreeBuilds)

if(MSVC)
    message(FATAL_ERROR "do NOT support MSVC for now!")
endif()

if(NOT PROJECT_NAME MATCHES "^[A-Za-z0-9_-]+$")
    message(FATAL_ERROR "PROJECT_NAME should consist of [A-Za-z0-9_-]!")
endif()

string(TOUPPER ${PROJECT_NAME} XMAKE) # The Uppercase of Project Name
mark_as_advanced(XMAKE)

string(APPEND ${XMAKE}_RELEASE_VERSION "v${${XMAKE}_VERSION_MAJOR}")
string(APPEND ${XMAKE}_RELEASE_VERSION ".${${XMAKE}_VERSION_MINOR}")
string(APPEND ${XMAKE}_RELEASE_VERSION ".${${XMAKE}_VERSION_PATCH}")

math(EXPR ${XMAKE}_VERSION_NUMBER
    "(${${XMAKE}_VERSION_MAJOR} << 16)
    | (${${XMAKE}_VERSION_MINOR} << 8)
    | (${${XMAKE}_VERSION_PATCH})"
)

# User extra configuration for package
if(EXISTS ${CMAKE_SOURCE_DIR}/ConfigPkg.cmake)
    include(${CMAKE_SOURCE_DIR}/ConfigPkg.cmake)
elseif(EXISTS ${CMAKE_SOURCE_DIR}/cmake/ConfigPkg.cmake)
    include(${CMAKE_SOURCE_DIR}/cmake/ConfigPkg.cmake)
endif()

include(xmake/CheckHostSystem)

# If not set, auto use the lower case of project and make it hidden
if(NOT PKG_NAME)
    string(TOLOWER ${PROJECT_NAME} PKG_NAME)
endif()

if(NOT PKG_VERSION)
    set(PKG_VERSION "${${XMAKE}_RELEASE_VERSION}")
endif()

if(NOT PKG_LICENSE AND PKG_VENDOR)
    set(PKG_LICENSE "Copyright(C) 2018-${${XMAKE}_RELEASE_YEAR} ${PKG_VENDOR}")
endif()

if(NOT PKG_MANUAL_DIR)
    set(PKG_MANUAL_DIR "${CMAKE_BINARY_DIR}")
endif()

# The available build type values
if(NOT CMAKE_CONFIGURATION_TYPES)
    list(APPEND CMAKE_CONFIGURATION_TYPES "Dev")
    list(APPEND CMAKE_CONFIGURATION_TYPES "Debug")
    list(APPEND CMAKE_CONFIGURATION_TYPES "Coverage")

    list(APPEND CMAKE_CONFIGURATION_TYPES "Release")
    list(APPEND CMAKE_CONFIGURATION_TYPES "MinSizeRel")
    list(APPEND CMAKE_CONFIGURATION_TYPES "RelWithDebInfo")
endif()

# In case of not set, set default build type to 'Debug'
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose build type ..." FORCE)
endif()

if(CMAKE_BUILD_TYPE MATCHES "Dev"
   OR CMAKE_BUILD_TYPE MATCHES "Debug"
   OR CMAKE_BUILD_TYPE MATCHES "Coverage")
    add_compile_options(-g)
    add_compile_options(-O0)
    set(${XMAKE}_DEBUG_BUILD ON)
    if(NOT ${XMAKE}_LOG_TYPE OR NOT ${XMAKE}_LOG_LEVEL)
        set(${XMAKE}_LOG_LEVEL 0) # DEV
    endif()
else()
    add_compile_options(-O3)

    if(NOT ${XMAKE}_LOG_TYPE OR NOT ${XMAKE}_LOG_LEVEL)
        set(${XMAKE}_LOG_LEVEL 3) # WARN
    endif()
endif()

string(TOUPPER ${CMAKE_BUILD_TYPE} buildType)
mark_as_advanced(buildType)

# Enable verbose output from Makefile builds.
option(CMAKE_VERBOSE_MAKEFILE OFF)
# Output compile commands to compile_commands.json
option(CMAKE_EXPORT_COMPILE_COMMANDS OFF)

# Output for binary, static/shared libraries.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
foreach(type ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${type} TYPE)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${TYPE} ${CMAKE_BINARY_DIR}/${type}/bin)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${TYPE} ${CMAKE_BINARY_DIR}/${type}/lib)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${TYPE} ${CMAKE_BINARY_DIR}/${type}/lib)
endforeach()

include(xmake/PkgSrcPackage)
include(xmake/InstallHelper)
include(xmake/CompileFlags)
include(xmake/CopyFiles)
include(xmake/Utils)

if(CMAKE_BUILD_TYPE MATCHES "Debug")
    set(pkg_dir_name "${PKG_NAME}-latest")
else()
    set(pkg_dir_name "${PKG_NAME}-${PKG_VERSION}")
endif()
mark_as_advanced(pkg_dir_name)

# Change the default cmake value without overriding the user-provided one
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    if(NOT HOST_WINDOWS)
        set(pkg_install_dir "/opt/${pkg_dir_name}")
    else()
# ----------------------------------------------------------------------
#                    windows of 64-bit          windows of 32-bit
# ----------------------------------------------------------------------
# PROGRAMFILES       "C:\Program Files"         "C:\Program Files (x86)"
# ProgramW6432       "C:\Program Files"         "C:\Program Files"
# PROGRAMFILES(x86)  "C:\Program Files (x86)"   "C:\Program Files (x86)"
# ----------------------------------------------------------------------
# https://docs.microsoft.com/zh-cn/windows/desktop/WinProg64/wow64-implementation-details
        set(pkg_install_dir "$ENV{PROGRAMFILES}/${pkg_dir_name}")
    endif()

    mark_as_advanced(pkg_install_dir)
    set(CMAKE_INSTALL_PREFIX "${pkg_install_dir}" CACHE PATH "" FORCE)
endif()

# Make sure always install to $PKG_NAME-$PKG_VERSION
if(NOT CMAKE_INSTALL_PREFIX MATCHES ${pkg_dir_name})
    set(CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}/${pkg_dir_name}")
endif()

# for Dev, Coverage build
if(${XMAKE}_DEBUG_BUILD AND NOT CMAKE_BUILD_TYPE MATCHES "Debug")
    set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/usr")
endif()

#######################################################################
# dev/pre/nightly => alpha => beta => rc => lts/stable/release => eol #
# https://github.com/gkide/repo-hooks/blob/master/scripts/sync-release#
#######################################################################
if(NOT ${XMAKE}_VERSION_TWEAK)
    if(NOT CMAKE_BUILD_TYPE MATCHES "Release"
       AND NOT CMAKE_BUILD_TYPE STREQUAL "MinSizeRel"
       AND NOT CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
        set(${XMAKE}_VERSION_TWEAK "dev")
    endif()
endif()

if(${XMAKE}_VERSION_TWEAK)
    set(tweak_devs dev pre nightly)
    set(tweak_pres alpha beta rc)
    set(tweak_rels lts stable release eol)
    set(tweak "${${XMAKE}_VERSION_TWEAK}")

    mark_as_advanced(tweak_devs tweak_pres tweak_rels tweak)

    foreach(item IN ITEMS ${tweak_devs} ${tweak_pres} ${tweak_rels})
        if(tweak MATCHES "^(${item})?(\\.?([0-9]+))?(\\+?([a-f0-9]+))?$")
            set(is_normalized_tweak true)
            string(REGEX MATCH "^(${item})?(\\.?([0-9]+))?(\\+?([a-f0-9]+))?$"
               match_result "${tweak}")
            set(tweak_text "${CMAKE_MATCH_1}") # Software Release Cycle
            set(tweak_nums "${CMAKE_MATCH_3}") # YYYYMMDD or numbers
            set(tweak_hash "${CMAKE_MATCH_5}") # repo HASH
            break()
        endif()
    endforeach()

    mark_as_advanced(is_normalized_tweak)
    mark_as_advanced(tweak_text tweak_nums tweak_hash)

    if(NOT is_normalized_tweak)
        string(REPLACE ";" "/" tweak_devs "${tweak_devs}")
        string(REPLACE ";" " -> " tweak_pres "${tweak_pres}")
        string(REPLACE ";" "/" tweak_rels "${tweak_rels}")
        set(normalized "${tweak_devs} -> ${tweak_pres} -> ${tweak_rels}")
        mark_as_advanced(normalized)
        message(AUTHOR_WARNING "Consider the normalized tweaks:\n${normalized}\n")
    endif()

    set(pkg_version_tweak "")
    mark_as_advanced(pkg_version_tweak)
    if(tweak_text)
        set(pkg_version_tweak "${tweak_text}")
        string(APPEND ${XMAKE}_RELEASE_VERSION "-${tweak_text}")
    endif()

    string(LENGTH "${tweak_nums}" tweak_nums_len) # YYYYMMDD...
    mark_as_advanced(tweak_nums_len)
    if(tweak_nums_len GREATER 7 OR tweak_nums_len EQUAL 0)
        set(yyyymmdd "${${XMAKE}_RELEASE_TIMESTAMP}")
        string(REPLACE "\"" "" yyyymmdd "${yyyymmdd}")
        string(REPLACE "\\" "" yyyymmdd "${yyyymmdd}")
        string(SUBSTRING "${yyyymmdd}" 00 10 yyyymmdd)
        string(REPLACE "-" "" yyyymmdd ${yyyymmdd})
        string(APPEND ${XMAKE}_RELEASE_VERSION ".${yyyymmdd}")
        set(tweak_nums "${yyyymmdd}")
    endif()

    if(tweak_nums)
        if(tweak_text)
            set(pkg_version_tweak "${pkg_version_tweak}.${tweak_nums}")
        else()
            set(pkg_version_tweak "${tweak_nums}")
        endif()
    endif()
endif()

add_compile_options(-Wall)
add_compile_options(-fPIC)
add_compile_options(-Wextra)
add_compile_options(-Wunused)
add_compile_options(-Winit-self)
add_compile_options(-Wconversion)
add_compile_options(-Wfatal-errors)
add_compile_options(-Wuninitialized)
add_compile_options(-Wunused-parameter)

include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_BINARY_DIR})

if(${XMAKE}_USE_STATIC_GCC_LIBS)
    # do NOT need the binding gcc runtime library
    # {lib|msys-|cyg-}gcc_s-seh-1.dll for MinGW/MSYS/Cygwin
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -static-libgcc -static")
    # do NOT need the binding gcc runtime library
    # {lib|msys-|cyg-}stdc++-6.dll for MinGW/MSYS/Cygwin
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libstdc++ -static")
endif()

if(${XMAKE}_ENABLE_ASSERTION)
    if(CMAKE_C_FLAGS_${buildType} MATCHES DNDEBUG)
        string(REPLACE "-DNDEBUG" "" CMAKE_C_FLAGS_${buildType}
            "${CMAKE_C_FLAGS_${buildType}}")
    endif()
    if(CMAKE_CXX_FLAGS_${buildType} MATCHES DNDEBUG)
        string(REPLACE "-DNDEBUG" "" CMAKE_CXX_FLAGS_${buildType}
            "${CMAKE_CXX_FLAGS_${buildType}}")
    endif()
else()
    if(NOT CMAKE_C_FLAGS_${buildType} MATCHES DNDEBUG)
        set(CMAKE_C_FLAGS_${buildType}
            "-DNDEBUG ${CMAKE_C_FLAGS_${buildType}}")
    endif()
    if(NOT CMAKE_CXX_FLAGS_${buildType} MATCHES DNDEBUG)
        set(CMAKE_CXX_FLAGS_${buildType}
            "-DNDEBUG ${CMAKE_CXX_FLAGS_${buildType}}")
    endif()
endif()

# Disable CI build by default, like: Travis
if(${XMAKE}_ENABLE_CI)
    add_compile_options("-Werror")
endif()

# Enable code coverage
if(${XMAKE}_ENABLE_GCOV)
    include(xmake/CodeCoverage)
endif()

# Enable ccache for linux & likes by default
if(NOT HOST_WINDOWS AND NOT ${XMAKE}_DISABLE_CCACHE)
    find_program(CCACHE_PROG ccache)
    mark_as_advanced(CCACHE_PROG)
    if(CCACHE_PROG)
        set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ${CCACHE_PROG})
        set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ${CCACHE_PROG})
    else()
        message(AUTHOR_WARNING "NOT found ccache for speeds up recompilation")
    endif()
endif()

# Checking logging level
# DEV(0), DEBUG(1), INFO(2), WARN(3), ERROR(4), FATAL(5), DISABLE(6)
if(${XMAKE}_LOG_LEVEL MATCHES "^[0-6]$")
    if(${XMAKE}_LOG_LEVEL EQUAL 0)
        set(${XMAKE}_LOG_TYPE "DEV")
    elseif(${XMAKE}_LOG_LEVEL EQUAL 1)
        set(${XMAKE}_LOG_TYPE "DEBUG")
    elseif(${XMAKE}_LOG_LEVEL EQUAL 2)
        set(${XMAKE}_LOG_TYPE "INFO")
    elseif(${XMAKE}_LOG_LEVEL EQUAL 3)
        set(${XMAKE}_LOG_TYPE "WARN")
    elseif(${XMAKE}_LOG_LEVEL EQUAL 4)
        set(${XMAKE}_LOG_TYPE "ERROR")
    elseif(${XMAKE}_LOG_LEVEL EQUAL 5)
        set(${XMAKE}_LOG_TYPE "FATAL")
    else()
        set(${XMAKE}_LOG_TYPE "DISABLE")
        add_definitions("-D${XMAKE}_LOG_DISABLE")
    endif()
else()
    message(FATAL_ERROR "ERROR: log level is ${${XMAKE}_LOG_TYPE}(${${XMAKE}_LOG_LEVEL})")
endif()

# Enable static/shared Qt5 support
# - AUTOMATIC       Enable Qt5 support, auto detect from system
# - STATIC_PREFIX   Full path to Qt5 static install, like: /opt/qt-5.9.1
# - SHARED_PREFIX   Full path to Qt5 static install, like: /opt/Qt5.5.1/5.5/gcc_64
# - SHARED_PREFIX   Full path to Qt5 static install, like: /usr/lib/gcc/x86_64-linux-gnu
#
# This should be used in the top Qt5 directory. With the return(), thus on host
# that has Qt5 installed, build the Qt5 part; and on host that has no Qt5
# installed, just skip the Qt5 build part and continue with other parts
macro(XmakeQt5SupportSetup)
    set(optionValueArgs
        AUTOMATIC
        FATAL_ERROR_IF_NOT_FOUND
    )
    set(oneValueArgs
        STATIC_PREFIX  # Qt5 static install prefix
        SHARED_PREFIX  # Qt5 shared install prefix
    )
    set(multiValueArgs)
    cmake_parse_arguments(xmakeI_QT5 # prefix
        "${optionValueArgs}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN}
    )

    if(xmakeI_QT5_STATIC_PREFIX OR xmakeI_QT5_SHARED_PREFIX OR xmakeI_QT5_AUTOMATIC)
        include(xmake/Qt5Helper)
    else()
        if(xmakeI_QT5_FATAL_ERROR_IF_NOT_FOUND)
            message(FATAL_ERROR "NOT found Qt5 library, STOP!")
        endif()
        return()
    endif()
endmacro()

if(${XMAKE}_ENABLE_DEPENDENCY OR ${XMAKE}_ENABLE_APPIMAGE)
    include(xmake/Dependencies)
endif()

if(${XMAKE}_ENABLE_APPIMAGE)
    include(xmake/AppImageBuild)
endif()

if(${XMAKE}_EXPORT_AS_COMPILER_ARGS)
    include(xmake/ExportArgsToCc)
endif()

# Linker flags to be used to create executables
# --verbose is for debug the linking process of executables
#set(CMAKE_EXE_LINKER_FLAGS --verbose)

if(HOST_WINDOWS)
    include(xmake/WindowsConfig)
endif()

include(xmake/GetGitRepoInfo)

# https://cmake.org/cmake/help/latest/module/CTest.html
if(${XMAKE}_ENABLE_CTEST)
    option(${XMAKE}_BUILD_CTEST "Build ctest by default." ON)
    include(CTest) # Automatically creates a BUILD_TESTING, ON by default
endif()

# https://github.com/google/googletest
if(${XMAKE}_ENABLE_GTEST)
    include(xmake/BuildGtest)
    option(${XMAKE}_BUILD_GTEST "Build gtest by default." ON)
    option(BUILD_TESTING ON) # To make consistent, also set it ON
endif()

include(xmake/DoxygenHelper)
include(xmake/SearchLibrary)
include(xmake/PackageConfig)
include(xmake/PrintCmakeInfo)
#XmakePrintConfigurationInfo()
