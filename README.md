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
> make config-3rdParty
> make config-OMSimulator
```

2. build OMSimulator

```bash
> make OMSimulator
```

### Windows

0. install boost 1.63

- Download and install precompiled boost libs, e.g. from [this](https://sourceforge.net/projects/boost/files/boost-binaries/) source
- Set environment variable `BOOST_ROOT` to install path, e.g:
  - `BOOST_ROOT=C:\local\boost_1_63_0`

1. configure OMSimulator

```bash
> configWinVS14.bat
```

2. build OMSimulator

```bash
> buildWinVS14.bat
```
