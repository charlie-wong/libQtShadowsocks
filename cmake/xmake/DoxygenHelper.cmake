macro(UsrHasLocalDoxygen fileName)
    if(EXISTS ${CMAKE_SOURCE_DIR}/${fileName})
        set(DOXYGEN_CONFIG ${CMAKE_SOURCE_DIR}/${fileName})
    elseif(EXISTS ${CMAKE_SOURCE_DIR}/doc/${fileName})
        set(DOXYGEN_CONFIG ${CMAKE_SOURCE_DIR}/doc/${fileName})
    elseif(EXISTS ${CMAKE_SOURCE_DIR}/doc/doxygen/${fileName})
        set(DOXYGEN_CONFIG ${CMAKE_SOURCE_DIR}/doc/doxygen/${fileName})
    elseif(EXISTS ${CMAKE_SOURCE_DIR}/docs/${fileName})
        set(DOXYGEN_CONFIG ${CMAKE_SOURCE_DIR}/docs/${fileName})
    elseif(EXISTS ${CMAKE_SOURCE_DIR}/docs/doxygen/${fileName})
        set(DOXYGEN_CONFIG ${CMAKE_SOURCE_DIR}/docs/doxygen/${fileName})
    endif()
endmacro()

# Generate Doxygen API Manual
find_package(Doxygen)
if(DOXYGEN_PROG OR DOXYGEN_FOUND)
    if(NOT DOXYGEN_PROG)
        set(DOXYGEN_PROG ${DOXYGEN_EXECUTABLE})
    endif()

    if(NOT PKG_DOXYGEN_SOURCE)
        if(EXISTS ${CMAKE_SOURCE_DIR}/src)
            set(PKG_DOXYGEN_SOURCE ${CMAKE_SOURCE_DIR}/src)
        elseif(EXISTS ${CMAKE_SOURCE_DIR}/source)
            set(PKG_DOXYGEN_SOURCE ${CMAKE_SOURCE_DIR}/source)
        else()
            set(PKG_DOXYGEN_SOURCE ${CMAKE_SOURCE_DIR})
        endif()
    endif()

    set(pkg_version ${PKG_VERSION})
    if(pkg_version_tweak)
        set(pkg_version ${pkg_version}-${pkg_version_tweak})
    endif()

    # User extra 'Doxyfile'
    UsrHasLocalDoxygen(Doxyfile)
    if(DOXYGEN_CONFIG)
        add_custom_target(doxygen
            COMMENT "Generating API documentation by doxygen"
            COMMAND ${DOXYGEN_PROG} ${DOXYGEN_CONFIG}
        )
        return()
    endif()

    UsrHasLocalDoxygen(Doxyfile.in)
    if(DOXYGEN_CONFIG)
        configure_file("${DOXYGEN_CONFIG}"
            "${PKG_MANUAL_DIR}/doxygen/Doxyfile"
        )

        add_custom_target(doxygen
            COMMENT "Generating API documentation by doxygen"
            COMMAND ${DOXYGEN_PROG} ${PKG_MANUAL_DIR}/doxygen/Doxyfile
        )
        return()
    endif()

    # do NOT has local config, just use the default one
    configure_file("${CMAKE_CURRENT_LIST_DIR}/doxygen/Doxyfile.in"
        "${PKG_MANUAL_DIR}/doxygen/Doxyfile"
    )

    add_custom_target(doxygen
        COMMENT "Generating API documentation by doxygen"
        COMMAND ${DOXYGEN_PROG} ${PKG_MANUAL_DIR}/doxygen/Doxyfile
    )
endif()
