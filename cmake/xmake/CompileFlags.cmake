macro(xmakeI_GetCompilerFlags CMAKEC cflags location)
    # Create template akin to CMAKE_C_COMPILE_OBJECT
    set(compiler_flags
        "<COMPILER> "
        "<DEFINITIONS> "
        "<INCLUDES>"
        "<CFLAGS> "
        "<BUILD_CFLAGS>"
    )

    # Get C compiler
    string(REPLACE "<COMPILER>" "${${CMAKEC}_COMPILER}"
        compiler_flags "${compiler_flags}"
    )

    # Get flags set by add_definition()
    get_directory_property(attr_DEFINITIONS
        DIRECTORY "${location}" COMPILE_OPTIONS
    )
    string(REPLACE "<DEFINITIONS>" "${attr_DEFINITIONS}"
        compiler_flags "${compiler_flags}"
    )

    # Get include directories
    get_directory_property(attr_INCLUDE_DIRECTORIES
        DIRECTORY "${location}" INCLUDE_DIRECTORIES
    )
    foreach(incdir ${attr_INCLUDE_DIRECTORIES})
        set(attr_INCLUDES "${attr_INCLUDES} -I${incdir}")
    endforeach()
    string(REPLACE "<INCLUDES>" "${attr_INCLUDES}"
        compiler_flags "${compiler_flags}"
    )

    # Get general C flags
    string(REPLACE "<CFLAGS>" "${${CMAKEC}_FLAGS}"
        compiler_flags "${compiler_flags}"
    )

    # Get C flags specific to build type
    string(REPLACE "<BUILD_CFLAGS>" "${${CMAKEC}_FLAGS_${buildType}}"
        compiler_flags "${compiler_flags}"
    )

    # convert to cmake list
    string(REPLACE " " ";" compiler_flags "${compiler_flags}")
    # make sure only one ;
    string(REGEX REPLACE "([;])+" ";" compiler_flags "${compiler_flags}")
    # remove the duplicate ones
    list(REMOVE_DUPLICATES compiler_flags)

    # sort and make compiler first one
    list(SORT compiler_flags)
    list(REVERSE compiler_flags)

    # convert ; back to whitespace
    string(REGEX REPLACE ";" " " compiler_flags "${compiler_flags}")

    # make it short, remove '${CMAKE_SOURCE_DIR}/'
    string(REPLACE "${CMAKE_SOURCE_DIR}" "~/${PKG_NAME}"
        compiler_flags "${compiler_flags}"
    )
    string(REPLACE "${CMAKE_SOURCE_DIR}/" "~/${PKG_NAME}"
        compiler_flags "${compiler_flags}"
    )

    string(STRIP "${compiler_flags}" compiler_flags)
    set(${cflags} "${compiler_flags}" PARENT_SCOPE)
endmacro()

function(XmakeGetCFlags cflags location)
    xmakeI_GetCompilerFlags(CMAKE_C ${cflags} ${location})
endfunction()

function(XmakeGetCXXFlags cxxflags location)
    xmakeI_GetCompilerFlags(CMAKE_CXX ${cxxflags} ${location})
endfunction()
