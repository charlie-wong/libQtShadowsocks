get_filename_component(WIN_DLLS_C_DIR ${CMAKE_C_COMPILER} PATH)
get_filename_component(WIN_DLLS_CPP_DIR ${CMAKE_CXX_COMPILER} PATH)

if(NOT WIN_DLLS_C_DIR AND NOT WIN_DLLS_CPP_DIR)
    message(WARNING "can NOT found dll directory for windows, auto copy skip!")
    return() # skip any way
endif()

if(NOT WIN_DLLS_C_DIR STREQUAL WIN_DLLS_CPP_DIR)
    message(WARNING "windows dll directory from C and CPP not the same, skip!")
    return() # skip any way
endif()

set(WIN_DLLS_DIR ${WIN_DLLS_C_DIR})

macro(SetWinDllName _var _name)
    set(${_var} ${CMAKE_SHARED_LIBRARY_PREFIX}${_name}${CMAKE_SHARED_LIBRARY_SUFFIX})
endmacro()

if(HOST_WINDOWS_MSYS)
    SetWinDllName(msys20_dll "2.0") # msys-2.0.dll
    XmakeCopyInstallFiles(
        FILES       "${WIN_DLLS_DIR}/${msys20_dll}"
        INS_DEST    "${${XMAKE}_INSTALL_BIN_DIR}"
        CPY_TARGET  "copy-${msys20_dll}"
        CPY_DEST    "${CMAKE_BINARY_DIR}/${buildType}/bin"
    )
elseif(HOST_WINDOWS_CYGWIN)
    SetWinDllName(cygwin_dll "win1") # cygwin1.dll
    XmakeCopyInstallFiles(
        FILES       "${WIN_DLLS_DIR}/${cygwin_dll}"
        INS_DEST    "${${XMAKE}_INSTALL_BIN_DIR}"
        CPY_TARGET  "copy-${cygwin_dll}"
        CPY_DEST    "${CMAKE_BINARY_DIR}/${buildType}/bin"
    )
endif()

# copy {lib|msys-|cyg-}stdc++-6.dll
if(NOT ${XMAKE}_USE_STATIC_GCC_LIBS)
    SetWinDllName(gcc_s_seh_1_dll "gcc_s_seh-1") # {lib|msys-|cyg-}gcc_s_seh-1.dll
    XmakeCopyInstallFiles(
        FILES       "${WIN_DLLS_DIR}/${gcc_s_seh_1_dll}"
        INS_DEST    "${${XMAKE}_INSTALL_BIN_DIR}"
        CPY_TARGET  "copy-${gcc_s_seh_1_dll}"
        CPY_DEST    "${CMAKE_BINARY_DIR}/${buildType}/bin"
    )

    SetWinDllName(gcc_s_dw2_1_dll "gcc_s_dw2-1") # {lib|msys-|cyg-}gcc_s_dw2-1.dll
    XmakeCopyInstallFiles(
        FILES       "${WIN_DLLS_DIR}/${gcc_s_dw2_1_dll}"
        INS_DEST    "${${XMAKE}_INSTALL_BIN_DIR}"
        CPY_TARGET  "copy-${gcc_s_dw2_1_dll}"
        CPY_DEST    "${CMAKE_BINARY_DIR}/${buildType}/bin"
    )

    SetWinDllName(stdcxx_6_dll "stdc++-6") # {lib|msys-|cyg-}stdc++-6.dll
    XmakeCopyInstallFiles(
        FILES       "${WIN_DLLS_DIR}/${stdcxx_6_dll}"
        INS_DEST    "${${XMAKE}_INSTALL_BIN_DIR}"
        CPY_TARGET  "copy-${stdcxx_6_dll}"
        CPY_DEST    "${CMAKE_BINARY_DIR}/${buildType}/bin"
    )

    SetWinDllName(winpthread_1_dll "winpthread-1") # {lib|msys-|cyg-}winpthread-1.dll
    XmakeCopyInstallFiles(
        FILES       "${WIN_DLLS_DIR}/${winpthread_1_dll}"
        INS_DEST    "${${XMAKE}_INSTALL_BIN_DIR}"
        CPY_TARGET  "copy-${winpthread_1_dll}"
        CPY_DEST    "${CMAKE_BINARY_DIR}/${buildType}/bin"
    )
endif()
