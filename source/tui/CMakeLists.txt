file(GLOB SSCS_HEADERS *.h)
file(GLOB SSCS_SOURCES *.cpp)

add_executable(shadowsocks ${SSCS_HEADERS} ${SSCS_SOURCES})
add_dependencies(shadowsocks ${SSQT_CoreLib})
target_link_libraries(shadowsocks Qt5::Core Qt5::Network
    ${SSQT_CoreLib} ${botan2_LIBRARIES}
)
XmakeInstallHelper(TARGETS shadowsocks)
