RM=rm -rf

detected_OS ?= $(shell uname -s)

ifeq ($(detected_OS),Darwin)
	BUILD_DIR := build/mac
	INSTALL_DIR := install/mac
else ifeq (MINGW32,$(findstring MINGW32,$(detected_OS)))
	BUILD_DIR := build/mingw
	INSTALL_DIR := install/mingw
else ifeq (MINGW,$(findstring MINGW,$(detected_OS)))
	BUILD_DIR := build/mingw
	INSTALL_DIR := install/mingw
else
	BUILD_DIR := build/linux
	INSTALL_DIR := install/linux
endif

.PHONY: OMSimulator config-OMSimulator config-3rdParty distclean testsuite doc doc-html doc-doxygen pip

OMSimulator:
	@echo OS: $(detected_OS)
	@echo TLM: $(OMTLM)
	@echo LIBXML2: $(LIBXML2)
	@echo "# make OMSimulator"
	@echo
	cmake -G Ninja -S . -B build -DOMTLM:BOOL=OFF
	cmake --build build/ --target install
	test ! -z "$(DISABLE_RUN_OMSIMULATOR_VERSION)" || $(INSTALL_DIR)/bin/OMSimulator --version

config-OMSimulator:
	@echo
	@echo "# config OMSimulator"
	@echo

config-3rdParty:
	@echo
	@echo "# config 3rdParty"
	@echo

distclean:
	@echo
	@echo "# make distclean"
	@echo
	$(RM) build
	$(RM) install

testsuite:
	@echo
	@echo "# run testsuite"
	@echo
	@$(MAKE) -C testsuite all

doc:
	@$(MAKE) -C doc/UsersGuide latexpdf
	@$(MKDIR) install/doc
	@cp doc/UsersGuide/build/latex/OMSimulator.pdf install/doc

doc-html:
	@$(MAKE) -C doc/UsersGuide html
	@$(MKDIR) install/doc
	@$(RM) install/doc/html
	@$(CP) doc/UsersGuide/build/html/ install/doc/html

doc-doxygen:
	@$(RM) install/doc/OMSimulatorLib
	@$(MKDIR) install/doc/OMSimulatorLib
	@$(MAKE) -C doc/dev/OMSimulatorLib doc-doxygen
	@$(CP) doc/dev/OMSimulatorLib/html/* install/doc/OMSimulatorLib/
	@$(MAKE) -C doc/dev/OMSimulatorLib clean

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
