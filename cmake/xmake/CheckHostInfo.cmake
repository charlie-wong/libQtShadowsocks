# ARGC  actual args number pass to function when called
# ARGV  actual args list when called(all)
# ARGV0 number 1 actual argument
# ARGV1 number 2 actual argument
# ARGV2 number 3
# ...
# ARGN  when call function with more arguments then want,
#       this is the list of the that part args

function(HostNameUserNameLinux user_name host_name)
    find_program(WHOAMI_PROG   whoami)
    find_program(HOSTNAME_PROG hostname)
    mark_as_advanced(WHOAMI_PROG HOSTNAME_PROG)
    # user name
    if(EXISTS ${WHOAMI_PROG})
        execute_process(COMMAND ${WHOAMI_PROG}
            OUTPUT_STRIP_TRAILING_WHITESPACE
            OUTPUT_VARIABLE UserName)
        set(${user_name} "${UserName}" CACHE
            INTERNAL "User Name" FORCE)
    else()
        if(DEFINED ENV{USERNAME})
            set(${user_name} "$ENV{USERNAME}" CACHE
                INTERNAL "User Name" FORCE)
        elseif()
            set(${user_name} "anonymous" CACHE
                INTERNAL "User Name" FORCE)
        endif()
    endif()
    # host name
    if(EXISTS ${HOSTNAME_PROG})
        execute_process(COMMAND ${HOSTNAME_PROG}
                        OUTPUT_STRIP_TRAILING_WHITESPACE
                        OUTPUT_VARIABLE HostName)
        set(${host_name} "${HostName}" CACHE
            INTERNAL "Host Name" FORCE)
    else()
        if(DEFINED ENV{HOSTNAME})
            set(${host_name} "$ENV{HOSTNAME}" CACHE
                INTERNAL "Host Name" FORCE)
        elseif()
            set(${host_name} "anonymous" CACHE
                INTERNAL "Host Name" FORCE)
        endif()
    endif()
endfunction()

function(HostNameUserNameWindows user_name host_name)
    set(${user_name} "$ENV{USERNAME}" CACHE
        INTERNAL "User Name" FORCE)
    set(${host_name} "$ENV{USERDOMAIN}" CACHE
        INTERNAL "Host Name" FORCE)
endfunction()

function(HostSystemInfoLinux os_name os_version)
    find_program(LSB_RELEASE_PROG  lsb_release)
    mark_as_advanced(LSB_RELEASE_PROG)
    if(EXISTS ${LSB_RELEASE_PROG})
        execute_process(COMMAND ${LSB_RELEASE_PROG} -i
            OUTPUT_STRIP_TRAILING_WHITESPACE
            OUTPUT_VARIABLE  distributor_id)
        execute_process(COMMAND ${LSB_RELEASE_PROG} -r
            OUTPUT_STRIP_TRAILING_WHITESPACE
            OUTPUT_VARIABLE  release_version)
        string(REGEX REPLACE "^Release:[ \t]*([0-9.]*)$"
            "\\1" release_version "${release_version}")
        string(REGEX REPLACE "^Distributor ID:[ \t]*([^ ]*)$"
            "\\1" distributor_id "${distributor_id}")
        set(${os_name} "${distributor_id}" CACHE
            INTERNAL "System Name" FORCE)
        set(${os_version} "${release_version}" CACHE
            INTERNAL "System Version" FORCE)
        return()
    endif()

    execute_process(COMMAND cat /etc/issue
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
        RESULT_VARIABLE  read_status
        OUTPUT_VARIABLE  issue_txt)
    if(read_status EQUAL 0)
        string(REGEX REPLACE "^[ \t]*([^0-9. ]*)[ \t]*[0-9].*"
            "\\1" distributor_id "${issue_txt}")
        string(REGEX REPLACE "^[ \t]*[a-zA-Z]*[^0-9.]*[ \t]*([0-9.]+)[ \t]*.*"
            "\\1" release_version "${issue_txt}")
        set(${os_name} "${distributor_id}" CACHE
            INTERNAL "System Name" FORCE)
        set(${os_version} "${release_version}" CACHE
            INTERNAL "System Version" FORCE)
        return()
    endif()

    message(AUTHOR_WARNING "[Linux] => TODO: support more linux distribution.")

    set(${os_name} "Linux" CACHE
        INTERNAL "System Name" FORCE)
    set(${os_version} "" CACHE
        INTERNAL "System Version" FORCE)
endfunction()

function(HostSystemInfoWindows os_name os_version)
    execute_process(COMMAND cmd /c ver
        OUTPUT_STRIP_TRAILING_WHITESPACE
        OUTPUT_VARIABLE win_ver_val)

    string(REGEX MATCH "[0-9.]+" win_ver ${win_ver_val})
    set(${os_version} "${win_ver}" CACHE INTERNAL "System Version" FORCE)

    string(SUBSTRING "${win_ver}" 0 3 major_version)
    if(major_version STREQUAL "6.1")
        set(${os_name} "Windows 7" CACHE INTERNAL "System Name" FORCE)
        return()
    endif()

    if(major_version STREQUAL "6.2" OR major_version STREQUAL "6.3")
        set(${os_name} "Windows 8" CACHE INTERNAL "System Name" FORCE)
        return()
    endif()

    string(SUBSTRING "${win_ver}" 0 4 major_version)
    if(major_version STREQUAL "6.10")
        set(${os_name} "Windows 10" CACHE INTERNAL "System Name" FORCE)
        return()
    endif()

    set(${os_name} "Windows" CACHE INTERNAL "System Name" FORCE)
endfunction()

function(HostSystemInfoMacosx os_name os_version)
    find_program(SW_VERS_PROG  sw_vers)
    mark_as_advanced(SW_VERS_PROG)
    if(EXISTS ${SW_VERS_PROG})
        execute_process(COMMAND ${SW_VERS_PROG} -productName
            OUTPUT_STRIP_TRAILING_WHITESPACE
            OUTPUT_VARIABLE  mac_pro_name)
        execute_process(COMMAND ${SW_VERS_PROG} -productVersion
            OUTPUT_STRIP_TRAILING_WHITESPACE
            OUTPUT_VARIABLE  mac_pro_version)
        execute_process(COMMAND ${SW_VERS_PROG} -buildVersion
            OUTPUT_STRIP_TRAILING_WHITESPACE
            OUTPUT_VARIABLE  mac_build_version)
        set(${os_name} "${mac_pro_name}" CACHE INTERNAL "System Name" FORCE)
        set(${os_version} "${mac_pro_version}-${mac_build_version}" CACHE
            INTERNAL "System Version" FORCE)
        return()
    endif()

    message(AUTHOR_WARNING "[Macos] => do not found programme 'sw_vers'.")
    set(${os_name} "Macos" CACHE INTERNAL "System Name" FORCE)
    set(${os_version} "" CACHE INTERNAL "System Version" FORCE)
endfunction()

function(HostSystemTimeLinux system_time)
    find_program(DATE_PROG date)
    mark_as_advanced(DATE_PROG)
    if(EXISTS ${DATE_PROG})
        execute_process(COMMAND ${DATE_PROG} "+%Y-%m-%d\ %T\ %z"
            OUTPUT_STRIP_TRAILING_WHITESPACE
            OUTPUT_VARIABLE date_time)
        set(${system_time} "${date_time}" CACHE INTERNAL "Timestamp" FORCE)
        return()
    endif()
    set(${system_time} "xxxx-xx-xx xx:xx:xx" CACHE INTERNAL "Timestamp" FORCE)
endfunction()

function(HostSystemTimeWindows time)
    # see: https://stackoverflow.com/questions/5300572/show-execute-process-output-for-commands-like-dir-or-echo-on-stdout
    # cmd /c echo %date:~0,4%-%date:~5,2%-%date:~8,2% %time:~0,2%:%time:~3,2%:%time:~6,2%
    execute_process(COMMAND cmd /c echo %date:~0,4%-%date:~5,2%-%date:~8,2%
        OUTPUT_STRIP_TRAILING_WHITESPACE
        OUTPUT_VARIABLE date)
    execute_process(COMMAND cmd /c time /T
        OUTPUT_STRIP_TRAILING_WHITESPACE
        OUTPUT_VARIABLE hhmm)
    execute_process(COMMAND cmd /c echo %time:~6,2%
        OUTPUT_STRIP_TRAILING_WHITESPACE
        OUTPUT_VARIABLE ss)
    set(${time} "${date} ${hhmm}:${ss}" CACHE INTERNAL "Timestamp" FORCE)
endfunction()

function(HostNameUserName user_name host_name)
    if(HOST_LINUX OR HOST_MACOS)
        HostNameUserNameLinux(${user_name} ${host_name})
    else()
        HostNameUserNameWindows(${user_name} ${host_name})
    endif()
endfunction()

function(HostSystemInfo os_name os_version)
    if(HOST_MACOS)
        HostSystemInfoMacosx(${os_name} ${os_version})
    elseif(HOST_WINDOWS OR HOST_WINDOWS_MSYS
           OR HOST_WINDOWS_MINGW OR HOST_WINDOWS_CYGWIN)
        HostSystemInfoWindows(${os_name} ${os_version})
    else()
        HostSystemInfoLinux(${os_name} ${os_version})
    endif()
endfunction()

function(HostSystemTime time)
    if(HOST_LINUX OR HOST_MACOS)
        HostSystemTimeLinux(${time})
    else()
        HostSystemTimeWindows(${time})
    endif()
endfunction()
