# Tests for the OMSysIdent module

This folder contains tests for the OMSysIdent module.

## Tests for the OMSysIdent module C-API bindings

The tests that directly use the C-API can be executed by cmake/ctest, see
CMakeLists.txt, e.g., in the OMSimulator build directory do:

```shell
ctest -V
```

Additionally, the tests can be executed as ROOT Cling scripts using the CERN
ROOT Data Analysis Framework, [https://root.cern.ch/](https://root.cern.ch/).
Use the `runRoot.sh` for this purpose (this will start Root and load the required
shared libraries), e.g.:

```shell
./runRoot.sh ../../src/OMSysIdentLib/test_HelloWorld_cs_Fit.c
```

# Tests for the OMSysIdent module using the Lua bindings

The Lua scripts use the Lua bindings to the OMSysIdent C-API for parameter estimation.

The OMSimulator binary already contains a Lua interpreter including the Lua bindings
to the OMSysIdent library (if it has been built with the _optional_ OMSysIdent module).
Hence, scripts can be simply executed like shown below:

```shell
/my/path/to/OMSimulator HelloWorld_cs_Fit.lua
```

The scripts can also be executed by a standard Lua interpreter. In this case
the libraries need to be loaded similarly as shown below:

```lua
require("package")
OMSimulatorLua = package.loadlib("../../install/linux/lib/libOMSimulatorLua.so", "luaopen_OMSimulatorLua")
OMSimulatorLua()
```
