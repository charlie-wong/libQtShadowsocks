# Shadowsocks TUI Client/Server Name
set(APP_NAME "shadowsocksqt-tui")

file(GLOB TUI_APP_HEADERS *.h)
file(GLOB TUI_APP_SOURCES *.cpp)

list(APPEND TUI_APP_HEADERS ${CONFIG_HEADERS})
list(APPEND TUI_APP_SOURCES ${CONFIG_SOURCES})

add_executable(${APP_NAME}
    ${TUI_APP_HEADERS} ${TUI_APP_SOURCES}
)
XmakeQt5StaticPluginSrcAdd(${APP_NAME})
add_dependencies(${APP_NAME} ${SSQT_CoreLib})
target_link_libraries(${APP_NAME}
    Qt5::Core
    Qt5::Network
    ${SSQT_CoreLib}
    ${botan2_LIBRARIES}
)

XmakeInstallHelper(TARGETS ${APP_NAME})
