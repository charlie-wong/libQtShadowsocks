# Shadowsocks Client/Server GUI Name
set(APP_NAME "shadowsocks-gui")

find_package(PkgConfig)
# MinGW
# - $ pacman -S mingw-w64-i686-zbar
# - $ pacman -S mingw-w64-x86_64-zbar
pkg_check_modules(zbar REQUIRED zbar)

file(GLOB SSGUI_HEADERS *.h)
file(GLOB SSGUI_SOURCES *.cpp)

list(APPEND SSGUI_SOURCES icons.qrc translations.qrc)

if (WIN32)
    list(APPEND SSGUI_SOURCES ss-qt5.rc)
endif()

add_executable(${APP_NAME} WIN32
    ${SSGUI_HEADERS} ${SSGUI_SOURCES}
    ${SSQT_AUTO_QT5_SOURCES}
)

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
    ${SSQT_AUTO_QT5_LIBRARIES}
    ${zbar_LIBRARIES}
    ${botan2_LIBRARIES}
    ${qrencode_LIBRARIES}
)

install(TARGETS ${APP_NAME}
    RUNTIME DESTINATION bin
)

if (UNIX)
    install(FILES shadowsocks-qt5.desktop DESTINATION share/applications)
    install(FILES icons/shadowsocks-qt5.png DESTINATION share/icons/hicolor/512x512/apps)
endif()
