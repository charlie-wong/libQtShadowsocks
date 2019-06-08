add_definitions(-DHOST_NAME=\"${HOST_NAME}\")
add_definitions(-DHOST_USER=\"${HOST_USER}\")
add_definitions(-DHOST_ARCH=\"${HOST_ARCH}\")
add_definitions(-DHOST_SYSTEM_NAME=\"${HOST_SYSTEM_NAME}\")
add_definitions(-DHOST_SYSTEM_VERSION=\"${HOST_SYSTEM_VERSION}\")
add_definitions(-DHOST_OS_DIST_NAME=\"${HOST_OS_DIST_NAME}\")
add_definitions(-DHOST_OS_DIST_VERSION=\"${HOST_OS_DIST_VERSION}\")

add_definitions(-D${XMAKE}_VERSION_MAJOR=${${XMAKE}_VERSION_MAJOR})
add_definitions(-D${XMAKE}_VERSION_MINOR=${${XMAKE}_VERSION_MINOR})
add_definitions(-D${XMAKE}_VERSION_PATCH=${${XMAKE}_VERSION_PATCH})
add_definitions(-D${XMAKE}_VERSION_TWEAK=\"${${XMAKE}_VERSION_TWEAK}\")

add_definitions(-D${XMAKE}_RELEASE_TYPE=\"${CMAKE_BUILD_TYPE}\")
add_definitions(-D${XMAKE}_RELEASE_VERSION=\"${${XMAKE}_RELEASE_VERSION}\")
add_definitions(-D${XMAKE}_RELEASE_TIMESTAMP=\"${${XMAKE}_RELEASE_TIMESTAMP}\")

if(HOST_LINUX)
    add_definitions(-DHOST_LINUX)
endif()

if(HOST_MACOS)
    add_definitions(-DHOST_MACOS)
endif()

if(HOST_WINDOWS)
    add_definitions(-DHOST_WINDOWS)
endif()

if(HOST_WINDOWS_MSYS)
    add_definitions(-DHOST_WINDOWS_MSYS)
endif()

if(HOST_WINDOWS_MINGW)
    add_definitions(-DHOST_WINDOWS_MINGW)
endif()

if(HOST_WINDOWS_CYGWIN)
    add_definitions(-DHOST_WINDOWS_CYGWIN)
endif()

if(HOST_ARCH_32)
    add_definitions(-DHOST_ARCH_32)
endif()

if(HOST_ARCH_64)
    add_definitions(-DHOST_ARCH_64)
endif()

if(HOST_BIG_ENDIAN)
    add_definitions(-DHOST_BIG_ENDIAN)
endif()