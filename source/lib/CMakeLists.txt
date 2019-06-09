string(REPLACE "v" "" SSQTL_VERSION "${PKG_VERSION}")
add_definitions(-DQSS_VERSION="${SSQTL_RELEASE_VERSION}")

file(GLOB TYPES_HEADERS types/*.h)
file(GLOB TYPES_SOURCES types/*.cpp)
file(GLOB UTILS_HEADERS util/*.h)
file(GLOB UTILS_SOURCES util/*.cpp)
file(GLOB CRYPTO_HEADERS crypto/*.h)
file(GLOB CRYPTO_SOURCES crypto/*.cpp)
file(GLOB NETWORK_HEADERS network/*.h)
file(GLOB NETWORK_SOURCES network/*.cpp)

list(APPEND SSCL_HEADERS
    QtShadowsocks
    "${TYPES_HEADERS}"
    "${UTILS_HEADERS}"
    "${CRYPTO_HEADERS}"
    "${NETWORK_HEADERS}"
)

list(APPEND SSCL_SOURCES
    "${TYPES_SOURCES}"
    "${UTILS_SOURCES}"
    "${CRYPTO_SOURCES}"
    "${NETWORK_SOURCES}"
)

include_directories(${botan2_INCLUDE_DIRS})

add_library(static_LibQtShadowSocks STATIC ${SSCL_HEADERS} ${SSCL_SOURCES})
target_link_libraries(static_LibQtShadowSocks Qt5::Core Qt5::Network)
set_target_properties(static_LibQtShadowSocks
    PROPERTIES OUTPUT_NAME "${SSCL_NAME}"
)
add_dependencies(static_LibQtShadowSocks ${botan2_TARGET})
XmakeInstallHelper(TARGETS static_LibQtShadowSocks
    EXPORT_LIBRARY_INFO
    EXPORT_LIBRARY_WITH_EXTRA_LIBS Qt5Core Qt5Network botan-2
)

if(SSQTL_BUILD_SHARED)
    add_library(shared_LibQtShadowSocks SHARED ${SSCL_HEADERS} ${SSCL_SOURCES})
    target_link_libraries(shared_LibQtShadowSocks Qt5::Core Qt5::Network)
    set_target_properties(shared_LibQtShadowSocks PROPERTIES
        OUTPUT_NAME "${SSCL_NAME}"
        VERSION ${SSQTL_VERSION}
        SOVERSION ${SSQTL_VERSION_MAJOR}
    )
    add_dependencies(shared_LibQtShadowSocks ${botan2_TARGET})
    XmakeInstallHelper(TARGETS shared_LibQtShadowSocks
        EXPORT_LIBRARY_INFO
        EXPORT_LIBRARY_WITH_EXTRA_LIBS Qt5Core Qt5Network botan-2
    )
endif()

# Install ShadowSocks Core Library Headers
XmakeInstallHelper(FILES "${UTILS_HEADERS}"
    DESTINATION include/${SSCL_NAME}/util
)
XmakeInstallHelper(FILES "${TYPES_HEADERS}"
    DESTINATION include/${SSCL_NAME}/types
)
XmakeInstallHelper(FILES "${CRYPTO_HEADERS}"
    DESTINATION include/${SSCL_NAME}/crypto
)
XmakeInstallHelper(FILES "${NETWORK_HEADERS}"
    DESTINATION include/${SSCL_NAME}/network
)
XmakeInstallHelper(FILES QtShadowsocks
    DESTINATION include/${SSCL_NAME}/
)
