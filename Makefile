CC ?= gcc
CXX ?= g++
LINK ?= $(CXX)

RM=rm -rf
CP=cp -rf
MKDIR=mkdir -p
ROOT_DIR=$(shell pwd)

# Option to enable AddressSanitizer
ASAN ?= OFF
# Option to switch between Debug and Release builds
BUILD_TYPE ?= Release

detected_OS ?= $(shell uname -s)

INSTALL_DIR := install/
ifeq ($(detected_OS),Darwin)
	BUILD_DIR := build/mac
	CMAKE_TARGET=-DCMAKE_SYSTEM_NAME=$(detected_OS)
else ifeq (MINGW32,$(findstring MINGW32,$(detected_OS)))
	BUILD_DIR := build/mingw
	CMAKE_TARGET=-G "MSYS Makefiles"
else ifeq (MINGW,$(findstring MINGW,$(detected_OS)))
	BUILD_DIR := build/mingw
	CMAKE_TARGET=-G "MSYS Makefiles"
else
	BUILD_DIR := build/linux
	CMAKE_TARGET=-DCMAKE_SYSTEM_NAME=$(detected_OS)
endif

# use cmake from above if is set, otherwise cmake
ifeq ($(CMAKE),)
	CMAKE=cmake
endif

# Should we install everything into the OMBUILDDIR?
ifeq ($(OMBUILDDIR),)
	TOP_INSTALL_DIR=$(INSTALL_DIR)
	CMAKE_INSTALL_PREFIX=-DCMAKE_INSTALL_PREFIX=$(TOP_INSTALL_DIR)
else
	TOP_INSTALL_DIR=$(OMBUILDDIR)
	CMAKE_INSTALL_PREFIX=-DCMAKE_INSTALL_PREFIX=$(OMBUILDDIR)
endif

ifneq ($(CROSS_TRIPLE),)
  CROSS_TRIPLE_DASH = $(CROSS_TRIPLE)-
  HOST_CROSS_TRIPLE = "--host=$(CROSS_TRIPLE)"
  AR ?= $(CROSS_TRIPLE)-ar
  RANLIB ?= $(CROSS_TRIPLE)-ranlib
  ifeq (MINGW,$(findstring MINGW,$(detected_OS)))
    CMAKE_TARGET=-G "Unix Makefiles" -DCMAKE_SYSTEM_NAME=Windows -DCMAKE_RC_COMPILER=$(CROSS_TRIPLE)-windres
  endif
  ifeq ($(detected_OS),Darwin)
    EXTRA_CMAKE+=-DCMAKE_INSTALL_NAME_TOOL=$(CROSS_TRIPLE)-install_name_tool
  endif
  DISABLE_RUN_OMSIMULATOR_VERSION ?= 1
endif

.PHONY: OMSimulator OMSimulatorCore config-OMSimulator distclean testsuite doc doc-html doc-doxygen pip

OMSimulator:
	@echo OS: $(detected_OS)
	@echo "# make OMSimulator"
	@echo
	@$(MAKE) OMSimulatorCore
	test ! -z "$(DISABLE_RUN_OMSIMULATOR_VERSION)" || $(TOP_INSTALL_DIR)/bin/OMSimulator --version

OMSimulatorCore:
	@echo
	@echo "# make OMSimulatorCore"
	@echo
	@$(MAKE) -C $(BUILD_DIR) install

pip:
	@echo
	@echo "# make pip"
	@echo
	cd src/pip/install/ && python3 setup.py sdist
	@echo
	@echo "# All local packages:"
	@ls src/pip/install/dist/ -Art
	@echo
	@echo "# Run the following command to upload the package"
	@echo "> twine upload src/pip/install/dist/$(shell ls src/pip/install/dist/ -Art | tail -n 1)"

config-OMSimulator: $(BUILD_DIR)/Makefile
$(BUILD_DIR)/Makefile: CMakeLists.txt
	@echo
	@echo "# config OMSimulator"
	@echo
	$(MKDIR) $(BUILD_DIR)
	$(CMAKE) -S . -B $(BUILD_DIR) $(CMAKE_TARGET) -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON -DASAN:BOOL=$(ASAN) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) $(CMAKE_INSTALL_PREFIX) $(HOST_SHORT) $(EXTRA_CMAKE) $(CMAKE_STATIC)

distclean:
	@echo
	@echo "# make distclean"
	@echo
	$(RM) $(BUILD_DIR)
	$(RM) $(INSTALL_DIR)

testsuite:
	@echo
	@echo "# run testsuite"
	@echo
	@$(MAKE) -C testsuite all

doc:
	@$(MAKE) -C doc/UsersGuide latexpdf
	@$(MKDIR) $(INSTALL_DIR)/doc
	@cp doc/UsersGuide/build/latex/OMSimulator.pdf $(INSTALL_DIR)/doc

doc-html:
	@$(MAKE) -C doc/UsersGuide html
	@$(MKDIR) $(INSTALL_DIR)/doc
	@$(RM) $(INSTALL_DIR)/doc/html
	@$(CP) doc/UsersGuide/build/html/ $(INSTALL_DIR)/doc/html

doc-doxygen:
	@$(RM) $(INSTALL_DIR)/doc/OMSimulatorLib
	@$(MKDIR) $(INSTALL_DIR)/doc/OMSimulatorLib
	@$(MAKE) -C doc/dev/OMSimulatorLib doc-doxygen
	@$(CP) doc/dev/OMSimulatorLib/html/* $(INSTALL_DIR)/doc/OMSimulatorLib/
	@$(MAKE) -C doc/dev/OMSimulatorLib clean

gitclean:
	git submodule foreach --recursive 'git clean -fdx'
	git clean -fdx -e .project
