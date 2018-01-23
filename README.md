# OMSimulator

OpenModelica FMI &amp; TLM based simulator

## Dependencies

- [cmake](http://www.cmake.org)
- [Boost](http://www.boost.org/) (system, filesystem, program_options)
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

### Windows

The following versions of MS Visual Studio are supported:

- MS Visual Studio 2015 [Win64]:
  - configWinVS14.bat
  - buildWinVS14.bat
- MS Visual Studio 2017 [Win64]:
  - configWinVS15.bat
  - buildWinVS15.bat

It is not strictly required to install the full Visual Studio IDE. The batch
scripts only require the *Visual C++ 2015 Build Tools* which are also available
as a leaner standalone package, which can be downloaded from [Microsoft](http://landinghub.visualstudio.com/visual-cpp-build-tools).

1. install boost 1.63 (or 1.64 for VS 2017)

   - Download and install precompiled boost libs, e.g. from [this](https://sourceforge.net/projects/boost/files/boost-binaries/) source
   - Set environment variable `BOOST_ROOT` to install path, e.g:
     - `BOOST_ROOT=C:\local\boost_1_63_0`

1. configure OMSimulator

   ```bash
   configWinVS14.bat
   ```

1. build OMSimulator

   ```bash
   buildWinVS14.bat
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
