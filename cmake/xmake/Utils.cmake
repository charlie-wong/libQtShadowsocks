function(XmakePrint)
    # ARGC: arguments number passed into the function
    # ARGV: list of all arguments given to the function
    # ARGN: list of arguments past the last expected argument
    set(optionValueArgs  STOP)
    set(oneValueArgs)
    set(multiValueArgs)
    cmake_parse_arguments(xp # prefix
        "${optionValueArgs}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN}
    )

    foreach(varName ${xp_UNPARSED_ARGUMENTS})
        message(STATUS "${varName}=[${${varName}}]")
    endforeach()

    if(xp_STOP)
        message(FATAL_ERROR "Cmake Debug STOP by xmake!")
    endif()
endfunction()
