find_package(PkgConfig)
pkg_check_modules(ZBAR REQUIRED zbar)
find_library(ZBAR_LIBRARY_VAR
    NAMES ${ZBAR_LIBRARIES}
    HINTS ${ZBAR_LIBRARY_DIRS} ${ZBAR_LIBDIR}
)
