if(${XMAKE}_DEBUG_BUILD AND NOT CMAKE_BUILD_TYPE MATCHES "Debug")
    return()
endif()

# project name, logo, vendor, maintainer, summory
set(CPACK_PACKAGE_NAME "${PKG_NAME}")
set(CPACK_PACKAGE_ICON "${PKG_LOGO}")
set(CPACK_PACKAGE_VENDOR "${PKG_VENDOR}")
set(CPACK_PACKAGE_CONTACT "${PKG_MAINTAINER_EMAIL}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PKG_BRIEF_SUMMARY}")

set(CPACK_PACKAGE_VERSION_MAJOR "${XDEMO_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${XDEMO_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${XDEMO_VERSION_PATCH}")

set(CPACK_DEBIAN_PACKAGE_MAINTAINER "${PKG_MAINTAINER_EMAIL}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "${PKG_NAME}-${PKG_VERSION}")

# The source package name, not including the extension
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${PKG_NAME}-${PKG_VERSION}-src")

# NOTE Normally, the debug build should not make the binary package,
# but to make it consistent, do the following to rename package file.
set(pkg_bin_fname "${PKG_NAME}-${PKG_VERSION}")
if(CMAKE_BUILD_TYPE MATCHES "Debug")
    set(pkg_bin_fname "${PKG_NAME}-latest")
endif()
mark_as_advanced(pkg_bin_fname)

# The binary package name, not including the extension
set(CPACK_PACKAGE_FILE_NAME "${pkg_bin_fname}")

# Check the license file and project description file
macro(xmakeI_UsrHasLocalFile varName fileName)
    if(NOT ${varName})
        if(EXISTS ${CMAKE_SOURCE_DIR}/${fileName})
            set(${varName} "${CMAKE_SOURCE_DIR}/${fileName}")
        elseif(EXISTS ${CMAKE_SOURCE_DIR}/${fileName}.txt)
            set(${varName} "${CMAKE_SOURCE_DIR}/${fileName}.txt")
        elseif(EXISTS ${CMAKE_SOURCE_DIR}/doc/${fileName})
            set(${varName} "${CMAKE_SOURCE_DIR}/doc/${fileName}")
        elseif(EXISTS ${CMAKE_SOURCE_DIR}/doc/${fileName}.txt)
            set(${varName} "${CMAKE_SOURCE_DIR}/doc/${fileName}.txt")
        elseif(EXISTS ${CMAKE_SOURCE_DIR}/docs/${fileName})
            set(${varName} "${CMAKE_SOURCE_DIR}/docs/${fileName}")
        elseif(EXISTS ${CMAKE_SOURCE_DIR}/docs/${fileName}.txt)
            set(${varName} "${CMAKE_SOURCE_DIR}/docs/${fileName}.txt")
        endif()
    endif()
endmacro()

xmakeI_UsrHasLocalFile(CPACK_RESOURCE_FILE_LICENSE "LICENSE")
xmakeI_UsrHasLocalFile(CPACK_RESOURCE_FILE_LICENSE "license")
xmakeI_UsrHasLocalFile(CPACK_RESOURCE_FILE_LICENSE "COPYRIGHT")
xmakeI_UsrHasLocalFile(CPACK_RESOURCE_FILE_LICENSE "copyright")
xmakeI_UsrHasLocalFile(CPACK_PACKAGE_DESCRIPTION_FILE "DESCRIPTION")
xmakeI_UsrHasLocalFile(CPACK_PACKAGE_DESCRIPTION_FILE "description")
if(NOT CPACK_PACKAGE_DESCRIPTION_FILE)
    set(CPACK_PACKAGE_DESCRIPTION_FILE "${CPACK_RESOURCE_FILE_LICENSE}")
endif()

if(NOT CPACK_PACKAGE_DESCRIPTION_FILE)
    message(FATAL_ERROR "Cpack LICENSE do NOT exist, STOP!")
endif()

set(CPACK_GENERATOR "TGZ;STGZ") # *.tar.gz, *.sh
set(CPACK_SOURCE_GENERATOR "TGZ;ZIP") # *.tar.gz, *.zip

# Windows Installers
if(HOST_WINDOWS)
    set(CPACK_SOURCE_GENERATOR "")

    if(HOST_ARCH_32)
        set(CPACK_GENERATOR "NSIS")
    else()
        set(CPACK_GENERATOR "NSIS64")
    endif()

    # Root install directory, displayed to end user at installer-run time
    set(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES")
    if(HOST_ARCH_32)
        # "NSIS Package Display Name", text used in the installer GUI
        set(CPACK_NSIS_PACKAGE_NAME "${PKG_NAME} ${PKG_VERSION} WIN32")
    else()
        set(CPACK_NSIS_PACKAGE_NAME "${PKG_NAME} ${PKG_VERSION} WIN64")
    endif()
    # Registry key used to store info about the installation
    set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "${CPACK_NSIS_PACKAGE_NAME}")
endif()

if(${XMAKE}_ENABLE_IFW)
    list(APPEND CPACK_GENERATOR "IFW")
endif()

# The source package files do NOT stripping
set(CPACK_SOURCE_STRIP_FILES false)

# The binary package files to strip
set(CPACK_STRIP_FILES false)

# The source package ignored file regex patterns
set(CPACK_SOURCE_IGNORE_FILES
    # version control files
    "/\\\\.svn/"
    "/\\\\.git/"
    "/\\\\.github/"
    "/\\\\.gitignore$"
    "/\\\\.hooks-config$"
    "/\\\\.gitattributes$"

    # generated temp files
    "/#"
    "~$"
    "\\\\.#"
    "\\\\.swp$"
    "\\\\.swap$"

    # local log/config/debug files
    "\\\\.log$"
    "^local\\\\.mk$"
    "^CMakeLists\\\\.txt\\\\.user$"
    "/\\\\.standard-release/"

    # build and external-deps directory
    "/build/"
    "/\\\\.deps/"

    # Extra common regular temporary files
    "/tmp/"
    "/temp/"
    "/todo/"

    # Extra ignore files set by user
    "${PKG_SOURCE_EXCLUDES}"
)

# Set the options file that needs to be included
configure_file("${CMAKE_CURRENT_LIST_DIR}/CPackOptions.cmake.in"
    "${CMAKE_BINARY_DIR}/CPackOptions.cmake" @ONLY)
set(CPACK_PROJECT_CONFIG_FILE "${CMAKE_BINARY_DIR}/CPackOptions.cmake")

# For preparing a package
#set(CPACK_SET_DESTDIR true)

# The packing working directory
set(CPACK_PACKAGE_DIRECTORY "${CMAKE_BINARY_DIR}/ReleasePackage")

include(xmake/QtIFW)

include(CPack)

if(${XMAKE}_ENABLE_IFW)
    include(CPackIFW)
endif()
