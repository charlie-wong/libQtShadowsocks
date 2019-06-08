# GetGitBranchInfo(name sha1)
#     get current branch name and full-sha1
#
# GetGitRepoDir(<var>)
#     find current project git repository: .git
#
# GegGitRecentTag(<var> [<additional arguments to git describe> ...])
#     get the current branch recent tag, set it to <var>
#
# GetGitCommitTime(<var> [<additional arguments to git log> ...])
#     get the current branch's commit date time
#
# Just include and processing once
if(__CheckGitRepoInfo__)
    return()
endif()

set(__CheckGitRepoInfo__ TRUE)
mark_as_advanced(__CheckGitRepoInfo__)

# we can use "FindGit.cmake" in CMAKE_MODULE_PATH to find git, then set
#   GIT_FOUND                True if the Git command-line client was found.
#   GIT_EXECUTABLE           Path to Git command-line client.
#   GIT_VERSION_STRING       The version of Git found.
# If git is not in the env-var 'PATH', then
# it can be given by hand using 'GIT_PROG'
if(NOT GIT_FOUND AND GIT_PROG)
    set(GIT_FOUND true)
    set(GIT_EXECUTABLE ${GIT_PROG})
endif()

# must run the following at "include" time, not at function call time,
# to find the full path to this file(CheckGitRepoInfo.cmake) rather than
# the path to a calling list file
get_filename_component(XMAKE_DIR ${CMAKE_CURRENT_LIST_FILE} PATH)
mark_as_advanced(XMAKE_DIR __CheckGitRepoInfo__)

function(GetGitRepoDir git_dir)
    if(FORCED_GIT_DIR) # check FORCED_GIT_DIR first
        set(${git_dir} ${FORCED_GIT_DIR} PARENT_SCOPE)
        return()
    endif()

    set(GIT_DIR $ENV{GIT_DIR})
    if(NOT GIT_DIR) # check GIT_DIR in environment
        set(GIT_PARENT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
        set(GIT_DIR ${GIT_PARENT_DIR}/.git)
    endif()

    # .git dir not found, search parent directories
    while(NOT EXISTS ${GIT_DIR})
        set(GIT_PREVIOUS_PARENT ${GIT_PARENT_DIR})
        get_filename_component(GIT_PARENT_DIR ${GIT_PARENT_DIR} PATH)
        if(GIT_PARENT_DIR STREQUAL GIT_PREVIOUS_PARENT)
            return()
        endif()
        set(GIT_DIR ${GIT_PARENT_DIR}/.git)
    endwhile()

    # check if this is a submodule
    if(NOT IS_DIRECTORY ${GIT_DIR})
        file(READ ${GIT_DIR} submodule)
        string(REGEX REPLACE "gitdir: (.*)\n$"
            "\\1" GIT_DIR_RELATIVE ${submodule})
        get_filename_component(SUBMODULE_DIR ${GIT_DIR} PATH)
        get_filename_component(GIT_DIR
            ${SUBMODULE_DIR}/${GIT_DIR_RELATIVE} ABSOLUTE)
    endif()
    set(${git_dir} ${GIT_DIR} PARENT_SCOPE)
endfunction()

function(GetGitBranchInfo branch_name commit_sha1)
    GetGitRepoDir(GIT_DIR)
    if(NOT GIT_DIR)
        return()
    endif()

    set(USER_MODULES_DIR
        ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/UserModules)
    if(NOT EXISTS ${USER_MODULES_DIR})
        file(MAKE_DIRECTORY ${USER_MODULES_DIR})
    endif()

    if(NOT EXISTS ${GIT_DIR}/HEAD)
        return()
    endif()

    set(HEAD_FILE ${USER_MODULES_DIR}/HEAD)
    configure_file(${GIT_DIR}/HEAD ${HEAD_FILE} COPYONLY)

    # @ONLY
    # - Restrict variable replacement to references of the form @VAR@
    # - This is useful for configuring scripts that use ${VAR} syntax
    configure_file(${XMAKE_DIR}/CheckGitRepoInfo.cmake.in
        ${USER_MODULES_DIR}/CheckGitRepoInfo.cmake @ONLY)
    include(${USER_MODULES_DIR}/CheckGitRepoInfo.cmake)

    set(${branch_name} ${HEAD_BRANCH_NAME} PARENT_SCOPE)
    set(${commit_sha1} ${HEAD_COMMIT_HASH} PARENT_SCOPE)
endfunction()

function(GegGitRecentTag tag_name)
    GetGitRepoDir(GIT_DIR)
    if(NOT GIT_DIR)
        return()
    endif()

    if(NOT GIT_FOUND)
        find_package(Git QUIET)
    endif()

    if(NOT GIT_FOUND)
        set(${tag_name} "git-NOTFOUND" PARENT_SCOPE)
        return()
    endif()

    GetGitBranchInfo(_branch_name _commit_sha1)
    if(NOT _commit_sha1)
        set(${tag_name} "head-hash-NOTFOUND" PARENT_SCOPE)
        return()
    endif()

    execute_process(
        COMMAND ${GIT_EXECUTABLE} describe ${_commit_sha1} ${ARGN}
        WORKING_DIRECTORY ${GIT_DIR}
        RESULT_VARIABLE   res
        OUTPUT_VARIABLE   out
        ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)

    if(NOT res EQUAL 0)
        set(out "${_commit_sha1}-NOTAGNAME")
    endif()

    set(${tag_name} ${out} PARENT_SCOPE)
endfunction()

function(GetGitCommitTime timestamp)
    GetGitRepoDir(GIT_DIR)

    if(NOT GIT_DIR)
        return()
    endif()

    if(NOT GIT_FOUND)
        find_package(Git QUIET)
    endif()

    if(NOT GIT_FOUND)
        return()
    endif()

    execute_process(
        COMMAND ${GIT_EXECUTABLE} log -1 --format="%ci"
        WORKING_DIRECTORY   ${GIT_DIR}
        RESULT_VARIABLE     res
        OUTPUT_VARIABLE     out
        ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)

    if(NOT res EQUAL 0)
        set(out "DateTime-NOTFOUND")
    endif()

    set(${timestamp} ${out} PARENT_SCOPE)
endfunction()
