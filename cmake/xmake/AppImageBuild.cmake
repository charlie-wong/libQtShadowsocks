include(CMakeParseArguments)

# About AppImage
# https://appimage.org
# https://docs.appimage.org
if(HOST_WINDOWS OR HOST_MACOS)
    message(WARNING "AppImage only support for linux, SKIP!")
    return()
endif()

if(${XMAKE}_APPIMAGE_DOWNLOAD_SKIP_SHA256)
    set(download_SKIP_SHA256 "SKIP_SHA256")
endif()

function(AppImageDownload_appimagetool save_as)
    set(downloadUrl
        "https://github.com/AppImage/AppImageKit/releases/download/11"
    )

    if(HOST_ARCH_32)
        XmakeDownloadFile(MARK_EXECUTABLE ${download_SKIP_SHA256}
            OUTPUT  ${save_as}
            SHA256  "d691fac50728fe5d46ca584c7843088a306751ff6d3dcf790bf42c0784f7a213"
            URL     "${downloadUrl}/appimagetool-i686.AppImage"
        )
    else()
        XmakeDownloadFile(MARK_EXECUTABLE ${download_SKIP_SHA256}
            OUTPUT  ${save_as}
            SHA256  "c13026b9ebaa20a17e7e0a4c818a901f0faba759801d8ceab3bb6007dde00372"
            URL     "${downloadUrl}/appimagetool-x86_64.AppImage"
        )
    endif()
endfunction()

function(AppImageDownload_appimageupdate cli_save_as gui_save_as)
    set(downloadUrl
        "https://github.com/AppImage/AppImageUpdate/releases/download/continuous"
    )

    if(HOST_ARCH_32)
        if(gui_save_as)
            XmakeDownloadFile(MARK_EXECUTABLE ${download_SKIP_SHA256}
                OUTPUT  ${gui_save_as}
                SHA256  "3905425078c7b9856d7d71f9977d95877fe4ef1d8fe9aaad2a2cc37fc518c63e"
                URL     "${downloadUrl}/AppImageUpdate-i386.AppImage"
            )
        endif()

        if(cli_save_as)
            XmakeDownloadFile(MARK_EXECUTABLE ${download_SKIP_SHA256}
                OUTPUT  ${cli_save_as}
                SHA256  "84ea913c1ed5b0f1fb648381943160a8e1c63d4db68241f8e8cd2fae1ab46ab2"
                URL     "${downloadUrl}/appimageupdatetool-i386.AppImage"
            )
        endif()
    else()
        if(gui_save_as)
            XmakeDownloadFile(MARK_EXECUTABLE ${download_SKIP_SHA256}
                OUTPUT  ${gui_save_as}
                SHA256  "4addc77e6fcb8af0c433c7eefc6875b351b7eb4e599687a23fb39f94af8ed0ee"
                URL     "${downloadUrl}/AppImageUpdate-x86_64.AppImage"
            )
        endif()

        if(cli_save_as)
            XmakeDownloadFile(MARK_EXECUTABLE ${download_SKIP_SHA256}
                OUTPUT  ${cli_save_as}
                SHA256  "690c200d2f26b73b8968f7073a26256b94902f690f5aa70f782a667e1cc2ee48"
                URL     "${downloadUrl}/appimageupdatetool-x86_64.AppImage"
            )
        endif()
    endif()
endfunction()

function(AppImageDownload_linuxdeploy save_as)
    set(downloadUrl
        "https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous"
    )

    if(HOST_ARCH_32)
        XmakeDownloadFile(MARK_EXECUTABLE ${download_SKIP_SHA256}
            OUTPUT  ${save_as}
            SHA256  "ea99328a4741af2abe522790c374ffbc7d8b1b37fb263747beff9a8529fb5b7a"
            URL     "${downloadUrl}/linuxdeploy-i386.AppImage"
        )
    else()
        XmakeDownloadFile(MARK_EXECUTABLE ${download_SKIP_SHA256}
            OUTPUT  ${save_as}
            SHA256  "5dcf5630b05b16ef1a91dac84a6d260d2ae4aa9387414f191097c13be4775430"
            URL     "${downloadUrl}/linuxdeploy-x86_64.AppImage"
        )
    endif()
endfunction()

function(AppImageDownload_linuxdeployqt save_as)
    set(downloadUrl
        "https://github.com/probonopd/linuxdeployqt/releases/download/6"
    )

    if(HOST_ARCH_64)
        XmakeDownloadFile(MARK_EXECUTABLE ${download_SKIP_SHA256}
            OUTPUT  ${save_as}
            SHA256  "562d2e4dca44767bbf2410523d50b2834aa3e8c91199716e0f7d5690de47f0d1"
            URL     "${downloadUrl}/linuxdeployqt-6-x86_64.AppImage"
        )
    endif()
endfunction()

# AppImageTool      Create AppImages
# https://docs.appimage.org/introduction/software-overview.html
set(ait_appimagetool "${DEPS_BIN_DIR}/appimagetool.appimage")
AppImageDownload_appimagetool("${ait_appimagetool}")

if(false)
# AppImageUpdate    Update AppImages
# https://docs.appimage.org/introduction/software-overview.html
set(ait_appimageupdate_gui "${DEPS_BIN_DIR}/AppImageUpdate.appimage")
set(ait_appimageupdate_cli "${DEPS_BIN_DIR}/appimageupdatetool.appimage")
AppImageDownload_appimageupdate("${ait_appimageupdate_cli}" "${ait_appimageupdate_gui}")

# linuxdeploy       AppDir creation and maintenance tool
# https://github.com/linuxdeploy/linuxdeploy
set(ait_linuxdeploy "${DEPS_BIN_DIR}/linuxdeploy.appimage")
AppImageDownload_linuxdeploy("${ait_linuxdeploy}")

# linuxdeployqt     Makes Linux applications self-contained
set(ait_linuxdeployqt "${DEPS_BIN_DIR}/linuxdeployqt.appimage")
AppImageDownload_linuxdeployqt("${ait_linuxdeployqt}")
endif()

# User extra configuration for package
if(EXISTS ${CMAKE_SOURCE_DIR}/ConfigAppImage.cmake)
    include(${CMAKE_SOURCE_DIR}/ConfigAppImage.cmake)
elseif(EXISTS ${CMAKE_SOURCE_DIR}/cmake/ConfigAppImage.cmake)
    include(${CMAKE_SOURCE_DIR}/cmake/ConfigAppImage.cmake)
endif()

# The app is TUI by default
set(pkg_app_is_tui true)
if(PKG_APP_GUI)
    set(pkg_app_is_tui false)
endif()

# AppRun    *.desktop   *.[png, svg]    .DirIcon
# https://docs.appimage.org/reference/appdir.html
set(appdir_ROOT "${CMAKE_BINARY_DIR}/usr")
set(CMAKE_INSTALL_PREFIX "${appdir_ROOT}")

# AppRun
if(PKG_APPIMAGE_APPRUN)
    if(EXISTS "${PKG_APPIMAGE_APPRUN}")
        install(PROGRAMS "${PKG_APPIMAGE_APPRUN}"
            DESTINATION "${appdir_ROOT}" RENAME "AppRun"
        )
    endif()
else()
    configure_file("${CMAKE_CURRENT_LIST_DIR}/appdir/AppRun.in"
        "${appdir_ROOT}/AppRun" @ONLY
    )
endif()

# *.desktop
if(PKG_APPIMAGE_DESKTOP)
    if(EXISTS "${PKG_APPIMAGE_DESKTOP}")
        install(FILES "${PKG_APPIMAGE_DESKTOP}"
            DESTINATION "${appdir_ROOT}/share/applications"
            RENAME "${PKG_NAME}.desktop"
        )
    endif()
else()
    configure_file("${CMAKE_CURRENT_LIST_DIR}/appdir/app.desktop.in"
        "${appdir_ROOT}/share/applications/${PKG_NAME}.desktop" @ONLY
    )
endif()

# Icon Theme Specification
# https://specifications.freedesktop.org/icon-theme-spec/icon-theme-spec-latest.html
macro(AppImageIcosSetup SIZE)
    if(EXISTS "${PKG_APPIMAGE_ICON${SIZE}}")
        get_filename_component(ext "${PKG_APPIMAGE_ICON${SIZE}}" EXT)
        install(FILES "${PKG_APPIMAGE_ICON${SIZE}}"
            DESTINATION "${appdir_ROOT}/share/icons/hicolor/${SIZE}x${SIZE}/apps/"
            RENAME "${PKG_NAME}${ext}"
        )
        set(appdir_ICON
            "${appdir_ROOT}/share/icons/hicolor/${SIZE}x${SIZE}/apps/${PKG_NAME}${ext}")
    endif()
endmacro()

AppImageIcosSetup(16)
AppImageIcosSetup(32)
AppImageIcosSetup(48)
AppImageIcosSetup(64)
AppImageIcosSetup(128)
AppImageIcosSetup(256)

if(EXISTS "${PKG_LOGO}")
    get_filename_component(ext "${PKG_LOGO}" EXT)
    install(FILES ${PKG_LOGO}
        DESTINATION "${appdir_ROOT}"
        RENAME "${PKG_NAME}${ext}"
    )
    set(appdir_ICON_SetupCmds
        COMMAND ${CMAKE_COMMAND} -E create_symlink
            "${PKG_NAME}${ext}" "${appdir_ROOT}/.DirIcon" # appdir_ROOT
    )
elseif(appdir_ICON)
    string(REPLACE "${appdir_ROOT}/" "" appdir_ICON "${appdir_ICON}")
    set(appdir_ICON_SetupCmds
        COMMAND ${CMAKE_COMMAND} -E create_symlink
            "${appdir_ICON}" "${appdir_ROOT}/${PKG_NAME}${ext}" # appdir_ROOT
        COMMAND ${CMAKE_COMMAND} -E create_symlink
            "${PKG_NAME}${ext}" "${appdir_ROOT}/.DirIcon" # appdir_ROOT
    )
else()
    message(FATAL_ERROR "AppDir missing ICON, must set at least one of:
PKG_LOGO, PKG_APPIMAGE_ICON{16|32|48|64|128|256}"
    )
endif()

add_custom_target(pkg-appimage
    COMMENT "AppImage packaging ..."
    ${appdir_ICON_SetupCmds}
    COMMAND ${CMAKE_COMMAND} -E create_symlink
        share/applications/${PKG_NAME}.desktop # appdir_ROOT
        ${appdir_ROOT}/${PKG_NAME}.desktop
    COMMAND
         ${ait_appimagetool} ${appdir_ROOT} --verbose
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
# Linux Free Desktop
# - https://www.freedesktop.org/wiki
# Linux Free Desktop Specifications
# - https://www.freedesktop.org/wiki/Specifications
# XDG Base Directory Specification
# - https://specifications.freedesktop.org/basedir-spec/latest
# Desktop Entry Specification
# - https://specifications.freedesktop.org/desktop-entry-spec/latest
# Desktop Menu Specification
# - https://specifications.freedesktop.org/menu-spec/latest
