# OMSimulator [![License: OSMC-PL](https://img.shields.io/badge/license-OSMC--PL-lightgrey.svg)](OSMC-License.txt)

The OpenModelica FMI & SSP-based co-simulation environment.

## Downloads

OMSimulator can be installed as stand-alone application, as C library to be linked into custom applications, and as python package. OMSimulator is also shipped with the OpenModelica installer, which also includes OMEdit as the graphical editor.

* [OpenModelica](https://openmodelica.org/)
* [Stand-alone package](https://build.openmodelica.org/omsimulator/)
* Python 3.8+: `pip3 install OMSimulator`

## Documentation

The latest documentation is available as [pdf](https://openmodelica.org/doc/OMSimulator/master/OMSimulator.pdf) and [html](https://openmodelica.org/doc/OMSimulator/master/html/) versions.
For OMSimulatorLib a [Doxygen source code documentation](https://openmodelica.org/doc/OMSimulator/master/OMSimulatorLib/) is available as well.

## FMI cross-check results

* https://libraries.openmodelica.org/fmi-crosschecking/OMSimulator/history/

## Dependencies

### Tools
Required
- A C++ compiler with C++17 support.
- [cmake](http://www.cmake.org)

Optional
- [Sphinx](http://www.sphinx-doc.org/en/stable/) for generating documentation.
- [Doxygen](http://www.sphinx-doc.org/en/stable/) for generating documentation.
- [GNU Flex](https://github.com/westes/flex) for verifying testsuite results.

### Libraries
- [readline (if using Lua)](http://git.savannah.gnu.org/cgit/readline.git)


## Compilation

> **Note**
> Make sure you have fetched and initialized the OMSimulator submodules

You can do this in one of two ways

1. If you have already cloned OMSimulator, then run

   ```bash
   git submodule update --init --recursive
   ```
1. If you are cloning the repository, then use

   ```bash
   git clone https://github.com/OpenModelica/OMSimulator.git --recurse-submodules
   ```

### Linux / MacOS

1. Configure OMSimulator

   ```bash
   cd OMSimulator
   cmake -S . -B build/ -DCMAKE_INSTALL_PREFIX=install/
   ```

2. Build and install OMSimulator

   ```bash
   cd OMSimulator
   cmake --build build/ --target install
   ./install/bin/OMSimulator --version
   ```

### Windows (OMDev mingw)

1. Setup OMDev

   - Clone OMDev (OpenModelica Development Environment): `git clone https://openmodelica.org/git/OMDev.git`
   - Follow the instructions in `OMDev/INSTALL.txt`

1. Open a MinGW-64 terminal

1. Configure OMSimulator


   ```bash
   cd OMSimulator
   cmake -S . -B build/ -G "MSYS Makefiles" -DCMAKE_INSTALL_PREFIX=install/
   ```

1. Build and install OMSimulator

   ```bash
   cd OMSimulator
   cmake --build build/ --target install
   ./install/bin/OMSimulator --version
   ```

### Windows (Visual Studio)

The following versions of Visual Studio are supported:

- Visual Studio 15 2017 Win64 and later

It is not strictly required to install the full Visual Studio IDE. The build only requires *[Visual C++ Build Tools](http://landinghub.visualstudio.com/visual-cpp-build-tools)*.


1. Open a [Visual Studio command prompt](https://learn.microsoft.com/en-us/visualstudio/ide/reference/command-prompt-powershell?view=vs-2022)

1. Configure OMSimulator

   ```bash
   cd OMSimulator
   cmake -S . -B build/ -DCMAKE_INSTALL_PREFIX=install/
   ```

1. Build and install OMSimulator

   ```bash
   cd OMSimulator
   cmake --build build/ --config Release --target install
   ./install/bin/OMSimulator.exe --version
   ```

## Test your build

The testsuite of OMSimulator is run on Jenkins for every commit and creating
[test reports](https://test.openmodelica.org/jenkins/job/OMSimulator/job/master/lastSuccessfulBuild/testReport/).
The project is tested for the following OS:
   - linux64 without OMPython
   - mingw-ucrt64
   - msvc64
   - cross-compiled OSX

In addition the [OpenModelica project](https://github.com/OpenModelica/OpenModelica) has a number of test cases using OMSimulator for FMU simulation that can be find in this [OpenModelica test reports](https://test.openmodelica.org/jenkins/job/OpenModelica/job/master/lastSuccessfulBuild/testReport/).

To verify your build is compiled and installed correctly see the following instructions.

### Linux / MacOS / Windows (OMDev mingw)

1. Build test dependencies
   ```bash
   cmake --build build/ --target testsuite-depends
   ```

2. Run partest

   ```bash
   cd testsuite/partest/
   ./runtests.pl -j4
   ```
   Use `-jN` to test with `N` threads.

   To disable Python tests add `-asan`.

### Windows (Visual Studio)

We currently have no bat-Script to build and test with CMD, so you have to use OMDev mingw shell for the tests.

1. Build test dependencies
   ```bash
   cmake --build build/ --target testsuite-depends
   ```

2. Run partest

   ```bash
   cd testsuite/partest/
   ./runtests.pl -j4 -platform=win
   ```
   Use `-jN` to test with `N` threads.
   To disable TLM tests add `-notlm`, to disable Python tests add `-asan`.
