# The Local configuration
# For more details see 'docs/local.mk'
-include local.mk

# The xmake user-friendly template
include cmake/xmake.init.mk

PHONY += all
all: xmake-all

PHONY += install
install: xmake-install

PHONY += clean
clean: xmake-clean

PHONY += distclean
distclean: xmake-distclean

PHONY += pre-build
pre-build: xmake-ran-top-cmake-force

PHONY += doxygen
doxygen: | xmake-doxygen

PHONY += release
release: xmake-release

PHONY += pkg-source
pkg-source: xmake-pkg-source

PHONY += help
help:
	@echo "-------------------------------------------------------------------------"
	@echo "$$ make V=1 ...      verbose of make & cmake, default is silent."
	@echo "$$ make O=path ...   build directory abs-path, default is 'build'."
	@echo "$$ make I=path ...   for set install prefix directory of abs-path."
	@echo "-------------------------------------------------------------------------"
	@echo "The <target> of the xmake Makefile are as following:"
	@echo "    all              build the project."
	@echo "    pre-build        reinit by run the top cmake."
	@echo "    clean            clean the build directory."
	@echo "    distclean        remove all generated files."
	@echo "    install          install all the targets."
	@echo "    test             build and run test."
	@echo "    release          do project release."
	@echo "    doxygen          generate doxygen mannual."
	@echo "    pkg-source       build source release package."
	@echo "    ccr-html         build HTML code coverage report."
	@echo "    astyle           format the source code by astyle."
	@echo "-------------------------------------------------------------------------"
	@echo "See 'docs/local.mk' for much more setting details."

PHONY += test
test: all
	$(XMAKE) -C $(BUILD_DIR) test

PHONY += ccr-html
ccr-html: test
ifeq ($(filter-out Dev Debug Coverage,$(BUILD_TYPE)),)
	$(XMAKE) -C $(BUILD_DIR) code.coverage-mpack
endif

AstyleRc := $(SOURCE_DIR)/.astylerc
AstyleRc := $(shell if [ -f $(AstyleRc) ]; then echo "Has"; else echo ""; fi;)
AstyleUrl:=https://gitlab.com/gkide/prebuild/astyle/raw/master/v3.1/astyle-cpp
PHONY += astyle
astyle:
ifneq ($(AstyleRc),Has) # download by wget
	@wget $(AstyleUrl) -O $(SOURCE_DIR)/.astylerc
endif
ifeq ($(ASTYLE_VERSION),3.1)
	$(ASTYLE_PROG) $(ASTYLE_ARGS)
else
	@echo "SKIP: wanted astyle-v3.1, but got astyle-v$(ASTYLE_VERSION)"
endif

.PHONY: $(PHONY)
