find_package(PkgConfig QUIET) # PKG_CONFIG_FOUND

macro(SLSetParentVars pkg version varLibaries varIncludes SearchInfo)
    if(${varIncludes} AND ${varLibaries})
        set(${pkg}_FOUND TRUE PARENT_SCOPE)
        set(${pkg}_VERSION "${version}" PARENT_SCOPE)
        set(${pkg}_LIBRARIES "${${varLibaries}}" PARENT_SCOPE)
        set(${pkg}_INCLUDE_DIRS "${${varIncludes}}" PARENT_SCOPE)
        mark_as_advanced(${pkg}_FOUND ${pkg}_VERSION
            ${pkg}_LIBRARIES ${pkg}_INCLUDE_DIRS
        )
        #message(STATUS "OK - ${SearchInfo} => ${pkg}")
        return()
    endif()
    #message(STATUS "SKIP - ${SearchInfo} => ${pkg}")
endmacro()

# Found library by using find_package, pkg-config, find_path, find_library
#
#  ${pkg_PREFIX}_FOUND         Set to true found if found libraries
#  ${pkg_PREFIX}_VERSION       The found library version
#  ${pkg_PREFIX}_LIBRARIES     The found libraries for linking(-L/-l or file)
#  ${pkg_PREFIX}_INCLUDE_DIRS  The directories for include header
function(XmakeSearchLibrary)
    set(options
        VERBOSE           # Verbose search message, default OFF
        REQUIRED          # Cmake stop if library not found
        SHARED            # Search for shared library only
        STATIC            # Search for static library only
    )
    set(oneValueArgs
        NAME              # The library name to search
        VERSION           # Major[.Minor[.Patch]]
        PREFIX            # The prefix name for output variable
    )
    set(multiValueArgs
        FIND_PACKAGE_ARGS # For find_package args
        PKGCONFIG_ARGS    # For pkg_check_modules args
        FIND_PATH_ARGS    # For find_path args
        FIND_LIBRARY_ARGS # For find_library args
        HEADER_FILES      # The library header files
        EXTRA_SEARCH_DIRS # The extra directory to search
    )

    cmake_parse_arguments(pkg # prefix
        "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN}
    )

    if(NOT pkg_NAME) # The library name
        message(FATAL_ERROR "Must set NAME for library search")
    endif()

    if(NOT pkg_PREFIX)
        set(pkg_PREFIX "${pkg_NAME}")
    endif()

    # Skip if already found the library package
    if(${pkg_PREFIX}_FOUND)
        #return()
    endif()

    if(pkg_VERSION) # version must be format of X.Y.Z
        if(NOT pkg_VERSION MATCHES "^([0-9]+)((\\.([0-9]+))?(\\.([0-9]+))?)?$")
            message(FATAL_ERROR "VERSION must like X.Y.Z, but got '${pkg_VERSION}'")
        endif()
        # Show each part of required version for debug
        #string(REGEX MATCH "^([0-9]+)((\\.([0-9]+))?(\\.([0-9]+))?)?$"
        #       match_result "${pkg_VERSION}")
        #message(STATUS "PKG MAJOR=${CMAKE_MATCH_1}")
        #message(STATUS "PKG MINOR=${CMAKE_MATCH_4}")
        #message(STATUS "PKG PATCH=${CMAKE_MATCH_6}")
    endif()

    set(keep_QUIET QUIET) # Keep quiet by default
    if(pkg_VERBOSE)
        set(keep_QUIET)
    endif()

    if(pkg_EXTRA_SEARCH_DIRS)
        list(APPEND pkg_FIND_PACKAGE_ARGS HINTS ${pkg_EXTRA_SEARCH_DIRS})
        list(APPEND pkg_FIND_PATH_ARGS    HINTS ${pkg_EXTRA_SEARCH_DIRS})
        list(APPEND pkg_FIND_LIBRARY_ARGS HINTS ${pkg_EXTRA_SEARCH_DIRS})
    endif()

    # Module mode check first to found 'Find*.cmake'
    # Config mode check second to found '*Config.cmake' or '*-config.cmake'
    find_package(${pkg_NAME} ${pkg_VERSION}
        ${keep_QUIET} ${pkg_FIND_PACKAGE_ARGS}
    )
    if(${pkg_NAME}_FOUND)
        if(pkg_STATIC)
            set(static_lfn # The static library file name
                ${CMAKE_STATIC_LIBRARY_PREFIX}${pkg_NAME}${CMAKE_STATIC_LIBRARY_SUFFIX}
            )

            foreach(file ${${pkg_NAME}_LIBRARIES})
                get_filename_component(fname "${file}" NAME)
                if("${fname}" STREQUAL "${static_lfn}")
                    set(static_lfn_FOUND TRUE)
                    break()
                endif()
            endforeach()

            if(static_lfn_FOUND)
                SLSetParentVars(${pkg_PREFIX} "${${pkg_NAME}_VERSION}"
                    ${pkg_NAME}_LIBRARIES  ${pkg_NAME}_INCLUDE_DIRS
                    "find_package-STATIC" # debug message
                )
            endif()
        else() # shared or what ever found
            SLSetParentVars(${pkg_PREFIX} "${${pkg_NAME}_VERSION}"
                ${pkg_NAME}_LIBRARIES  ${pkg_NAME}_INCLUDE_DIRS
                "find_package" # debug message
            )
        endif()
    endif()

    if(PKG_CONFIG_FOUND)
        # CMAKE_PREFIX_PATH, CMAKE_FRAMEWORK_PATH, CMAKE_APPBUNDLE_PATH
        set(PKG_CONFIG_USE_CMAKE_PREFIX_PATH ON)
        # Environment variable will be added to pkg-config search path
        if(pkg_EXTRA_SEARCH_DIRS)
            foreach(item "${pkg_EXTRA_SEARCH_DIRS}")
                set(pkg_env "${item}:${pkg_env}")
            endforeach()

            if("$ENV{PKG_CONFIG_PATH}" STREQUAL "")
                set(ENV{PKG_CONFIG_PATH} "${pkg_env}")
            else()
                set(old_PKG_CONFIG_PATH "$ENV{PKG_CONFIG_PATH}")
                set(ENV{PKG_CONFIG_PATH} "${pkg_env}:${old_PKG_CONFIG_PATH}")
            endif()
        endif()

        if(NOT pkg_VERSION)
            pkg_check_modules(${pkg_PREFIX} ${pkg_NAME} ${keep_QUIET}
                ${pkg_PKGCONFIG_ARGS}
            )
        else()
            pkg_check_modules(${pkg_PREFIX} ${pkg_NAME}>=${pkg_VERSION}
                ${keep_QUIET} ${pkg_PKGCONFIG_ARGS}
            )
        endif()

        # restore pkg-config environment variable any way
        set(ENV{PKG_CONFIG_PATH} "${old_PKG_CONFIG_PATH}")
        if(${pkg_PREFIX}_FOUND)
            if(pkg_STATIC)
                SLSetParentVars(${pkg_PREFIX} "${${pkg_PREFIX}_VERSION}"
                    ${pkg_PREFIX}_STATIC_LDFLAGS  ${pkg_PREFIX}_STATIC_INCLUDE_DIRS
                    "pkg_check_modules-STATIC" # debug message
                )
            else() # shared or what ever found
                SLSetParentVars(${pkg_PREFIX} "${${pkg_PREFIX}_VERSION}"
                    ${pkg_PREFIX}_LDFLAGS  ${pkg_PREFIX}_INCLUDE_DIRS
                    "pkg_check_modules" # debug message
                )
            endif()
        endif()
    endif()

    # library header files to find
    if(pkg_HEADER_FILES)
        foreach(file ${pkg_HEADER_FILES})
            find_path(hdr_dir NAMES ${file} ${pkg_FIND_PATH_ARGS})
            if(NOT hdr_dir)
                message(FATAL_ERROR "NOT found header ${file}")
            endif()
            set(${pkg_PREFIX}_HDR_DIRS ${hdr_dir} ${${pkg_PREFIX}_HDR_DIRS})
            unset(hdr_dir) # clear the cache entry variable
        endforeach()
    endif()

    # library files to find
    if(pkg_STATIC)
        list(APPEND lib_NAMES # only to find the static library
             ${CMAKE_STATIC_LIBRARY_PREFIX}${pkg_NAME}${CMAKE_STATIC_LIBRARY_SUFFIX}
        )
    elseif(pkg_SHARED)
        list(APPEND lib_NAMES # only to find the shared library
             ${CMAKE_SHARED_LIBRARY_PREFIX}${pkg_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX}
        )
    else()
        list(APPEND lib_NAMES # then check shared library
             ${CMAKE_STATIC_LIBRARY_PREFIX}${pkg_NAME}${CMAKE_STATIC_LIBRARY_SUFFIX}
        )
        list(APPEND lib_NAMES # then check static library
             ${CMAKE_SHARED_LIBRARY_PREFIX}${pkg_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX}
        )
    endif()

    list(APPEND lib_NAMES ${pkg_NAME}) # library name
    find_library(${pkg_PREFIX}_LIB_FILES NAMES ${lib_NAMES} ${pkg_FIND_LIBRARY_ARGS})

    if(${pkg_PREFIX}_HDR_DIRS OR ${pkg_PREFIX}_LIB_FILES)
        SLSetParentVars(${pkg_PREFIX} "" ${pkg_PREFIX}_LIB_FILES
            ${pkg_PREFIX}_HDR_DIRS "find_path-find_library" # debug message
        )
    endif()

    if(NOT pkg_REQUIRED)
        return()
    endif()

    message(FATAL_ERROR "NOT found library ${pkg_NAME}")
endfunction()

# Test
if(false)
    message(STATUS "================================================")
    set(pkgName glib-2.0)
    XmakeSearchLibrary(NAME ${pkgName} VERBOSE
        VERSION 1.2.3
    )
    message(STATUS "${pkgName}_FOUND=${${pkgName}_FOUND}")
    message(STATUS "${pkgName}_VERSION=${${pkgName}_VERSION}")
    message(STATUS "${pkgName}_LIBRARIES=${${pkgName}_LIBRARIES}")
    message(STATUS "${pkgName}_INCLUDE_DIRS=${${pkgName}_INCLUDE_DIRS}")

    message(STATUS "================================================")
    set(pkgName xdot)
    XmakeSearchLibrary(NAME ${pkgName}
        VERSION 4.5.6
    )
    message(STATUS "${pkgName}_FOUND=${${pkgName}_FOUND}")
    message(STATUS "${pkgName}_VERSION=${${pkgName}_VERSION}")
    message(STATUS "${pkgName}_LIBRARIES=${${pkgName}_LIBRARIES}")
    message(STATUS "${pkgName}_INCLUDE_DIRS=${${pkgName}_INCLUDE_DIRS}")

    message(STATUS "================================================")
    set(pkgName QtGui)
    XmakeSearchLibrary(NAME ${pkgName}
        VERSION 3.87.963
    )
    message(STATUS "${pkgName}_FOUND=${${pkgName}_FOUND}")
    message(STATUS "${pkgName}_VERSION=${${pkgName}_VERSION}")
    message(STATUS "${pkgName}_LIBRARIES=${${pkgName}_LIBRARIES}")
    message(STATUS "${pkgName}_INCLUDE_DIRS=${${pkgName}_INCLUDE_DIRS}")

    message(STATUS "================================================")
    set(pkgName foobar)
    XmakeSearchLibrary(NAME ${pkgName}
        VERSION 0.8.963
        EXTRA_SEARCH_DIRS ${CMAKE_SOURCE_DIR}/build/usr
        HEADER_FILES foobar.h
        FIND_PATH_ARGS HINTS ${CMAKE_SOURCE_DIR}/build/usr/include
        FIND_LIBRARY_ARGS HINTS ${CMAKE_SOURCE_DIR}/build/usr/lib
    )
    message(STATUS "${pkgName}_FOUND=${${pkgName}_FOUND}")
    message(STATUS "${pkgName}_VERSION=${${pkgName}_VERSION}")
    message(STATUS "${pkgName}_LIBRARIES=${${pkgName}_LIBRARIES}")
    message(STATUS "${pkgName}_INCLUDE_DIRS=${${pkgName}_INCLUDE_DIRS}")

    message(FATAL_ERROR "STOP")
endif()
