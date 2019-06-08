include(CMakeParseArguments)

# Download & Install Prebuild Binary
# The prebuild binary name, will be cmake top target
function(XmakeDepBinaryInstall name)
    set(optionValueArgs
        SKIP        # Skip prebuild binary download and install if true
        REPO        # Download prebuild binary repo, patch and install
        TARBALL     # Download prebuild binary tarball, extract, patch and install
    )
    set(oneValueArgs
        URL         # The prebuild binary tarball or repo URL to download
        SHA256      # The prebuild binary SHA256 for tarball checking
        VERSION     # The prebuild binary version
    )
    set(multiValueArgs
        PATCH_CMD   # The prebuild binary patch commands
        INSTALL_CMD # The prebuild binary install commands
    )
    cmake_parse_arguments(bin # prefix
        "${optionValueArgs}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN}
    )

    if(bin_SKIP)
        return() # skip download & install
    endif()

    if(NOT bin_REPO AND NOT bin_TARBALL)
        message(FATAL_ERROR "Must set REPO or TARBALL for ${name}.")
    endif()

    if(NOT bin_URL)
        message(FATAL_ERROR "Must set URL for ${name}.")
    endif()

    if(NOT bin_INSTALL_CMD)
        message(FATAL_ERROR "Must set INSTALL_CMD for ${name}.")
    endif()

    if(NOT bin_PATCH_CMD)
        set(bin_PATCH_CMD "")
    endif()

    if(bin_TARBALL)
        # Download tarball, extract, and install the prebuild binary
        XmakeDepTarballBuild(${name}
            VERSION     ${bin_VERSION}
            URL         ${bin_URL}
            SHA256      ${bin_SHA256}
            PATCH_CMD   ${bin_PATCH_CMD}
            INSTALL_CMD ${bin_INSTALL_CMD}
        )
        return()
    endif()

    # Download git repo, and install the prebuild binary
    XmakeDepRepoBuild(${name}
        REPO_URL    ${bin_URL}
        PATCH_CMD   ${bin_PATCH_CMD}
        INSTALL_CMD ${bin_INSTALL_CMD}
    )

    #list(APPEND THIRD_PARTY_DEPS ${_bin_TARGET})
endfunction()
