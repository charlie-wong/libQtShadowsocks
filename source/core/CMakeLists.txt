# ShadowSocks Core Library Name
set(SSCL_NAME "QtShadowsocks")

file(GLOB UTILS_HEADERS utils/*.h)
file(GLOB UTILS_SOURCES utils/*.cpp)
file(GLOB CRYPTO_HEADERS crypto/*.h)
file(GLOB CRYPTO_SOURCES crypto/*.cpp)
file(GLOB NETWORK_HEADERS network/*.h)
file(GLOB NETWORK_SOURCES network/*.cpp)

list(APPEND SSCL_HEADERS
    QtShadowsocks
    "${UTILS_HEADERS}"
    "${CRYPTO_HEADERS}"
    "${NETWORK_HEADERS}"
)

list(APPEND SSCL_SOURCES
    "${UTILS_SOURCES}"
    "${CRYPTO_SOURCES}"
    "${NETWORK_SOURCES}"
)

include_directories(${botan2_INCLUDE_DIRS})
add_compile_options(-Wno-missing-field-initializers)

add_library(static_LibQtShadowSocks STATIC ${SSCL_HEADERS} ${SSCL_SOURCES})
target_link_libraries(static_LibQtShadowSocks Qt5::Core Qt5::Network)
set_target_properties(static_LibQtShadowSocks
    PROPERTIES OUTPUT_NAME "${SSCL_NAME}"
)

if(SSQT_DBI_BOTAN2)
    add_dependencies(static_LibQtShadowSocks ${botan2_TARGET})
endif()

XmakeInstallHelper(TARGETS static_LibQtShadowSocks
    EXPORT_LIBRARY_INFO
    EXPORT_LIBRARY_WITH_EXTRA_LIBS Qt5Core Qt5Network botan-2
)

if(SSQT_BUILD_SHARED)
    add_library(shared_LibQtShadowSocks SHARED ${SSCL_HEADERS} ${SSCL_SOURCES})
    target_link_libraries(shared_LibQtShadowSocks Qt5::Core Qt5::Network)
    set_target_properties(shared_LibQtShadowSocks PROPERTIES
        OUTPUT_NAME "${SSCL_NAME}"
        VERSION ${SSQT_VERSION_MMP}
        SOVERSION ${SSQT_VERSION_MAJOR}
    )

    if(SSQT_DBI_BOTAN2)
        add_dependencies(shared_LibQtShadowSocks ${botan2_TARGET})
    endif()

    XmakeInstallHelper(TARGETS shared_LibQtShadowSocks
        EXPORT_LIBRARY_INFO
        EXPORT_LIBRARY_WITH_EXTRA_LIBS Qt5Core Qt5Network botan-2
    )
endif()

# Install ShadowSocks Core Library Headers
XmakeInstallHelper(FILES "${UTILS_HEADERS}"
    DESTINATION include/${SSCL_NAME}/util
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
