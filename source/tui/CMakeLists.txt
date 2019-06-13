file(GLOB SSTUI_HEADERS *.h)
file(GLOB SSTUI_SOURCES *.cpp)

# Shadowsocks Client/Server TUI Name
set(APP_NAME "shadowsocks-tui")

add_executable(${APP_NAME} ${SSTUI_HEADERS} ${SSTUI_SOURCES}
    ${SSQT_AUTO_QT5_SOURCES}
)
add_dependencies(${APP_NAME} ${SSQT_CoreLib})
target_link_libraries(${APP_NAME} Qt5::Core Qt5::Network
    ${SSQT_CoreLib} ${botan2_LIBRARIES} ${SSQT_AUTO_QT5_LIBRARIES}
)

XmakeInstallHelper(TARGETS ${APP_NAME})
