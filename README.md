# OMSimulator [![License: OSMC-PL](https://img.shields.io/badge/license-OSMC--PL-lightgrey.svg)](OSMC-License.txt)

The OMSimulator project is a FMI-based co-simulation environment that supports ordinary (i.e., non-delayed) and TLM connections.

## Downloads

Pre-compiled binaries are available for [Windows, Linux, and Mac](https://test.openmodelica.org/jenkins/job/OMSimulator/job/master/lastSuccessfulBuild/artifact/).

The latest documentation is avilable as [pdf](https://openmodelica.org/doc/OMSimulator/master/OMSimulator.pdf) and [html](https://openmodelica.org/doc/OMSimulator/master/html/).

## Dependencies

- [Boost](http://www.boost.org/) (system, filesystem)
- [cmake](http://www.cmake.org)
- [Sphinx](http://www.sphinx-doc.org/en/stable/)
- [readline (if using Lua)](http://git.savannah.gnu.org/cgit/readline.git)
- [3rdParty subproject](https://github.com/OpenModelica/OMFMISimulator-3rdParty)
  - FMILibrary
  - Lua
  - PugiXML
  - SUNDIALS CVODE
  - SUNDIALS KINSOL
  - CTPL
  - gflags
  - glog
  - Ceres Solver

## Working with the repository

OMSimulator.git is a superproject. Clone the project recursive using:
```bash
> MY_FORK=MyGitHubUserName; git clone https://openmodelica.org/git-readonly/OMSimulator.git --recursive && (cd OMSimulator && git remote set-url --push origin git@github.com:$MY_FORK/OMSimulator.git && git submodule foreach --recursive 'git remote set-url --push origin `git config --get remote.origin.url | sed s,^.*/,git@github.com:'$MY_FORK'/,`')
```
Faster pulling by using openmodelica.org read-only mirror (low latency in Europe; very important when updating all submodules).
Replace the openmodelica.org pull URL with https://github.com/OpenModelica/OMSimulator.git if you want to pull directly from github.
The default choice is to push to your fork on github.com (SSH).

## Compilation

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

1. build OMSimulator

   ```bash
   buildWinVS.bat VS15-Win64
   ```

## The optional parameter estimation module

OMSimulator includes an optional parameter optimization module. It is enabled by default. It can be disabled in order to speed up the compilation process or if it causes compilation problems. It is not supported when using MinGW (and automatically disabled in that case). It can be disabled by using the options `CERES=OFF` (disables building its dependencies) and `OMSYSIDENT=OFF`.

```bash
make config-3rdParty OMSYSIDENT=OFF
make config-OMSimulator OMSYSIDENT=OFF
```
