if(NOT SSQT_BUILD_SSTUI)
    return()
endif()

file(GLOB SSCS_HEADERS *.h)
file(GLOB SSCS_SOURCES *.cpp)
include_directories(${PROJECT_SOURCE_DIR}/source/lib)
add_executable(shadowsocks ${SSCS_HEADERS} ${SSCS_SOURCES})
add_dependencies(shadowsocks shared_LibQtShadowSocks)
target_link_libraries(shadowsocks Qt5::Core Qt5::Network
    shared_LibQtShadowSocks ${botan2_LIBRARIES}
)
XmakeInstallHelper(TARGETS shadowsocks)
