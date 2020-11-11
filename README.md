# OMSimulator [![License: OSMC-PL](https://img.shields.io/badge/license-OSMC--PL-lightgrey.svg)](OSMC-License.txt)

The OMSimulator project is a FMI-based co-simulation environment that supports ordinary (i.e., non-delayed) and TLM connections.

## Downloads

### Full package

Pre-compiled binaries are available for [Windows, Linux, and Mac](https://build.openmodelica.org/omsimulator/).

### Python

Installation using `pip` is recommended:

```bash
pip3 install OMSimulator
```

## Documentation

The latest documentation is avilable as [pdf](https://openmodelica.org/doc/OMSimulator/master/OMSimulator.pdf) and [html](https://openmodelica.org/doc/OMSimulator/master/html/).
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

1. install libxml2-dev

   ```bash
   sudo apt-get install libxml2-dev
   ```

1. configure OMSimulator

   ```bash
   make config-3rdParty
   make config-OMSimulator
   ```

   For the debug configuration add `BUILD_TYPE=Debug` to `make config-OMSimulator`.

1. build OMSimulator

   ```bash
   make OMSimulator -j4
   ```

### Windows (OMDev mingw)

1. setup OMDev

   - Checkout OMDev (OpenModelica Development Environment): `git clone https://openmodelica.org/git/OMDev.git`
   - Follow the instructions in `OMDev/INSTALL.txt`

1. configure OMSimulator

   ```bash
   make config-3rdParty
   make config-OMSimulator
   ```

   For the debug configuration add `BUILD_TYPE=Debug` to `make config-OMSimulator`.

1. build OMSimulator

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

1. install boost (VS14: 1.63, VS15: 1.64)

   - Download and install precompiled boost libs, e.g. from [this](https://sourceforge.net/projects/boost/files/boost-binaries/) source
   - Set environment variable `BOOST_ROOT` to install path, e.g:
     - `BOOST_ROOT=C:\local\boost_1_64_0`

1. configure OMSimulator

   ```bash
   configWinVS.bat VS15-Win64
   ```

   To build the debug version change `CMAKE_BUILD_TYPE` to `Debug` or change the release type in Visual Studio to `debug`.

1. build OMSimulator

   ```bash
   buildWinVS.bat VS15-Win64
   ```
