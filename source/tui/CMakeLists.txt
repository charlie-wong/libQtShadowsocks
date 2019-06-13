file(GLOB SSTUI_HEADERS *.h)
file(GLOB SSTUI_SOURCES *.cpp)

add_executable(shadowsocks-tui ${SSTUI_HEADERS} ${SSTUI_SOURCES}
    ${SSQT_AUTO_QT5_SOURCES}
)
add_dependencies(shadowsocks-tui ${SSQT_CoreLib})
target_link_libraries(shadowsocks-tui Qt5::Core Qt5::Network
    ${SSQT_CoreLib} ${botan2_LIBRARIES} ${SSQT_AUTO_QT5_LIBRARIES}
)

XmakeInstallHelper(TARGETS shadowsocks-tui)
