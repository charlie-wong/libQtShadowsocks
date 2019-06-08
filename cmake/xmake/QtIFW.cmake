# https://doc.qt.io/qtinstallerframework/index.html
# https://cmake.org/cmake/help/latest/module/CPackIFW.html
# https://gitlab.kitware.com/cmake/community/wikis/home

# The Qt Installer Framework tool search path provided by user
set(QTIFWDIR "${${XMAKE}_IFW_PREFIX}")

# Set to ON to enable addition debug output
SET(CPACK_IFW_VERBOSE ON)

set(CPACK_IFW_PACKAGE_TITLE "${PKG_NAME} Installer Setup")
set(CPACK_IFW_PRODUCT_URL "${PKG_REPO}")
set(CPACK_IFW_PACKAGE_WINDOW_ICON "${PKG_LOGO}")
set(CPACK_IFW_TARGET_DIRECTORY "${CPACK_PACKAGE_INSTALL_DIRECTORY}")
