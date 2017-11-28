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

0. install boost 1.63

```bash
wget -O boost_1_63_0.tar.gz http://sourceforge.net/projects/boost/files/boost/1.63.0/boost_1_63_0.tar.gz/download
tar xzvf boost_1_63_0.tar.gz
cd boost_1_63_0/

sudo apt update
sudo apt install build-essential g++ python-dev autotools-dev libicu-dev build-essential libbz2-dev libboost-all-dev

./bootstrap.sh --prefix=/usr/
./b2
sudo ./b2 install
```

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

* Download and install precompiled boost libs, e.g. from this source:
  * https://sourceforge.net/projects/boost/files/boost-binaries/
* Set environment variable `BOOST_ROOT` to install path, e.g:
  * `BOOST_ROOT=C:\local\boost_1_63_0`

1. configure OMSimulator

```bash
> configWinVS14.bat
```

2. build OMSimulator

```bash
> buildWinVS14.bat
```
