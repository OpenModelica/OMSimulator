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
   - Clone OMDev: `git clone https://openmodelica.org/git/OMDev.git`
   - Follow the setup instructions in `OMDev/INSTALL.txt`

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

1. **Build Test Dependencies:**
   ```bash
   cmake --build build/ --target testsuite-depends
   ```

2. **Run Tests:**
   ```bash
   cd testsuite/partest/
   ./runtests.pl -j4
   ```
   - Use `-jN` to specify `N` threads.

### Windows (Visual Studio)

Testing requires the OMDev mingw shell:

1. **Build Test Dependencies:**
   ```bash
   cmake --build build/ --target testsuite-depends
   ```

2. **Run Tests:**
   ```bash
   cd testsuite/partest/
   ./runtests.pl -j4 -platform=win
   ```
   - Use `-jN` to specify `N` threads.