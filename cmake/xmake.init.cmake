# The xmake entry point cmake file
set(XmakeMain "${CMAKE_SOURCE_DIR}/cmake/xmake.cmake")

# Init xmake by download tarball, extrat, and do related setup
#
# This macro function has two arguments, the first one is required,
# which is a string variable contains the xmake tag version value,
# the second one is optional, which is a string variable contains
# the SHA256 value for the tarball that to be download
function(xmakeI_InitTarball)
    #message(STATUS "ARGC=${ARGC}")
    #message(STATUS "ARGV0=${ARGV0}")
    #message(STATUS "ARGV1=${ARGV1}")
    if(${ARGC})
        set(xm_version "${${ARGV0}}")
    endif()

    if(ARGC GREATER 1) # has SHA256
        set(xm_sha256 "${${ARGV1}}")
    endif()

    # The xmake tarball name for downloading
    set(xm_tarball "xmake-${xm_version}.tar.gz")
    # The local file for save the download tarball
    set(xm_dstfile "${CMAKE_SOURCE_DIR}/cmake/${xm_tarball}")

    ####################
    # Download tarball #
    ####################
    if(NOT EXISTS "${xm_dstfile}")
        set(timeout 60)
        set(timeout_msg "${timeout} seconds")
        set(timeout_arg INACTIVITY_TIMEOUT ${timeout})

        # The release tarball download base URL
        set(xm_downloadUrl "https://github.com/gkide/xmake/releases/download")
        set(xm_downloadUrl "${xm_downloadUrl}/${xm_version}/${xm_tarball}")

        message(STATUS "${xm_tarball}: Downloading ...
   Timeout  = ${timeout_msg}
   From URL = ${xm_downloadUrl}
   Save As  = ${xm_dstfile}")

        file(DOWNLOAD ${xm_downloadUrl} ${xm_dstfile}
            ${timeout_arg} SHOW_PROGRESS STATUS status LOG errorLog
        )

        list(GET status 0 errorCode)
        list(GET status 1 errorMsg)

        if(errorCode)
            set(emsg "${xm_tarball}: Downloading failed\n")
            set(emsg "${emsg}Error Code: ${errorCode}\n")
            set(emsg "${emsg}Error String: ${errorMsg}\n")
            set(emsg "${emsg}Error Log: ${errorLog}")
            message(FATAL_ERROR "${emsg}")
        endif()
    endif()

    ###################
    # SHA256 Checking #
    ###################
    if(NOT EXISTS "${xm_dstfile}")
        message(FATAL_ERROR "NOT exist tarball file ${xm_dstfile}")
    endif()

    if(xm_sha256) # just check if the SHA256 value length is valid
        string(LENGTH "${xm_sha256}" xm_sha256_len)
        if(NOT xm_sha256_len EQUAL 64) # SHA256 length is fixed, 64-chars
            set(wmsg "Invalid SHA256 length, it must be 64-chars")
            set(wmsg "${wmsg}, but got '${xm_sha256_len}'")
            message(WARNING "${wmsg}")
            unset(xm_sha256) # invalid SHA256 value, skip SHA256 checking
        endif()
    endif()

    if(xm_sha256) # do SHA256 checking
        message(STATUS "${xm_tarball}: SHA256 Checking ...")
        file(SHA256 ${xm_dstfile} TB_SHA256) # tarball SHA256

        set(zero_SHA256
            "0000000000000000000000000000000000000000000000000000000000000000")
        set(empty_SHA256
            "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855")

        if(TB_SHA256 STREQUAL "${empty_SHA256}")
            # File was empty. It's likely due to lack of SSL support.
            file(REMOVE ${xm_dstfile})
            message(FATAL_ERROR "Failed to download ${xm_dstfile}.
The file is empty and likely means CMake was built without SSL support.
Please use a version of CMake with proper SSL support and try again.")
            elseif((NOT xm_sha256 STREQUAL zero_SHA256)
                AND (NOT xm_sha256 STREQUAL TB_SHA256))
                file(REMOVE ${xm_dstfile}) # NOT null SHA256 and NOT match
                message(FATAL_ERROR "SHA256 checking failed, fix and try again.
Actually SHA256: ${TB_SHA256}
Expected SHA256: ${xm_sha256}")
        endif()
    endif()

    ########################
    # Extracting and Setup #
    ########################
    set(xm_extdest ${CMAKE_SOURCE_DIR}/cmake)
    message(STATUS "${xm_tarball}: Extracting ...
   SRC = ${xm_dstfile}
   DST = ${xm_extdest}")

    execute_process(COMMAND ${CMAKE_COMMAND} -E
        tar xzf ${xm_dstfile}
        WORKING_DIRECTORY ${xm_extdest}
        RESULT_VARIABLE extract_status
    )

    if(NOT extract_status EQUAL 0)
        message(STATUS "${xm_tarball}: Extracting ... error clean up")
        file(REMOVE_RECURSE "${xm_extdest}")
        message(FATAL_ERROR "Error: failed extract ${xm_dstfile}")
    endif()

    message(STATUS "${xm_tarball}: xmake setup and clean ...")
    set(xm_tmpdir ${xm_extdest}/xmake-${xm_version})
    file(COPY ${xm_tmpdir}/cmake/xmake
        DESTINATION ${CMAKE_SOURCE_DIR}/cmake)
    file(COPY ${xm_tmpdir}/cmake/xmake.cmake
        DESTINATION ${CMAKE_SOURCE_DIR}/cmake)
    file(REMOVE_RECURSE "${xm_tmpdir}")
    file(REMOVE ${xm_dstfile})
endfunction()

function(xmakeI_InitGitClone)
    if(NOT GIT_PROG)
        find_program(GIT_PROG git)
        if(NOT GIT_PROG)
            message(FATAL_ERROR "xmake latest repo clone do NOT found git, STOP!")
        endif()
    endif()

    set(xm_repo_url "https://github.com/gkide/xmake")

    message(STATUS "xmake init by git clone ...")
    execute_process(TIMEOUT 120 # 2-min if not done, just STOP
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/cmake
        COMMAND ${GIT_PROG} clone --depth=1 ${xm_repo_url} xmake-latest
        RESULT_VARIABLE is_ok
        ERROR_VARIABLE  log_msg
        OUTPUT_VARIABLE log_msg
    )

    if(NOT is_ok EQUAL 0)
        message(FATAL_ERROR "xmake init error, git-clone return code is ${is_ok}
The error log is:\n${log_msg}")
    endif()

    message(STATUS "xmake init setup ...")
    execute_process(TIMEOUT 120 # 2-min if not done, just STOP
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/cmake
        COMMAND mv xmake-latest/cmake/xmake xmake
        COMMAND mv xmake-latest/cmake/xmake.cmake xmake.cmake
        RESULT_VARIABLE is_ok
        ERROR_VARIABLE  log_msg
        OUTPUT_VARIABLE log_msg
    )

    if(NOT is_ok EQUAL 0)
        message(FATAL_ERROR "xmake init setup error, the return code is ${is_ok}
The error log is:\n${log_msg}")
    endif()

    if(NOT EXISTS ${CMAKE_SOURCE_DIR}/cmake/xmake)
        message(FATAL_ERROR "xmake init setup error")
    endif()

    message(STATUS "xmake init clean up ...")
    file(REMOVE_RECURSE ${CMAKE_SOURCE_DIR}/cmake/xmake-latest)
endfunction()

macro(XmakeInit)
    #message(STATUS "ARGC=${ARGC}")
    #message(STATUS "ARGV0=${ARGV0}")
    #message(STATUS "ARGV1=${ARGV1}")
    if(${ARGC})
        set(xm_VERSION "${ARGV0}") # the git tag string
    endif()

    if(${ARGC} GREATER 1)
        set(xm_SHA256 "${ARGV1}") # the tarball SHA256 value
    endif()

    if(NOT xm_VERSION MATCHES "^[v|V]([0-9]+)((\\.([0-9]+))?(\\.([0-9]+))?)?$")
        unset(xm_SHA256)
        unset(xm_VERSION)
        set(xm_GIT_CLONE true) # do git clone if invalid version
    endif()

    if(NOT EXISTS "${XmakeMain}")
        if(xm_GIT_CLONE)
            xmakeI_InitGitClone()
        else()
            xmakeI_InitTarball(xm_VERSION xm_SHA256)
        endif()
        # xmake just setup, CMakeCache.txt may not correct, just
        # exit the current cmake process, makefile will remove it.
        return()
    endif()

    include(xmake) # include the xmake MAIN ENTRY
endmacro()
