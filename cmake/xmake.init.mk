XmakeDownloadUrl := https://github.com/gkide/xmake/releases/download

# xmake.cmake.init will do git clone
XmakeInitBy ?= CmakeGitClone
# If set if must be the values of repo tag
XmakeVersion ?= DevRepo
# If set it should be the downloading tarballs SHA256
XmakeTarballSHA256 ?=

# tar/unzip tools for decompress the xmake tarball
TAR ?= $(shell (command -v tar))
XmakeTarball := xmake-$(XmakeVersion).tar.gz
UNTGZ := $(TAR) -xvf $(XmakeTarball)
ifeq ($(TAR),)
    UNTGZ :=
    UNZIP ?= $(shell (command -v unzip))
    ifneq ($(UNZIP),)
        XmakeTarball := xmake-$(XmakeVersion).zip
        UNTGZ := $(UNZIP) $(XmakeTarball)
    endif
endif

# curl/wget tools for download the xmake tarball
WGET ?= $(shell (command -v wget))
FETCH := $(WGET) -O $(XmakeTarball)
ifeq ($(WGET),)
    CURL ?= $(shell (command -v curl))
    ifeq ($(CURL),)
        $(error do NOT found 'curl' and 'wget', xmake init STOP.)
    endif
    FETCH := $(CURL) -L -o $(XmakeTarball)
endif

# check if xmake is inited or not
XmakeInit := cmake/xmake.cmake
XmakeInit := $(shell if [ -f $(XmakeInit) ]; then echo "Done"; else echo ""; fi;)

# The target to init xmake
ifeq ($(XmakeInit),)

# The Project Root Directory
SOURCE_DIR := $(CURDIR)

BUILD_TYPE ?= Debug
GENERATOR ?= 'Unix Makefiles'
BUILD_DIR ?= $(SOURCE_DIR)/build
CMAKE_PROG ?= $(shell (command -v cmake3 || echo cmake))
GIT_PROG ?= $(shell (command -v git || echo git))
CMAKE_ARGS += -DCMAKE_BUILD_TYPE=$(BUILD_TYPE)
CMAKE_ARGS += -DGIT_PROG="$(GIT_PROG)"
CMAKE_ARGS += -DXMAKE_VERSION="$(XmakeVersion)"
CMAKE_ARGS += -DXMAKE_TARBALL_SHA256="$(XmakeTarballSHA256)"
CMAKE_ARGS += $(SOURCE_DIR)

PHONY += xmake-init
xmake-init:
ifeq ($(XmakeInitBy),CmakeGitClone)
	mkdir -p $(BUILD_DIR) && cd $(BUILD_DIR) && $(CMAKE_PROG) -G $(GENERATOR) $(CMAKE_ARGS)
	rm -rf $(BUILD_DIR)/CMakeCache.txt # cmake config maybe not correct
else
ifeq ($(UNTGZ),)
	@echo "do NOT found 'tar' and 'unzip', xmake init STOP"
endif
	CMAKE="$(PWD)/cmake"; mkdir -p $${CMAKE} && cd $${CMAKE} && $(FETCH) \
	$(XmakeDownloadUrl)/$(XmakeVersion)/$(XmakeTarball) && $(UNTGZ) && \
	mv xmake-$(XmakeVersion)/cmake/* ./ && rm -rf xmake-$(XmakeVersion)*
endif
	@echo "*********************************************************"
	@echo "* xmake init done, run make again to build the project. *"
	@echo "*********************************************************"
endif

# The xmake user-friendly template
-include cmake/xmake/xmake.mk
