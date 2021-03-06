# BUILD_TYPE := Dev
# BUILD_TYPE := Debug
# BUILD_TYPE := Coverage
# BUILD_TYPE := Release
# BUILD_TYPE := MinSizeRel
# BUILD_TYPE := RelWithDebInfo

#BUILD_CMD := ninja
#GENERATOR := 'Ninja'

#BUILD_CMD := $(MAKE)
#GENERATOR := 'Unix Makefiles'

#BUILD_DIR := $(SOURCE_DIR)/build
#INSTALL_PREFIX := $(BUILD_DIR)/usr

#DEPS_BUILD_TYPE := Release
#DEPS_ROOT_DIR := $(SOURCE_DIR)/.deps

#CMAKE_PROG :=
#CMAKE_ARGS += ...

#EXTRA_CMAKE_ARGS += ...
#EXTRA_CMAKE_ARGS += -DSSQT_ENABLE_CI=ON
#EXTRA_CMAKE_ARGS += -DSSQT_ENABLE_GCOV=OFF
#EXTRA_CMAKE_ARGS += -DSSQT_DISABLE_CCACHE=ON
#EXTRA_CMAKE_ARGS += -DSSQT_ENABLE_ASSERTION=OFF

#EXTRA_CMAKE_ARGS += -DSSQT_AUTOMATIC_QT5=ON
#EXTRA_CMAKE_ARGS += -DSSQT_QT5_STATIC_PREFIX=/opt/qt-5.9.1
#EXTRA_CMAKE_ARGS += -DSSQT_QT5_SHARED_PREFIX=/opt/Qt5.5.1/5.5/gcc_64
#EXTRA_CMAKE_ARGS += -DSSQT_QT5_SHARED_PREFIX=/usr/lib/gcc/x86_64-linux-gnu

# botan2 download/build/install, try to used the system one by default
#EXTRA_CMAKE_ARGS += -DSSQT_DBI_BOTAN2=ON
#
# libqrencode download/build/install, try to used the system one by default
#EXTRA_CMAKE_ARGS += -DSSQT_DBI_QRENCODE=ON
#
# Disable build & use shared shadowsocks core library by default
#EXTRA_CMAKE_ARGS += -DSSQT_BUILD_SHARED=ON

# Build shadowsocks TUI server/client.
#EXTRA_CMAKE_ARGS += -DSSQT_BUILD_TUI=OFF
#
# Build shadowsocks GUI server/client.
#EXTRA_CMAKE_ARGS += -DSSQT_BUILD_GUI=OFF

# Build shadowsocks Qt5 testing case.
#EXTRA_CMAKE_ARGS += -DSSQT_BUILD_TEST=OFF

#ASTYLE_PROG := /usr/bin/astyle
#ASTYLE_ARGS := --project '*.h' '*.c' '*.cpp'
#
#ASTYLE_FILES := 'source/tui/*.cpp'
#ASTYLE_FILES := 'source/core/crypto/*cpp'
#ASTYLE_FILES := 'source/core/crypto/abc?cpp'
#ASTYLE_FILES := source/core/crypto/cipher.cpp

#GIT_PROG :=
#MAKE_PROG :=

# STRIP_PROG, STRIP_ARGS
# EUSTRIP_PROG, EUSTRIP_ARGS
# DOXYGEN_PROG, DOXYGEN_ARGS
# CPPCHECK_PROG, CPPCHECK_ARGS
# => CPPCHECK17_PROG, CPPCHECK18_PROG
