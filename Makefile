RM=rm -rf
MKDIR=mkdir -p

detected_OS := $(shell uname -s)
ifeq ($(detected_OS),Darwin)
	BUILD_DIR := build/mac
	INSTALL_DIR := install/mac
else ifeq (MINGW32,$(findstring MINGW32,$(detected_OS)))
	BUILD_DIR := build/mingw
	INSTALL_DIR := install/mingw
	CMAKE_TARGET=-G "MSYS Makefiles"
	FMIL_FLAGS=-DFMILIB_FMI_PLATFORM=win32
else ifeq (MINGW,$(findstring MINGW,$(detected_OS)))
	BUILD_DIR := build/mingw
	INSTALL_DIR := install/mingw
	CMAKE_TARGET=-G "MSYS Makefiles"
	FMIL_FLAGS=-DFMILIB_FMI_PLATFORM=win64
else
	BUILD_DIR := build/linux
	INSTALL_DIR := install/linux
endif

.PHONY: OMSimulator config-OMSimulator config-fmil config-lua config-cvode config-kinsol config-gflags config-glog config-ceres-solver config-3rdParty distclean testsuite doc doc-html

OMSimulator:
	@echo
	@echo "# make OMSimulator"
	@echo
	$(RM) $(INSTALL_DIR)
	@$(MAKE) -C $(BUILD_DIR) install

config-3rdParty: config-fmil config-lua config-cvode config-kinsol config-gflags config-glog config-ceres-solver

config-OMSimulator:
	@echo
	@echo "# config OMSimulator"
	@echo
	$(RM) $(BUILD_DIR)
	$(MKDIR) $(BUILD_DIR)
	@cd $(BUILD_DIR) && cmake $(CMAKE_TARGET) -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON ../..

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
	$(RM) 3rdParty/gflags/build-linux
	$(RM) 3rdParty/gflags/$(INSTALL_DIR)
	$(MKDIR) 3rdParty/gflags/build-linux
	cd 3rdParty/gflags/build-linux && cmake -DCMAKE_INSTALL_PREFIX=../$(INSTALL_DIR) .. -DBUILD_SHARED_LIBS="OFF" -DBUILD_TESTING="OFF" -DCMAKE_C_FLAGS="-fPIC" -DCMAKE_CXX_FLAGS="-fPIC" -DCMAKE_BUILD_TYPE="Release" && $(MAKE) install

config-glog: config-gflags
	@echo
	@echo "# config glog"
	@echo
	$(RM) 3rdParty/glog/build-linux
	$(RM) 3rdParty/glog/$(INSTALL_DIR)
	$(MKDIR) 3rdParty/glog/build-linux
	cd 3rdParty/glog/build-linux && cmake -DCMAKE_INSTALL_PREFIX=../$(INSTALL_DIR) .. -DBUILD_SHARED_LIBS="OFF" -DBUILD_TESTING="OFF" -DCMAKE_C_FLAGS="-fPIC" -DCMAKE_CXX_FLAGS="-fPIC" -DCMAKE_BUILD_TYPE="Release" && $(MAKE) install

config-ceres-solver: config-glog
	@echo
	@echo "# config ceres-solver"
	@echo
	$(RM) 3rdParty/ceres-solver/build-linux
	$(RM) 3rdParty/ceres-solver/$(INSTALL_DIR)
	$(MKDIR) 3rdParty/ceres-solver/build-linux
	cd 3rdParty/ceres-solver/build-linux && cmake -DCMAKE_INSTALL_PREFIX=../$(INSTALL_DIR) -DCXX11="ON" -DEXPORT_BUILD_DIR="ON" -DEIGEN_INCLUDE_DIR_HINTS="../eigen" -DGLOG_INCLUDE_DIR_HINTS="../glog/install/linux/include" -DGLOG_LIBRARY_DIR_HINTS="../glog/install/linux/lib" -DGFLAGS_INCLUDE_DIR_HINTS="../gflags/install/linux/include" -DGFLAGS_LIBRARY_DIR_HINTS="../gflags/install/linux/lib" -DBUILD_EXAMPLES="OFF" -DBUILD_TESTING="OFF" -DCMAKE_BUILD_TYPE="Release" .. && $(MAKE) install

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
