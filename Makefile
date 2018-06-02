RM=rm -rf
CP=cp -rf
MKDIR=mkdir -p
ROOT_DIR=$(shell pwd)

# Option to build Ceres-Solver and its dependencies as part of the 3rdParty projects
CERES ?= ON
# Option to build LIBXML2 as part of the 3rdParty projects
LIBXML2 ?= ON
# Option to enable the OMSysIdent parameter estimation module within the OMSimulator project
OMSYSIDENT ?= ON
# Option to enable AddressSanitizer
ASAN ?= OFF

detected_OS := $(shell uname -s)
ifeq ($(detected_OS),Darwin)
	BUILD_DIR := build/mac
	INSTALL_DIR := install/mac
	# MINGW detected => NO SUPPORT FOR BUILDING CERES SOLVER (yet)
	CERES := OFF
	LIBXML2 := OFF
	OMSYSIDENT := OFF
	export ABI := MAC64
	FEXT=.dylib
else ifeq (MINGW32,$(findstring MINGW32,$(detected_OS)))
	BUILD_DIR := build/mingw
	INSTALL_DIR := install/mingw
	CMAKE_TARGET=-G "MSYS Makefiles"
	FMIL_FLAGS=-DFMILIB_FMI_PLATFORM=win32
	# MINGW detected => NO SUPPORT FOR BUILDING CERES SOLVER  (yet)
	CERES := OFF
	OMSYSIDENT := OFF
	OMFIT := OFF
	export ABI := WINDOWS32
	FEXT=.dll
else ifeq (MINGW,$(findstring MINGW,$(detected_OS)))
	BUILD_DIR := build/mingw
	INSTALL_DIR := install/mingw
	CMAKE_TARGET=-G "MSYS Makefiles"
	FMIL_FLAGS=-DFMILIB_FMI_PLATFORM=win64
	# MINGW detected => NO SUPPORT FOR BUILDING CERES SOLVER  (yet)
	CERES := OFF
	OMSYSIDENT := OFF
	export ABI := WINDOWS64
	FEXT=.dll
else
	BUILD_DIR := build/linux
	INSTALL_DIR := install/linux
	export ABI := LINUX64
	FEXT=.so
endif

.PHONY: OMSimulator OMSimulatorCore config-OMSimulator config-fmil config-lua config-cvode config-kinsol config-gflags config-glog config-ceres-solver config-3rdParty distclean testsuite doc doc-html doc-doxygen OMTLMSimulator

OMSimulator:
	@echo
	@echo "# make OMSimulator"
	@echo
	@$(MAKE) OMTLMSimulator
	@$(MAKE) OMSimulatorCore
	$(INSTALL_DIR)/bin/OMSimulator --version

OMSimulatorCore:
	@echo
	@echo "# make OMSimulatorCore"
	@echo
	@$(MAKE) -C $(BUILD_DIR) install
	test ! `uname` = Darwin || (install_name_tool -change MAC64/libomtlmsimulator.dylib "@loader_path/libomtlmsimulator.dylib" $(INSTALL_DIR)/bin/OMSimulator)

OMTLMSimulator:
	@echo
	@echo "# make OMTLMSimulator"
	@echo
	@echo $(ABI)
	@$(MAKE) -C OMTLMSimulator omtlmlib
	test ! `uname` != Darwin || $(MAKE) -C OMTLMSimulator/FMIWrapper install
	@$(MKDIR) $(INSTALL_DIR)/lib
	@$(MKDIR) $(INSTALL_DIR)/bin
	cp OMTLMSimulator/bin/libomtlmsimulator$(FEXT) $(INSTALL_DIR)/lib/
	cp OMTLMSimulator/bin/libomtlmsimulator$(FEXT) $(INSTALL_DIR)/bin/
	test ! `uname` != Darwin || cp OMTLMSimulator/bin/FMIWrapper $(INSTALL_DIR)/bin/
	test ! `uname` != Darwin || cp OMTLMSimulator/bin/StartTLMFmiWrapper $(INSTALL_DIR)/bin/
	test ! `uname` != Darwin || cp OMTLMSimulator/bin/libfmilib_shared$(FEXT) $(INSTALL_DIR)/lib/

config-3rdParty: config-fmil config-lua config-cvode config-kinsol config-gflags config-glog config-ceres-solver config-libxml2

config-OMSimulator:
	@echo
	@echo "# config OMSimulator"
	@echo
	$(RM) $(BUILD_DIR)
	$(MKDIR) $(BUILD_DIR)
	cd $(BUILD_DIR) && cmake $(CMAKE_TARGET) ../.. -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON -DOMSYSIDENT:BOOL=$(OMSYSIDENT) -DASAN:BOOL=$(ASAN)

config-fmil:
	@echo
	@echo "# config fmil"
	@echo
	$(RM) 3rdParty/FMIL/$(BUILD_DIR)
	$(RM) 3rdParty/FMIL/$(INSTALL_DIR)
	$(MKDIR) 3rdParty/FMIL/$(BUILD_DIR)
	cd 3rdParty/FMIL/$(BUILD_DIR) && cmake $(CMAKE_TARGET) ../.. -DFMILIB_INSTALL_PREFIX=../../$(INSTALL_DIR) -DFMILIB_BUILD_TESTS:BOOL=0 -DFMILIB_GENERATE_DOXYGEN_DOC:BOOL=0 -DFMILIB_BUILD_STATIC_LIB:BOOL=1 -DFMILIB_BUILD_SHARED_LIB:BOOL=0 -DBUILD_TESTING:BOOL=0 -DFMILIB_BUILD_BEFORE_TESTS:BOOL=0 $(FMIL_FLAGS) && $(MAKE) install

config-lua:
	@echo
	@echo "# config Lua"
	@echo
	$(RM) 3rdParty/Lua/$(INSTALL_DIR)
	@$(MAKE) -C 3rdParty/Lua

config-cvode:
	@echo
	@echo "# config cvode"
	@echo
	$(RM) 3rdParty/cvode/$(BUILD_DIR)
	$(RM) 3rdParty/cvode/$(INSTALL_DIR)
	$(MKDIR) 3rdParty/cvode/$(BUILD_DIR)
	cd 3rdParty/cvode/$(BUILD_DIR) && cmake $(CMAKE_TARGET) ../.. -DCMAKE_INSTALL_PREFIX=../../$(INSTALL_DIR) -DEXAMPLES_ENABLE:BOOL=0 -DBUILD_SHARED_LIBS:BOOL=0 -DCMAKE_C_FLAGS="-fPIC" && $(MAKE) install

config-kinsol:
	@echo
	@echo "# config kinsol"
	@echo
	$(RM) 3rdParty/kinsol/$(BUILD_DIR)
	$(RM) 3rdParty/kinsol/$(INSTALL_DIR)
	$(MKDIR) 3rdParty/kinsol/$(BUILD_DIR)
	cd 3rdParty/kinsol/$(BUILD_DIR) && cmake $(CMAKE_TARGET) ../.. -DCMAKE_INSTALL_PREFIX=../../$(INSTALL_DIR) -DEXAMPLES_ENABLE:BOOL=0 -DBUILD_SHARED_LIBS:BOOL=0 -DCMAKE_C_FLAGS="-fPIC" && $(MAKE) install

config-gflags:
	@echo
	@echo "# config gflags"
	@echo
	$(RM) 3rdParty/gflags/$(BUILD_DIR)
	$(RM) 3rdParty/gflags/$(INSTALL_DIR)
	$(MKDIR) 3rdParty/gflags/$(BUILD_DIR)
	cd 3rdParty/gflags/$(BUILD_DIR) && cmake $(CMAKE_TARGET) ../../gflags -DCMAKE_INSTALL_PREFIX=../../$(INSTALL_DIR) -DBUILD_SHARED_LIBS:BOOL=OFF -DBUILD_TESTING:BOOL=OFF -DCMAKE_C_FLAGS="-fPIC" -DCMAKE_CXX_FLAGS="-fPIC" -DCMAKE_BUILD_TYPE="Release" && $(MAKE) install

config-glog: config-gflags
	@echo
	@echo "# config glog"
	@echo
	$(RM) 3rdParty/glog/$(BUILD_DIR)
	$(RM) 3rdParty/glog/$(INSTALL_DIR)
	$(MKDIR) 3rdParty/glog/$(BUILD_DIR)
	cd 3rdParty/glog/$(BUILD_DIR) && cmake $(CMAKE_TARGET) ../../glog -DCMAKE_INSTALL_PREFIX=../../$(INSTALL_DIR) -DBUILD_SHARED_LIBS:BOOL=OFF -DBUILD_TESTING:BOOL=OFF -DCMAKE_C_FLAGS="-fPIC" -DCMAKE_CXX_FLAGS="-fPIC" -DCMAKE_BUILD_TYPE="Release" && $(MAKE) install

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
	cd 3rdParty/ceres-solver/$(BUILD_DIR) && cmake $(CMAKE_TARGET) ../../ceres-solver -DCMAKE_INSTALL_PREFIX=../../$(INSTALL_DIR) -DCXX11:BOOL=ON -DEXPORT_BUILD_DIR=ON -DEIGEN_INCLUDE_DIR_HINTS=../../eigen/eigen -DBUILD_EXAMPLES:BOOL=OFF -DBUILD_TESTING:BOOL=OFF -DCMAKE_BUILD_TYPE="Release" && $(MAKE) install
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
	cd 3rdParty/libxml2 && ./autogen.sh --prefix="$(ROOT_DIR)/3rdParty/libxml2/$(INSTALL_DIR)" --without-python && $(MAKE) && $(MAKE) install
	cd 3rdParty/libxml2 && git clean -fdx -e 'install/'
endif

distclean:
	@echo
	@echo "# make distclean"
	@echo
	$(RM) $(BUILD_DIR)
	$(RM) $(INSTALL_DIR)
	$(RM) 3rdParty/FMIL/$(BUILD_DIR)
	$(RM) 3rdParty/FMIL/$(INSTALL_DIR)
	@$(MAKE) -C 3rdParty/Lua distclean
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
