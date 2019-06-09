# https://cmake.org/cmake/help/latest/module/CMakePackageConfigHelpers.html
include(CMakePackageConfigHelpers)

set(LIBRARY_INSTALL_DIR lib)
set(INCLUDE_INSTALL_DIR include)
set(pkgconfig_DIR ${CMAKE_CURRENT_LIST_DIR}/pkgconfig)

function(XmakePackageConfig)
    set(options)
    set(oneValueArgs TARGET NAME DOMAIN)
    set(multiValueArgs)

    cmake_parse_arguments(lib
        "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN}
    )

    if(NOT lib_TARGET)
        message(FATAL_ERROR "Must set TARGET for package")
    endif()

    if(NOT lib_NAME)
        set(lib_NAME "${lib_TARGET}")
    endif()

    set(lib_CMAKE_DEST lib/cmake/${lib_NAME})

    string(REGEX REPLACE "[v|V]" "" lib_VERSION "${PKG_VERSION}")

    # export library information for CMake project
    install(EXPORT "${lib_TARGET}"
        FILE ${lib_NAME}.cmake
        DESTINATION "${lib_CMAKE_DEST}"
    )

    configure_package_config_file(${pkgconfig_DIR}/Config.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/pkgconfig/${lib_NAME}Config.cmake
        INSTALL_DESTINATION "${lib_CMAKE_DEST}"
        PATH_VARS INCLUDE_INSTALL_DIR LIBRARY_INSTALL_DIR
    )

    write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/pkgconfig/${lib_NAME}ConfigVersion.cmake
        VERSION ${lib_VERSION} COMPATIBILITY AnyNewerVersion
    )

    install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/pkgconfig/${lib_NAME}Config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/pkgconfig/${lib_NAME}ConfigVersion.cmake
        DESTINATION "${lib_CMAKE_DEST}"
    )

    set(lib_LIBRARY_DIR ${CMAKE_INSTALL_PREFIX}/lib)
    set(lib_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/include)
    if(lib_DOMAIN)
        set(lib_LIBRARY_DIR ${lib_LIBRARY_DIR}/${lib_DOMAIN})
    endif()

    set(lib_EXTRA_LIBS) # EXPORT_LIBRARY_WITH_EXTRA_LIBS
    foreach(item ${lib_UNPARSED_ARGUMENTS})
        set(lib_EXTRA_LIBS "${lib_EXTRA_LIBS} -l${item}")
    endforeach()

    # export library information for pkgconfig
    configure_file(${pkgconfig_DIR}/pkgconfig.pc.in
        ${CMAKE_CURRENT_BINARY_DIR}/pkgconfig/${lib_NAME}.pc @ONLY
    )

    set(lib_PKGCONFIG_DEST lib/pkgconfig)
    install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/pkgconfig/${lib_NAME}.pc
        DESTINATION "${lib_PKGCONFIG_DEST}"
    )
endfunction()
