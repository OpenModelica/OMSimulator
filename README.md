# OMSimulator

The OMSimulator project is a FMI-based co-simulation environment that supports ordinary (i.e., non-delayed) and TLM connections.

The latest documentation is avilable as [pdf](https://openmodelica.org/doc/OMSimulator/OMSimulator.pdf) and [html](https://openmodelica.org/doc/OMSimulator/html/).

## Dependencies

- [Boost](http://www.boost.org/) (system, filesystem, program_options)
- [cmake](http://www.cmake.org)
- [Sphinx](http://www.sphinx-doc.org/en/stable/)
- [3rdParty subproject](https://github.com/OpenModelica/OMFMISimulator-3rdParty)
  - FMILibrary
  - Lua
  - PugiXML
  - SUNDIALS CVODE
  - SUNDIALS KINSOL

## Compilation

### Linux

1. configure OMSimulator

   ```bash
   make config-3rdParty
   make config-OMSimulator
   ```

1. build OMSimulator

   ```bash
   make OMSimulator
   ```

### Windows (OMDev mingw)

1. setup OMDev

   - Checkout OMDev (OpenModelica Development Environment) from [OpenModelica/OMDev](https://github.com/OpenModelica/OMDev)
   - Follow the instructions in `OMDev/INSTALL.txt`

1. configure OMSimulator

   ```bash
   make -f Makefile.omdev.mingw config-3rdParty
   make -f Makefile.omdev.mingw config-OMSimulator
   ```

1. build OMSimulator

   ```bash
   make -f Makefile.omdev.mingw OMSimulator
   ```

### Windows (Visual Studio)

The following versions of Visual Studio are supported:

- Visual Studio 2015 [Win64]:
  - configWinVS14.bat
  - buildWinVS14.bat
- Visual Studio 2017 [Win64]:
  - configWinVS15.bat
  - buildWinVS15.bat

It is not strictly required to install the full Visual Studio IDE. The batch scripts only require *[Visual C++ Build Tools](http://landinghub.visualstudio.com/visual-cpp-build-tools)*.

1. install boost (VS14: 1.63, VS15: 1.64)

   - Download and install precompiled boost libs, e.g. from [this](https://sourceforge.net/projects/boost/files/boost-binaries/) source
   - Set environment variable `BOOST_ROOT` to install path, e.g:
     - `BOOST_ROOT=C:\local\boost_1_64_0`

1. configure OMSimulator

   ```bash
   configWinVS15.bat
   ```

1. build OMSimulator

   ```bash
   buildWinVS15.bat
   ```

## Working with the repository

OMSimulator.git is a superproject.
Clone the project using:

```bash
MY_FORK=MyGitHubUserName ; git clone https://github.com/OpenModelica/OMSimulator.git --recursive && (cd OMSimulator && git remote set-url --push origin https://github.com/$MY_FORK/OMSimulator.git && git submodule foreach --recursive 'git remote set-url --push origin `git config --get remote.origin.url | sed s,^.*/,https://github.com/'$MY_FORK'/,`')
```

If you are a developer and want to track the latest heads, use:

```bash
# After cloning
git submodule foreach --recursive "git checkout master"
# To update; you will need to merge each submodule, but your changes will remain
git submodule foreach --recursive "git pull"
```

In order to push to the repository, you will push to your own fork of OMSimulator.git, OMSimulator-testsuite.git, etc. You will need to create a fork of each repository that you want to push to (by clicking the Fork button in the GitHub web interface).
