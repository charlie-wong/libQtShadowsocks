# Download tarball & build botan
# https://github.com/randombit/botan
# https://botan.randombit.net/manual/contents.html

list(APPEND botan_CONFIG_ARGS --cc=gcc)
if(HOST_ARCH_32)
    list(APPEND botan_CONFIG_ARGS --cpu=x86)
else()
    list(APPEND botan_CONFIG_ARGS --cpu=x86_64)
endif()

if(SSQT_DBI_BOTAN2)
    set(botan2_TARGET libBotan)
    XmakeDepTarballBuild(${botan2_TARGET}
        VERSION     2.10.0
        URL         https://github.com/randombit/botan/archive/2.10.0.tar.gz
        SHA256      b5f27e65e733fb43c0802aef8f86ff981eb47bcfeafbf085233a3e4046e3cba8
        CONFIG_CMD  ${DEPS_BUILD_DIR}/libBotan/configure.py
            ${botan_CONFIG_ARGS}
            --optimize-for-size
            --without-documentation
            --disable-shared-library
            --prefix=${DEPS_INSTALL_DIR}
        BUILD_CMD   ${MAKE_PROG} libs # just build the library
        INSTALL_CMD ${MAKE_PROG} install
    )
    set(botan2_LIBRARIES -L${DEPS_INSTALL_DIR}/lib botan-2)
    set(botan2_INCLUDE_DIRS ${DEPS_INSTALL_DIR}/include/botan-2)
else() # Try to find the system wide botan2 library
    XmakeSearchLibrary(PREFIX botan2
        NAME botan-2 VERSION 2.10.0
        STATIC REQUIRED # VERBOSE
    )
endif()

#message(STATUS "botan2_LIBRARIES=${botan2_LIBRARIES}")
#message(STATUS "botan2_INCLUDE_DIRS=${botan2_INCLUDE_DIRS}")
