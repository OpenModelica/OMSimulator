#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include <OMSimulator.h>

#ifdef WITH_OMSYSIDENT
#include <OMSysIdent.h>
#endif /* WITH_OMSYSIDENT */

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

/* ************************************ */
/* OMSimulator 2.0                      */
/*                                      */
/* TODO: replace prefix oms2 with oms   */
/* ************************************ */

//const char* oms2_getVersion();
static int OMSimulatorLua_oms2_getVersion(lua_State *L)
{
  if (lua_gettop(L) != 0)
    return luaL_error(L, "expecting no arguments");

  const char* version = oms2_getVersion();
  lua_pushstring(L, version);
  return 1;
}

//int oms2_compareSimulationResults(const char* filenameA, const char* filenameB, const char* var, double relTol, double absTol);
static int OMSimulatorLua_oms2_compareSimulationResults(lua_State *L)
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
  int rc = oms2_compareSimulationResults(filenameA, filenameB, var, relTol, absTol);
  lua_pushinteger(L, rc);
  return 1;
}

//oms_status_enu_t oms2_newFMIModel(const char* ident);
static int OMSimulatorLua_oms2_newFMIModel(lua_State *L)
{
  if (lua_gettop(L) != 1)
    return luaL_error(L, "expecting exactly 1 argument");
  luaL_checktype(L, 1, LUA_TSTRING);

  const char* ident = lua_tostring(L, 1);
  oms_status_enu_t status = oms2_newFMIModel(ident);
  lua_pushinteger(L, status);
  return 1;
}

//oms_status_enu_t oms2_newTLMModel(const char* ident);
static int OMSimulatorLua_oms2_newTLMModel(lua_State *L)
{
  if (lua_gettop(L) != 1)
    return luaL_error(L, "expecting exactly 1 argument");
  luaL_checktype(L, 1, LUA_TSTRING);

  const char* ident = lua_tostring(L, 1);
  oms_status_enu_t status = oms2_newTLMModel(ident);
  lua_pushinteger(L, status);
  return 1;
}

//oms_status_enu_t oms2_unloadModel(const char* ident);
static int OMSimulatorLua_oms2_unloadModel(lua_State *L)
{
  if (lua_gettop(L) != 1)
    return luaL_error(L, "expecting exactly 1 argument");
  luaL_checktype(L, 1, LUA_TSTRING);

  const char* ident = lua_tostring(L, 1);
  oms_status_enu_t status = oms2_unloadModel(ident);
  lua_pushinteger(L, status);
  return 1;
}

//oms_status_enu_t oms2_addFMU(const char* modelIdent, const char* fmuPath, const char* fmuIdent);
static int OMSimulatorLua_oms2_addFMU(lua_State *L)
{
  if (lua_gettop(L) != 3)
    return luaL_error(L, "expecting exactly 3 argument");
  luaL_checktype(L, 1, LUA_TSTRING);
  luaL_checktype(L, 2, LUA_TSTRING);
  luaL_checktype(L, 3, LUA_TSTRING);

  const char* modelIdent = lua_tostring(L, 1);
  const char* fmuPath = lua_tostring(L, 2);
  const char* fmuIdent = lua_tostring(L, 3);
  oms_status_enu_t status = oms2_addFMU(modelIdent, fmuPath, fmuIdent);
  lua_pushinteger(L, status);
  return 1;
}

//oms_status_enu_t oms2_addTable(const char* modelIdent, const char* tablePath, const char* tableIdent);
static int OMSimulatorLua_oms2_addTable(lua_State *L)
{
  if (lua_gettop(L) != 3)
    return luaL_error(L, "expecting exactly 3 argument");
  luaL_checktype(L, 1, LUA_TSTRING);
  luaL_checktype(L, 2, LUA_TSTRING);
  luaL_checktype(L, 3, LUA_TSTRING);

  const char* modelIdent = lua_tostring(L, 1);
  const char* tablePath = lua_tostring(L, 2);
  const char* tableIdent = lua_tostring(L, 3);
  oms_status_enu_t status = oms2_addTable(modelIdent, tablePath, tableIdent);
  lua_pushinteger(L, status);
  return 1;
}

//oms_status_enu_t oms2_deleteSubModel(const char* modelIdent, const char* subModelIdent);
static int OMSimulatorLua_oms2_deleteSubModel(lua_State *L)
{
  if (lua_gettop(L) != 2)
    return luaL_error(L, "expecting exactly 2 argument");
  luaL_checktype(L, 1, LUA_TSTRING);
  luaL_checktype(L, 2, LUA_TSTRING);

  const char* modelIdent = lua_tostring(L, 1);
  const char* fmuIdent = lua_tostring(L, 2);
  oms_status_enu_t status = oms2_deleteSubModel(modelIdent, fmuIdent);
  lua_pushinteger(L, status);
  return 1;
}

//oms_status_enu_t oms2_rename(const char* identOld, const char* identNew);
static int OMSimulatorLua_oms2_rename(lua_State *L)
{
  if (lua_gettop(L) != 2)
    return luaL_error(L, "expecting exactly 2 argument");
  luaL_checktype(L, 1, LUA_TSTRING);
  luaL_checktype(L, 2, LUA_TSTRING);

  const char* identOld = lua_tostring(L, 1);
  const char* identNew = lua_tostring(L, 2);
  oms_status_enu_t status = oms2_rename(identOld, identNew);
  lua_pushinteger(L, status);
  return 1;
}

// TODO: oms_status_enu_t oms2_getComponentType(const char* ident, oms_element_type_enu_t* type);

//oms_status_enu_t oms2_loadModel(const char* filename, char** ident);
static int OMSimulatorLua_oms2_loadModel(lua_State *L)
{
  if (lua_gettop(L) != 1)
    return luaL_error(L, "expecting exactly 1 argument");
  luaL_checktype(L, 1, LUA_TSTRING);

  const char* filename = lua_tostring(L, 1);
  char* ident = NULL;
  oms_status_enu_t status = oms2_loadModel(filename, &ident);

  lua_pushinteger(L, status);
  if (ident)
    lua_pushstring(L, ident);
  else
    lua_pushstring(L, "");
  return 2;
}

//oms_status_enu_t oms2_saveModel(const char* filename, const char* ident);
static int OMSimulatorLua_oms2_saveModel(lua_State *L)
{
  if (lua_gettop(L) != 2)
    return luaL_error(L, "expecting exactly 2 argument");
  luaL_checktype(L, 1, LUA_TSTRING);
  luaL_checktype(L, 2, LUA_TSTRING);

  const char* filename = lua_tostring(L, 1);
  const char* ident = lua_tostring(L, 2);
  oms_status_enu_t status = oms2_saveModel(filename, ident);

  lua_pushinteger(L, status);
  return 1;
}

// TODO: oms_status_enu_t oms2_getElement(const char* cref, const ssd_element_geometry_t** geometry);
// TODO: oms_status_enu_t oms2_setElementGeometry(const char* cref, const ssd_element_geometry_t* geometry);

//oms_status_enu_t oms2_getElements(const char* cref, oms_element_t*** components);
static int OMSimulatorLua_oms2_getElements(lua_State *L)
{
  if (lua_gettop(L) != 1)
    return luaL_error(L, "expecting exactly 1 argument");
  luaL_checktype(L, 1, LUA_TSTRING);

  const char* cref = lua_tostring(L, 1);
  oms_element_t** components = NULL;
  oms_status_enu_t status = oms2_getElements(cref, &components);

  lua_pushinteger(L, status);
  push_pointer(L, components);
  return 2;
}

// TODO: oms_status_enu_t oms2_getConnections(const char* cref, oms_connection_t*** connections);

//oms_status_enu_t oms2_addConnection(const char* cref, const char* conA, const char* conB);
static int OMSimulatorLua_oms2_addConnection(lua_State *L)
{
  if (lua_gettop(L) != 3)
    return luaL_error(L, "expecting exactly 3 argument");
  luaL_checktype(L, 1, LUA_TSTRING);
  luaL_checktype(L, 2, LUA_TSTRING);
  luaL_checktype(L, 3, LUA_TSTRING);

  const char* cref = lua_tostring(L, 1);
  const char* conA = lua_tostring(L, 2);
  const char* conB = lua_tostring(L, 3);
  oms_status_enu_t status = oms2_addConnection(cref, conA, conB);

  lua_pushinteger(L, status);
  return 1;
}

//oms_status_enu_t oms2_deleteConnection(const char* cref, const char* conA, const char* conB);
static int OMSimulatorLua_oms2_deleteConnection(lua_State *L)
{
  if (lua_gettop(L) != 3)
    return luaL_error(L, "expecting exactly 3 argument");
  luaL_checktype(L, 1, LUA_TSTRING);
  luaL_checktype(L, 2, LUA_TSTRING);
  luaL_checktype(L, 3, LUA_TSTRING);

  const char* cref = lua_tostring(L, 1);
  const char* conA = lua_tostring(L, 2);
  const char* conB = lua_tostring(L, 3);
  oms_status_enu_t status = oms2_deleteConnection(cref, conA, conB);

  lua_pushinteger(L, status);
  return 1;
}

// TODO: oms_status_enu_t oms2_updateConnection(const char* cref, const char* conA, const char* conB, const oms_connection_t* connection);

//void oms2_setLoggingLevel(int logLevel);
static int OMSimulatorLua_oms2_setLoggingLevel(lua_State *L)
{
  if (lua_gettop(L) != 1)
    return luaL_error(L, "expecting exactly 1 argument");
  luaL_checktype(L, 1, LUA_TNUMBER);

  int logLevel = lua_tointeger(L, 1);
  oms2_setLoggingLevel(logLevel);
  return 0;
}

//oms_status_enu_t oms2_setTempDirectory(const char* path);
static int OMSimulatorLua_oms2_setTempDirectory(lua_State *L)
{
  if (lua_gettop(L) != 1)
    return luaL_error(L, "expecting exactly 1 argument");
  luaL_checktype(L, 1, LUA_TSTRING);

  const char* path = lua_tostring(L, 1);
  oms_status_enu_t status = oms2_setTempDirectory(path);
  lua_pushinteger(L, status);
  return 1;
}

//oms_status_enu_t oms2_setWorkingDirectory(const char* path);
static int OMSimulatorLua_oms2_setWorkingDirectory(lua_State *L)
{
  if (lua_gettop(L) != 1)
    return luaL_error(L, "expecting exactly 1 argument");
  luaL_checktype(L, 1, LUA_TSTRING);

  const char* path = lua_tostring(L, 1);
  oms_status_enu_t status = oms2_setWorkingDirectory(path);
  lua_pushinteger(L, status);
  return 1;
}

//oms_status_enu_t oms2_setLogFile(const char* filename);
static int OMSimulatorLua_oms2_setLogFile(lua_State *L)
{
  if (lua_gettop(L) != 1)
    return luaL_error(L, "expecting exactly 1 argument");
  luaL_checktype(L, 1, LUA_TSTRING);

  const char* filename = lua_tostring(L, 1);
  oms_status_enu_t status = oms2_setLogFile(filename);
  lua_pushinteger(L, status);
  return 1;
}

//oms_status_enu_t oms2_setRealParameter(const char* signal, double value);
static int OMSimulatorLua_oms2_setRealParameter(lua_State *L)
{
  if (lua_gettop(L) != 2)
    return luaL_error(L, "expecting exactly 2 argument");
  luaL_checktype(L, 1, LUA_TSTRING);
  luaL_checktype(L, 2, LUA_TNUMBER);

  const char* signal = lua_tostring(L, 1);
  double value = lua_tonumber(L, 2);
  oms_status_enu_t status = oms2_setRealParameter(signal, value);
  lua_pushinteger(L, status);
  return 1;
}

//oms_status_enu_t oms2_setStartTime(const char* cref, double startTime);
static int OMSimulatorLua_oms2_setStartTime(lua_State *L)
{
  if (lua_gettop(L) != 2)
    return luaL_error(L, "expecting exactly 2 argument");
  luaL_checktype(L, 1, LUA_TSTRING);
  luaL_checktype(L, 2, LUA_TNUMBER);

  const char* cref = lua_tostring(L, 1);
  double startTime = lua_tonumber(L, 2);
  oms_status_enu_t status = oms2_setStartTime(cref, startTime);
  lua_pushinteger(L, status);
  return 1;
}

//oms_status_enu_t oms2_setStopTime(const char* cref, double stopTime);
static int OMSimulatorLua_oms2_setStopTime(lua_State *L)
{
  if (lua_gettop(L) != 2)
    return luaL_error(L, "expecting exactly 2 argument");
  luaL_checktype(L, 1, LUA_TSTRING);
  luaL_checktype(L, 2, LUA_TNUMBER);

  const char* cref = lua_tostring(L, 1);
  double startTime = lua_tonumber(L, 2);
  oms_status_enu_t status = oms2_setStopTime(cref, startTime);
  lua_pushinteger(L, status);
  return 1;
}

//oms_status_enu_t oms2_setCommunicationInterval(const char* cref, double communicationInterval);
static int OMSimulatorLua_oms2_setCommunicationInterval(lua_State *L)
{
  if (lua_gettop(L) != 2)
    return luaL_error(L, "expecting exactly 2 argument");
  luaL_checktype(L, 1, LUA_TSTRING);
  luaL_checktype(L, 2, LUA_TNUMBER);

  const char* cref = lua_tostring(L, 1);
  double communicationInterval = lua_tonumber(L, 2);
  oms_status_enu_t status = oms2_setCommunicationInterval(cref, communicationInterval);
  lua_pushinteger(L, status);
  return 1;
}

//oms_status_enu_t oms2_setResultFile(const char* cref, const char* filename);
static int OMSimulatorLua_oms2_setResultFile(lua_State *L)
{
  if (lua_gettop(L) != 2)
    return luaL_error(L, "expecting exactly 2 argument");
  luaL_checktype(L, 1, LUA_TSTRING);
  luaL_checktype(L, 2, LUA_TSTRING);

  const char* cref = lua_tostring(L, 1);
  const char* filename = lua_tostring(L, 2);
  oms_status_enu_t status = oms2_setResultFile(cref, filename);
  lua_pushinteger(L, status);
  return 1;
}

//oms_status_enu_t oms2_setMasterAlgorithm(const char* cref, const char* masterAlgorithm);
static int OMSimulatorLua_oms2_setMasterAlgorithm(lua_State *L)
{
  if (lua_gettop(L) != 2)
    return luaL_error(L, "expecting exactly 2 argument");
  luaL_checktype(L, 1, LUA_TSTRING);
  luaL_checktype(L, 2, LUA_TSTRING);

  const char* cref = lua_tostring(L, 1);
  const char* masterAlgorithm = lua_tostring(L, 2);
  oms_status_enu_t status = oms2_setMasterAlgorithm(cref, masterAlgorithm);
  lua_pushinteger(L, status);
  return 1;
}

//oms_status_enu_t experimental_setActivationRatio(const char* cref, int k);
static int OMSimulatorLua_experimental_setActivationRatio(lua_State *L)
{
  if (lua_gettop(L) != 2)
    return luaL_error(L, "expecting exactly 2 argument");
  luaL_checktype(L, 1, LUA_TSTRING);
  luaL_checktype(L, 2, LUA_TNUMBER);

  const char* cref = lua_tostring(L, 1);
  int k = lua_tointeger(L, 2);
  oms_status_enu_t status = experimental_setActivationRatio(cref, k);
  lua_pushinteger(L, status);
  return 1;
}

//oms_status_enu_t experimental_simulate_realtime(const char* ident);
static int OMSimulatorLua_experimental_simulate_realtime(lua_State *L)
{
  if (lua_gettop(L) != 1)
    return luaL_error(L, "expecting exactly 1 argument");
  luaL_checktype(L, 1, LUA_TSTRING);

  const char* ident = lua_tostring(L, 1);
  oms_status_enu_t status = experimental_simulate_realtime(ident);
  lua_pushinteger(L, status);
  return 1;
}

//oms_status_enu_t oms2_exportCompositeStructure(const char* cref, const char* filename);
static int OMSimulatorLua_oms2_exportCompositeStructure(lua_State *L)
{
  if (lua_gettop(L) != 2)
    return luaL_error(L, "expecting exactly 2 argument");
  luaL_checktype(L, 1, LUA_TSTRING);
  luaL_checktype(L, 2, LUA_TSTRING);

  const char* cref = lua_tostring(L, 1);
  const char* filename = lua_tostring(L, 2);
  oms_status_enu_t status = oms2_exportCompositeStructure(cref, filename);
  lua_pushinteger(L, status);
  return 1;
}

//oms_status_enu_t oms2_exportDependencyGraphs(const char* cref, const char* initialization, const char* simulation);
static int OMSimulatorLua_oms2_exportDependencyGraphs(lua_State *L)
{
  if (lua_gettop(L) != 3)
    return luaL_error(L, "expecting exactly 3 argument");
  luaL_checktype(L, 1, LUA_TSTRING);
  luaL_checktype(L, 2, LUA_TSTRING);
  luaL_checktype(L, 3, LUA_TSTRING);

  const char* cref = lua_tostring(L, 1);
  const char* initialization = lua_tostring(L, 2);
  const char* simulation = lua_tostring(L, 3);
  oms_status_enu_t status = oms2_exportDependencyGraphs(cref, initialization, simulation);
  lua_pushinteger(L, status);
  return 1;
}

//oms_status_enu_t oms2_initialize(const char* ident);
static int OMSimulatorLua_oms2_initialize(lua_State *L)
{
  if (lua_gettop(L) != 1)
    return luaL_error(L, "expecting exactly 1 argument");
  luaL_checktype(L, 1, LUA_TSTRING);

  const char* ident = lua_tostring(L, 1);
  oms_status_enu_t status = oms2_initialize(ident);
  lua_pushinteger(L, status);
  return 1;
}

//oms_status_enu_t oms2_reset(const char* ident);
static int OMSimulatorLua_oms2_reset(lua_State *L)
{
  if (lua_gettop(L) != 1)
    return luaL_error(L, "expecting exactly 1 argument");
  luaL_checktype(L, 1, LUA_TSTRING);

  const char* ident = lua_tostring(L, 1);
  oms_status_enu_t status = oms2_reset(ident);
  lua_pushinteger(L, status);
  return 1;
}

//oms_status_enu_t oms2_terminate(const char* ident);
static int OMSimulatorLua_oms2_terminate(lua_State *L)
{
  if (lua_gettop(L) != 1)
    return luaL_error(L, "expecting exactly 1 argument");
  luaL_checktype(L, 1, LUA_TSTRING);

  const char* ident = lua_tostring(L, 1);
  oms_status_enu_t status = oms2_terminate(ident);
  lua_pushinteger(L, status);
  return 1;
}

//oms_status_enu_t oms2_simulate(const char* ident);
static int OMSimulatorLua_oms2_simulate(lua_State *L)
{
  if (lua_gettop(L) != 1)
    return luaL_error(L, "expecting exactly 1 argument");
  luaL_checktype(L, 1, LUA_TSTRING);

  const char* ident = lua_tostring(L, 1);
  oms_status_enu_t status = oms2_simulate(ident);
  lua_pushinteger(L, status);
  return 1;
}

//oms_status_enu_t oms2_doSteps(const char* ident, const int numberOfSteps);
static int OMSimulatorLua_oms2_doSteps(lua_State *L)
{
  if (lua_gettop(L) != 2)
    return luaL_error(L, "expecting exactly 2 argument");
  luaL_checktype(L, 1, LUA_TSTRING);
  luaL_checktype(L, 2, LUA_TNUMBER);

  const char* ident = lua_tostring(L, 1);
  const int numberOfSteps = lua_tointeger(L, 2);
  oms_status_enu_t status = oms2_doSteps(ident, numberOfSteps);
  lua_pushinteger(L, status);
  return 1;
}

//oms_status_enu_t oms2_stepUntil(const char* ident, const double timeValue);
static int OMSimulatorLua_oms2_stepUntil(lua_State *L)
{
  if (lua_gettop(L) != 2)
    return luaL_error(L, "expecting exactly 2 argument");
  luaL_checktype(L, 1, LUA_TSTRING);
  luaL_checktype(L, 2, LUA_TNUMBER);

  const char* ident = lua_tostring(L, 1);
  const double timeValue = lua_tonumber(L, 2);
  oms_status_enu_t status = oms2_stepUntil(ident, timeValue);
  lua_pushinteger(L, status);
  return 1;
}

//oms_status_enu_t oms2_getReal(const char* ident, double* value);
static int OMSimulatorLua_oms2_getReal(lua_State *L)
{
  if (lua_gettop(L) != 1)
    return luaL_error(L, "expecting exactly 1 arguments");
  luaL_checktype(L, 1, LUA_TSTRING);

  const char *ident = lua_tostring(L, 1);
  double value = 0.0;

  oms_status_enu_t status = oms2_getReal(ident, &value);
  lua_pushnumber(L, value);
  lua_pushnumber(L, status);
  return 2;
}

//oms_status_enu_t oms2_setReal(const char* signal, double value);
static int OMSimulatorLua_oms2_setReal(lua_State *L)
{
  if (lua_gettop(L) != 2)
    return luaL_error(L, "expecting exactly 2 argument");
  luaL_checktype(L, 1, LUA_TSTRING);
  luaL_checktype(L, 2, LUA_TNUMBER);

  const char* signal = lua_tostring(L, 1);
  double value = lua_tonumber(L, 2);
  oms_status_enu_t status = oms2_setReal(signal, value);
  lua_pushinteger(L, status);
  return 1;
}

//oms_status_enu_t oms2_getInteger(const char* ident, int* value);
static int OMSimulatorLua_oms2_getInteger(lua_State *L)
{
  if (lua_gettop(L) != 1)
    return luaL_error(L, "expecting exactly 1 arguments");
  luaL_checktype(L, 1, LUA_TSTRING);

  const char *ident = lua_tostring(L, 1);
  int value = 0;

  oms_status_enu_t status = oms2_getInteger(ident, &value);
  lua_pushnumber(L, value);
  lua_pushnumber(L, status);
  return 2;
}

//oms_status_enu_t oms2_setInteger(const char* signal, int value);
static int OMSimulatorLua_oms2_setInteger(lua_State *L)
{
  if (lua_gettop(L) != 2)
    return luaL_error(L, "expecting exactly 2 argument");
  luaL_checktype(L, 1, LUA_TSTRING);
  luaL_checktype(L, 2, LUA_TNUMBER);

  const char* signal = lua_tostring(L, 1);
  int value = lua_tointeger(L, 2);
  oms_status_enu_t status = oms2_setInteger(signal, value);
  lua_pushinteger(L, status);
  return 1;
}

//oms_status_enu_t oms2_getBoolean(const char* ident, bool* value);
static int OMSimulatorLua_oms2_getBoolean(lua_State *L)
{
  if (lua_gettop(L) != 1)
    return luaL_error(L, "expecting exactly 1 arguments");
  luaL_checktype(L, 1, LUA_TSTRING);

  const char *ident = lua_tostring(L, 1);
  bool value = false;

  oms_status_enu_t status = oms2_getBoolean(ident, &value);
  lua_pushnumber(L, value);
  lua_pushnumber(L, status);
  return 2;
}

//oms_status_enu_t oms2_setBoolean(const char* signal, bool value);
static int OMSimulatorLua_oms2_setBoolean(lua_State *L)
{
  if (lua_gettop(L) != 2)
    return luaL_error(L, "expecting exactly 2 argument");
  luaL_checktype(L, 1, LUA_TSTRING);
  luaL_checktype(L, 2, LUA_TNUMBER);

  const char* signal = lua_tostring(L, 1);
  bool value = lua_tointeger(L, 2);
  oms_status_enu_t status = oms2_setBoolean(signal, value);
  lua_pushinteger(L, status);
  return 1;
}

//oms_status_enu_t oms2_getCurrentTime(const char* model, double* time);
static int OMSimulatorLua_oms2_getCurrentTime(lua_State *L)
{
  if (lua_gettop(L) != 1)
    return luaL_error(L, "expecting exactly 1 argument");
  luaL_checktype(L, 1, LUA_TSTRING);

  const char *model = lua_tostring(L, 1);

  double time = 0.0;
  oms_status_enu_t status = oms2_getCurrentTime(model, &time);
  lua_pushnumber(L, time);
  return 1;
}

//oms_status_enu_t oms2_addExternalModel(const char *cref, const char *name, const char *modelfile, const char *startscript);
static int OMSimulatorLua_oms2_addExternalModel(lua_State *L)
{
  if (lua_gettop(L) != 4)
    return luaL_error(L, "expecting exactly 4 arguments");
  luaL_checktype(L, 1, LUA_TSTRING);
  luaL_checktype(L, 2, LUA_TSTRING);
  luaL_checktype(L, 3, LUA_TSTRING);
  luaL_checktype(L, 4, LUA_TSTRING);

  const char *cref =        lua_tostring(L, 1);
  const char *name =        lua_tostring(L, 2);
  const char *modelfile =   lua_tostring(L, 3);
  const char *startscript = lua_tostring(L, 4);

  oms_status_enu_t status = oms2_addExternalModel(cref, name, modelfile, startscript);
  lua_pushnumber(L, status);
  return 1;
}

//oms_status_enu_t oms2_addExternalModel(const char *cref, const char *subref);
static int OMSimulatorLua_oms2_addFMISubModel(lua_State *L)
{
  if (lua_gettop(L) != 2)
    return luaL_error(L, "expecting exactly 2 arguments");
  luaL_checktype(L, 1, LUA_TSTRING);
  luaL_checktype(L, 2, LUA_TSTRING);

  const char *cref =        lua_tostring(L, 1);
  const char *subref =        lua_tostring(L, 2);

  oms_status_enu_t status = oms2_addFMISubModel(cref, subref);
  lua_pushnumber(L, status);
  return 1;
}

//oms_status_enu_t oms2_addTLMInterface(const char *cref, const char *subref, const char *name, int dimensions, oms_causality_enu_t causality, const char *domain);
static int OMSimulatorLua_oms2_addTLMInterface(lua_State *L)
{
  //First parse initial arguments (6 or 7)
  if(lua_gettop(L) < 6) {
    return luaL_error(L, "expecting at least 6 arguments");
  }

  luaL_checktype(L, 1, LUA_TSTRING);
  luaL_checktype(L, 2, LUA_TSTRING);
  luaL_checktype(L, 3, LUA_TSTRING);
  luaL_checktype(L, 4, LUA_TNUMBER);
  luaL_checktype(L, 5, LUA_TNUMBER);
  luaL_checktype(L, 6, LUA_TSTRING);

  const char *cref =    lua_tostring(L, 1);
  const char *subref =  lua_tostring(L, 2);
  const char *name =    lua_tostring(L, 3);
  int dimensions =      lua_tonumber(L, 4);
  int causality =       lua_tonumber(L, 5);
  const char *domain =  lua_tostring(L, 6);
  int interpolation = 0;
  int initialArguments = 6;
  if(lua_gettop(L) >= 7 && lua_type(L, 7) == LUA_TNUMBER) {
    interpolation = lua_tonumber(L, 7);
    initialArguments++;
  }

  //Figure out how many signal references to expect
  int nsigrefs;
  if(lua_gettop(L) == initialArguments) {
    nsigrefs = 0;             //None provided (= external tool interface)
  }
  else if(dimensions == 1 &&
     causality != oms_causality_bidir) {
    nsigrefs = 1;
  }
  else if(dimensions == 1 && causality == oms_causality_bidir && interpolation == oms_tlm_no_interpolation) {
    nsigrefs = 3;
  }
  else if(dimensions == 1 && causality == oms_causality_bidir && interpolation == oms_tlm_coarse_grained) {
    nsigrefs = 4;
  }
  else if(dimensions == 1 && causality == oms_causality_bidir && interpolation == oms_tlm_fine_grained) {
    nsigrefs = 23;
  }
  else if(dimensions == 3 && causality == oms_causality_bidir && interpolation == oms_tlm_no_interpolation) {
    nsigrefs = 24;
  }
  else if(dimensions == 3 && causality == oms_causality_bidir && interpolation == oms_tlm_coarse_grained) {
    nsigrefs = 26;
  }
  else if(dimensions == 3 && causality == oms_causality_bidir && interpolation == oms_tlm_fine_grained) {
    nsigrefs = 90;
  }

  if(lua_gettop(L) != initialArguments+nsigrefs) {
    char msg[100];
    sprintf(msg, "expecting exactly %d arguments.", initialArguments+nsigrefs);
    return luaL_error(L, msg);
  }
  for(int i=1; i<nsigrefs+1; ++i) {
    luaL_checktype(L, initialArguments+i, LUA_TSTRING);
  }

  const char *sigrefs[100];
  for(int i=0; i<nsigrefs; ++i) {
    sigrefs[i] = lua_tostring(L, initialArguments+i+1);
  }

  oms_status_enu_t status = oms2_addTLMInterface(cref, subref, name, dimensions, (oms_causality_enu_t)causality, (oms_tlm_interpolation_t)interpolation, domain, sigrefs, nsigrefs);
  lua_pushnumber(L, status);
  return 1;
}

//oms_status_enu_t oms2_addTLMConnection(const char *cref, const char *from, const char *to, double delay, double alpha, double Zf, double Zfr)
static int OMSimulatorLua_oms2_addTLMConnection(lua_State *L)
{
  /// \todo Cleanup this mess
  if (lua_gettop(L) != 7)
    return luaL_error(L, "expecting exactly 7 arguments");
  luaL_checktype(L, 1, LUA_TSTRING);
  luaL_checktype(L, 2, LUA_TSTRING);
  luaL_checktype(L, 3, LUA_TSTRING);
  luaL_checktype(L, 4, LUA_TNUMBER);
  luaL_checktype(L, 5, LUA_TNUMBER);
  luaL_checktype(L, 6, LUA_TNUMBER);
  luaL_checktype(L, 7, LUA_TNUMBER);

  const char *cref =  lua_tostring(L, 1);
  const char *from =  lua_tostring(L, 2);
  const char *to =    lua_tostring(L, 3);
  double delay =      lua_tonumber(L, 4);
  double alpha =      lua_tonumber(L, 5);
  double Zf =         lua_tonumber(L, 6);
  double Zfr =        lua_tonumber(L, 7);

  oms_status_enu_t status = oms2_addTLMConnection(cref, from, to, delay, alpha, Zf, Zfr);

  lua_pushnumber(L, status);
  return 1;
}

//oms_status_enu_t oms2_setTLMSocketData(const char* cref, const char* address, int managerPort, int monitorPort)
static int OMSimulatorLua_oms2_setTLMSocketData(lua_State *L)
{
  if (lua_gettop(L) != 4)
    return luaL_error(L, "expecting exactly 4 arguments");
  luaL_checktype(L, 1, LUA_TSTRING);
  luaL_checktype(L, 2, LUA_TSTRING);
  luaL_checktype(L, 3, LUA_TNUMBER);
  luaL_checktype(L, 4, LUA_TNUMBER);

  const char *cref =        lua_tostring(L, 1);
  const char *address =     lua_tostring(L, 2);
  int managerPort = lua_tonumber(L, 3);
  int monitorPort =      lua_tonumber(L, 4);

  oms_status_enu_t status = oms2_setTLMSocketData(cref, address, managerPort, monitorPort);
  lua_pushnumber(L, status);
  return 1;
}

/* **************************************************** */
/* OMSysIdent API                                       */
/* **************************************************** */

#ifdef WITH_OMSYSIDENT
//void* omsi_newSysIdentModel(const char* ident);
static int OMSimulatorLua_omsi_newSysIdentModel(lua_State *L)
{
  if (lua_gettop(L) != 1)
    return luaL_error(L, "expecting exactly 1 argument");
  luaL_checktype(L, 1, LUA_TSTRING);

  const char* ident = lua_tostring(L, 1);

  void *pModel = omsi_newSysIdentModel(ident);
  push_pointer(L, pModel);
  return 1;
}

//void omsi_freeSysIdentModel(void* simodel);
static int OMSimulatorLua_omsi_freeSysIdentModel(lua_State *L)
{
  if (lua_gettop(L) != 1)
    return luaL_error(L, "expecting exactly 1 argument");
  luaL_checktype(L, 1, LUA_TUSERDATA);

  void *model = topointer(L, 1);
  omsi_freeSysIdentModel(model);
  return 0;
}

//void oms_setReal(void* model, const char* var, double value);
// oms_status_enu_t omsi_initialize(void* simodel, size_t nSeries, const double* time, size_t nTime, char const* const* inputvars, size_t nInputvars, char const* const* measurementvars, size_t nMeasurementvars);
static int OMSimulatorLua_omsi_initialize(lua_State *L)
{
  if (lua_gettop(L) != 5)
    return luaL_error(L, "expecting exactly 5 arguments");
  luaL_checktype(L, 1, LUA_TUSERDATA); // simodel
  luaL_checktype(L, 2, LUA_TNUMBER);   // nSeries
  luaL_checktype(L, 3, LUA_TTABLE);    // time
  luaL_checktype(L, 4, LUA_TTABLE);    // inputvars
  luaL_checktype(L, 5, LUA_TTABLE);    // measurementvars
  // luaL_checktype(L, 2, LUA_TSTRING);

  void *model = topointer(L, 1);
  size_t nSeries = lua_tonumber(L, 2);

  // get size of tables
  size_t nTime = luaL_len(L, 3);
  size_t nInputvars = luaL_len(L, 4);
  size_t nMeasurementvars = luaL_len(L, 5);
  // create buffers
  double* time = (double*) malloc(nTime*sizeof(double));
  char const* * inputvars = (char const**) malloc(nInputvars*sizeof(char));
  char const* * measurementvars = (char const**) malloc(nMeasurementvars*sizeof(char));

  //printf("nTime: %d, nInputvars: %d, nMeasurementvars %d\n", (int)nTime, (int)nInputvars, (int)nMeasurementvars);

  int i;
  for (i=0; i < nTime; ++i) {
    lua_rawgeti(L, 3, i+1); // push value on stack
    time[i] = lua_tonumber(L, -1);
    lua_pop(L, 1); // pop value from stack
  }

  for (i=0; i < nInputvars; ++i) {
    lua_rawgeti(L, 4, i+1);
    inputvars[i] = lua_tostring(L, -1);
    lua_pop(L, 1);
  }

  for (i=0; i < nMeasurementvars; ++i) {
    lua_rawgeti(L, 5, i+1);
    measurementvars[i] = lua_tostring(L, -1);
    lua_pop(L, 1);
  }

  oms_status_enu_t returnValue =
    omsi_initialize(model, nSeries, time, nTime, inputvars, nInputvars, measurementvars, nMeasurementvars);
  lua_pushinteger(L, returnValue);

  free(time); free(inputvars); free(measurementvars);
  return 1;
}

//oms_status_enu_t omsi_describe(void* simodel);
static int OMSimulatorLua_omsi_describe(lua_State *L)
{
  if (lua_gettop(L) != 1)
    return luaL_error(L, "expecting exactly 1 argument");
  luaL_checktype(L, 1, LUA_TUSERDATA);

  void *model = topointer(L, 1);
  oms_status_enu_t returnValue = omsi_describe(model);
  lua_pushinteger(L, returnValue);
  return 1;
}

// oms_status_enu_t omsi_addMeasurement(void* simodel, size_t iSeries, const char* var, const double* values, size_t nValues);
static int OMSimulatorLua_omsi_addMeasurement(lua_State *L)
{
  if (lua_gettop(L) != 4)
    return luaL_error(L, "expecting exactly 4 arguments");
  luaL_checktype(L, 1, LUA_TUSERDATA); // simodel
  luaL_checktype(L, 2, LUA_TNUMBER);   // iSeries
  luaL_checktype(L, 3, LUA_TSTRING);   // var
  luaL_checktype(L, 4, LUA_TTABLE);    // values

  void *model = topointer(L, 1);
  size_t iSeries = lua_tonumber(L, 2);
  const char* var = lua_tostring(L, 3);
  size_t nValues = luaL_len(L, 4);

  double* values = (double*) malloc(nValues*sizeof(double));
  int i;
  for (i=0; i < nValues; ++i) {
    lua_rawgeti(L, 4, i+1); // push value on stack
    values[i] = lua_tonumber(L, -1);
    lua_pop(L, 1); // pop value from stack
  }

  oms_status_enu_t returnValue =
    omsi_addMeasurement(model, iSeries, var, values, nValues);
  lua_pushinteger(L, returnValue);

  free(values);
  return 1;
}

// oms_status_enu_t omsi_addInput(void* simodel, const char* var, const double* values, size_t nValues);
static int OMSimulatorLua_omsi_addInput(lua_State *L)
{
  if (lua_gettop(L) != 3)
    return luaL_error(L, "expecting exactly 3 arguments");
  luaL_checktype(L, 1, LUA_TUSERDATA); // simodel
  luaL_checktype(L, 2, LUA_TSTRING);   // var
  luaL_checktype(L, 3, LUA_TTABLE);    // values

  void *model = topointer(L, 1);
  const char* var = lua_tostring(L, 2);
  size_t nValues = luaL_len(L, 3);

  double* values = (double*) malloc(nValues*sizeof(double));
  int i;
  for (i=0; i < nValues; ++i) {
    lua_rawgeti(L, 3, i+1); // push value on stack
    values[i] = lua_tonumber(L, -1);
    lua_pop(L, 1); // pop value from stack
  }

  oms_status_enu_t returnValue =
    omsi_addInput(model, var, values, nValues);
  lua_pushinteger(L, returnValue);

  free(values);
  return 1;
}

// oms_status_enu_t omsi_addParameter(void* simodel, const char* var, double startvalue);
static int OMSimulatorLua_omsi_addParameter(lua_State *L)
{
  if (lua_gettop(L) != 3)
    return luaL_error(L, "expecting exactly 3 arguments");
  luaL_checktype(L, 1, LUA_TUSERDATA); // simodel
  luaL_checktype(L, 2, LUA_TSTRING);   // var
  luaL_checktype(L, 3, LUA_TNUMBER);   // startvalue

  void *model = topointer(L, 1);
  const char* var = lua_tostring(L, 2);
  double startvalue = lua_tonumber(L, 3);

  oms_status_enu_t returnValue =
    omsi_addParameter(model, var, startvalue);
  lua_pushinteger(L, returnValue);
  return 1;
}

// oms_status_enu_t omsi_getParameter(void* simodel, const char* var, double* startvalue, double* estimatedvalue);
static int OMSimulatorLua_omsi_getParameter(lua_State *L)
{
  if (lua_gettop(L) != 2)
    return luaL_error(L, "expecting exactly 2 arguments");
  luaL_checktype(L, 1, LUA_TUSERDATA); // simodel
  luaL_checktype(L, 2, LUA_TSTRING);   // var

  void *model = topointer(L, 1);
  const char* var = lua_tostring(L, 2);

  double startvalue, estimatedvalue;
  oms_status_enu_t returnValue =
    omsi_getParameter(model, var, &startvalue, &estimatedvalue);
  lua_pushinteger(L, returnValue);
  lua_pushnumber(L, startvalue);
  lua_pushnumber(L, estimatedvalue);
  return 3;
}

// oms_status_enu_t omsi_solve(void* simodel, const char* reporttype);
static int OMSimulatorLua_omsi_solve(lua_State *L)
{
  if (lua_gettop(L) != 2)
    return luaL_error(L, "expecting exactly 2 arguments");
  luaL_checktype(L, 1, LUA_TUSERDATA); // simodel
  luaL_checktype(L, 2, LUA_TSTRING);   // reporttype

  void *model = topointer(L, 1);
  const char* reporttype = lua_tostring(L, 2);

  oms_status_enu_t returnValue =
    omsi_solve(model, reporttype);
  lua_pushinteger(L, returnValue);
  return 1;
}

// oms_status_enu_t omsi_setOptions_max_num_iterations(void* simodel, size_t max_num_iterations);
static int OMSimulatorLua_omsi_setOptions_max_num_iterations(lua_State *L)
{
  if (lua_gettop(L) != 2)
    return luaL_error(L, "expecting exactly 2 arguments");
  luaL_checktype(L, 1, LUA_TUSERDATA); // simodel
  luaL_checktype(L, 2, LUA_TNUMBER);   // max_num_iterations

  void *model = topointer(L, 1);
  int max_num_iterations = lua_tointeger(L, 2);

  oms_status_enu_t returnValue =
    omsi_setOptions_max_num_iterations(model, max_num_iterations);
  lua_pushinteger(L, returnValue);
  return 1;
}

// oms_status_enu_t omsi_getState(void* simodel, omsi_simodelstate_t* state);
static int OMSimulatorLua_omsi_getState(lua_State *L)
{
  if (lua_gettop(L) != 1)
    return luaL_error(L, "expecting exactly 1 argument");
  luaL_checktype(L, 1, LUA_TUSERDATA); // simodel

  void *model = topointer(L, 1);

  omsi_simodelstate_t state;
  oms_status_enu_t returnValue =
    omsi_getState(model, &state);
  lua_pushinteger(L, returnValue);
  switch (state) {
    case omsi_simodelstate_constructed: lua_pushstring(L, "constructed"); break;
    case omsi_simodelstate_initialized: lua_pushstring(L, "initialized"); break;
    case omsi_simodelstate_convergence: lua_pushstring(L, "convergence"); break;
    case omsi_simodelstate_no_convergence: lua_pushstring(L, "no_convergence"); break;
    case omsi_simodelstate_failure: lua_pushstring(L, "failure"); break;
  }
  return 2;
}
#endif /* WITH_OMSYSIDENT */

DLLEXPORT int luaopen_OMSimulatorLua(lua_State *L)
{
  /* ************************************ */
  /* OMSimulator 2.0                      */
  /*                                      */
  /* TODO: replace prefix oms2 with oms   */
  /* ************************************ */
  REGISTER_LUA_CALL(experimental_setActivationRatio);
  REGISTER_LUA_CALL(experimental_simulate_realtime);
  REGISTER_LUA_CALL(oms2_addConnection);
  REGISTER_LUA_CALL(oms2_addConnection);
  REGISTER_LUA_CALL(oms2_addExternalModel);
  REGISTER_LUA_CALL(oms2_addFMISubModel);
  REGISTER_LUA_CALL(oms2_addFMU);
  REGISTER_LUA_CALL(oms2_addTable);
  REGISTER_LUA_CALL(oms2_addTLMConnection);
  REGISTER_LUA_CALL(oms2_addTLMInterface);
  REGISTER_LUA_CALL(oms2_compareSimulationResults);
  REGISTER_LUA_CALL(oms2_deleteConnection);
  REGISTER_LUA_CALL(oms2_deleteSubModel);
  REGISTER_LUA_CALL(oms2_exportCompositeStructure);
  REGISTER_LUA_CALL(oms2_exportDependencyGraphs);
  REGISTER_LUA_CALL(oms2_getBoolean);
  REGISTER_LUA_CALL(oms2_getCurrentTime);
  REGISTER_LUA_CALL(oms2_getElements);
  REGISTER_LUA_CALL(oms2_getInteger);
  REGISTER_LUA_CALL(oms2_getReal);
  REGISTER_LUA_CALL(oms2_getVersion);
  REGISTER_LUA_CALL(oms2_initialize);
  REGISTER_LUA_CALL(oms2_loadModel);
  REGISTER_LUA_CALL(oms2_newFMIModel);
  REGISTER_LUA_CALL(oms2_newTLMModel);
  REGISTER_LUA_CALL(oms2_rename);
  REGISTER_LUA_CALL(oms2_reset);
  REGISTER_LUA_CALL(oms2_saveModel);
  REGISTER_LUA_CALL(oms2_setBoolean);
  REGISTER_LUA_CALL(oms2_setCommunicationInterval);
  REGISTER_LUA_CALL(oms2_setInteger);
  REGISTER_LUA_CALL(oms2_setLogFile);
  REGISTER_LUA_CALL(oms2_setLoggingLevel);
  REGISTER_LUA_CALL(oms2_setMasterAlgorithm);
  REGISTER_LUA_CALL(oms2_setReal);
  REGISTER_LUA_CALL(oms2_setRealParameter);
  REGISTER_LUA_CALL(oms2_setResultFile);
  REGISTER_LUA_CALL(oms2_setStartTime);
  REGISTER_LUA_CALL(oms2_setStopTime);
  REGISTER_LUA_CALL(oms2_setTempDirectory);
  REGISTER_LUA_CALL(oms2_setTLMSocketData);
  REGISTER_LUA_CALL(oms2_setWorkingDirectory);
  REGISTER_LUA_CALL(oms2_simulate);
  REGISTER_LUA_CALL(oms2_stepUntil);
  REGISTER_LUA_CALL(oms2_terminate);
  REGISTER_LUA_CALL(oms2_unloadModel);

  /* ************************************ */
  /* OMSysIdent API                       */
  /* ************************************ */
#ifdef WITH_OMSYSIDENT
  REGISTER_LUA_CALL(omsi_addMeasurement);
  REGISTER_LUA_CALL(omsi_addInput);
  REGISTER_LUA_CALL(omsi_addParameter);
  REGISTER_LUA_CALL(omsi_describe);
  REGISTER_LUA_CALL(omsi_freeSysIdentModel);
  REGISTER_LUA_CALL(omsi_getParameter);
  REGISTER_LUA_CALL(omsi_getState);
  REGISTER_LUA_CALL(omsi_initialize);
  REGISTER_LUA_CALL(omsi_newSysIdentModel);
  REGISTER_LUA_CALL(omsi_setOptions_max_num_iterations);
  REGISTER_LUA_CALL(omsi_solve);
#endif /* WITH_OMSYSIDENT */

  // Add enumerable constants
  lua_pushnumber(L, 0);
  lua_setglobal(L, "input");
  lua_pushnumber(L, 1);
  lua_setglobal(L, "output");
  lua_pushnumber(L, 3);
  lua_setglobal(L, "bidirectional");

  lua_pushnumber(L, 0);
  lua_setglobal(L, "default");
  lua_pushnumber(L, 1);
  lua_setglobal(L, "coarsegrained");
  lua_pushnumber(L, 2);
  lua_setglobal(L, "finegrained");

  return 0;
}
