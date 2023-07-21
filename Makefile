CC ?= gcc
CXX ?= g++
LINK ?= $(CXX)

RM=rm -rf
CP=cp -rf
MKDIR=mkdir -p
ROOT_DIR=$(shell pwd)

# Option to build LIBXML2 as part of the 3rdParty projects
LIBXML2 ?= $(OMTLM)
# Option to enable OMTLM
OMTLM ?= OFF
# Option to enable AddressSanitizer
ASAN ?= OFF
# Statically link dependencies as much as possible
STATIC ?= OFF
# Option to switch between Debug and Release builds
BUILD_TYPE ?= Release

detected_OS ?= $(shell uname -s)

ifeq ($(detected_OS),Darwin)
	BUILD_DIR := build/mac
	INSTALL_DIR := install/mac
	CMAKE_TARGET=-DCMAKE_SYSTEM_NAME=$(detected_OS)
	LIBXML2 := OFF
	OMTLM := OFF
	export ABI := MAC64
	FEXT=.dylib
	CMAKE_FPIC=-DCMAKE_C_FLAGS="-fPIC"
else ifeq (MINGW32,$(findstring MINGW32,$(detected_OS)))
	BUILD_DIR := build/mingw
	INSTALL_DIR := install/mingw
	CMAKE_TARGET=-G "MSYS Makefiles"
	LIBXML2 := OFF
	export ABI := WINDOWS32
	FEXT=.dll
	EEXT=.exe
	ifeq (clang,$(findstring clang,$(CC)))
		DISABLE_SHARED = --disable-shared
	endif
else ifeq (MINGW,$(findstring MINGW,$(detected_OS)))
	BUILD_DIR := build/mingw
	INSTALL_DIR := install/mingw
	CMAKE_TARGET=-G "MSYS Makefiles"
	LIBXML2 := OFF
	export ABI := WINDOWS64
	FEXT=.dll
	EEXT=.exe
	ifeq (clang,$(findstring clang,$(CC)))
		DISABLE_SHARED = --disable-shared
	endif
else
	BUILD_DIR := build/linux
	INSTALL_DIR := install/linux
	CMAKE_TARGET=-DCMAKE_SYSTEM_NAME=$(detected_OS)
# if empty is LINUX64, else LINUX32
ifneq (,$(filter i386% i486% i586% i686%,$(host_short)))
	export ABI := LINUX32
else
	export ABI := LINUX64
endif
	FEXT=.so
	CMAKE_FPIC=-DCMAKE_C_FLAGS="-fPIC"
endif

ifeq ($(STATIC),ON)
  # Do not use -DBoost_USE_STATIC_LIBS=ON; it messes up -static in alpine/musl
  CMAKE_STATIC=-DBUILD_SHARED=OFF
endif

# use cmake from above if is set, otherwise cmake
ifeq ($(CMAKE),)
	CMAKE=cmake
endif

# Should we install everything into the OMBUILDDIR?
ifeq ($(OMBUILDDIR),)
	TOP_INSTALL_DIR=$(INSTALL_DIR)
	CMAKE_INSTALL_PREFIX=-DCMAKE_INSTALL_PREFIX=$(TOP_INSTALL_DIR)
	HOST_SHORT=
else
	TOP_INSTALL_DIR=$(OMBUILDDIR)
	CMAKE_INSTALL_PREFIX=-DCMAKE_INSTALL_PREFIX=$(OMBUILDDIR)
	ifeq ($(host_short),)
		HOST_SHORT=-DHOST_SHORT=
	else
		HOST_SHORT_OMC=$(host_short)/omc
		HOST_SHORT=-DHOST_SHORT=$(HOST_SHORT_OMC)
	endif
endif

ifeq ($(detected_OS),Darwin)
	EXTRA_CMAKE=-DCMAKE_MACOSX_RPATH=ON -DCMAKE_INSTALL_RPATH="`pwd`/$(TOP_INSTALL_DIR)/lib/$(HOST_SHORT_OMC)"
endif

ifneq ($(CROSS_TRIPLE),)
  OMTLM := OFF
  LIBXML2 := OFF
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

ifeq ($(BOOST_ROOT),)
else
	CMAKE_BOOST_ROOT="-DBOOST_ROOT=$(BOOST_ROOT)"
endif

.PHONY: OMSimulator OMSimulatorCore config-OMSimulator config-xerces config-3rdParty distclean testsuite doc doc-html doc-doxygen OMTLMSimulator OMTLMSimulatorClean RegEx pip

OMSimulator:
	@echo OS: $(detected_OS)
	@echo TLM: $(OMTLM)
	@echo LIBXML2: $(LIBXML2)
	@echo "# make OMSimulator"
	@echo
	@$(MAKE) CC="$(CC)" CXX="$(CXX)" OMTLMSimulator
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

ifeq ($(OMTLM),ON)
OMTLMSimulator: RegEx
	@echo
	@echo "# make OMTLMSimulator"
	@echo
	@echo $(ABI)
	$(MAKE) -C OMTLMSimulator omtlmlib
	test ! `uname` != Darwin || $(MAKE) -C OMTLMSimulator/FMIWrapper install
	@$(MKDIR) $(TOP_INSTALL_DIR)/lib/$(HOST_SHORT_OMC)
	@$(MKDIR) $(TOP_INSTALL_DIR)/bin
	test ! "$(FEXT)" != ".dll" || cp OMTLMSimulator/bin/libomtlmsimulator$(FEXT) $(TOP_INSTALL_DIR)/lib/$(HOST_SHORT_OMC)
	test ! "$(detected_OS)" = Darwin || ($(CROSS_TRIPLE_DASH)install_name_tool -id "@rpath/libomtlmsimulator$(FEXT)" $(TOP_INSTALL_DIR)/lib/$(HOST_SHORT_OMC)/libomtlmsimulator$(FEXT))
	test ! "$(FEXT)" = ".dll" || cp OMTLMSimulator/bin/libomtlmsimulator$(FEXT) $(TOP_INSTALL_DIR)/bin/
	test ! `uname` != Darwin || cp OMTLMSimulator/bin/FMIWrapper $(TOP_INSTALL_DIR)/bin/
	test ! `uname` != Darwin || cp OMTLMSimulator/bin/StartTLMFmiWrapper $(TOP_INSTALL_DIR)/bin/

OMTLMSimulatorStandalone: RegEx
	@echo
	@echo "# make OMTLMSimulator Standalone"
	@echo
	@echo $(ABI)
	@$(MAKE) -C OMTLMSimulator install
else
OMTLMSimulator:
OMTLMSimulatorStandalone:
endif

OMTLMSimulatorClean:
	@echo
	@echo "# clean OMTLMSimulator"
	@echo
	@$(MAKE) -C OMTLMSimulator clean

# build our RegEx executable that will tell us if we need to use std::regex or boost::regex
RegEx: 3rdParty/RegEx/OMSRegEx$(EEXT)
3rdParty/RegEx/OMSRegEx$(EEXT): 3rdParty/RegEx/RegEx.h 3rdParty/RegEx/OMSRegEx.cpp
	$(MAKE) -C 3rdParty/RegEx

3rdParty/README.md:
	@echo "Please checkout the 3rdParty submodule, e.g. using \"git submodule update --init 3rdParty\", and try again."
	@false

config-3rdParty: 3rdParty/README.md

config-OMSimulator: $(BUILD_DIR)/Makefile
$(BUILD_DIR)/Makefile: RegEx CMakeLists.txt
	@echo
	@echo "# config OMSimulator"
	@echo
	$(eval STD_REGEX := $(shell 3rdParty/RegEx/OMSRegEx$(EEXT)))
	$(MKDIR) $(BUILD_DIR)
	$(CMAKE) -S . -B $(BUILD_DIR) $(CMAKE_TARGET) -DABI=$(ABI) -DSTD_REGEX=$(STD_REGEX) -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON -DOMTLM:BOOL=$(OMTLM) -DASAN:BOOL=$(ASAN) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) $(CMAKE_BOOST_ROOT) $(CMAKE_INSTALL_PREFIX) $(HOST_SHORT) $(EXTRA_CMAKE) $(CMAKE_STATIC)

config-xerces: 3rdParty/xerces/$(INSTALL_DIR)/lib/libxerces-c.a
3rdParty/xerces/$(INSTALL_DIR)/lib/libxerces-c.a: 3rdParty/xerces/$(BUILD_DIR)/Makefile
	$(MAKE) -C 3rdParty/xerces/$(BUILD_DIR)/ install
3rdParty/xerces/$(BUILD_DIR)/Makefile: 3rdParty/xerces/CMakeLists.txt
	@echo
	@echo "# config xerces"
	@echo
	$(MKDIR) 3rdParty/xerces/$(BUILD_DIR)
	cd 3rdParty/xerces/$(BUILD_DIR) && $(CMAKE) $(CMAKE_TARGET) ../.. -DCMAKE_INSTALL_PREFIX=../../$(INSTALL_DIR) -DBUILD_SHARED_LIBS:BOOL=OFF

distclean:
	@echo
	@echo "# make distclean"
	@echo
	@$(MAKE) OMTLMSimulatorClean
	$(RM) $(BUILD_DIR)
	$(RM) $(INSTALL_DIR)
	$(RM) 3rdParty/RegEx/OMSRegEx$(EEXT)
	$(RM) 3rdParty/xerces/$(BUILD_DIR)
	$(RM) 3rdParty/xerces/$(INSTALL_DIR)

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
