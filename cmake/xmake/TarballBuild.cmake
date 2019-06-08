include(CMakeParseArguments)

# The external project name tbName, will be cmake top target
function(XmakeDepTarballBuild tbName)
    set(optionValueArgs)
    set(oneValueArgs
        URL            # The project tarball URL to download
        SHA256         # The tarball SHA256 for tarball checking
        VERSION        # The project version
    )
    set(multiValueArgs
        PATCH_CMD      # The project patch command
        CONFIG_CMD     # The project config command
        BUILD_CMD      # The project build command
        INSTALL_CMD    # The project install command
    )
    cmake_parse_arguments(tarball # prefix
        "${optionValueArgs}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN}
    )

    if(NOT tarball_VERSION)
        message(FATAL_ERROR "Must set VERSION for ${tbName}.")
    endif()

    if(NOT tarball_URL)
        message(FATAL_ERROR "Must set URL for ${tbName}.")
    endif()

    if(NOT tarball_SHA256)
        message(FATAL_ERROR "Must set SHA256 for ${tbName}.")
    endif()

    if(NOT tarball_CONFIG_CMD AND
       NOT tarball_BUILD_CMD AND
       NOT tarball_INSTALL_CMD)
        message(FATAL_ERROR
            "Must set one of CONFIG_CMD, BUILD_CMD, INSTALL_CMD for ${tbName}.")
    endif()

    if(NOT tarball_PATCH_CMD)
        set(tarball_PATCH_CMD "")
    endif()

    if(NOT tarball_CONFIG_CMD)
        set(tarball_CONFIG_CMD "")
    endif()

    if(NOT tarball_BUILD_CMD)
        set(tarball_BUILD_CMD "")
    endif()

    if(NOT tarball_INSTALL_CMD)
        set(tarball_INSTALL_CMD "")
    endif()

    XmakeDownloadExtract(TARGET "${tbName}"
        URL "${tarball_URL}"
        EXPECTED_SHA256 "${tarball_SHA256}"
    )

    ExternalProject_Add(${tbName}
        # General
        PREFIX              ${DEPS_BUILD_DIR}
        STAMP_DIR           ${DEPS_BUILD_DIR}/${tbName}-stamp
        # Download
        DOWNLOAD_DIR        "${DEPS_DOWNLOAD_DIR}"
        # Patch
        PATCH_COMMAND       "${tarball_PATCH_CMD}"
        # Configure
        SOURCE_DIR          "${DEPS_BUILD_DIR}/${tbName}"
        CONFIGURE_COMMAND   "${tarball_CONFIG_CMD}"
        # Build
        BINARY_DIR          "${DEPS_BUILD_DIR}/${tbName}"
        BUILD_COMMAND       "${tarball_BUILD_CMD}"
        # Install
        INSTALL_DIR         "${DEPS_INSTALL_DIR}"
        INSTALL_COMMAND     "${tarball_INSTALL_CMD}"
    )
endfunction()
