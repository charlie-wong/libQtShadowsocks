include(CMakeParseArguments)

function(XmakeCopyInstallFiles)
    set(optionValueArgs)
    set(oneValueArgs
        INS_DEST        # The install destionation for DLLs
        CPY_DEST        # The copy destionation for DLLs
        CPY_TARGET      # The target for the copy DLLs
    )
    set(multiValueArgs
        FILES           # The DLLs files to operation
        CPY_CMDS_PRE    # The commands before the DLLs copy
        CPY_CMDS_SUF    # The commands after the DLLs copy
    )
    cmake_parse_arguments(auto # prefix
        "${optionValueArgs}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN}
    )

    if(NOT auto_FILES)
        message(FATAL_ERROR "must set FILES")
    endif()

    if(NOT auto_INS_DEST AND NOT auto_CPY_DEST)
        message(FATAL_ERROR "must set INS_DEST/CPY_DEST, or both.")
    endif()

    # check if files exist, auto skip the none exists ones
    foreach(file ${auto_FILES})
        # ${file} must be full path to working
        # - for windows, it must be start by 'X:/'
        # - for unix likes, it should be start by '/'
        if(EXISTS ${file})
            list(APPEND auto_files "${file}")
        endif()
    endforeach()

    if(NOT auto_files)
        return()
    endif()

    if(auto_INS_DEST)
        XmakeInstallHelper(FILES ${auto_files} DESTINATION ${auto_INS_DEST})
    endif()

    if(auto_CPY_DEST)
        if(NOT auto_CPY_TARGET)
            message(FATAL_ERROR "must set CPY_TARGET for auto update.")
        endif()

        # do not copy for each build, just do it onece for performance
        foreach(file ${auto_files})
            get_filename_component(fname ${file} NAME)
            if(NOT EXISTS ${auto_CPY_DEST}/${fname})
                message(STATUS "COPY DLL: ${file} => ${auto_CPY_DEST}/${fname}")
                list(APPEND auto_files_todo "${file}")
            endif()
        endforeach()

        if(NOT auto_files_todo)
            return()
        endif()

        # Always update and run for each build
        add_custom_target(${auto_CPY_TARGET} ALL
            COMMAND ${CMAKE_COMMAND} -E make_directory ${auto_CPY_DEST}
            ${auto_CPY_CMDS_PRE} # before copy do something if any
            COMMAND ${CMAKE_COMMAND} -E copy ${auto_files_todo} ${auto_CPY_DEST}
            ${auto_CPY_CMDS_SUF} # after copy do something if any
        )
    endif()
endfunction()

function(XmakeCopyWinAppDlls execTarget)
    if(NOT HOST_WINDOWS)
        return() # NOT windows, just skip
    endif()

    get_target_property(targetType ${execTarget} TYPE)
    if(NOT targetType STREQUAL EXECUTABLE)
        return() # skip for none executable
    endif()

    set(tmpTgtLocFile "${CMAKE_BINARY_DIR}/tmp.loc.${execTarget}")
    if(EXISTS "${tmpTgtLocFile}")
        file(READ "${tmpTgtLocFile}" targetLoc)
    else()
        file(GENERATE OUTPUT "${tmpTgtLocFile}"
            CONTENT "$<TARGET_FILE:${execTarget}>"
        )
        return() # do not generated yet, just skip
    endif()

    if(NOT EXISTS ${targetLoc})
        return() # do NOT build yet, skip
    endif()

    # Step1. run 'ldd' on the target to get all needed dlls
    execute_process(COMMAND ldd ${execTarget}
        OUTPUT_STRIP_TRAILING_WHITESPACE
        RESULT_VARIABLE isLddOk
        OUTPUT_VARIABLE dllsDepsText
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/${buildType}/bin
    )

    if(NOT isLddOk EQUAL 0)
        message(FATAL_ERROR "ldd ${execTarget} ERROR, STOP")
    endif()

    # Step2. parssing the ldd output, found what dll need to copy
    # write output text to a file for debug usage
    #file(WRITE ${CMAKE_BINARY_DIR}/${buildType}/bin/abc.txt "${dllsDepsText}")

    # convert to cmake list format for easy to process
    string(REPLACE "\n\r" ";" dllsDepsText "${dllsDepsText}")
    string(REPLACE "\n" ";" dllsDepsText "${dllsDepsText}")

    foreach(line ${dllsDepsText})
        # message(STATUS "LINE=[${line}]")
        # Get dll name and referenced file full path
        string(REGEX MATCH "^( *\t*)([^=>]*)( *\t*)=>( *\t*)([^\\(]*).*$"
            match_result "${line}")
        set(dll_name "${CMAKE_MATCH_2}")
        set(dll_file "${CMAKE_MATCH_5}")

        # Remove leading/trailing white spaces
        string(REGEX MATCH "^( *\t*)([^ \t]+)( *\t*)$"
            match_result "${dll_name}")
        set(dll_name "${CMAKE_MATCH_2}")
        string(REGEX MATCH "^( *\t*)([^ \t]+)( *\t*)$"
            match_result "${dll_file}")
        set(dll_file "${CMAKE_MATCH_2}")

        # SKIP the system ones, which start by '/c/Windows'
        string(REGEX MATCH "^/c/Windows/.*$"
            is_windows_dll "${dll_file}")
        if(is_windows_dll)
            #message(STATUS "WIN DLL=[${dll_name}] => [${dll_file}]")
            continue()
        endif()

        # CMAKE_BINARY_DIR may leading with a drive char, fix this
        string(REGEX MATCH "^([A-Za-z]):(.*)$"
            has_drive_char "${CMAKE_BINARY_DIR}")
        if(has_drive_char)
            string(TOLOWER "${CMAKE_MATCH_1}" win_drive)
            set(usr_build_dir "/${win_drive}${CMAKE_MATCH_2}")
        else()
            set(usr_build_dir "${CMAKE_BINARY_DIR}")
        endif()

        # SKIP the BUILD ones, which start by '${usr_build_dir}'
        string(REGEX MATCH "^${usr_build_dir}/.*$"
            is_build_dll "${dll_file}")
        if(is_build_dll)
            #message(STATUS "USR DLL=[${dll_name}] => [${dll_file}]")
            continue()
        endif()

        #message(STATUS "CIL DLL=[${dll_name}] => [${dll_file}]")
        list(APPEND copy_dlls_list ${dll_file})
    endforeach()

    # Step3. copy and install DLLS file as needed
    foreach(dll_file ${copy_dlls_list})
        #message(STATUS "CI DLL => [${dll_file}]")
        # Find the full path, which start by 'X:/'
        get_filename_component(dll_fname ${dll_file} NAME)
        list(APPEND copy_install_dlls "${WIN_DLLS_DIR}/${dll_fname}")
    endforeach()

    if(NOT copy_install_dlls)
        return() # nothing to do, just skip
    endif()

    # Step4. Just do it
    XmakeCopyInstallFiles(
        FILES       "${copy_install_dlls}"
        INS_DEST    "${${XMAKE}_INSTALL_BIN_DIR}"
        CPY_TARGET  "copy-dlls-for-${execTarget}"
        CPY_DEST    "${CMAKE_BINARY_DIR}/${buildType}/bin"
    )
endfunction()
