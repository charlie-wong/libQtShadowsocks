include(CMakeParseArguments)

# Check prereqs
find_program(GCOV_PROG gcov)
find_program(GCOVR_PROG gcovr)
find_program(PYTHON_PROG python)
find_program(LCOV_PROG NAMES lcov lcov.bat lcov.exe lcov.perl)
find_program(GENHTML_PROG NAMES genhtml genhtml.perl genhtml.bat)

mark_as_advanced(GCOV_PROG GCOVR_PROG PYTHON_PROG LCOV_PROG GENHTML_PROG)

if("${CMAKE_CXX_COMPILER_ID}" MATCHES "(Apple)?[Cc]lang"
   AND "${CMAKE_CXX_COMPILER_VERSION}" VERSION_LESS 3)
    message(AUTHOR_WARNING "Clang version must be 3.0.0 or later, code coverage skip!")
    set(ccr_skip true)
elseif(NOT CMAKE_COMPILER_IS_GNUCXX)
    message(AUTHOR_WARNING "Compiler is not GNU GCC, code coverage skip!")
    set(ccr_skip true)
endif()

if(NOT ${XMAKE}_DEBUG_BUILD)
    set(ccr_skip true)
endif()

mark_as_advanced(ccr_skip)
if(ccr_skip)
    function(XmakeCCRAppendFlags)
        #message(STATUS "Skip Code Coverage for ${CMAKE_BUILD_TYPE} Build!")
    endfunction()

    function(XmakeCCRLcovHtml)
        #message(STATUS "Skip Code Coverage for ${CMAKE_BUILD_TYPE} Build!")
    endfunction()
    function(XmakeCCRLcovTrace)
        #message(STATUS "Skip Code Coverage for ${CMAKE_BUILD_TYPE} Build!")
    endfunction()
    function(XmakeCCRLcovTraceReport)
        #message(STATUS "Skip Code Coverage for ${CMAKE_BUILD_TYPE} Build!")
    endfunction()

    function(XmakeCCRGcovrXml)
        #message(STATUS "Skip Code Coverage for ${CMAKE_BUILD_TYPE} Build!")
    endfunction()
    function(XmakeCCRGcovrHtml)
        #message(STATUS "Skip Code Coverage for ${CMAKE_BUILD_TYPE} Build!")
    endfunction()
    function(XmakeCCRGcovrText)
        #message(STATUS "Skip Code Coverage for ${CMAKE_BUILD_TYPE} Build!")
    endfunction()
    return()
endif()

set(code_coverage_flags "-g -O0 -fprofile-arcs -ftest-coverage --coverage "
    CACHE INTERNAL "")
mark_as_advanced(code_coverage_flags)

set(CMAKE_C_FLAGS_${buildType} ${code_coverage_flags}
    CACHE STRING "The C compiler flags for code coverage." FORCE)
set(CMAKE_CXX_FLAGS_${buildType} ${code_coverage_flags}
    CACHE STRING "The C++ compiler flags for code coverage." FORCE)

#set(CMAKE_EXE_LINKER_FLAGS_${buildType} ""
#    CACHE STRING "The binaries linking flags for code coverage." FORCE)
#set(CMAKE_SHARED_LINKER_FLAGS_${buildType} ""
#    CACHE STRING "The shared libraries linking flags for code coverage." FORCE)

if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
    link_libraries(gcov) # For all targets
else()
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
endif()

# Append C/C++ compiler flags for code coverage
function(XmakeCCRAppendFlags)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${code_coverage_flags}" PARENT_SCOPE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${code_coverage_flags}" PARENT_SCOPE)
    # message(STATUS "Code coverage compiler flags: ${code_coverage_flags}")
endfunction()

# code coverage report root directory
set(ccrd ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/code.coverage)

# Exclude the system wide include headers for code coverage by default
list(APPEND SYSTEM_EXCLUDES ${CMAKE_C_IMPLICIT_INCLUDE_DIRECTORIES})
list(APPEND SYSTEM_EXCLUDES ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
list(APPEND SYSTEM_EXCLUDES ${CMAKE_PLATFORM_IMPLICIT_INCLUDE_DIRECTORIES})
mark_as_advanced(SYSTEM_EXCLUDES)

# Enable branch-coverage make it slow and seem useless
set(LCOV_EXTRA_ARGS
    --rc genhtml_legend=1
    --rc genhtml_num_spaces=4
    --rc genhtml_line_field_width=10
    --rc genhtml_branch_field_width=10
#   --rc genhtml_branch_coverage=1
    --rc genhtml_function_coverage=1
#   --rc lcov_branch_coverage=1
    --rc lcov_function_coverage=1
)
mark_as_advanced(LCOV_EXTRA_ARGS)

# Defines a target for running and collection code coverage information.
# Builds dependencies first, runs the given executable and outputs reports.
#
# NOTE! The executable should always have a ZERO as exit code otherwise
# the coverage generation will not complete, but this can be skipped by
# given EXECUTABLE_FORCE_SUCCESS
function(XmakeCCRLcovHtml)
    set(optionValueArgs
        EXECUTABLE_FORCE_SUCCESS  # Executable force success if set
    )
    set(oneValueArgs
        TARGET  # Target name, if not set, set to code.coverage-*
    )
    set(multiValueArgs
        DEPENDENCIES      # Dependencies to build first
        EXECUTABLE        # Executable to run
        EXECUTABLE_ARGS   # Executable arguments
        LCOV_ARGS         # Extra arguments for lcov
        LCOV_EXCLUDES     # Report exclude patterns, auto remove system ones
        GENHTML_ARGS      # Extra arguments for genhtml
    )
    cmake_parse_arguments(cct # prefix
        "${optionValueArgs}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN}
    )

    if(NOT LCOV_PROG)
        message(WARNING "NOT found lcov, code coverage skip!")
        return()
    endif()

    if(NOT GCOV_PROG)
        message(WARNING "NOT found gcov, code coverage skip!")
        return()
    endif()

    if(NOT GENHTML_PROG)
        message(WARNING "NOT found genhtml, code coverage skip!")
        return()
    endif()

    if(NOT cct_EXECUTABLE)
        message(FATAL_ERROR "Must set EXECUTABLE for code coverage.")
    endif()

    get_filename_component(runner ${cct_EXECUTABLE} NAME)
    if(NOT cct_TARGET)
        set(cct_TARGET code.coverage-${runner})
    endif()

    if(cct_EXECUTABLE_FORCE_SUCCESS)
        set(force_success || true)
    endif()

    list(APPEND cct_DEPENDENCIES ${cct_EXECUTABLE})
    set(report_dir ${ccrd}/${runner}) # INFO/HTML report directory

    set(based_traceinfo ${report_dir}/${runner}-trace.info.based)
    set(build_traceinfo ${report_dir}/${runner}-trace.info.build)
    set(total_traceinfo ${report_dir}/${runner}-trace.info.total)
    set(clean_traceinfo ${report_dir}/${runner}-trace.info.clean)

    # Setup target and generat coverage data
    add_custom_target(${cct_TARGET}
        # Cleanup lcov
        COMMAND ${LCOV_PROG} ${LCOV_EXTRA_ARGS} ${cct_LCOV_ARGS}
            --gcov-tool ${GCOV_PROG} --directory . --zerocounters
        # Create HTML output folder
        COMMAND ${CMAKE_COMMAND} -E make_directory ${report_dir}
        # Create baseline to make sure untouched files show up in the report
        COMMAND ${LCOV_PROG} ${LCOV_EXTRA_ARGS} ${cct_LCOV_ARGS}
            --gcov-tool ${GCOV_PROG} --capture --initial --directory .
            --output-file ${based_traceinfo}
        # Run coverage tests
        COMMAND ${cct_EXECUTABLE} ${cct_EXECUTABLE_ARGS} ${force_success}
        # Capturing lcov counters for generating report
        COMMAND ${LCOV_PROG} ${LCOV_EXTRA_ARGS} ${cct_LCOV_ARGS}
            --gcov-tool ${GCOV_PROG} --directory . --capture
            --output-file ${build_traceinfo}
        # Merget baseline & runners
        COMMAND ${LCOV_PROG} ${LCOV_EXTRA_ARGS} ${cct_LCOV_ARGS}
            --gcov-tool ${GCOV_PROG} --add-tracefile ${based_traceinfo}
            --add-tracefile ${build_traceinfo} --output-file ${total_traceinfo}
        # Prepare for cleanup
        COMMAND ${CMAKE_COMMAND} -E copy ${total_traceinfo} ${clean_traceinfo}
        # cleanup
        COMMAND ${CMAKE_COMMAND} -E remove ${based_traceinfo}
        DEPENDS ${cct_DEPENDENCIES}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Processing code coverage counters and generating report."
    )

    # Exclude the system ones by default
    foreach(item ${SYSTEM_EXCLUDES})
        add_custom_command(TARGET ${cct_TARGET}
            COMMENT "Ignore SYS Sources for HTML report: ${item}"
            POST_BUILD COMMAND ${LCOV_PROG} ${LCOV_EXTRA_ARGS} ${cct_LCOV_ARGS}
                --gcov-tool ${GCOV_PROG} --remove ${clean_traceinfo} '${item}*'
                --output-file ${clean_traceinfo}
        )
    endforeach()

    # Exclude the user given patterns
    foreach(item ${cct_LCOV_EXCLUDES})
        add_custom_command(TARGET ${cct_TARGET}
            COMMENT "Ignore USR Sources for HTML report: ${item}"
            POST_BUILD COMMAND ${LCOV_PROG} ${LCOV_EXTRA_ARGS} ${cct_LCOV_ARGS}
                --gcov-tool ${GCOV_PROG} --remove ${clean_traceinfo} '${item}*'
                --output-file ${clean_traceinfo}
        )
    endforeach()

    string(REPLACE "${CMAKE_BINARY_DIR}/" "" rccrd "${ccrd}")
    set(html_report ${rccrd}/${runner}/index.html)
    set(info_report ${rccrd}/${runner}/${runner}-trace.info.build)

    # Show where to find the lcov INFO report
    add_custom_command(TARGET ${cct_TARGET}
        POST_BUILD COMMAND ;
        COMMENT "Lcov code coverage INFO report: ${info_report}"
    )
    # Show info where to find the HTML report
    add_custom_command(TARGET ${cct_TARGET}
        COMMENT "Lcov code coverage HTML report: ${html_report}"
        POST_BUILD COMMAND ${GENHTML_PROG} ${LCOV_EXTRA_ARGS} ${cct_GENHTML_ARGS}
            --output-directory ${report_dir} ${clean_traceinfo}
    )
endfunction()

# Defines a target for collection code coverage information of given runner.
# Builds dependencies first, remove system default file and outputs reports.
function(XmakeCCRLcovTraceReport)
    set(optionValueArgs)
    set(oneValueArgs
        TARGET        # Target name
    )
    set(multiValueArgs
        EXECUTABLE    # Executable to run
        DEPENDENCIES  # Dependencies to build first
        LCOV_ARGS     # Extra arguments for lcov
        LCOV_EXCLUDES # Exclude patterns from the report
        GENHTML_ARGS  # Extra arguments for genhtml
    )
    cmake_parse_arguments(cct # prefix
        "${optionValueArgs}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN}
    )

    if(NOT LCOV_PROG)
        message(WARNING "NOT found lcov, code coverage skip!")
        return()
    endif()

    if(NOT GCOV_PROG)
        message(WARNING "NOT found gcov, code coverage skip!")
        return()
    endif()

    if(NOT GENHTML_PROG)
        message(WARNING "NOT found genhtml, code coverage skip!")
        return()
    endif()

    if(NOT cct_EXECUTABLE)
        message(FATAL_ERROR "Must set EXECUTABLE for code coverage.")
    endif()

    if(NOT cct_TARGET)
        message(FATAL_ERROR "Must set TARGET for code coverage.")
    endif()

    list(APPEND cct_DEPENDENCIES ${cct_EXECUTABLE})
    get_filename_component(runner ${cct_EXECUTABLE} NAME)

    set(report_dir ${ccrd}/${runner}) # trace INFO/HTML directory

    string(STRIP "${CCRTI_${cct_EXECUTABLE}}" trace_data_args)
    if(NOT trace_data_args)
        message(WARNING "NO code coverage data found for ${cct_EXECUTABLE}.")
        return()
    endif()

    set(total_traceinfo ${report_dir}/trace.info.total)
    string(REPLACE " " ";" trace_data_deps "${trace_data_args}")
    string(REPLACE "${CMAKE_BINARY_DIR}/" "" rccrd "${ccrd}")
    set(html_report ${rccrd}/${runner}/index.html)

    foreach(item ${trace_data_deps})
        list(APPEND merge_tracefile_args --add-tracefile ${item})
    endforeach()

    # Merge all the trace info
    add_custom_target(${cct_TARGET}-prepare
        COMMAND ${LCOV_PROG} ${LCOV_EXTRA_ARGS} ${cct_LCOV_ARGS}
            --gcov-tool ${GCOV_PROG} ${merge_tracefile_args}
            --output-file ${total_traceinfo}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        DEPENDS ${cct_DEPENDENCIES} ${trace_data_deps}
        COMMENT "Clean up code coverage trace info for HTML report."
    )

    # Exclude the system ones by default
    foreach(item ${SYSTEM_EXCLUDES})
        add_custom_command(TARGET ${cct_TARGET}-prepare
            COMMENT "Ignore SYS Sources for HTML report: ${item}"
            POST_BUILD COMMAND ${LCOV_PROG} ${LCOV_EXTRA_ARGS} ${cct_LCOV_ARGS}
                --gcov-tool ${GCOV_PROG} --remove ${total_traceinfo} '${item}*'
                --output-file ${total_traceinfo}
        )
    endforeach()

    # Exclude the user given patterns
    foreach(item ${cct_LCOV_EXCLUDES})
        add_custom_command(TARGET ${cct_TARGET}-prepare
            COMMENT "Ignore USR Sources for HTML report: ${item}"
            POST_BUILD COMMAND ${LCOV_PROG} ${LCOV_EXTRA_ARGS} ${cct_LCOV_ARGS}
                --gcov-tool ${GCOV_PROG} --remove ${total_traceinfo} '${item}*'
                --output-file ${total_traceinfo}
        )
    endforeach()

    # Generating the HTML report
    add_custom_target(${cct_TARGET}
        COMMENT "Lcov code coverage HTML report: ${html_report}"
        COMMAND ${GENHTML_PROG} ${LCOV_EXTRA_ARGS} ${cct_GENHTML_ARGS}
            ${total_traceinfo} --show-details
            --output-directory ${report_dir}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        DEPENDS ${cct_TARGET}-prepare
    )
endfunction()

# Defines a target for running and collection code coverage information.
# Builds dependencies first, runs the given executable and outputs reports.
#
# NOTE! The executable should always have a ZERO as exit code otherwise
# the coverage generation will not complete, but this can be skipped by
# given EXECUTABLE_FORCE_SUCCESS
function(XmakeCCRLcovTrace)
    set(optionValueArgs
        EXECUTABLE_FORCE_SUCCESS # Executable force success if set
    )
    set(oneValueArgs
        TEST_NAME # Test name to run with given args for `CodeCoverageLcovTrace`
    )
    set(multiValueArgs
        DEPENDENCIES     # Dependencies to build first
        EXECUTABLE       # Executable to run
        EXECUTABLE_ARGS  # Executable arguments
        LCOV_ARGS        # Extra arguments for lcov
        LCOV_EXCLUDES    # Report exclude patterns, auto remove system
    )
    cmake_parse_arguments(cct # prefix
        "${optionValueArgs}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN}
    )

    if(NOT LCOV_PROG)
        message(WARNING "NOT found lcov, code coverage skip!")
        return()
    endif()

    if(NOT GCOV_PROG)
        message(WARNING "NOT found gcov, code coverage skip!")
        return()
    endif()

    if(NOT GENHTML_PROG)
        message(WARNING "NOT found genhtml, code coverage skip!")
        return()
    endif()

    if(NOT cct_EXECUTABLE)
        message(FATAL_ERROR "Must set EXECUTABLE for code coverage.")
    endif()

    if(NOT cct_TEST_NAME)
        message(FATAL_ERROR "Must set TEST_NAME for code coverage.")
    endif()

    list(APPEND cct_DEPENDENCIES ${cct_EXECUTABLE})
    get_filename_component(runner ${cct_EXECUTABLE} NAME)

    set(report_dir ${ccrd}/${runner}) # trace INFO directory

    if(cct_EXECUTABLE_FORCE_SUCCESS)
        set(force_success || true)
    endif()

    set(output_traceinfo ${report_dir}/trace.info.${cct_TEST_NAME})

    # Setup target and generat coverage data
    add_custom_command(OUTPUT ${output_traceinfo}
        # Cleanup coverage data
        COMMAND ${LCOV_PROG} ${LCOV_EXTRA_ARGS} ${cct_LCOV_ARGS}
            --gcov-tool ${GCOV_PROG} --directory . --zerocounters
        # Create trace INFO output folder
        COMMAND ${CMAKE_COMMAND} -E make_directory ${report_dir}
        # Run coverage tests
        COMMAND ${cct_EXECUTABLE} ${cct_EXECUTABLE_ARGS} ${force_success}
        # Capturing lcov coverage data
        COMMAND ${LCOV_PROG} ${LCOV_EXTRA_ARGS} ${cct_LCOV_ARGS}
            --gcov-tool ${GCOV_PROG} --directory . --capture
            --output-file ${output_traceinfo}
            --test-name "${cct_TEST_NAME}"
        DEPENDS ${cct_DEPENDENCIES}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Processing code coverage test case: ${cct_TEST_NAME}"
    )

    set(CCRTI_${cct_EXECUTABLE}
        "${CCRTI_${cct_EXECUTABLE}} ${output_traceinfo}" PARENT_SCOPE)
endfunction()

# Defines a target for running and collection code coverage information.
# Builds dependencies first, runs the given executable and outputs reports.
#
# NOTE! The executable should always have a ZERO as exit code otherwise
# the coverage generation will not complete, but this can be skipped by
# given EXECUTABLE_FORCE_SUCCESS
function(XmakeCCRGcovrXml)
    set(optionValueArgs
        EXECUTABLE_FORCE_SUCCESS # Executable force success if set
    )
    set(oneValueArgs
        TARGET # Target name, if not set, set to code.coverage-*
    )
    set(multiValueArgs
        EXECUTABLE      # Executable to run
        EXECUTABLE_ARGS # Executable arguments
        DEPENDENCIES    # Dependencies to build first
        GCOVR_ARGS      # Extra arguments for gcovr
        GCOVR_EXCLUDES  # Extra arguments for gcovr
    )
    cmake_parse_arguments(cct # prefix
        "${optionValueArgs}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN}
    )

    if(NOT GCOVR_PROG)
        message(WARNING "NOT found gcovr, code coverage skip!")
        return()
    endif()

    if(NOT PYTHON_PROG)
        message(WARNING "NOT found python, code coverage skip!")
        return()
    endif()

    if(NOT cct_EXECUTABLE)
        message(FATAL_ERROR "Must set EXECUTABLE for code coverage.")
    endif()

    get_filename_component(runner ${cct_EXECUTABLE} NAME)

    if(NOT cct_TARGET)
        set(cct_TARGET code.coverage-${runner})
    endif()

    if(cct_EXECUTABLE_FORCE_SUCCESS)
        set(force_success || true)
    endif()

    # Generating the XML report
    set(report_dir ${ccrd}/${runner})
    set(xml_report ${report_dir}/${runner}.xml)

    # Combine excludes to several -e arguments
    set(gcovr_excludes "")
    foreach(item ${SKIP_ITEMS})
        list(APPEND gcovr_excludes "--exclude" "${item}")
    endforeach()

    # Exclude the user given patterns
    foreach(item ${cct_GCOVR_EXCLUDES})
        list(APPEND gcovr_excludes "--exclude" "${item}")
    endforeach()

    add_custom_target(${cct_TARGET}
        # Clean old coverage data
        COMMAND find ${CMAKE_BINARY_DIR} -type f -iname '*.gcda' -delete
        # Run test executable
        COMMAND ${cct_EXECUTABLE} ${cct_EXECUTABLE_ARGS} ${force_success}
        # Create XML output folder
        COMMAND ${CMAKE_COMMAND} -E make_directory ${report_dir}
        # Running gcovr
        COMMAND ${GCOVR_PROG} --xml --xml-pretty ${cct_GCOVR_ARGS}
            --root ${CMAKE_SOURCE_DIR} ${gcovr_excludes}
            --object-directory=${CMAKE_BINARY_DIR}
            --output ${xml_report}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        DEPENDS ${cct_DEPENDENCIES}
        COMMENT "Running gcovr to produce XML code coverage report."
    )

    # Exclude the system ones by default
    foreach(item ${SYSTEM_EXCLUDES})
        add_custom_command(TARGET ${cct_TARGET} POST_BUILD
            COMMAND ;
            COMMENT "Ignore SYS Sources for XML report: ${item}"
        )
    endforeach()

    # Exclude the user given patterns
    foreach(item ${cct_GCOVR_EXCLUDES})
        add_custom_command(TARGET ${cct_TARGET} POST_BUILD
            COMMAND ;
            COMMENT "Ignore USR Sources for XML report: ${item}"
        )
    endforeach()

    string(REPLACE "${CMAKE_BINARY_DIR}/" "" xml_report "${xml_report}")
    # Show info where to find the report
    add_custom_command(TARGET ${cct_TARGET} POST_BUILD
        COMMAND ;
        COMMENT "Gcovr XML code coverage report: ${xml_report}"
    )
endfunction()

# Defines a target for running and collection code coverage information.
# Builds dependencies first, runs the given executable and outputs reports.
#
# NOTE! The executable should always have a ZERO as exit code otherwise
# the coverage generation will not complete, but this can be skipped by
# given EXECUTABLE_FORCE_SUCCESS
function(XmakeCCRGcovrHtml)
    set(optionValueArgs
        EXECUTABLE_FORCE_SUCCESS # Executable force success if set
    )
    set(oneValueArgs
        TARGET # Target name, if not set, set to code.coverage-*
    )
    set(multiValueArgs
        EXECUTABLE      # Executable to run
        EXECUTABLE_ARGS # Executable arguments
        DEPENDENCIES    # Dependencies to build first
        GCOVR_ARGS      # Extra arguments for gcovr
        GCOVR_EXCLUDES  # Extra arguments for gcovr
    )
    cmake_parse_arguments(cct # prefix
        "${optionValueArgs}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN}
    )

    if(NOT GCOVR_PROG)
        message(WARNING "NOT found gcovr, code coverage skip!")
        return()
    endif()

    if(NOT PYTHON_PROG)
        message(WARNING "NOT found python, code coverage skip!")
        return()
    endif()

    if(NOT cct_EXECUTABLE)
        message(FATAL_ERROR "Must set EXECUTABLE for code coverage.")
    endif()

    get_filename_component(runner ${cct_EXECUTABLE} NAME)

    if(NOT cct_TARGET)
        set(cct_TARGET code.coverage-${runner})
    endif()

    if(cct_EXECUTABLE_FORCE_SUCCESS)
        set(force_success || true)
    endif()

    # Generating the XML report
    set(report_dir ${ccrd}/${runner})

    # Combine excludes to several -e arguments
    set(gcovr_excludes "")
    foreach(item ${SYSTEM_EXCLUDES})
        list(APPEND gcovr_excludes "--exclude" "${item}")
    endforeach()

    # Exclude the user given patterns
    foreach(item ${cct_GCOVR_EXCLUDES})
        list(APPEND gcovr_excludes "--exclude" "${item}")
    endforeach()

    add_custom_target(${cct_TARGET}
        # Clean old coverage data
        COMMAND find ${CMAKE_BINARY_DIR} -type f -iname '*.gcda' -delete
        # Run tests runner
        COMMAND ${cct_EXECUTABLE} ${cct_EXECUTABLE_ARGS} ${force_success}
        # Create HTML output folder
        COMMAND ${CMAKE_COMMAND} -E make_directory ${report_dir}
        # Running gcovr
        COMMAND ${GCOVR_PROG} --html --html-details ${cct_GCOVR_ARGS}
            --root ${CMAKE_SOURCE_DIR} ${gcovr_excludes}
            --object-directory=${CMAKE_BINARY_DIR}
            -o ${report_dir}/index.html
        DEPENDS ${cct_DEPENDENCIES}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Running gcovr to produce HTML code coverage report."
    )

    # Exclude the system ones by default
    foreach(item ${SYSTEM_EXCLUDES})
        add_custom_command(TARGET ${cct_TARGET} POST_BUILD
            COMMAND ;
            COMMENT "Ignore SYS Sources for HTML report: ${item}"
        )
    endforeach()

    # Exclude the user given patterns
    foreach(item ${cct_GCOVR_EXCLUDES})
        add_custom_command(TARGET ${cct_TARGET} POST_BUILD
            COMMAND ;
            COMMENT "Ignore USR Sources for HTML report: ${item}"
        )
    endforeach()

    string(REPLACE "${CMAKE_BINARY_DIR}/" "" report_dir "${report_dir}")
    # Show info where to find the HTML report
    add_custom_command(TARGET ${cct_TARGET} POST_BUILD
        COMMAND ;
        COMMENT "Gcovr HTML code coverage report: ${report_dir}/index.html"
    )
endfunction()

# Defines a target for running and collection code coverage information.
# Builds dependencies first, runs the given executable and outputs reports.
#
# NOTE! The executable should always have a ZERO as exit code otherwise
# the coverage generation will not complete, but this can be skipped by
# given EXECUTABLE_FORCE_SUCCESS
function(XmakeCCRGcovrText)
    set(optionValueArgs
        EXECUTABLE_FORCE_SUCCESS # Executable force success if set
    )
    set(oneValueArgs
        TARGET # Target name, if not set, set to code.coverage-*
    )
    set(multiValueArgs
        EXECUTABLE       # Executable to run
        EXECUTABLE_ARGS  # Executable arguments
        DEPENDENCIES     # Dependencies to build first
        GCOVR_ARGS       # Extra arguments for gcovr
        GCOVR_EXCLUDES   # Extra arguments for gcovr
    )
    cmake_parse_arguments(cct # prefix
        "${optionValueArgs}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN}
    )

    if(NOT GCOVR_PROG)
        message(WARNING "NOT found gcovr, code coverage skip!")
        return()
    endif()

    if(NOT PYTHON_PROG)
        message(WARNING "NOT found python, code coverage skip!")
        return()
    endif()

    if(NOT cct_EXECUTABLE)
        message(FATAL_ERROR "Must set EXECUTABLE for code coverage.")
    endif()

    get_filename_component(runner ${cct_EXECUTABLE} NAME)

    if(NOT cct_TARGET)
        set(cct_TARGET code.coverage-${runner})
    endif()

    if(cct_EXECUTABLE_FORCE_SUCCESS)
        set(force_success || true)
    endif()

    # Generating the XML report
    set(report_dir ${ccrd}/${runner})
    set(text_report ${report_dir}/${runner}.text)

    # Combine excludes to several -e arguments
    set(gcovr_excludes "")
    foreach(item ${SYSTEM_EXCLUDES})
        list(APPEND gcovr_excludes "--exclude" "${item}")
    endforeach()

    # Exclude the user given patterns
    foreach(item ${cct_GCOVR_EXCLUDES})
        list(APPEND gcovr_excludes "--exclude" "${item}")
    endforeach()

    add_custom_target(${cct_TARGET}
        # Clean old coverage data
        COMMAND find ${CMAKE_BINARY_DIR} -type f -iname '*.gcda' -delete
        # Run tests
        COMMAND ${cct_EXECUTABLE} ${cct_EXECUTABLE_ARGS} ${force_success}
        # Create TEXT output folder
        COMMAND ${CMAKE_COMMAND} -E make_directory ${report_dir}
        # Running gcovr
        COMMAND ${GCOVR_PROG} ${cct_GCOVR_ARGS}
            --root ${CMAKE_SOURCE_DIR} ${gcovr_excludes}
            --object-directory=${CMAKE_BINARY_DIR}
            -o ${text_report}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        DEPENDS ${cct_DEPENDENCIES}
        COMMENT "Running gcovr to produce TEXT code coverage report."
    )

    # Exclude the system ones by default
    foreach(item ${SYSTEM_EXCLUDES})
        add_custom_command(TARGET ${cct_TARGET} POST_BUILD
            COMMAND ;
            COMMENT "Ignore SYS Sources for TEXT report: ${item}"
        )
    endforeach()

    # Exclude the user given patterns
    foreach(item ${cct_GCOVR_EXCLUDES})
        add_custom_command(TARGET ${cct_TARGET} POST_BUILD
            COMMAND ;
            COMMENT "Ignore USR Sources for TEXT report: ${item}"
        )
    endforeach()

    string(REPLACE "${CMAKE_BINARY_DIR}/" "" text_report "${text_report}")
    # Show info where to find the report
    add_custom_command(TARGET ${cct_TARGET} POST_BUILD
        COMMAND ;
        COMMENT "Gcovr TEXT code coverage report: ${text_report}"
    )
endfunction()
