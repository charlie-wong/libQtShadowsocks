if(NOT DEPS_ROOT_DIR)
    set(DEPS_ROOT_DIR "${CMAKE_SOURCE_DIR}/.deps"
        CACHE PATH "Dependencies root directory.")
endif()

set(DEPS_DOWNLOAD_DIR "${DEPS_ROOT_DIR}/downloads"
    CACHE PATH "Dependencies download directory.")

set(DEPS_BUILD_DIR "${DEPS_ROOT_DIR}/build"
    CACHE PATH "Dependencies build directory.")
set(DEPS_INSTALL_DIR "${DEPS_ROOT_DIR}/usr"
    CACHE PATH "Dependencies install directory.")

if(HOST_WINDOWS)
    if(HOST_WINDOWS_MSYS)
        set(DEPS_BUILD_DIR "${DEPS_ROOT_DIR}/msys/build")
        set(DEPS_INSTALL_DIR "${DEPS_ROOT_DIR}/msys/usr")
    elseif(HOST_WINDOWS_CYGWIN)
        set(DEPS_BUILD_DIR "${DEPS_ROOT_DIR}/cygwin/build")
        set(DEPS_INSTALL_DIR "${DEPS_ROOT_DIR}/cygwin/usr")
    elseif(HOST_WINDOWS_MINGW AND HOST_ARCH_32)
        set(DEPS_BUILD_DIR "${DEPS_ROOT_DIR}/mingw32/build")
        set(DEPS_INSTALL_DIR "${DEPS_ROOT_DIR}/mingw32/usr")
    elseif(HOST_WINDOWS_MINGW AND HOST_ARCH_64)
        set(DEPS_BUILD_DIR "${DEPS_ROOT_DIR}/mingw64/build")
        set(DEPS_INSTALL_DIR "${DEPS_ROOT_DIR}/mingw64/usr")
    endif()
endif()

set(DEPS_BIN_DIR "${DEPS_INSTALL_DIR}/bin"
    CACHE PATH "Dependencies binary install directory.")
set(DEPS_LIB_DIR "${DEPS_INSTALL_DIR}/lib"
    CACHE PATH "Dependencies library install directory.")
set(DEPS_INCLUDE_DIR "${DEPS_INSTALL_DIR}/include"
    CACHE PATH "Dependencies header install directory.")

# External project targets
include(ExternalProject)
include(CheckCCompilerFlag)
include(CMakeParseArguments)

if(NOT DEPS_BUILD_TYPE)
    set(DEPS_BUILD_TYPE "Release")
endif()

if(NOT MAKE_PROG)
    find_program(MAKE_PROG NAMES gmake make)
    if(MAKE_PROG)
        execute_process(
            COMMAND "${MAKE_PROG}" --version
            OUTPUT_VARIABLE MAKE_VERSION_INFO)
        if(NOT "${MAKE_VERSION_INFO}" MATCHES ".*GNU.*")
            unset(MAKE_PROG)
        endif()
    endif()

    if(NOT MAKE_PROG)
        message(FATAL_ERROR "GNU Make is required to build the dependencies.")
    else()
        message(STATUS "Found GNU Make at ${MAKE_PROG}")
    endif()
endif()

if(NOT GIT_PROG)
    find_program(GIT_PROG NAMES git)
    if(NOT GIT_PROG)
        message(WARNING "Git Program NOT found.")
    else()
        message(STATUS "Found Git at ${GIT_PROG}")
    endif()
endif()

file(MAKE_DIRECTORY ${DEPS_DOWNLOAD_DIR})
include_directories(${DEPS_INSTALL_DIR}/include)

include(xmake/TarballBuild)
include(xmake/RepoTreeBuild)
include(xmake/DownloadExtract)
include(xmake/PrebuildInstall)
