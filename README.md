# OMSimulator [![License: OSMC-PL](https://img.shields.io/badge/license-OSMC--PL-lightgrey.svg)](OSMC-License.txt)

The OpenModelica FMI & SSP-based co-simulation environment.

## Downloads

### Full package

Pre-compiled binaries are available for [Windows, Linux, and Mac](https://build.openmodelica.org/omsimulator/).

Besides the stable versions, you can also [download](https://test.openmodelica.org/jenkins/job/OMSimulator/job/master/lastSuccessfulBuild/artifact/) the latest development version.

### Python

Installation using `pip` is recommended:

```bash
pip3 install OMSimulator
```
Python needs to be version 3.8 or above.

## Documentation

The latest documentation is available as [pdf](https://openmodelica.org/doc/OMSimulator/master/OMSimulator.pdf) and [html](https://openmodelica.org/doc/OMSimulator/master/html/).
For OMSimulatorLib a [Doxygen source code documentation](https://openmodelica.org/doc/OMSimulator/master/OMSimulatorLib/) is available as well.

## Dependencies

- [Boost](http://www.boost.org/) (system, filesystem)
- [cmake](http://www.cmake.org)
- [Sphinx](http://www.sphinx-doc.org/en/stable/)
- [readline (if using Lua)](http://git.savannah.gnu.org/cgit/readline.git)
- [3rdParty subproject](https://github.com/OpenModelica/OMSimulator-3rdParty)
  - FMILibrary
  - Lua
  - PugiXML
  - SUNDIALS CVODE
  - SUNDIALS KINSOL
  - CTPL

## Compilation

Note: Make sure to fetch the submodules, e.g., using `git submodule update --init`.

### Linux / MacOS

1. Install libxml2-dev

   ```bash
   sudo apt-get install libxml2-dev
   ```

1. Configure OMSimulator

   ```bash
   make config-3rdParty
   make config-OMSimulator
   ```

   For the debug configuration add `BUILD_TYPE=Debug` to `make config-OMSimulator`.

1. Build OMSimulator

   ```bash
   make OMSimulator -j4
   ```

### Windows (OMDev mingw)

1. Setup OMDev

   - Checkout OMDev (OpenModelica Development Environment): `git clone https://openmodelica.org/git/OMDev.git`
   - Follow the instructions in `OMDev/INSTALL.txt`

1. Configure OMSimulator

   ```bash
   make config-3rdParty
   make config-OMSimulator
   ```

   For the debug configuration add `BUILD_TYPE=Debug` to `make config-OMSimulator`.

1. Build OMSimulator

   ```bash
   make OMSimulator -j4
   ```

### Windows (Visual Studio)

The following versions of Visual Studio are supported:

- "VS14-Win32" -> "Visual Studio 14 2015"
- "VS14-Win64" -> "Visual Studio 14 2015 Win64"
- "VS15-Win32" -> "Visual Studio 15 2017"
- "VS15-Win64" -> "Visual Studio 15 2017 Win64"

It is not strictly required to install the full Visual Studio IDE. The batch scripts only require *[Visual C++ Build Tools](http://landinghub.visualstudio.com/visual-cpp-build-tools)*.

1. Install boost (VS14: 1.63, VS15: 1.64)

   - Download and install precompiled boost libs, e.g. from [this](https://sourceforge.net/projects/boost/files/boost-binaries/) source
   - Set environment variable `BOOST_ROOT` to install path, e.g:
     - `BOOST_ROOT=C:\local\boost_1_64_0`

1. Configure OMSimulator

   ```bash
   .\configWinVS.bat VS15-Win64
   ```

   To build the debug version change `CMAKE_BUILD_TYPE` to `Debug` or change the release type in Visual Studio to `debug`.

1. Build OMSimulator

   ```bash
   .\buildWinVS.bat VS15-Win64
   ```

## Test your build

The testsuite of OMSimulator is run on Jenkins for every commit and creating
[test reports](https://test.openmodelica.org/jenkins/job/OMSimulator/job/master/lastSuccessfulBuild/testReport/).
The project is tested for the following OS:
   - linux-arm32
   - linux64 without OMPython
   - cross-compiled mingw64
   - msvc64
   - cross-compiled OSX

In addition the [OpenModelica project](https://github.com/OpenModelica/OpenModelica) has a number of test cases using OMSimulator for FMU simulation that can be find in this [OpenModelica test reports](https://test.openmodelica.org/jenkins/job/OpenModelica/job/master/lastSuccessfulBuild/testReport/).

To verify your build is compiled and installed corrrectly see the following instructions.

### Linux / MacOS / Windows (OMDev mingw)

1. Build test dependencies
   ```bash
   make -C testsuite/ difftool resources
   ```

2. Run partest

   ```bash
   cd testsuite/partest/
   ./runtests.pl -j4
   ```
   Use `-jN` to test with `N` threads.
   To disable TLM tests add `-notlm`, to disable Python tests add `-asan`.

### Windows (Visual Studio)

We currently have no bat-Script to build and test with CMD, so you have to use OMDev mingw shell for the tests.

1. Build test dependencies
   ```bash
   make -C testsuite/ difftool resources
   ```

2. Run partest

   ```bash
   cd testsuite/partest/
   ./runtests.pl -j4 -platform=win
   ```
   Use `-jN` to test with `N` threads.
   To disable TLM tests add `-notlm`, to disable Python tests add `-asan`.