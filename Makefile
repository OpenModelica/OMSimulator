
CC ?= gcc
CXX ?= g++
LINK ?= $(CXX)

RM=rm -rf
CP=cp -rf
MKDIR=mkdir -p
ROOT_DIR=$(shell pwd)

# Option to build Ceres-Solver and its dependencies as part of the 3rdParty projects
CERES ?= ON
# Option to build LIBXML2 as part of the 3rdParty projects
LIBXML2 ?= $(OMTLM)
# Option to enable the OMSysIdent parameter estimation module within the OMSimulator project
OMSYSIDENT ?= ON
# Option to enable OMTLM
OMTLM ?= ON
# Option to enable AddressSanitizer
ASAN ?= OFF
# Option to switch between Debug and Release builds
BUILD_TYPE ?= Release

detected_OS ?= $(shell uname -s)

ifeq ($(detected_OS),Darwin)
	BUILD_DIR := build/mac
	INSTALL_DIR := install/mac
	CMAKE_TARGET=-DCMAKE_SYSTEM_NAME=$(detected_OS)
	# Darwin detected => NO SUPPORT FOR BUILDING CERES SOLVER (yet)
	CERES := OFF
	LIBXML2 := OFF
	OMSYSIDENT := OFF
	export ABI := MAC64
	FEXT=.dylib
	CMAKE_FPIC=-DCMAKE_C_FLAGS="-fPIC"
else ifeq (MINGW32,$(findstring MINGW32,$(detected_OS)))
	BUILD_DIR := build/mingw
	INSTALL_DIR := install/mingw
	CMAKE_TARGET=-G "MSYS Makefiles"
	FMIL_FLAGS?=-DFMILIB_FMI_PLATFORM=win32
	# MINGW detected => NO SUPPORT FOR BUILDING CERES SOLVER (yet)
	CERES := OFF
	OMSYSIDENT := OFF
	OMFIT := OFF
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
	FMIL_FLAGS?=-DFMILIB_FMI_PLATFORM=win64
	# MINGW detected => NO SUPPORT FOR BUILDING CERES SOLVER (yet)
	CERES := OFF
	OMSYSIDENT := OFF
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

# use cmake from above if is set, otherwise cmake
ifeq ($(CMAKE),)
	CMAKE=CC="$(CC)" CXX="$(CXX)" CFLAGS="$(CFLAGS)" CPPFLAGS="$(CPPFLAGS)" CXXFLAGS="$(CXXFLAGS)" cmake
endif

# Should we install everything into the OMBUILDDIR?
ifeq ($(OMBUILDDIR),)
	TOP_INSTALL_DIR=$(INSTALL_DIR)
	CMAKE_INSTALL_PREFIX=
	HOST_SHORT=
else
	TOP_INSTALL_DIR=$(OMBUILDDIR)
	CMAKE_INSTALL_PREFIX=-DCMAKE_INSTALL_PREFIX=$(OMBUILDDIR)
ifeq ($(host_short),)
	HOST_SHORT=
else
	HOST_SHORT_OMC=$(host_short)/omc
	HOST_SHORT=-DHOST_SHORT=$(HOST_SHORT_OMC)
endif
endif

ifeq ($(detected_OS),Darwin)
	EXTRA_CMAKE=-DCMAKE_MACOSX_RPATH=ON -DCMAKE_INSTALL_RPATH="`pwd`/$(TOP_INSTALL_DIR)/lib/$(HOST_SHORT_OMC)"
endif

ifeq ($(CROSS_TRIPLE),)
  CMAKE=cmake $(CMAKE_TARGET)
else
  LUA_EXTRA_FLAGS=CC=$(CC) CXX=$(CXX) RANLIB=$(CROSS_TRIPLE)-ranlib detected_OS=$(detected_OS)
  OMTLM := OFF
  CERES := OFF
  OMSYSIDENT := OFF
  LIBXML2 := OFF
  CROSS_TRIPLE_DASH = $(CROSS_TRIPLE)-
  HOST_CROSS_TRIPLE = "--host=$(CROSS_TRIPLE)"
  FMIL_FLAGS ?=
  AR ?= $(CROSS_TRIPLE)-ar
  RANLIB ?= $(CROSS_TRIPLE)-ranlib
  CMAKE=cmake $(CMAKE_TARGET)
  ifeq (MINGW,$(findstring MINGW,$(detected_OS)))
    CMAKE_TARGET=-G "Unix Makefiles" -DCMAKE_SYSTEM_NAME=Windows -DCMAKE_RC_COMPILER=$(CROSS_TRIPLE)-windres
  endif
  DISABLE_RUN_OMSIMULATOR_VERSION ?= 1
endif

ifeq ($(BOOST_ROOT),)
else
	CMAKE_BOOST_ROOT="-DBOOST_ROOT=$(BOOST_ROOT)"
endif

.PHONY: OMSimulator OMSimulatorCore config-OMSimulator config-fmil config-lua config-cvode config-kinsol config-gflags config-glog config-ceres-solver config-3rdParty distclean testsuite doc doc-html doc-doxygen OMTLMSimulator OMTLMSimulatorClean RegEx

OMSimulator:
	@echo OS: $(detected_OS)
	@echo TLM: $(OMTLM)
	@echo CERES: $(CERES)
	@echo LIBXML2: $(LIBXML2)
	@echo "# make OMSimulator"
	@echo
	@$(MAKE) CC="$(CXX)" CXX="$(CXX)" OMTLMSimulator
	@$(MAKE) OMSimulatorCore
	test ! -z "$(DISABLE_RUN_OMSIMULATOR_VERSION)" || $(TOP_INSTALL_DIR)/bin/OMSimulator --version

OMSimulatorCore:
	@echo
	@echo "# make OMSimulatorCore"
	@echo
	@$(MAKE) -C $(BUILD_DIR) install
	test ! "$(detected_OS)" = Darwin || ($(CROSS_TRIPLE_DASH)install_name_tool -add_rpath "@loader_path/../../../../lib/$(HOST_SHORT_OMC)" $(TOP_INSTALL_DIR)/bin/OMSimulator)
	test ! "$(detected_OS)" = Darwin || ($(CROSS_TRIPLE_DASH)install_name_tool -add_rpath "@loader_path/../lib/$(HOST_SHORT_OMC)" $(TOP_INSTALL_DIR)/bin/OMSimulator)

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
	cp OMTLMSimulator/bin/libomtlmsimulator$(FEXT) $(TOP_INSTALL_DIR)/lib/$(HOST_SHORT_OMC)
	test ! "$(detected_OS)" = Darwin || ($(CROSS_TRIPLE_DASH)install_name_tool -id "@rpath/libomtlmsimulator$(FEXT)" $(TOP_INSTALL_DIR)/lib/$(HOST_SHORT_OMC)/libomtlmsimulator$(FEXT))
	test ! "$(FEXT)" = ".dll" || cp OMTLMSimulator/bin/libomtlmsimulator$(FEXT) $(TOP_INSTALL_DIR)/bin/
	test ! `uname` != Darwin || cp OMTLMSimulator/bin/FMIWrapper $(TOP_INSTALL_DIR)/bin/
	test ! `uname` != Darwin || cp OMTLMSimulator/bin/StartTLMFmiWrapper $(TOP_INSTALL_DIR)/bin/

OMTLMSimulatorStandalone: RegEx config-fmil
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

config-3rdParty: config-fmil config-lua config-cvode config-kinsol config-ceres-solver config-libxml2

config-OMSimulator: RegEx
	@echo
	@echo "# config OMSimulator"
	@echo
	$(eval STD_REGEX := $(shell 3rdParty/RegEx/OMSRegEx$(EEXT)))
	$(RM) $(BUILD_DIR)
	$(MKDIR) $(BUILD_DIR)
	cd $(BUILD_DIR) && $(CMAKE) $(CMAKE_TARGET) ../.. -DABI=$(ABI) -DSTD_REGEX=$(STD_REGEX) -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON -DOMSYSIDENT:BOOL=$(OMSYSIDENT) -DOMTLM:BOOL=$(OMTLM) -DASAN:BOOL=$(ASAN) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) $(CMAKE_BOOST_ROOT) $(CMAKE_INSTALL_PREFIX) $(HOST_SHORT) $(EXTRA_CMAKE)

config-fmil:
	@echo
	@echo "# config fmil"
	@echo
	$(RM) 3rdParty/FMIL/$(BUILD_DIR)
	$(RM) 3rdParty/FMIL/$(INSTALL_DIR)
	$(MKDIR) 3rdParty/FMIL/$(BUILD_DIR)
	cd 3rdParty/FMIL/$(BUILD_DIR) && $(CMAKE) $(CMAKE_TARGET) ../.. -DFMILIB_INSTALL_PREFIX=../../$(INSTALL_DIR) -DFMILIB_BUILD_TESTS:BOOL=0 -DFMILIB_GENERATE_DOXYGEN_DOC:BOOL=0 -DFMILIB_BUILD_STATIC_LIB:BOOL=1 -DFMILIB_BUILD_SHARED_LIB:BOOL=0 -DBUILD_TESTING:BOOL=0 -DFMILIB_BUILD_BEFORE_TESTS:BOOL=0 $(FMIL_FLAGS) && $(MAKE) install

config-lua:
	@echo
	@echo "# config Lua"
	@echo
	$(RM) 3rdParty/Lua/$(INSTALL_DIR)
	$(MAKE) -C 3rdParty/Lua $(LUA_EXTRA_FLAGS)

config-cvode:
	@echo
	@echo "# config cvode"
	@echo
	$(RM) 3rdParty/cvode/$(BUILD_DIR)
	$(RM) 3rdParty/cvode/$(INSTALL_DIR)
	$(MKDIR) 3rdParty/cvode/$(BUILD_DIR)
	cd 3rdParty/cvode/$(BUILD_DIR) && $(CMAKE) $(CMAKE_TARGET) ../.. -DCMAKE_INSTALL_PREFIX=../../$(INSTALL_DIR) -DEXAMPLES_ENABLE:BOOL=0 -DBUILD_SHARED_LIBS:BOOL=0 $(CMAKE_FPIC) && $(MAKE) install

config-kinsol:
	@echo
	@echo "# config kinsol"
	@echo
	$(RM) 3rdParty/kinsol/$(BUILD_DIR)
	$(RM) 3rdParty/kinsol/$(INSTALL_DIR)
	$(MKDIR) 3rdParty/kinsol/$(BUILD_DIR)
	cd 3rdParty/kinsol/$(BUILD_DIR) && $(CMAKE) $(CMAKE_TARGET) ../.. -DCMAKE_INSTALL_PREFIX=../../$(INSTALL_DIR) -DEXAMPLES_ENABLE:BOOL=0 -DBUILD_SHARED_LIBS:BOOL=0 $(CMAKE_FPIC) && $(MAKE) install

config-gflags:
	@echo
	@echo "# config gflags"
	@echo
	$(RM) 3rdParty/gflags/$(BUILD_DIR)
	$(RM) 3rdParty/gflags/$(INSTALL_DIR)
	$(MKDIR) 3rdParty/gflags/$(BUILD_DIR)
	cd 3rdParty/gflags/$(BUILD_DIR) && $(CMAKE) $(CMAKE_TARGET) ../../gflags -DCMAKE_INSTALL_PREFIX=../../$(INSTALL_DIR) -DBUILD_SHARED_LIBS:BOOL=OFF -DBUILD_TESTING:BOOL=OFF $(CMAKE_FPIC) -DCMAKE_CXX_FLAGS="-fPIC" -DCMAKE_BUILD_TYPE="Release" && $(MAKE) install

config-glog: config-gflags
	@echo
	@echo "# config glog"
	@echo
	$(RM) 3rdParty/glog/$(BUILD_DIR)
	$(RM) 3rdParty/glog/$(INSTALL_DIR)
	$(MKDIR) 3rdParty/glog/$(BUILD_DIR)
	cd 3rdParty/glog/$(BUILD_DIR) && $(CMAKE) $(CMAKE_TARGET) ../../glog -DCMAKE_INSTALL_PREFIX=../../$(INSTALL_DIR) -DBUILD_SHARED_LIBS:BOOL=OFF -DBUILD_TESTING:BOOL=OFF $(CMAKE_FPIC) -DCMAKE_CXX_FLAGS="-fPIC" -DCMAKE_BUILD_TYPE="Release" && $(MAKE) install

ifeq ($(CERES),OFF)
config-ceres-solver:
	@echo
	@echo "# CERES=OFF => Skipping build of 3rdParty library Ceres-Solver. Ceres-Solver is a dependency of the (optional) parameter estimation module."
	@echo
else
config-ceres-solver: config-glog
	@echo
	@echo "# config ceres-solver"
	@echo
	$(RM) 3rdParty/ceres-solver/$(BUILD_DIR)
	$(RM) 3rdParty/ceres- solver/$(INSTALL_DIR)
	$(MKDIR) 3rdParty/ceres-solver/$(BUILD_DIR)
	cd 3rdParty/ceres-solver/$(BUILD_DIR) && $(CMAKE) $(CMAKE_TARGET) ../../ceres-solver -DCMAKE_INSTALL_PREFIX=../../$(INSTALL_DIR) -DCXX11:BOOL=ON -DEXPORT_BUILD_DIR=ON -DEIGEN_INCLUDE_DIR_HINTS=../../eigen/eigen -DBUILD_EXAMPLES:BOOL=OFF -DBUILD_TESTING:BOOL=OFF -DCMAKE_BUILD_TYPE="Release" && $(MAKE) install
endif

ifeq ($(LIBXML2),OFF)
config-libxml2:
	@echo
	@echo "# LIBXML2=OFF => Skipping build of 3rdParty library libxml2 (must be installed on system instead)."
	@echo
else
config-libxml2:
	@echo
	@echo "# config libxml2"
	@echo
	$(MKDIR) 3rdParty/libxml2/$(INSTALL_DIR)
	cd 3rdParty/libxml2 && $(FPIC) ./autogen.sh --prefix="$(ROOT_DIR)/3rdParty/libxml2/$(INSTALL_DIR)" $(DISABLE_SHARED) --without-python --without-zlib --without-lzma $(HOST_CROSS_TRIPLE) && $(MAKE) && $(MAKE) install
endif

distclean:
	@echo
	@echo "# make distclean"
	@echo
	@$(MAKE) OMTLMSimulatorClean
	$(RM) $(BUILD_DIR)
	$(RM) $(INSTALL_DIR)
	$(RM) 3rdParty/FMIL/$(BUILD_DIR)
	$(RM) 3rdParty/FMIL/$(INSTALL_DIR)
	@$(MAKE) -C 3rdParty/Lua distclean
	$(RM) 3rdParty/RegEx/OMSRegEx$(EEXT)
	$(RM) 3rdParty/cvode/$(BUILD_DIR)
	$(RM) 3rdParty/cvode/$(INSTALL_DIR)
	$(RM) 3rdParty/kinsol/$(BUILD_DIR)
	$(RM) 3rdParty/kinsol/$(INSTALL_DIR)
	$(RM) 3rdParty/gflags/$(BUILD_DIR)
	$(RM) 3rdParty/gflags/$(INSTALL_DIR)
	$(RM) 3rdParty/glog/$(BUILD_DIR)
	$(RM) 3rdParty/glog/$(INSTALL_DIR)
	$(RM) 3rdParty/ceres-solver/$(BUILD_DIR)
	$(RM) 3rdParty/ceres-solver/$(INSTALL_DIR)

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
	@$(MAKE) -C doc/dev/OMSimulatorLib OMSimulatorLib.png
	@$(CP) doc/dev/OMSimulatorLib/OMSimulatorLib.png $(INSTALL_DIR)/doc/OMSimulatorLib/
	@$(MAKE) -C doc/dev/OMSimulatorLib doc-doxygen
	@$(CP) doc/dev/OMSimulatorLib/html/* $(INSTALL_DIR)/doc/OMSimulatorLib/
	@$(MAKE) -C doc/dev/OMSimulatorLib clean

gitclean:
	git submodule foreach --recursive 'git clean -fdx'
	git clean -fdx -e .project

