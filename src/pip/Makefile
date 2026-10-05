.PHONY: pip

# Build the sdist with CMake; see the pip_sdist target in CMakeLists.txt.
# Usage: make pip BUILD_DIR=../../build
BUILD_DIR ?= ../../build

pip:
	@echo
	@echo "# make pip"
	@echo
	cmake -S ../.. -B $(BUILD_DIR) -DOMS_ENABLE_PIP=ON
	cmake --build $(BUILD_DIR) --target pip_sdist
	@echo
	@echo "# Run the following command to upload the package"
	@echo "> twine upload $(BUILD_DIR)/src/pip/dist/*"
