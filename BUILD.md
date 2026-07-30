# OMSimulator Build Instructions

## Dependencies

### Required Tools

- **C++ compiler** with C++17 support
- **CMake**: [Download here](http://www.cmake.org)

### Optional Tools

- **Sphinx**: for documentation ([Download](http://www.sphinx-doc.org/en/stable/))
- **Doxygen**: for documentation ([Download](https://www.doxygen.nl/))
- **GNU Flex**: for test suite verification ([GitHub](https://github.com/westes/flex))

### Libraries

- **readline** (for Lua support): [Download here](http://git.savannah.gnu.org/cgit/readline.git)

## Compilation Instructions

### Submodule Initialization

Ensure you have initialized OMSimulator submodules:

```bash
git submodule update --init --recursive
```

Or, when cloning:

```bash
git clone https://github.com/OpenModelica/OMSimulator.git --recurse-submodules
```

### Build Instructions

#### Linux / macOS

1. **Configure OMSimulator:**

   ```bash
   cd OMSimulator
   cmake -S . -B build/ -DCMAKE_INSTALL_PREFIX=install/
   ```

2. **Build and Install:**

   ```bash
   cmake --build build/ --target install
   ./install/bin/OMSimulator --version
   ```

#### Windows (OMDev mingw)

1. **Set Up OMDev:**
   - Clone OMDev: `https://gitlab.liu.se/OpenModelica/OMDevUCRT.git`
   - Follow the setup instructions in `OMDev/INSTALL.md`

2. **Configure OMSimulator:**

   ```bash
   cd OMSimulator
   cmake -S . -B build/ -G "MSYS Makefiles" -DCMAKE_INSTALL_PREFIX=install/
   ```

3. **Build and Install:**

   ```bash
   cmake --build build/ --target install
   ./install/bin/OMSimulator --version
   ```

#### Windows (Visual Studio)

- Supported: Visual Studio 2017 and later.
- Required: **Visual C++ Build Tools** ([Download](https://visualstudio.microsoft.com/downloads/))

1. **Open Visual Studio Command Prompt**
2. **Configure OMSimulator:**

   ```bash
   cd OMSimulator
   cmake -S . -B build/ -DCMAKE_INSTALL_PREFIX=install/
   ```

3. **Build and Install:**

   ```bash
   cmake --build build/ --config Release --target install
   ./install/bin/OMSimulator.exe --version
   ```

## Testing the Build

### Linux / macOS / Windows (OMDev mingw)

By default the testsuite is not enabled by the cmake build. To enable the testsuite run the following command

1. **Configure testsuite :**

   ```bash
   cmake -S . -B build/ -DOMS_ENABLE_TESTSUITE=ON
   ```

2. **Install OMSimulator:**

   ```bash
   cmake --build build/ --target install
   ```

   The tests run against the installed OMSimulator, because the Python bindings are only usable
   from an install tree.

3. **Run Tests:**

   ```bash
   ctest --test-dir build/ -j4
   ```

   - Use `-jN` to specify `N` threads.
   - Use `--output-on-failure` to see the log and the diff of failing tests.

   See [testsuite/README.md](testsuite/README.md) for running a subset of the tests and for
   updating the expected results.
