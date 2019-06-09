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
#EXTRA_CMAKE_ARGS += -DSSQTL_ENABLE_CI=ON
#EXTRA_CMAKE_ARGS += -DSSQTL_ENABLE_GCOV=OFF
#EXTRA_CMAKE_ARGS += -DSSQTL_DISABLE_CCACHE=ON
#EXTRA_CMAKE_ARGS += -DSSQTL_ENABLE_ASSERTION=OFF

#EXTRA_CMAKE_ARGS += -DSSQTL_AUTOMATIC_QT5=ON
#EXTRA_CMAKE_ARGS += -DSSQTL_QT5_STATIC_PREFIX=/opt/qt-5.9.1
#EXTRA_CMAKE_ARGS += -DSSQTL_QT5_SHARED_PREFIX=/opt/Qt5.5.1/5.5/gcc_64
#EXTRA_CMAKE_ARGS += -DSSQTL_QT5_SHARED_PREFIX=/usr/lib/gcc/x86_64-linux-gnu

# Skip botan2 download/build/install, try to used the system one
#EXTRA_CMAKE_ARGS += -DSSQTL_DBI_BOTAN2=OFF
# Build shadowsocks Qt5 testing case.
#EXTRA_CMAKE_ARGS += -DSSQTL_BUILD_TEST=OFF
# Build shadowsocks TUI server/client.
#EXTRA_CMAKE_ARGS += -DSSQTL_BUILD_SSTUI=OFF
# Build shared shadowsocks core library.
#EXTRA_CMAKE_ARGS += -DSSQTL_BUILD_SHARED=OFF

#ASTYLE_PROG := /usr/bin/astyle
#ASTYLE_ARGS := --project '*.h' '*.c' '*.cpp'
#ASTYLE_FILES := source/lib/crypto/cipher.cpp

#GIT_PROG :=
#MAKE_PROG :=

# STRIP_PROG, STRIP_ARGS
# EUSTRIP_PROG, EUSTRIP_ARGS
# DOXYGEN_PROG, DOXYGEN_ARGS
# CPPCHECK_PROG, CPPCHECK_ARGS
# => CPPCHECK17_PROG, CPPCHECK18_PROG