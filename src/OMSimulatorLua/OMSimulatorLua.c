#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include <OMSimulator.h>

#define REGISTER_LUA_CALL(name) lua_register(L, #name, OMSimulatorLua_##name)

#ifdef _WIN32
  #define DLLEXPORT __declspec(dllexport)
#else
  #define DLLEXPORT
#endif

void push_pointer(lua_State *L, void *p)
{
  void **bp = (void**)lua_newuserdata(L, sizeof(p));
  *bp = p;
}

void* topointer(lua_State *L, int index)
{
  void **bp = (void**)lua_touserdata(L, index);
  return *bp;
}

//void* oms_newModel();
static int OMSimulatorLua_newModel(lua_State *L)
{
  if (lua_gettop(L) != 0)
    return luaL_error(L, "expecting no arguments");

  void *pModel = oms_newModel();
  push_pointer(L, pModel);
  return 1;
}

//void* oms_loadModel(const char* filename);
static int OMSimulatorLua_loadModel(lua_State *L)
{
  if (lua_gettop(L) != 1)
    return luaL_error(L, "expecting exactly 1 argument");
  luaL_checktype(L, 1, LUA_TSTRING);

  const char* filename = lua_tostring(L, -1);
  void *pModel = oms_loadModel(filename);
  push_pointer(L, pModel);
  return 1;
}

//void oms_unload(void* model);
static int OMSimulatorLua_unload(lua_State *L)
{
  if (lua_gettop(L) != 1)
    return luaL_error(L, "expecting exactly 1 argument");
  luaL_checktype(L, 1, LUA_TUSERDATA);

  void *model = topointer(L, 1);
  oms_unload(model);
  return 0;
}

//void oms_instantiateFMU(void* model, const char* filename, const char* instanceName);
static int OMSimulatorLua_instantiateFMU(lua_State *L)
{
  if (lua_gettop(L) != 3)
    return luaL_error(L, "expecting exactly 3 arguments");
  luaL_checktype(L, 1, LUA_TUSERDATA);
  luaL_checktype(L, 2, LUA_TSTRING);
  luaL_checktype(L, 3, LUA_TSTRING);

  void *model = topointer(L, 1);
  const char* filename = lua_tostring(L, 2);
  const char* instanceName = lua_tostring(L, 3);
  oms_instantiateFMU(model, filename, instanceName);
  return 0;
}

//void oms_instantiateTable(void* model, const char* filename, const char* instanceName);
static int OMSimulatorLua_instantiateTable(lua_State *L)
{
  if (lua_gettop(L) != 3)
    return luaL_error(L, "expecting exactly 3 arguments");
  luaL_checktype(L, 1, LUA_TUSERDATA);
  luaL_checktype(L, 2, LUA_TSTRING);
  luaL_checktype(L, 3, LUA_TSTRING);

  void *model = topointer(L, 1);
  const char* filename = lua_tostring(L, 2);
  const char* instanceName = lua_tostring(L, 3);
  oms_instantiateTable(model, filename, instanceName);
  return 0;
}

//void oms_setReal(void* model, const char* var, double value);
static int OMSimulatorLua_setReal(lua_State *L)
{
  if (lua_gettop(L) != 3)
    return luaL_error(L, "expecting exactly 3 arguments");
  luaL_checktype(L, 1, LUA_TUSERDATA);
  luaL_checktype(L, 2, LUA_TSTRING);
  luaL_checktype(L, 3, LUA_TNUMBER);

  void *model = topointer(L, 1);
  const char *var = lua_tostring(L, 2);
  double value = lua_tonumber(L, 3);
  oms_setReal(model, var, value);
  return 0;
}

//void oms_setInteger(void* model, const char* var, int value);
static int OMSimulatorLua_setInteger(lua_State *L)
{
  if (lua_gettop(L) != 3)
    return luaL_error(L, "expecting exactly 3 arguments");
  luaL_checktype(L, 1, LUA_TUSERDATA);
  luaL_checktype(L, 2, LUA_TSTRING);
  luaL_checktype(L, 3, LUA_TNUMBER);

  void *model = topointer(L, 1);
  const char *var = lua_tostring(L, 2);
  int value = lua_tointeger(L, 3);
  oms_setInteger(model, var, value);
  return 0;
}

//void oms_setBoolean(void* model, const char* var, int value);
static int OMSimulatorLua_setBoolean(lua_State *L)
{
  if (lua_gettop(L) != 3)
    return luaL_error(L, "expecting exactly 3 arguments");
  luaL_checktype(L, 1, LUA_TUSERDATA);
  luaL_checktype(L, 2, LUA_TSTRING);
  luaL_checktype(L, 3, LUA_TNUMBER);

  void *model = topointer(L, 1);
  const char *var = lua_tostring(L, 2);
  int value = lua_tointeger(L, 3);
  oms_setBoolean(model, var, value);
  return 0;
}
// TODO: setString

//double oms_getReal(void* model, const char* var);
static int OMSimulatorLua_getReal(lua_State *L)
{
  if (lua_gettop(L) != 2)
    return luaL_error(L, "expecting exactly 2 arguments");
  luaL_checktype(L, 1, LUA_TUSERDATA);
  luaL_checktype(L, 2, LUA_TSTRING);

  void *model = topointer(L, 1);
  const char *var = lua_tostring(L, 2);

  double value = oms_getReal(model, var);
  lua_pushnumber(L, value);
  return 1;
}

//double oms_getInteger(void* model, const char* var);
static int OMSimulatorLua_getInteger(lua_State *L)
{
  if (lua_gettop(L) != 2)
    return luaL_error(L, "expecting exactly 2 arguments");
  luaL_checktype(L, 1, LUA_TUSERDATA);
  luaL_checktype(L, 2, LUA_TSTRING);

  void *model = topointer(L, 1);
  const char *var = lua_tostring(L, 2);

  int value = oms_getInteger(model, var);
  lua_pushnumber(L, value);
  return 1;
}

//double oms_getBoolean(void* model, const char* var);
static int OMSimulatorLua_getBoolean(lua_State *L)
{
  if (lua_gettop(L) != 2)
    return luaL_error(L, "expecting exactly 2 arguments");
  luaL_checktype(L, 1, LUA_TUSERDATA);
  luaL_checktype(L, 2, LUA_TSTRING);

  void *model = topointer(L, 1);
  const char *var = lua_tostring(L, 2);

  int value = oms_getBoolean(model, var);
  lua_pushnumber(L, value);
  return 1;
}
// TODO: getString

//void oms_addConnection(void* model, const char* from, const char* to);
static int OMSimulatorLua_addConnection(lua_State *L)
{
  if (lua_gettop(L) != 3)
    return luaL_error(L, "expecting exactly 3 arguments");
  luaL_checktype(L, 1, LUA_TUSERDATA);
  luaL_checktype(L, 2, LUA_TSTRING);
  luaL_checktype(L, 3, LUA_TSTRING);

  void *model = topointer(L, 1);
  const char *from = lua_tostring(L, 2);
  const char *to = lua_tostring(L, 3);
  oms_addConnection(model, from, to);
  return 0;
}

//oms_status_t oms_simulate(void* model);
static int OMSimulatorLua_simulate(lua_State *L)
{
  if (lua_gettop(L) != 1)
    return luaL_error(L, "expecting exactly 1 argument");
  luaL_checktype(L, 1, LUA_TUSERDATA);

  void *model = topointer(L, 1);
  oms_status_t returnValue = oms_simulate(model);
  lua_pushinteger(L, returnValue);
  return 1;
}

//oms_status_t oms_doSteps(const void* model, const int numberOfSteps);
static int OMSimulatorLua_doSteps(lua_State *L)
{
  if (lua_gettop(L) != 2)
    return luaL_error(L, "expecting exactly 2 argument");
  luaL_checktype(L, 1, LUA_TUSERDATA);
  luaL_checktype(L, 2, LUA_TNUMBER);

  void *model = topointer(L, 1);
  int numberOfSteps = (int)lua_tointeger(L, 2);
  oms_status_t returnValue = oms_doSteps(model, numberOfSteps);
  lua_pushinteger(L, returnValue);
  return 1;
}

//oms_status_t oms_stepUntil(const void* model, const double timeValue);
static int OMSimulatorLua_stepUntil(lua_State *L)
{
  if (lua_gettop(L) != 2)
    return luaL_error(L, "expecting exactly 2 argument");
  luaL_checktype(L, 1, LUA_TUSERDATA);
  luaL_checktype(L, 2, LUA_TNUMBER);

  void *model = topointer(L, 1);
  double timeValue = lua_tonumber(L, 2);
  oms_status_t returnValue = oms_stepUntil(model, timeValue);
  lua_pushinteger(L, returnValue);
  return 1;
}

//void oms_describe(void* model);
static int OMSimulatorLua_describe(lua_State *L)
{
  if (lua_gettop(L) != 1)
    return luaL_error(L, "expecting exactly 1 argument");
  luaL_checktype(L, 1, LUA_TUSERDATA);

  void *model = topointer(L, 1);
  oms_describe(model);
  return 0;
}

//void oms_exportXML(void* model, const char* filename);
static int OMSimulatorLua_exportXML(lua_State *L)
{
  if (lua_gettop(L) != 2)
    return luaL_error(L, "expecting exactly 2 argument");
  luaL_checktype(L, 1, LUA_TUSERDATA);
  luaL_checktype(L, 2, LUA_TSTRING);

  void *model = topointer(L, 1);
  const char* filename = lua_tostring(L, 2);
  oms_exportXML(model, filename);
  return 0;
}

//void oms_importXML(void* model, const char* filename);
static int OMSimulatorLua_importXML(lua_State *L)
{
  if (lua_gettop(L) != 2)
    return luaL_error(L, "expecting exactly 2 argument");
  luaL_checktype(L, 1, LUA_TUSERDATA);
  luaL_checktype(L, 2, LUA_TSTRING);

  void *model = topointer(L, 1);
  const char* filename = lua_tostring(L, 2);
  oms_importXML(model, filename);
  return 0;
}

//void oms_exportDependencyGraph(void* model, const char* prefix);
static int OMSimulatorLua_exportDependencyGraph(lua_State *L)
{
  if (lua_gettop(L) != 2)
    return luaL_error(L, "expecting exactly 2 argument");
  luaL_checktype(L, 1, LUA_TUSERDATA);
  luaL_checktype(L, 2, LUA_TSTRING);

  void *model = topointer(L, 1);
  const char* prefix = lua_tostring(L, 2);
  oms_exportDependencyGraph(model, prefix);
  return 0;
}

//void oms_exportCompositeStructure(void* model, const char* prefix);
static int OMSimulatorLua_exportCompositeStructure(lua_State *L)
{
  if (lua_gettop(L) != 2)
    return luaL_error(L, "expecting exactly 2 argument");
  luaL_checktype(L, 1, LUA_TUSERDATA);
  luaL_checktype(L, 2, LUA_TSTRING);

  void *model = topointer(L, 1);
  const char* prefix = lua_tostring(L, 2);
  oms_exportCompositeStructure(model, prefix);
  return 0;
}

//oms_status_t oms_initialize(void* model);
static int OMSimulatorLua_initialize(lua_State *L)
{
  if (lua_gettop(L) != 1)
    return luaL_error(L, "expecting exactly 1 argument");
  luaL_checktype(L, 1, LUA_TUSERDATA);

  void *model = topointer(L, 1);
  oms_initialize(model);
  return 0;
}

//oms_status_t oms_terminate(void* model);
static int OMSimulatorLua_terminate(lua_State *L)
{
  if (lua_gettop(L) != 1)
    return luaL_error(L, "expecting exactly 1 argument");
  luaL_checktype(L, 1, LUA_TUSERDATA);

  void *model = topointer(L, 1);
  oms_terminate(model);
  return 0;
}

//oms_status_t oms_reset(void* model);
static int OMSimulatorLua_reset(lua_State *L)
{
  if (lua_gettop(L) != 1)
    return luaL_error(L, "expecting exactly 1 argument");
  luaL_checktype(L, 1, LUA_TUSERDATA);

  void *model = topointer(L, 1);
  oms_reset(model);
  return 0;
}

//oms_status_t oms_getCurrentTime(const void* model, double* time);
static int OMSimulatorLua_getCurrentTime(lua_State *L)
{
  if (lua_gettop(L) != 1)
    return luaL_error(L, "expecting exactly 1 argument");
  luaL_checktype(L, 1, LUA_TUSERDATA);

  void *model = topointer(L, 1);

  double time = 0.0;
  oms_status_t status = oms_getCurrentTime(model, &time);
  lua_pushnumber(L, time);
  return 1;
}

//void oms_setTempDirectory(const char* filename);
static int OMSimulatorLua_setTempDirectory(lua_State *L)
{
  if (lua_gettop(L) != 1)
    return luaL_error(L, "expecting exactly 1 argument");
  luaL_checktype(L, 1, LUA_TSTRING);

  const char* filename = lua_tostring(L, 1);
  oms_setTempDirectory(filename);
  return 0;
}

//void oms_setWorkingDirectory(const char* path);
static int OMSimulatorLua_setWorkingDirectory(lua_State *L)
{
  if (lua_gettop(L) != 1)
    return luaL_error(L, "expecting exactly 1 argument");
  luaL_checktype(L, 1, LUA_TSTRING);

  const char* path = lua_tostring(L, 1);
  oms_setWorkingDirectory(path);
  return 0;
}

//void oms_setStartTime(void* model, double startTime);
static int OMSimulatorLua_setStartTime(lua_State *L)
{
  if (lua_gettop(L) != 2)
    return luaL_error(L, "expecting exactly 2 argument");
  luaL_checktype(L, 1, LUA_TUSERDATA);
  luaL_checktype(L, 2, LUA_TNUMBER);

  void *model = topointer(L, 1);
  double startTime = lua_tonumber(L, 2);
  oms_setStartTime(model, startTime);
  return 0;
}

//void oms_setStopTime(void* model, double stopTime);
static int OMSimulatorLua_setStopTime(lua_State *L)
{
  if (lua_gettop(L) != 2)
    return luaL_error(L, "expecting exactly 2 argument");
  luaL_checktype(L, 1, LUA_TUSERDATA);
  luaL_checktype(L, 2, LUA_TNUMBER);

  void *model = topointer(L, 1);
  double stopTime = lua_tonumber(L, 2);
  oms_setStopTime(model, stopTime);
  return 0;
}

//void oms_setTolerance(void* model, double tolerance);
static int OMSimulatorLua_setTolerance(lua_State *L)
{
  if (lua_gettop(L) != 2)
    return luaL_error(L, "expecting exactly 2 argument");
  luaL_checktype(L, 1, LUA_TUSERDATA);
  luaL_checktype(L, 2, LUA_TNUMBER);

  void *model = topointer(L, 1);
  double tolerance = lua_tonumber(L, 2);
  oms_setTolerance(model, tolerance);
  return 0;
}

//void oms_setCommunicationInterval(void* model, double communicationInterval);
static int OMSimulatorLua_setCommunicationInterval(lua_State *L)
{
  if (lua_gettop(L) != 2)
    return luaL_error(L, "expecting exactly 2 argument");
  luaL_checktype(L, 1, LUA_TUSERDATA);
  luaL_checktype(L, 2, LUA_TNUMBER);

  void *model = topointer(L, 1);
  double communicationInterval = lua_tonumber(L, 2);
  oms_setCommunicationInterval(model, communicationInterval);
  return 0;
}

//void oms_setResultFile(void* model, const char* filename);
static int OMSimulatorLua_setResultFile(lua_State *L)
{
  if (lua_gettop(L) != 2)
    return luaL_error(L, "expecting exactly 2 argument");
  luaL_checktype(L, 1, LUA_TUSERDATA);
  luaL_checktype(L, 2, LUA_TSTRING);

  void *model = topointer(L, 1);
  const char* filename = lua_tostring(L, 2);
  oms_setResultFile(model, filename);
  return 0;
}

//void oms_setSolverMethod(void* model, const char* instanceName, const char* method);
static int OMSimulatorLua_setSolverMethod(lua_State *L)
{
  if (lua_gettop(L) != 3)
    return luaL_error(L, "expecting exactly 3 argument");
  luaL_checktype(L, 1, LUA_TUSERDATA);
  luaL_checktype(L, 2, LUA_TSTRING);
  luaL_checktype(L, 3, LUA_TSTRING);

  void *model = topointer(L, 1);
  const char* instanceName = lua_tostring(L, 2);
  const char* method = lua_tostring(L, 3);
  oms_setSolverMethod(model, instanceName, method);
  return 0;
}

//void oms_setLogFile(const char* filename);
static int OMSimulatorLua_setLogFile(lua_State *L)
{
  if (lua_gettop(L) != 1)
    return luaL_error(L, "expecting exactly 1 argument");
  luaL_checktype(L, 1, LUA_TSTRING);

  const char* filename = lua_tostring(L, 1);
  oms_setLogFile(filename);
  return 0;
}

//const char* oms_getVersion();
static int OMSimulatorLua_getVersion(lua_State *L)
{
  if (lua_gettop(L) != 0)
    return luaL_error(L, "expecting no arguments");

  const char* version = oms_getVersion();
  lua_pushstring(L, version);
  return 1;
}

//int oms_compareSimulationResults(const char* filenameA, const char* filenameB, const char* var, double relTol, double absTol);
static int OMSimulatorLua_compareSimulationResults(lua_State *L)
{
  if (lua_gettop(L) != 5)
    return luaL_error(L, "expecting exactly 5 arguments");
  luaL_checktype(L, 1, LUA_TSTRING);
  luaL_checktype(L, 2, LUA_TSTRING);
  luaL_checktype(L, 3, LUA_TSTRING);
  luaL_checktype(L, 4, LUA_TNUMBER);
  luaL_checktype(L, 5, LUA_TNUMBER);

  const char *filenameA = lua_tostring(L, 1);
  const char *filenameB = lua_tostring(L, 2);
  const char *var = lua_tostring(L, 3);
  double relTol = lua_tonumber(L, 4);
  double absTol = lua_tonumber(L, 5);
  int rc = oms_compareSimulationResults(filenameA, filenameB, var, relTol, absTol);
  lua_pushinteger(L, rc);
  return 1;
}

//void oms_setVariableFilter(void* model, const char* instanceFilter, const char* variableFilter);
static int OMSimulatorLua_setVariableFilter(lua_State *L)
{
  if (lua_gettop(L) != 3)
    return luaL_error(L, "expecting exactly 3 arguments");
  luaL_checktype(L, 1, LUA_TUSERDATA);
  luaL_checktype(L, 2, LUA_TSTRING);
  luaL_checktype(L, 3, LUA_TSTRING);

  void *model = topointer(L, 1);
  const char *instanceFilter = lua_tostring(L, 2);
  const char *variableFilter = lua_tostring(L, 3);
  oms_setVariableFilter(model, instanceFilter, variableFilter);
  return 0;
}

DLLEXPORT int luaopen_OMSimulatorLua(lua_State *L)
{
  REGISTER_LUA_CALL(addConnection);
  REGISTER_LUA_CALL(compareSimulationResults);
  REGISTER_LUA_CALL(describe);
  REGISTER_LUA_CALL(doSteps);
  REGISTER_LUA_CALL(exportDependencyGraph);
  REGISTER_LUA_CALL(exportCompositeStructure);
  REGISTER_LUA_CALL(exportXML);
  REGISTER_LUA_CALL(getCurrentTime);
  REGISTER_LUA_CALL(getReal);
  REGISTER_LUA_CALL(getInteger);
  REGISTER_LUA_CALL(getBoolean);
  REGISTER_LUA_CALL(getVersion);
  REGISTER_LUA_CALL(importXML);
  REGISTER_LUA_CALL(initialize);
  REGISTER_LUA_CALL(instantiateFMU);
  REGISTER_LUA_CALL(instantiateTable);
  REGISTER_LUA_CALL(loadModel);
  REGISTER_LUA_CALL(setLogFile);
  REGISTER_LUA_CALL(newModel);
  REGISTER_LUA_CALL(reset);
  REGISTER_LUA_CALL(setCommunicationInterval);
  REGISTER_LUA_CALL(setReal);
  REGISTER_LUA_CALL(setInteger);
  REGISTER_LUA_CALL(setBoolean);
  REGISTER_LUA_CALL(setResultFile);
  REGISTER_LUA_CALL(setSolverMethod);
  REGISTER_LUA_CALL(setStartTime);
  REGISTER_LUA_CALL(setStopTime);
  REGISTER_LUA_CALL(setTempDirectory);
  REGISTER_LUA_CALL(setTolerance);
  REGISTER_LUA_CALL(setVariableFilter);
  REGISTER_LUA_CALL(setWorkingDirectory);
  REGISTER_LUA_CALL(simulate);
  REGISTER_LUA_CALL(stepUntil);
  REGISTER_LUA_CALL(terminate);
  REGISTER_LUA_CALL(unload);
  return 0;
}
