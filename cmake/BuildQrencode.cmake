# Library for encoding data in a QR Code symbol
if(SSQT_DBI_QRENCODE)
    set(qrencode_TARGET libqrencode)
    XmakeDepTarballBuild(${qrencode_TARGET}
        VERSION     4.0.2
        URL         https://fukuchi.org/works/qrencode/qrencode-4.0.2.tar.gz
        SHA256      dbabe79c07614625d1f74d8c0ae2ee5358c4e27eab8fd8fe31f9365f821a3b1d
        CONFIG_CMD  ./configure --prefix=${DEPS_INSTALL_DIR}
            --enable-static=yes --enable-shared=no
        BUILD_CMD   ${MAKE_PROG}
        INSTALL_CMD ${MAKE_PROG} install
    )
    set(qrencode_INCLUDE_DIRS ${DEPS_INSTALL_DIR}/include)
    set(qrencode_LIBRARIES -L${DEPS_INSTALL_DIR}/lib -lqrencode)
else()
    # Try to find the libqrencode library
    XmakeSearchLibrary(PREFIX qrencode
        NAME libqrencode VERSION 4.0.2
        REQUIRED # STATIC VERBOSE
    )
endif()

#message(STATUS "qrencode_LIBRARIES=${qrencode_LIBRARIES}")
#message(STATUS "qrencode_INCLUDE_DIRS=${qrencode_INCLUDE_DIRS}")
