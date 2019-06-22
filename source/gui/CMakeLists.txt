# Shadowsocks GUI Client/Server Name
set(APP_NAME "shadowsocksqt-gui")

find_package(PkgConfig)
# MinGW
# - $ pacman -S mingw-w64-i686-zbar
# - $ pacman -S mingw-w64-x86_64-zbar
pkg_check_modules(zbar REQUIRED zbar)

file(GLOB GUI_APP_HEADERS *.h)
file(GLOB GUI_APP_SOURCES *.cpp)

list(APPEND GUI_APP_HEADERS ${CONFIG_HEADERS})
list(APPEND GUI_APP_SOURCES ${CONFIG_SOURCES})

list(APPEND GUI_APP_SOURCES icons.qrc translations.qrc)

if(WIN32)
    list(APPEND GUI_APP_SOURCES ssqt.rc)
endif()

add_executable(${APP_NAME} WIN32
    ${GUI_APP_HEADERS} ${GUI_APP_SOURCES}
)
XmakeQt5StaticPluginSrcAdd(${APP_NAME})
target_include_directories(${APP_NAME}
    PRIVATE ${botan2_INCLUDE_DIRS}
    PRIVATE ${qrencode_INCLUDE_DIRS}
)

target_link_libraries(${APP_NAME}
    Qt5::Gui
    Qt5::Core
    Qt5::Widgets
    Qt5::Network
    ${SSQT_CoreLib}
    ${Qt5DBus_LIBRARIES}
    ${zbar_LIBRARIES}
    ${botan2_LIBRARIES}
    ${qrencode_LIBRARIES}
)

XmakeInstallHelper(TARGETS ${APP_NAME})

if(UNIX)
    XmakeInstallHelper(FILES "shadowsocksqt.desktop"
        DESTINATION share/applications
    )
    XmakeInstallHelper(FILES "icons/shadowsocksqt.png"
        DESTINATION share/icons/hicolor/512x512/apps
    )
endif()
