# Search Static Qt5 Libraries
# - Link sequence is much more important
# - https://doc.qt.io/qt-5/cmake-manual.html
# - https://doc.qt.io/qt-5/linux-requirements.html

function(Qt5StaticLibFind name qt5_libs_search_path output)
    set(libName ${CMAKE_STATIC_LIBRARY_PREFIX}${name}${CMAKE_STATIC_LIBRARY_SUFFIX})
    find_library(qt5StaticLibrary_${name}
        NAMES ${libName} NAMES_PER_DIR
        PATHS ${qt5_libs_search_path}
        NO_CMAKE_PATH
        NO_DEFAULT_PATH
        NO_CMAKE_ENVIRONMENT_PATH
        NO_SYSTEM_ENVIRONMENT_PATH
        NO_CMAKE_SYSTEM_PATH
    )
    mark_as_advanced(qt5StaticLibrary_${name})
    # message(STATUS "${libName} => ${qt5StaticLibrary_${name}}")
    if(EXISTS ${qt5StaticLibrary_${name}})
        set(${output} ${qt5StaticLibrary_${name}} PARENT_SCOPE)
    else()
        message(FATAL_ERROR "Do NOT find Qt5 static library: ${libName}")
    endif()
endfunction()

# Qt5 Static Library Search Prefix
set(Qt5SLSPrefix ${xmakeI_QT5_STATIC_PREFIX})
mark_as_advanced(Qt5SLSPrefix)

# Static Qt5 Library: plugins/platforms/libqxcb.a
Qt5StaticLibFind(qxcb ${Qt5SLSPrefix}/plugins/platforms Qt5StaticLibrary)
list(APPEND ${XMAKE}_AUTO_QT5_LIBRARIES ${Qt5StaticLibrary})

# Static Qt5 Library: lib/libQt5XcbQpa.a
Qt5StaticLibFind(Qt5XcbQpa ${Qt5SLSPrefix}/lib Qt5StaticLibrary)
list(APPEND ${XMAKE}_AUTO_QT5_LIBRARIES ${Qt5StaticLibrary})

# Static Qt5 Library: lib/libQt5ServiceSupport.a
Qt5StaticLibFind(Qt5ServiceSupport ${Qt5SLSPrefix}/lib Qt5StaticLibrary)
list(APPEND ${XMAKE}_AUTO_QT5_LIBRARIES ${Qt5StaticLibrary})

# Static Qt5 Library: lib/libQt5ThemeSupport.a
Qt5StaticLibFind(Qt5ThemeSupport ${Qt5SLSPrefix}/lib Qt5StaticLibrary)
list(APPEND ${XMAKE}_AUTO_QT5_LIBRARIES ${Qt5StaticLibrary})

# Static Qt5 Library: lib/libQt5DBus.a
Qt5StaticLibFind(Qt5DBus ${Qt5SLSPrefix}/lib Qt5StaticLibrary)
list(APPEND ${XMAKE}_AUTO_QT5_LIBRARIES ${Qt5StaticLibrary})

# Static Qt5 Library: lib/libQt5EventDispatcherSupport.a
Qt5StaticLibFind(Qt5EventDispatcherSupport ${Qt5SLSPrefix}/lib Qt5StaticLibrary)
list(APPEND ${XMAKE}_AUTO_QT5_LIBRARIES ${Qt5StaticLibrary})

# Static Qt5 Library: lib/libQt5FontDatabaseSupport.a
Qt5StaticLibFind(Qt5FontDatabaseSupport ${Qt5SLSPrefix}/lib Qt5StaticLibrary)
list(APPEND ${XMAKE}_AUTO_QT5_LIBRARIES ${Qt5StaticLibrary})

# Multi Arch Spec
# - https://wiki.ubuntu.com/MultiarchSpec
# - https://err.no/debian/amd64-multiarch-3
if(HOST_ARCH_32)
    list(APPEND MultiArchSearchDirs "/lib32")
    list(APPEND MultiArchSearchDirs "/usr/lib32")
    list(APPEND MultiArchSearchDirs "/lib/i386-linux-gnu")
    list(APPEND MultiArchSearchDirs "/usr/lib/i386-linux-gnu")
else()
    list(APPEND MultiArchSearchDirs "/lib64")
    list(APPEND MultiArchSearchDirs "/usr/lib64")
    list(APPEND MultiArchSearchDirs "/lib/x86_64-linux-gnu")
    list(APPEND MultiArchSearchDirs "/usr/lib/x86_64-linux-gnu")
endif()
mark_as_advanced(MultiArchSearchDirs)

function(Qt5SystemLibFind name output shared_first)
    list(APPEND libNames "${name}")
    list(INSERT libNames 0
        ${CMAKE_SHARED_LIBRARY_PREFIX}${name}${CMAKE_SHARED_LIBRARY_SUFFIX})
    list(INSERT libNames 0
        ${CMAKE_STATIC_LIBRARY_PREFIX}${name}${CMAKE_STATIC_LIBRARY_SUFFIX})

    if(shared_first)
        list(REVERSE libNames) # shared libray come first
    endif()

    find_library(qt5SystemLibrary_${name}
        NAMES ${libNames} NAMES_PER_DIR
        PATHS ${MultiArchSearchDirs}
    )

    mark_as_advanced(qt5SystemLibrary_${name})
    if(EXISTS ${qt5SystemLibrary_${name}})
        set(${output} ${qt5SystemLibrary_${name}} PARENT_SCOPE)
    else()
        message(FATAL_ERROR "Do NOT find Qt5 system library: ${libName}")
    endif()
endfunction()

# System Library: libfontconfig.a, libfontconfig.so
Qt5SystemLibFind(fontconfig Qt5SystemLibrary OFF)
list(APPEND ${XMAKE}_AUTO_QT5_LIBRARIES ${Qt5SystemLibrary})

# System Library: libexpat.a, libexpat.so
Qt5SystemLibFind(expat Qt5SystemLibrary OFF)
list(APPEND ${XMAKE}_AUTO_QT5_LIBRARIES ${Qt5SystemLibrary})

# System Library: libfreetype.a, libfreetype.so
Qt5SystemLibFind(freetype Qt5SystemLibrary OFF)
list(APPEND ${XMAKE}_AUTO_QT5_LIBRARIES ${Qt5SystemLibrary})

# The X.Org project provides an open source implementation
# of the X Window System, which is widely used in linux system,
# so just link to the dynamic libraries.
#
# FindX11.cmake for Find X11 installation
# - X11_FOUND        True if X11 is available
# - X11_INCLUDE_DIR  Include directories to use X11
# - X11_LIBRARIES    Link against these to use X11
find_package(X11 REQUIRED)
if(NOT X11_FOUND)
    # libX11.so: library for the X Window System
    message(FATAL_ERROR "Do NOT find X Window System library: libX11.so ")
endif()
if(NOT X11_Xi_FOUND)
    # libXi.so: library for the X Input Extension
    message(FATAL_ERROR "Do NOT find X Window System library: libXi.so ")
endif()
# libXau.so: A Sample Authorization Protocol for X
list(APPEND ${XMAKE}_AUTO_QT5_LIBRARIES ${X11_Xi_LIB})
list(APPEND ${XMAKE}_AUTO_QT5_LIBRARIES ${X11_X11_LIB})

# Xlib/XCB interface library provides functions needed by
# clients which take advantage of Xlib/XCB to mix calls to
# both Xlib and XCB over the same X connection.
#
# System Library: libX11-xcb.a, libX11-xcb.so
Qt5SystemLibFind(X11-xcb Qt5SystemLibrary OFF)
list(APPEND ${XMAKE}_AUTO_QT5_LIBRARIES ${Qt5SystemLibrary})

# Static Qt5 Library: lib/libxcb-static.a
Qt5StaticLibFind(xcb-static ${Qt5SLSPrefix}/lib Qt5StaticLibrary)
list(APPEND ${XMAKE}_AUTO_QT5_LIBRARIES ${Qt5StaticLibrary})

# The X protocol C-language Binding (XCB) is a replacement for Xlib
# featuring a small footprint, latency hiding, direct access to the
# protocol, improved threading support, and extensibility. On Linux,
# the xcb QPA (Qt Platform Abstraction) platform plugin is used.
#
# System Library: libxcb.a, libxcb.so
Qt5SystemLibFind(xcb Qt5SystemLibrary ON)
list(APPEND ${XMAKE}_AUTO_QT5_LIBRARIES ${Qt5SystemLibrary})

# Static Qt5 Library: plugins/imageformats/libqgif.a
Qt5StaticLibFind(qgif ${Qt5SLSPrefix}/plugins/imageformats Qt5StaticLibrary)
list(APPEND ${XMAKE}_AUTO_QT5_LIBRARIES ${Qt5StaticLibrary})

# Static Qt5 Library: plugins/imageformats/libqicns.a
Qt5StaticLibFind(qicns ${Qt5SLSPrefix}/plugins/imageformats Qt5StaticLibrary)
list(APPEND ${XMAKE}_AUTO_QT5_LIBRARIES ${Qt5StaticLibrary})

# Static Qt5 Library: plugins/imageformats/libqico.a
Qt5StaticLibFind(qico ${Qt5SLSPrefix}/plugins/imageformats Qt5StaticLibrary)
list(APPEND ${XMAKE}_AUTO_QT5_LIBRARIES ${Qt5StaticLibrary})

# Static Qt5 Library: plugins/imageformats/libqjpeg.a
Qt5StaticLibFind(qjpeg ${Qt5SLSPrefix}/plugins/imageformats Qt5StaticLibrary)
list(APPEND ${XMAKE}_AUTO_QT5_LIBRARIES ${Qt5StaticLibrary})

# Static Qt5 Library: plugins/imageformats/libqtga.a
Qt5StaticLibFind(qtga ${Qt5SLSPrefix}/plugins/imageformats Qt5StaticLibrary)
list(APPEND ${XMAKE}_AUTO_QT5_LIBRARIES ${Qt5StaticLibrary})

# Static Qt5 Library: plugins/imageformats/libqtiff.a
Qt5StaticLibFind(qtiff ${Qt5SLSPrefix}/plugins/imageformats Qt5StaticLibrary)
list(APPEND ${XMAKE}_AUTO_QT5_LIBRARIES ${Qt5StaticLibrary})

# Static Qt5 Library: plugins/imageformats/libqwbmp.a
Qt5StaticLibFind(qwbmp ${Qt5SLSPrefix}/plugins/imageformats Qt5StaticLibrary)
list(APPEND ${XMAKE}_AUTO_QT5_LIBRARIES ${Qt5StaticLibrary})

# Static Qt5 Library: plugins/imageformats/libqwebp.a
Qt5StaticLibFind(qwebp ${Qt5SLSPrefix}/plugins/imageformats Qt5StaticLibrary)
list(APPEND ${XMAKE}_AUTO_QT5_LIBRARIES ${Qt5StaticLibrary})

# Static Qt5 Library: lib/libQt5Core.a
find_package(Qt5Core CONFIG REQUIRED)
mark_as_advanced(Qt5Core_DIR)
get_target_property(Qt5StaticLibrary Qt5::Core LOCATION)
list(APPEND ${XMAKE}_AUTO_QT5_LIBRARIES ${Qt5StaticLibrary})

# Static Qt5 Library: lib/libQt5Gui.a
find_package(Qt5Gui CONFIG REQUIRED)
mark_as_advanced(Qt5Gui_DIR)
get_target_property(Qt5StaticLibrary Qt5::Gui LOCATION)
list(APPEND ${XMAKE}_AUTO_QT5_LIBRARIES ${Qt5StaticLibrary})
foreach(plugin ${Qt5Gui_PLUGINS})
    get_target_property(location ${plugin} LOCATION)
    list(APPEND ${XMAKE}_AUTO_QT5_LIBRARIES ${Qt5StaticLibrary})
    # message("Qt5Gui Plugin: ${plugin} => ${location}")
endforeach()

# Static Qt5 Library: lib/libQt5Sql.a
find_package(Qt5Sql CONFIG REQUIRED)
mark_as_advanced(Qt5Sql_DIR)
get_target_property(Qt5StaticLibrary Qt5::Sql LOCATION)
list(APPEND ${XMAKE}_AUTO_QT5_LIBRARIES ${Qt5StaticLibrary})
foreach(plugin ${Qt5Sql_PLUGINS})
    get_target_property(location ${plugin} LOCATION)
    list(APPEND ${XMAKE}_AUTO_QT5_LIBRARIES ${Qt5StaticLibrary})
    # message("Qt5Sql Plugin: ${plugin} => ${location}")
endforeach()

# Static Qt5 Library: lib/libQt5Widgets.a
find_package(Qt5Widgets CONFIG REQUIRED)
mark_as_advanced(Qt5Widgets_DIR)
get_target_property(Qt5StaticLibrary Qt5::Widgets LOCATION)
list(APPEND ${XMAKE}_AUTO_QT5_LIBRARIES ${Qt5StaticLibrary})
foreach(plugin ${Qt5Widgets_PLUGINS})
    get_target_property(location ${plugin} LOCATION)
    list(APPEND ${XMAKE}_AUTO_QT5_LIBRARIES ${Qt5StaticLibrary})
    # message("Qt5Widgets Plugin: ${plugin} => ${location}")
endforeach()

# Static Qt5 Library: lib/libQt5Network.a
find_package(Qt5Network CONFIG REQUIRED)
mark_as_advanced(Qt5Network_DIR)
get_target_property(Qt5StaticLibrary Qt5::Network LOCATION)
list(APPEND ${XMAKE}_AUTO_QT5_LIBRARIES ${Qt5StaticLibrary})
foreach(plugin ${Qt5Network_PLUGINS})
    get_target_property(location ${plugin} LOCATION)
    list(APPEND ${XMAKE}_AUTO_QT5_LIBRARIES ${Qt5StaticLibrary})
    # message("Qt5Network Plugin: ${plugin} => ${location}")
endforeach()

# System Library: libpng.a, libpng.so
Qt5SystemLibFind(png Qt5SystemLibrary OFF)
list(APPEND ${XMAKE}_AUTO_QT5_LIBRARIES ${Qt5SystemLibrary})
# System Library: libpng12.a, libpng12.so
Qt5SystemLibFind(png12 Qt5SystemLibrary OFF)
list(APPEND ${XMAKE}_AUTO_QT5_LIBRARIES ${Qt5SystemLibrary})

# Static Qt5 Library: lib/libqtharfbuzz.a
Qt5StaticLibFind(qtharfbuzz ${Qt5SLSPrefix}/lib Qt5StaticLibrary)
list(APPEND ${XMAKE}_AUTO_QT5_LIBRARIES ${Qt5StaticLibrary})

# ICU - International Components for Unicode if use the
# static ICU, the output will get bigger nearly double size.
#
# System Library: libicui18n.a, libicui18n.so
Qt5SystemLibFind(icui18n Qt5SystemLibrary ON)
list(APPEND ${XMAKE}_AUTO_QT5_LIBRARIES ${Qt5SystemLibrary})
# System Library: libicuuc.a, libicuuc.so
Qt5SystemLibFind(icuuc Qt5SystemLibrary ON)
list(APPEND ${XMAKE}_AUTO_QT5_LIBRARIES ${Qt5SystemLibrary})
# System Library: libicudata.a, libicudata.so
Qt5SystemLibFind(icudata Qt5SystemLibrary ON)
list(APPEND ${XMAKE}_AUTO_QT5_LIBRARIES ${Qt5SystemLibrary})

# System Library: libm.a, libm.so
Qt5SystemLibFind(m Qt5SystemLibrary ON)
list(APPEND ${XMAKE}_AUTO_QT5_LIBRARIES ${Qt5SystemLibrary})
# System Library: libz.a, libz.so
Qt5SystemLibFind(z Qt5SystemLibrary ON)
list(APPEND ${XMAKE}_AUTO_QT5_LIBRARIES ${Qt5SystemLibrary})

# Static Qt5 Library: lib/libqtpcre2.a
Qt5StaticLibFind(qtpcre2 ${Qt5SLSPrefix}/lib Qt5StaticLibrary)
list(APPEND ${XMAKE}_AUTO_QT5_LIBRARIES ${Qt5StaticLibrary})

# System Library: libdl.a, libdl.so
Qt5SystemLibFind(dl Qt5SystemLibrary ON)
list(APPEND ${XMAKE}_AUTO_QT5_LIBRARIES ${Qt5SystemLibrary})

# Gthread: part of Glib
# Pthread: POSIX thread standard
#
# System Library: libgthread-2.0.a, libgthread-2.0.so
Qt5SystemLibFind(gthread-2.0 Qt5SystemLibrary ON)
list(APPEND ${XMAKE}_AUTO_QT5_LIBRARIES ${Qt5SystemLibrary})

# The GLib provides the core application building blocks
# for libraries and applications written in C.
#
# It contains low-level libraries useful for providing data
# structure handling for C, portability wrappers and interfaces
# for such runtime functionality as an event loop, threads, dynamic
# loading and an object system.
#
# System Library: libglib-2.0.a, libglib-2.0.so
Qt5SystemLibFind(glib-2.0 Qt5SystemLibrary ON)
list(APPEND ${XMAKE}_AUTO_QT5_LIBRARIES ${Qt5SystemLibrary})

if(false)
    message(STATUS "${XMAKE}_AUTO_QT5_SOURCES=${${XMAKE}_AUTO_QT5_SOURCES}")
    foreach(item ${${XMAKE}_AUTO_QT5_LIBRARIES})
        message(STATUS "Qt5Lib: ${item}")
    endforeach()
    message(FATAL_ERROR "STOP")
endif()

mark_as_advanced(${XMAKE}_AUTO_QT5_LIBRARIES ${XMAKE}_AUTO_QT5_SOURCES)
