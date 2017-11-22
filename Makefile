RM=rm -rf
MKDIR=mkdir -p

# ugly fix for macosx
detected_OS := $(shell uname -s)
ifeq ($(detected_OS),Darwin)
	BUILD_DIR := build/mac
	INSTALL_DIR := install/mac
	LIB_EXT := .dylib
else
	BUILD_DIR := build/linux
	INSTALL_DIR := install/linux
	LIB_EXT := .so
endif

.PHONY: OMSimulator config config-OMSimulator config-fmil config-lua config-cvode config-kinsol config-3rdParty distclean testsuite

OMSimulator:
	@echo
	@echo "# make OMSimulator"
	@echo
	@$(MAKE) -C $(BUILD_DIR) install

config: config-OMSimulator
config-3rdParty: config-fmil config-lua config-cvode config-kinsol

config-OMSimulator:
	@echo
	@echo "# config OMSimulator"
	@echo
	$(RM) $(BUILD_DIR)
	$(MKDIR) $(BUILD_DIR)
	cd $(BUILD_DIR) && cmake ../.. && $(MAKE) install
# patch for FMIL linking on Mac
ifeq ($(detected_OS),Darwin)
	install_name_tool -change libfmilib_shared.dylib "@loader_path/libfmilib_shared.dylib" install/bin/OMSimulator
endif


config-fmil:
	@echo
	@echo "# config fmil"
	@echo
	$(RM) 3rdParty/FMIL/$(BUILD_DIR)
	$(RM) 3rdParty/FMIL/$(INSTALL_DIR)
	$(MKDIR) 3rdParty/FMIL/$(BUILD_DIR)
	cd 3rdParty/FMIL/$(BUILD_DIR) && cmake -DFMILIB_INSTALL_PREFIX=../../$(INSTALL_DIR) ../.. && $(MAKE) install
	$(MKDIR) install/bin/
	$(MKDIR) install/lib/
	cp 3rdParty/FMIL/$(INSTALL_DIR)/lib/libfmilib_shared$(LIB_EXT) install/bin/
	cp 3rdParty/FMIL/$(INSTALL_DIR)/lib/libfmilib_shared$(LIB_EXT) install/lib/

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
	$(RM) 3rdParty/cvode/build-linux
	$(RM) 3rdParty/cvode/$(INSTALL_DIR)
	$(MKDIR) 3rdParty/cvode/build-linux
	cd 3rdParty/cvode/build-linux && cmake -DCMAKE_INSTALL_PREFIX=../$(INSTALL_DIR) .. -DEXAMPLES_ENABLE:BOOL="0" -DBUILD_SHARED_LIBS:BOOL="0" -DCMAKE_C_FLAGS="-fPIC" && $(MAKE) install

config-kinsol:
	@echo
	@echo "# config kinsol"
	@echo
	$(RM) 3rdParty/kinsol/build-linux
	$(RM) 3rdParty/kinsol/$(INSTALL_DIR)
	$(MKDIR) 3rdParty/kinsol/build-linux
	cd 3rdParty/kinsol/build-linux && cmake -DCMAKE_INSTALL_PREFIX=../$(INSTALL_DIR) .. -DEXAMPLES_ENABLE:BOOL="0" -DBUILD_SHARED_LIBS:BOOL="0" -DCMAKE_C_FLAGS="-fPIC" && $(MAKE) install

distclean:
	@echo
	@echo "# make distclean"
	@echo
	$(RM) $(BUILD_DIR)
	$(RM) install
	$(RM) 3rdParty/FMIL/$(BUILD_DIR)
	$(RM) 3rdParty/FMIL/$(INSTALL_DIR)
	$(RM) 3rdParty/cvode/build-linux
	$(RM) 3rdParty/cvode/$(INSTALL_DIR)

testsuite:
	@echo
	@echo "# run testsuite"
	@echo
	@$(MAKE) -C testsuite all
