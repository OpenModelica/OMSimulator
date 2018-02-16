RM=rm -rf
MKDIR=mkdir -p

# Option to build Ceres-Solver and its dependencies as part of the 3rdParty projects
CERES ?= ON
# Option to enable the OMFit parameter estimation module within the OMSimulator project
OMFIT ?= ON

detected_OS := $(shell uname -s)
ifeq ($(detected_OS),Darwin)
	BUILD_DIR := build/mac
	BUILD_DIR2 := build-mac
	INSTALL_DIR := install/mac
	INSTALL_DIR2 := install-mac
else ifeq (MINGW32,$(findstring MINGW32,$(detected_OS)))
	BUILD_DIR := build/mingw
	BUILD_DIR2 := build-mingw
	INSTALL_DIR := install/mingw
	INSTALL_DIR2 := install-mingw
	CMAKE_TARGET=-G "MSYS Makefiles"
	FMIL_FLAGS=-DFMILIB_FMI_PLATFORM=win32
	# MINGW detected => NO SUPPORT FOR BUILDING CERES SOLVER
	CERES := OFF
	OMFIT := OFF
else ifeq (MINGW,$(findstring MINGW,$(detected_OS)))
	BUILD_DIR := build/mingw
	BUILD_DIR2 := build-mingw
	INSTALL_DIR := install/mingw
	INSTALL_DIR2 := install-mingw
	CMAKE_TARGET=-G "MSYS Makefiles"
	FMIL_FLAGS=-DFMILIB_FMI_PLATFORM=win64
	# MINGW detected => NO SUPPORT FOR BUILDING CERES SOLVER
	CERES := OFF
	OMFIT := OFF
else
	BUILD_DIR := build/linux
	BUILD_DIR2 := build-linux
	INSTALL_DIR := install/linux
	INSTALL_DIR2 := install-linux
endif

.PHONY: OMSimulator config-OMSimulator config-fmil config-lua config-cvode config-kinsol config-gflags config-glog config-ceres-solver config-3rdParty distclean testsuite doc doc-html doc-doxygen

OMSimulator:
	@echo
	@echo "# make OMSimulator"
	@echo
	$(RM) $(INSTALL_DIR)
	@$(MAKE) -C $(BUILD_DIR) install



ifeq ($(CERES),OFF)
config-3rdParty: config-fmil config-lua config-cvode config-kinsol
	@echo
	@echo "# CERES=OFF => Skipping build of 3rdParty library Ceres-Solver. Ceres-Solver is a dependency of the (optional) parameter estimation module."
	@echo
else
config-3rdParty: config-fmil config-lua config-cvode config-kinsol config-gflags config-glog config-ceres-solver
endif

config-OMSimulator:
	@echo
	@echo "# config OMSimulator"
	@echo
	$(RM) $(BUILD_DIR)
	$(MKDIR) $(BUILD_DIR)
	@cd $(BUILD_DIR) && cmake $(CMAKE_TARGET) -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON -DOMFIT=$(OMFIT) ../..

config-fmil:
	@echo
	@echo "# config fmil"
	@echo
	$(RM) 3rdParty/FMIL/$(BUILD_DIR)
	$(RM) 3rdParty/FMIL/$(INSTALL_DIR)
	$(MKDIR) 3rdParty/FMIL/$(BUILD_DIR)
	cd 3rdParty/FMIL/$(BUILD_DIR) && cmake $(CMAKE_TARGET) -DFMILIB_INSTALL_PREFIX=../../$(INSTALL_DIR) -DFMILIB_BUILD_TESTS:BOOL="0" -DFMILIB_GENERATE_DOXYGEN_DOC:BOOL="0" -DFMILIB_BUILD_STATIC_LIB:BOOL="1" -DFMILIB_BUILD_SHARED_LIB:Bool="0" -DBUILD_TESTING:BOOL="0" -DFMILIB_BUILD_BEFORE_TESTS:BOOL="0" $(FMIL_FLAGS) ../.. && $(MAKE) install

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
	cd 3rdParty/cvode/$(BUILD_DIR) && cmake $(CMAKE_TARGET) -DCMAKE_INSTALL_PREFIX=../../$(INSTALL_DIR) ../.. -DEXAMPLES_ENABLE:BOOL="0" -DBUILD_SHARED_LIBS:BOOL="0" -DCMAKE_C_FLAGS="-fPIC" && $(MAKE) install

config-kinsol:
	@echo
	@echo "# config kinsol"
	@echo
	$(RM) 3rdParty/kinsol/$(BUILD_DIR)
	$(RM) 3rdParty/kinsol/$(INSTALL_DIR)
	$(MKDIR) 3rdParty/kinsol/$(BUILD_DIR)
	cd 3rdParty/kinsol/$(BUILD_DIR) && cmake $(CMAKE_TARGET) -DCMAKE_INSTALL_PREFIX=../../$(INSTALL_DIR) ../.. -DEXAMPLES_ENABLE:BOOL="0" -DBUILD_SHARED_LIBS:BOOL="0" -DCMAKE_C_FLAGS="-fPIC" && $(MAKE) install

config-gflags:
	@echo
	@echo "# config gflags"
	@echo
	$(RM) 3rdParty/gflags/$(BUILD_DIR2)
	$(RM) 3rdParty/gflags/$(INSTALL_DIR2)
	$(MKDIR) 3rdParty/gflags/$(BUILD_DIR2)
	cd 3rdParty/gflags/$(BUILD_DIR2) && cmake $(CMAKE_TARGET) -DCMAKE_INSTALL_PREFIX=../$(INSTALL_DIR2) .. -DBUILD_SHARED_LIBS="OFF" -DBUILD_TESTING="OFF" -DCMAKE_C_FLAGS="-fPIC" -DCMAKE_CXX_FLAGS="-fPIC" -DCMAKE_BUILD_TYPE="Release" && $(MAKE) install

config-glog: config-gflags
	@echo
	@echo "# config glog"
	@echo
	$(RM) 3rdParty/glog/$(BUILD_DIR2)
	$(RM) 3rdParty/glog/$(INSTALL_DIR2)
	$(MKDIR) 3rdParty/glog/$(BUILD_DIR2)
	cd 3rdParty/glog/$(BUILD_DIR2) && cmake $(CMAKE_TARGET) -DCMAKE_INSTALL_PREFIX=../$(INSTALL_DIR2) .. -DBUILD_SHARED_LIBS="OFF" -DBUILD_TESTING="OFF" -DCMAKE_C_FLAGS="-fPIC" -DCMAKE_CXX_FLAGS="-fPIC" -DCMAKE_BUILD_TYPE="Release" && $(MAKE) install

config-ceres-solver: config-glog
	@echo
	@echo "# config ceres-solver"
	@echo
	$(RM) 3rdParty/ceres-solver/$(BUILD_DIR2)
	$(RM) 3rdParty/ceres- solver/$(INSTALL_DIR2)
	$(MKDIR) 3rdParty/ceres-solver/$(BUILD_DIR2)
	cd 3rdParty/ceres-solver/$(BUILD_DIR2) && cmake $(CMAKE_TARGET) -DCMAKE_INSTALL_PREFIX=../$(INSTALL_DIR2) -DCXX11="ON" -DEXPORT_BUILD_DIR="ON" -DEIGEN_INCLUDE_DIR_HINTS="../eigen" -DBUILD_EXAMPLES="OFF" -DBUILD_TESTING="OFF" -DCMAKE_BUILD_TYPE="Release" .. && $(MAKE) install

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
	$(RM) 3rdParty/gflags/$(BUILD_DIR2)
	$(RM) 3rdParty/gflags/$(INSTALL_DIR2)
	$(RM) 3rdParty/glog/$(BUILD_DIR2)
	$(RM) 3rdParty/glog/$(INSTALL_DIR2)
	$(RM) 3rdParty/ceres-solver/$(BUILD_DIR2)
	$(RM) 3rdParty/ceres-solver/$(INSTALL_DIR2)

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
	@cp -rf doc/UsersGuide/build/html/ $(INSTALL_DIR)/doc/html

doc-doxygen:
	@$(RM) doc/dev/OMSimulatorLib/
	@doxygen doc/dev/OMSimulatorLib.doxyfile
	@$(MKDIR) $(INSTALL_DIR)/doc
	@$(RM) $(INSTALL_DIR)/doc/OMSimulatorLib
	@cp -rf doc/dev/OMSimulatorLib/html/ $(INSTALL_DIR)/doc/OMSimulatorLib
