#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <stdlib.h>

#include <OMSimulator.h>

#define REGISTER_LUA_CALL(name) lua_register(L, #name, OMSimulatorLua_##name)
#define REGISTER_LUA_ENUM(name) lua_pushnumber(L, name); lua_setglobal(L, #name)

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

//const char* oms_getVersion();
static int OMSimulatorLua_oms_getVersion(lua_State *L)
{
  if (lua_gettop(L) != 0)
    return luaL_error(L, "expecting no arguments");

  const char* version = oms_getVersion();

  lua_pushstring(L, version);
  return 1;
}

//oms_status_enu_t oms_setLogFile(const char* filename);
static int OMSimulatorLua_oms_setLogFile(lua_State *L)
{
  if (lua_gettop(L) != 1)
    return luaL_error(L, "expecting exactly 1 argument");
  luaL_checktype(L, 1, LUA_TSTRING);

  const char* filename = lua_tostring(L, 1);
  oms_status_enu_t status = oms_setLogFile(filename);
  lua_pushinteger(L, status);
  return 1;
}

//oms_status_enu_t oms_setLoggingInterval(const char* cref, double loggingInterval);
static int OMSimulatorLua_oms_setLoggingInterval(lua_State *L)
{
  if (lua_gettop(L) != 2)
    return luaL_error(L, "expecting exactly 2 arguments");
  luaL_checktype(L, 1, LUA_TSTRING);
  luaL_checktype(L, 2, LUA_TNUMBER);

  const char* cref = lua_tostring(L, 1);
  double loggingInterval = lua_tonumber(L, 2);
  oms_status_enu_t status = oms_setLoggingInterval(cref, loggingInterval);
  lua_pushinteger(L, status);
  return 1;
}

//oms_status_enu_t oms_setLoggingLevel(int logLevel);
static int OMSimulatorLua_oms_setLoggingLevel(lua_State *L)
{
  if (lua_gettop(L) != 1)
    return luaL_error(L, "expecting exactly 1 argument");
  luaL_checktype(L, 1, LUA_TNUMBER);

  int logLevel = lua_tointeger(L, 1);
  oms_status_enu_t status = oms_setLoggingLevel(logLevel);
  lua_pushinteger(L, status);
  return 1;
}

//void oms_setMaxLogFileSize(const unsigned long size);
static int OMSimulatorLua_oms_setMaxLogFileSize(lua_State *L)
{
  if (lua_gettop(L) != 1)
    return luaL_error(L, "expecting exactly 1 argument");
  luaL_checktype(L, 1, LUA_TNUMBER);

  unsigned long size = lua_tointeger(L, 1);
  oms_setMaxLogFileSize(size);
  return 0;
}

//oms_status_enu_t oms_setTempDirectory(const char* newTempDir);
static int OMSimulatorLua_oms_setTempDirectory(lua_State *L)
{
  if (lua_gettop(L) != 1)
    return luaL_error(L, "expecting exactly 1 argument");
  luaL_checktype(L, 1, LUA_TSTRING);

  const char* newTempDir = lua_tostring(L, 1);
  oms_status_enu_t status = oms_setTempDirectory(newTempDir);

  lua_pushinteger(L, status);
  return 1;
}

static int OMSimulatorLua_oms_setTolerance(lua_State *L)
{
  if (lua_gettop(L) != 2 && lua_gettop(L) != 3)
    return luaL_error(L, "expecting exactly 2 or 3 arguments");
  luaL_checktype(L, 1, LUA_TSTRING);
  luaL_checktype(L, 2, LUA_TNUMBER);

  const char* cref = lua_tostring(L, 1);
  double absoluteTolerance = lua_tonumber(L, 2);

  oms_status_enu_t status;
  if (lua_gettop(L) == 2)
    status = oms_setTolerance(cref, absoluteTolerance, absoluteTolerance);
  else
  {
    double relativeTolerance = lua_tonumber(L, 3);
    status = oms_setTolerance(cref, absoluteTolerance, relativeTolerance);
  }

  lua_pushinteger(L, status);
  return 1;
}

//OMSAPI oms_status_enu_t OMSCALL oms_setVariableStepSize(const char* cref, double initialStepSize, double minimumStepSize, double maximumStepSize);
static int OMSimulatorLua_oms_setVariableStepSize(lua_State *L)
{
  if (lua_gettop(L) != 4)
    return luaL_error(L, "expecting exactly 4 arguments");
  luaL_checktype(L, 1, LUA_TSTRING);
  luaL_checktype(L, 2, LUA_TNUMBER);
  luaL_checktype(L, 3, LUA_TNUMBER);
  luaL_checktype(L, 4, LUA_TNUMBER);

  const char* cref = lua_tostring(L, 1);
  double initialStepSize = lua_tonumber(L, 2);
  double minimumStepSize = lua_tonumber(L, 3);
  double maximumStepSize = lua_tonumber(L, 4);

  oms_status_enu_t status = oms_setVariableStepSize(cref, initialStepSize, minimumStepSize, maximumStepSize);

  lua_pushinteger(L, status);
  return 1;
}

//oms_status_enu_t oms_setWorkingDirectory(const char* newWorkingDir);
static int OMSimulatorLua_oms_setWorkingDirectory(lua_State *L)
{
  if (lua_gettop(L) != 1)
    return luaL_error(L, "expecting exactly 1 argument");
  luaL_checktype(L, 1, LUA_TSTRING);

  const char* newWorkingDir = lua_tostring(L, 1);
  oms_status_enu_t status = oms_setWorkingDirectory(newWorkingDir);

  lua_pushinteger(L, status);
  return 1;
}

//oms_status_enu_t oms_newModel(const char* cref);
static int OMSimulatorLua_oms_newModel(lua_State *L)
{
  if (lua_gettop(L) != 1)
    return luaL_error(L, "expecting exactly 1 argument");
  luaL_checktype(L, 1, LUA_TSTRING);

  const char* cref = lua_tostring(L, 1);
  oms_status_enu_t status = oms_newModel(cref);

  lua_pushinteger(L, status);
  return 1;
}

//oms_status_enu_t oms_rename(const char* cref, const char* newCref);
static int OMSimulatorLua_oms_rename(lua_State *L)
{
  if (lua_gettop(L) != 2)
    return luaL_error(L, "expecting exactly 2 arguments");
  luaL_checktype(L, 1, LUA_TSTRING);

  const char* cref = lua_tostring(L, 1);
  const char* newCref = lua_tostring(L, 2);
  oms_status_enu_t status = oms_rename(cref, newCref);

  lua_pushinteger(L, status);
  return 1;
}

//oms_status_enu_t oms_delete(const char* cref);
static int OMSimulatorLua_oms_delete(lua_State *L)
{
  if (lua_gettop(L) != 1)
    return luaL_error(L, "expecting exactly 1 argument");
  luaL_checktype(L, 1, LUA_TSTRING);

  const char* cref = lua_tostring(L, 1);
  oms_status_enu_t status = oms_delete(cref);

  lua_pushinteger(L, status);
  return 1;
}

//oms_status_enu_t oms_export(const char* cref, const char* filename);
static int OMSimulatorLua_oms_export(lua_State *L)
{
  if (lua_gettop(L) != 2)
    return luaL_error(L, "expecting exactly 2 arguments");
  luaL_checktype(L, 1, LUA_TSTRING);
  luaL_checktype(L, 2, LUA_TSTRING);

  const char* cref = lua_tostring(L, 1);
  const char* filename = lua_tostring(L, 2);
  oms_status_enu_t status = oms_export(cref, filename);

  lua_pushinteger(L, status);
  return 1;
}

//oms_status_enu_t oms_importFile(const char* filename, const char** cref);
static int OMSimulatorLua_oms_importFile(lua_State *L)
{
  if (lua_gettop(L) != 1)
    return luaL_error(L, "expecting exactly 1 argument");
  luaL_checktype(L, 1, LUA_TSTRING);

  const char* filename = lua_tostring(L, 1);
  char* cref = NULL;
  oms_status_enu_t status = oms_importFile(filename, &cref);

  lua_pushstring(L, cref ? cref : "");
  lua_pushinteger(L, status);
  return 2;
}

//oms_status_enu_t oms_exportSnapshot(const char* cref, char** contents);
static int OMSimulatorLua_oms_exportSnapshot(lua_State *L)
{
  if (lua_gettop(L) != 1)
    return luaL_error(L, "expecting exactly 1 argument");
  luaL_checktype(L, 1, LUA_TSTRING);

  const char* cref = lua_tostring(L, 1);
  char* contents = NULL;
  oms_status_enu_t status = oms_exportSnapshot(cref, &contents);

  lua_pushstring(L, contents ? contents : "");
  lua_pushinteger(L, status);

  if (contents)
    oms_freeMemory(contents);

  return 2;
}

//oms_status_enu_t oms_exportSSVTemplate(const char* cref, const char* filename);
static int OMSimulatorLua_oms_exportSSVTemplate(lua_State *L)
{
  if (lua_gettop(L) != 2)
    return luaL_error(L, "expecting exactly 2 arguments");
  luaL_checktype(L, 1, LUA_TSTRING);
  luaL_checktype(L, 2, LUA_TSTRING);

  const char* cref = lua_tostring(L, 1);
  const char* filename = lua_tostring(L, 2);
  oms_status_enu_t status = oms_exportSSVTemplate(cref, filename);

  lua_pushinteger(L, status);
  return 1;
}

//oms_status_enu_t oms_exportSSMTemplate(const char* cref, const char* filename);
static int OMSimulatorLua_oms_exportSSMTemplate(lua_State *L)
{
  if (lua_gettop(L) != 2)
    return luaL_error(L, "expecting exactly 2 arguments");
  luaL_checktype(L, 1, LUA_TSTRING);
  luaL_checktype(L, 2, LUA_TSTRING);

  const char* cref = lua_tostring(L, 1);
  const char* filename = lua_tostring(L, 2);
  oms_status_enu_t status = oms_exportSSMTemplate(cref, filename);

  lua_pushinteger(L, status);
  return 1;
}

//oms_status_enu_t oms_listUnconnectedConnectors(const char* cref, char** contents);
static int OMSimulatorLua_oms_listUnconnectedConnectors(lua_State *L)
{
  if (lua_gettop(L) != 1)
    return luaL_error(L, "expecting exactly 1 argument");
  luaL_checktype(L, 1, LUA_TSTRING);

  const char* cref = lua_tostring(L, 1);
  char* contents = NULL;
  oms_status_enu_t status = oms_listUnconnectedConnectors(cref, &contents);

  lua_pushstring(L, contents ? contents : "");
  lua_pushinteger(L, status);

  if (contents)
    oms_freeMemory(contents);

  return 2;
}

//oms_status_enu_t oms_importSnapshot(const char* cref, const char* snapshot, char** newCref);
static int OMSimulatorLua_oms_importSnapshot(lua_State *L)
{
  if (lua_gettop(L) != 2)
    return luaL_error(L, "expecting exactly 2 arguments\n<integer> = oms_importSnapshot(<string>, <string>)");
  luaL_checktype(L, 1, LUA_TSTRING);
  luaL_checktype(L, 2, LUA_TSTRING);

  const char* cref = lua_tostring(L, 1);
  const char* snapshot = lua_tostring(L, 2);

  char* newCref = NULL;
  oms_status_enu_t status = oms_importSnapshot(cref, snapshot, &newCref);

  lua_pushstring(L, newCref ? newCref : "");
  lua_pushinteger(L, status);
  return 2;
}

//oms_status_enu_t oms_exportDependencyGraphs(const char* cref, const char* initialization, const char* event, const char* simulation);
static int OMSimulatorLua_oms_exportDependencyGraphs(lua_State *L)
{
  if (lua_gettop(L) != 4)
    return luaL_error(L, "expecting exactly 4 arguments");

  luaL_checktype(L, 1, LUA_TSTRING);
  luaL_checktype(L, 2, LUA_TSTRING);
  luaL_checktype(L, 3, LUA_TSTRING);
  luaL_checktype(L, 4, LUA_TSTRING);

  const char* cref = lua_tostring(L, 1);
  const char* initialization = lua_tostring(L, 2);
  const char* event = lua_tostring(L, 3);
  const char* simulation = lua_tostring(L, 4);

  oms_status_enu_t status = oms_exportDependencyGraphs(cref, initialization, event, simulation);
  lua_pushinteger(L, status);
  return 1;
}

//int oms_compareSimulationResults(const char* filenameA, const char* filenameB, const char* var, double relTol, double absTol);
static int OMSimulatorLua_oms_compareSimulationResults(lua_State *L)
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

//oms_status_enu_t oms_addSystem(const char* cref, oms_system_enu_t type);
static int OMSimulatorLua_oms_addSystem(lua_State *L)
{
  if (lua_gettop(L) != 2)
    return luaL_error(L, "expecting exactly 2 arguments");
  luaL_checktype(L, 1, LUA_TSTRING);
  luaL_checktype(L, 2, LUA_TNUMBER);

  const char* cref = lua_tostring(L, 1);
  int type = lua_tointeger(L, 2);
  oms_status_enu_t status = oms_addSystem(cref, (oms_system_enu_t)type);

  lua_pushinteger(L, status);

  return 1;
}

//oms_status_enu_t oms_copySystem(const char* source, const char* target);
static int OMSimulatorLua_oms_copySystem(lua_State *L)
{
  if (lua_gettop(L) != 2)
    return luaL_error(L, "expecting exactly 2 arguments");
  luaL_checktype(L, 1, LUA_TSTRING);
  luaL_checktype(L, 2, LUA_TSTRING);

  const char* source = lua_tostring(L, 1);
  const char* target = lua_tostring(L, 2);
  oms_status_enu_t status = oms_copySystem(source, target);

  lua_pushinteger(L, status);

  return 1;
}

//oms_status_enu_t oms_setCommandLineOption(const char* cmd);
static int OMSimulatorLua_oms_setCommandLineOption(lua_State *L)
{
  if (lua_gettop(L) != 1)
    return luaL_error(L, "expecting exactly 1 argument");
  luaL_checktype(L, 1, LUA_TSTRING);

  const char* cmd = lua_tostring(L, 1);
  oms_status_enu_t status = oms_setCommandLineOption(cmd);

  lua_pushinteger(L, status);

  return 1;
}

//oms_status_enu_t oms_getSystemType(const char* cref, oms_system_enu_t* type);
static int OMSimulatorLua_oms_getSystemType(lua_State *L)
{
  if (lua_gettop(L) != 1)
    return luaL_error(L, "expecting exactly 1 argument");
  luaL_checktype(L, 1, LUA_TSTRING);

  const char* cref = lua_tostring(L, 1);
  oms_system_enu_t type;
  oms_status_enu_t status = oms_getSystemType(cref, &type);

  lua_pushinteger(L, type);
  lua_pushinteger(L, status);

  return 2;
}

//OMSAPI oms_status_enu_t OMSCALL oms_getVariableStepSize(const char* cref, double* initialStepSize, double* minimumStepSize, double* maximumStepSize);
static int OMSimulatorLua_oms_getVariableStepSize(lua_State *L)
{
  if (lua_gettop(L) != 1)
    return luaL_error(L, "expecting exactly 1 argument");
  luaL_checktype(L, 1, LUA_TSTRING);

  const char* cref = lua_tostring(L, 1);
  double initialStepSize = 0.0;
  double minimumStepSize = 0.0;
  double maximumStepSize = 0.0;
  oms_status_enu_t status = oms_getVariableStepSize(cref, &initialStepSize, &minimumStepSize, &maximumStepSize);

  lua_pushnumber(L, initialStepSize);
  lua_pushnumber(L, minimumStepSize);
  lua_pushnumber(L, maximumStepSize);
  lua_pushinteger(L, status);

  return 4;
}

//OMSAPI oms_status_enu_t OMSCALL oms_faultInjection(const char* signal, oms_fault_type_enu_t faultType, double faultValue);
static int OMSimulatorLua_oms_faultInjection(lua_State *L)
{
  if (lua_gettop(L) != 3)
    return luaL_error(L, "expecting exactly 3 arguments");
  luaL_checktype(L, 1, LUA_TSTRING);
  luaL_checktype(L, 2, LUA_TNUMBER);
  luaL_checktype(L, 3, LUA_TNUMBER);

  const char* signal = lua_tostring(L, 1);
  oms_fault_type_enu_t faultType = (oms_fault_type_enu_t)lua_tointeger(L, 2);
  double faultValue = lua_tonumber(L, 3);

  oms_status_enu_t status = oms_faultInjection(signal, faultType, faultValue);
  lua_pushinteger(L, status);
  return 1;
}

//oms_status_enu_t oms_getBoolean(const char* cref, bool* value);
static int OMSimulatorLua_oms_getBoolean(lua_State *L)
{
  if (lua_gettop(L) != 1)
    return luaL_error(L, "expecting exactly 1 argument");
  luaL_checktype(L, 1, LUA_TSTRING);

  const char* cref = lua_tostring(L, 1);
  bool value = 0.0;

  oms_status_enu_t status = oms_getBoolean(cref, &value);
  lua_pushinteger(L, value);
  lua_pushinteger(L, status);
  return 2;
}

//OMSAPI oms_status_enu_t OMSCALL oms_getFixedStepSize(const char* cref, double* stepSize);
static int OMSimulatorLua_oms_getFixedStepSize(lua_State *L)
{
  if (lua_gettop(L) != 1)
    return luaL_error(L, "expecting exactly 1 argument");
  luaL_checktype(L, 1, LUA_TSTRING);

  const char* cref = lua_tostring(L, 1);
  double stepSize = 0.0;

  oms_status_enu_t status = oms_getFixedStepSize(cref, &stepSize);
  lua_pushnumber(L, stepSize);
  lua_pushinteger(L, status);
  return 2;
}

//oms_status_enu_t oms_getInteger(const char* cref, bool* value);
static int OMSimulatorLua_oms_getInteger(lua_State *L)
{
  if (lua_gettop(L) != 1)
    return luaL_error(L, "expecting exactly 1 argument");
  luaL_checktype(L, 1, LUA_TSTRING);

  const char* cref = lua_tostring(L, 1);
  int value = 0.0;

  oms_status_enu_t status = oms_getInteger(cref, &value);
  lua_pushinteger(L, value);
  lua_pushinteger(L, status);
  return 2;
}

//oms_status_enu_t oms_getReal(const char* cref, double* value);
static int OMSimulatorLua_oms_getReal(lua_State *L)
{
  if (lua_gettop(L) != 1)
    return luaL_error(L, "expecting exactly 1 argument");
  luaL_checktype(L, 1, LUA_TSTRING);

  const char* cref = lua_tostring(L, 1);
  double value = 0.0;

  oms_status_enu_t status = oms_getReal(cref, &value);
  lua_pushnumber(L, value);
  lua_pushinteger(L, status);
  return 2;
}

//OMSAPI oms_status_enu_t OMSCALL oms_getSolver(const char* cref, oms_solver_enu_t* solver);
static int OMSimulatorLua_oms_getSolver(lua_State *L)
{
  if (lua_gettop(L) != 1)
    return luaL_error(L, "expecting exactly 1 argument");
  luaL_checktype(L, 1, LUA_TSTRING);

  const char* cref = lua_tostring(L, 1);
  oms_solver_enu_t solver = oms_solver_none;

  oms_status_enu_t status = oms_getSolver(cref, &solver);
  lua_pushinteger(L, solver);
  lua_pushinteger(L, status);
  return 2;
}

//oms_status_enu_t oms_setBoolean(const char* cref, double* value);
static int OMSimulatorLua_oms_setBoolean(lua_State *L)
{
  if (lua_gettop(L) != 2)
    return luaL_error(L, "expecting exactly 2 arguments");
  luaL_checktype(L, 1, LUA_TSTRING);
  luaL_checktype(L, 2, LUA_TNUMBER);

  const char* cref = lua_tostring(L, 1);
  bool value = lua_tointeger(L, 2);

  oms_status_enu_t status = oms_setBoolean(cref, value);
  lua_pushinteger(L, status);
  return 1;
}

//oms_status_enu_t oms_setInteger(const char* cref, double* value);
static int OMSimulatorLua_oms_setInteger(lua_State *L)
{
  if (lua_gettop(L) != 2)
    return luaL_error(L, "expecting exactly 2 arguments");
  luaL_checktype(L, 1, LUA_TSTRING);
  luaL_checktype(L, 2, LUA_TNUMBER);

  const char* cref = lua_tostring(L, 1);
  int value = lua_tointeger(L, 2);

  oms_status_enu_t status = oms_setInteger(cref, value);
  lua_pushinteger(L, status);
  return 1;
}

// oms_status_enu_t oms_setReal(const char* cref, double value);
static int OMSimulatorLua_oms_setReal(lua_State *L)
{
  if (lua_gettop(L) != 2)
    return luaL_error(L, "expecting exactly 2 arguments");
  luaL_checktype(L, 1, LUA_TSTRING);
  luaL_checktype(L, 2, LUA_TNUMBER);

  const char* cref = lua_tostring(L, 1);
  double value = lua_tonumber(L, 2);

  oms_status_enu_t status = oms_setReal(cref, value);
  lua_pushinteger(L, status);
  return 1;
}

// OMSAPI oms_status_enu_t OMSCALL oms_setRealInputDerivative(const char* cref, double value);
static int OMSimulatorLua_oms_setRealInputDerivative(lua_State *L)
{
  if (lua_gettop(L) != 2)
    return luaL_error(L, "expecting exactly 2 arguments");
  luaL_checktype(L, 1, LUA_TSTRING);
  luaL_checktype(L, 2, LUA_TNUMBER);

  const char* cref = lua_tostring(L, 1);
  double value = lua_tonumber(L, 2);

  oms_status_enu_t status = oms_setRealInputDerivative(cref, value);
  lua_pushinteger(L, status);
  return 1;
}

//oms_status_enu_t oms_addConnector(const char *cref, oms_causality_enu_t causality, oms_signal_type_enu_t type);
static int OMSimulatorLua_oms_addConnector(lua_State *L)
{
  if (lua_gettop(L) != 3)
    return luaL_error(L, "expecting exactly 3 arguments");
  luaL_checktype(L, 1, LUA_TSTRING);
  luaL_checktype(L, 2, LUA_TNUMBER);
  luaL_checktype(L, 3, LUA_TNUMBER);

  const char* cref = lua_tostring(L, 1);
  int causality = lua_tointeger(L, 2);
  int type = lua_tointeger(L, 3);
  oms_status_enu_t status = oms_addConnector(cref, (oms_causality_enu_t)causality, (oms_signal_type_enu_t)type);

  lua_pushinteger(L, status);

  return 1;
}

//oms_status_enu_t oms_addConnection(const char *crefA, const char *crefB);
static int OMSimulatorLua_oms_addConnection(lua_State *L)
{
  if (lua_gettop(L) != 2)
    return luaL_error(L, "expecting exactly 2 arguments");
  luaL_checktype(L, 1, LUA_TSTRING);
  luaL_checktype(L, 2, LUA_TSTRING);

  const char* crefA = lua_tostring(L, 1);
  const char* crefB = lua_tostring(L, 2);
  oms_status_enu_t status = oms_addConnection(crefA, crefB);

  lua_pushinteger(L, status);

  return 1;
}

//oms_status_enu_t oms_deleteConnection(const char *crefA, const char *crefB)
static int OMSimulatorLua_oms_deleteConnection(lua_State *L)
{
  if (lua_gettop(L) != 2)
    return luaL_error(L, "expecting exactly 2 arguments");
  luaL_checktype(L, 1, LUA_TSTRING);
  luaL_checktype(L, 2, LUA_TSTRING);

  const char* crefA = lua_tostring(L, 1);
  const char* crefB = lua_tostring(L, 2);
  oms_status_enu_t status = oms_deleteConnection(crefA, crefB);

  lua_pushinteger(L, status);

  return 1;
}

//oms_status_enu_t oms_addBus(const char *cref);
static int OMSimulatorLua_oms_addBus(lua_State *L)
{
  if (lua_gettop(L) != 1)
    return luaL_error(L, "expecting exactly 1 arguments");
  luaL_checktype(L, 1, LUA_TSTRING);

  const char* cref = lua_tostring(L, 1);
  oms_status_enu_t status = oms_addBus(cref);

  lua_pushinteger(L, status);

  return 1;
}

//oms_status_enu_t oms_addTLMBus(const char *cref, oms_tlm_domain_t domain, const char *dimension, const char *interpolation)
static int OMSimulatorLua_oms_addTLMBus(lua_State *L)
{
  if (lua_gettop(L) != 3 && lua_gettop(L) != 4)
    return luaL_error(L, "expecting exactly 3 or 4 arguments");
  luaL_checktype(L, 1, LUA_TSTRING);
  luaL_checktype(L, 2, LUA_TNUMBER);
  luaL_checktype(L, 3, LUA_TNUMBER);
  if(lua_gettop(L) > 3)
    luaL_checktype(L, 4, LUA_TNUMBER);

  const char* cref = lua_tostring(L, 1);
  int domain = lua_tointeger(L, 2);
  int dimensions = lua_tointeger(L, 3);
  int interpolation = (int)oms_tlm_no_interpolation;
  if(lua_gettop(L) > 3)
    interpolation = lua_tointeger(L, 4);

  oms_status_enu_t status = oms_addTLMBus(cref, (oms_tlm_domain_t)domain, dimensions, (oms_tlm_interpolation_t)interpolation);

  lua_pushinteger(L, status);

  return 1;
}

//oms_status_enu_t oms_addConnectorToBus(const char *busCref, const char *connectorCref)
static int OMSimulatorLua_oms_addConnectorToBus(lua_State *L)
{
  if (lua_gettop(L) != 2)
    return luaL_error(L, "expecting exactly 2 arguments");
  luaL_checktype(L, 1, LUA_TSTRING);
  luaL_checktype(L, 2, LUA_TSTRING);

  const char* busCref = lua_tostring(L, 1);
  const char* connectorCref = lua_tostring(L, 2);
  oms_status_enu_t status = oms_addConnectorToBus(busCref,connectorCref);

  lua_pushinteger(L, status);

  return 1;
}

//oms_status_enu_t oms_deleteConnectorFromBus(const char *busCref, const char *connectorCref)
static int OMSimulatorLua_oms_deleteConnectorFromBus(lua_State *L)
{
  if (lua_gettop(L) != 2)
    return luaL_error(L, "expecting exactly 2 arguments");
  luaL_checktype(L, 1, LUA_TSTRING);
  luaL_checktype(L, 2, LUA_TSTRING);

  const char* busCref = lua_tostring(L, 1);
  const char* connectorCref = lua_tostring(L, 2);
  oms_status_enu_t status = oms_deleteConnectorFromBus(busCref,connectorCref);

  lua_pushinteger(L, status);

  return 1;
}

//oms_status_enu_t oms_addConnectorToTLMBus(const char* busCref, const char* connectorCref, const char* type);
static int OMSimulatorLua_oms_addConnectorToTLMBus(lua_State *L)
{
  if (lua_gettop(L) != 3)
    return luaL_error(L, "expecting exactly 3 arguments");
  luaL_checktype(L, 1, LUA_TSTRING);
  luaL_checktype(L, 2, LUA_TSTRING);
  luaL_checktype(L, 3, LUA_TSTRING);

  const char* busCref = lua_tostring(L, 1);
  const char* connectorCref = lua_tostring(L, 2);
  const char* type = lua_tostring(L, 3);
  oms_status_enu_t status = oms_addConnectorToTLMBus(busCref,connectorCref,type);

  lua_pushinteger(L, status);

  return 1;
}

//oms_status_enu_t oms_deleteConnectorFromTLMBus(const char *busCref, const char *connectorCref)
static int OMSimulatorLua_oms_deleteConnectorFromTLMBus(lua_State *L)
{
  if (lua_gettop(L) != 2)
    return luaL_error(L, "expecting exactly 2 arguments");
  luaL_checktype(L, 1, LUA_TSTRING);
  luaL_checktype(L, 2, LUA_TSTRING);

  const char* busCref = lua_tostring(L, 1);
  const char* connectorCref = lua_tostring(L, 2);
  oms_status_enu_t status = oms_deleteConnectorFromTLMBus(busCref,connectorCref);

  lua_pushinteger(L, status);

  return 1;
}

//oms_status_enu_t oms_addTLMConnection(const char *crefA, const char *crefB, double delay, double alpha, double Zf, double Zfr)
static int OMSimulatorLua_oms_addTLMConnection(lua_State *L)
{
  if (lua_gettop(L) != 6)
    return luaL_error(L, "expecting exactly 6 arguments");
  luaL_checktype(L, 1, LUA_TSTRING);
  luaL_checktype(L, 2, LUA_TSTRING);
  luaL_checktype(L, 3, LUA_TNUMBER);
  luaL_checktype(L, 4, LUA_TNUMBER);
  luaL_checktype(L, 5, LUA_TNUMBER);
  luaL_checktype(L, 6, LUA_TNUMBER);

  const char* crefA = lua_tostring(L, 1);
  const char* crefB = lua_tostring(L, 2);
  double delay = lua_tonumber(L, 3);
  double alpha = lua_tonumber(L, 4);
  double linearimpedance = lua_tonumber(L, 5);
  double angularimpedance = lua_tonumber(L, 6);
  oms_status_enu_t status = oms_addTLMConnection(crefA, crefB, delay, alpha, linearimpedance, angularimpedance);

  lua_pushinteger(L, status);

  return 1;
}

//oms_status_enu_t oms_addDynamicValueIndicator(const char* signal, const char* lower, const char* upper, double stepSize);
static int OMSimulatorLua_oms_addDynamicValueIndicator(lua_State *L)
{
  if (lua_gettop(L) != 4)
    return luaL_error(L, "expecting exactly 4 arguments");
  luaL_checktype(L, 1, LUA_TSTRING);
  luaL_checktype(L, 2, LUA_TSTRING);
  luaL_checktype(L, 3, LUA_TSTRING);
  luaL_checktype(L, 4, LUA_TNUMBER);

  const char* cref = lua_tostring(L, 1);
  const char* lower = lua_tostring(L, 2);
  const char* upper = lua_tostring(L, 3);
  double stepSize = lua_tonumber(L, 4);
  oms_status_enu_t status = oms_addDynamicValueIndicator(cref, lower, upper, stepSize);

  lua_pushinteger(L, status);
  return 1;
}

//oms_status_enu_t oms_addEventIndicator(const char* signal);
static int OMSimulatorLua_oms_addEventIndicator(lua_State *L)
{
  if (lua_gettop(L) != 1)
    return luaL_error(L, "expecting exactly 1 argument");
  luaL_checktype(L, 1, LUA_TSTRING);

  const char* cref = lua_tostring(L, 1);
  oms_status_enu_t status = oms_addEventIndicator(cref);

  lua_pushinteger(L, status);
  return 1;
}

//oms_status_enu_t oms_addTimeIndicator(const char* signal);
static int OMSimulatorLua_oms_addTimeIndicator(lua_State *L)
{
  if (lua_gettop(L) != 1)
    return luaL_error(L, "expecting exactly 1 argument");
  luaL_checktype(L, 1, LUA_TSTRING);

  const char *ident = lua_tostring(L, 1);
  oms_status_enu_t status = oms_addTimeIndicator(ident);

  lua_pushinteger(L, status);
  return 1;
}

//oms_status_enu_t oms_addStaticValueIndicator(const char* signal, double lower, double upper, double stepSize);
static int OMSimulatorLua_oms_addStaticValueIndicator(lua_State *L)
{
  if (lua_gettop(L) != 4)
    return luaL_error(L, "expecting exactly 4 arguments");
  luaL_checktype(L, 1, LUA_TSTRING);
  luaL_checktype(L, 2, LUA_TNUMBER);
  luaL_checktype(L, 3, LUA_TNUMBER);
  luaL_checktype(L, 4, LUA_TNUMBER);

  const char *signal = lua_tostring(L, 1);
  double lower = lua_tonumber(L, 2);
  double upper = lua_tonumber(L, 3);
  double stepSize = lua_tonumber(L, 4);
  oms_status_enu_t status = oms_addStaticValueIndicator(signal,lower,upper,stepSize);

  lua_pushinteger(L, status);
  return 1;
}

//oms_status_enu_t oms_addExternalModel(const char *cref, const char *path, const char *startscript)
static int OMSimulatorLua_oms_addExternalModel(lua_State *L)
{
  if (lua_gettop(L) != 3)
    return luaL_error(L, "expecting exactly 3 arguments");
  luaL_checktype(L, 1, LUA_TSTRING);
  luaL_checktype(L, 2, LUA_TSTRING);
  luaL_checktype(L, 3, LUA_TSTRING);

  const char* cref = lua_tostring(L, 1);
  const char* path = lua_tostring(L, 2);
  const char* startscript = lua_tostring(L, 3);
  oms_status_enu_t status = oms_addExternalModel(cref, path, startscript);

  lua_pushinteger(L, status);

  return 1;
}

//oms_status_enu_t oms_addSubModel(const char* cref, const char* fmuPath);
static int OMSimulatorLua_oms_addSubModel(lua_State *L)
{
  if (lua_gettop(L) != 2)
    return luaL_error(L, "expecting exactly 2 arguments");
  luaL_checktype(L, 1, LUA_TSTRING);
  luaL_checktype(L, 2, LUA_TSTRING);

  const char* cref = lua_tostring(L, 1);
  const char* fmuPath = lua_tostring(L, 2);
  oms_status_enu_t status = oms_addSubModel(cref, fmuPath);

  if (status!=oms_status_ok)
  {
    return luaL_error(L, "oms_addSubModel(%s,%s) failed", cref, fmuPath);
  }

  lua_pushinteger(L, status);

  return 1;
}

//oms_status_enu_t oms_instantiate(const char* ident);
static int OMSimulatorLua_oms_instantiate(lua_State *L)
{
  if (lua_gettop(L) != 1)
    return luaL_error(L, "expecting exactly 1 argument");
  luaL_checktype(L, 1, LUA_TSTRING);

  const char* ident = lua_tostring(L, 1);
  oms_status_enu_t status = oms_instantiate(ident);
  lua_pushinteger(L, status);
  return 1;
}

//oms_status_enu_t oms_initialize(const char* ident);
static int OMSimulatorLua_oms_initialize(lua_State *L)
{
  if (lua_gettop(L) != 1)
    return luaL_error(L, "expecting exactly 1 argument");
  luaL_checktype(L, 1, LUA_TSTRING);

  const char* ident = lua_tostring(L, 1);
  oms_status_enu_t status = oms_initialize(ident);
  lua_pushinteger(L, status);
  return 1;
}

//oms_status_enu_t oms_terminate(const char* ident);
static int OMSimulatorLua_oms_terminate(lua_State *L)
{
  if (lua_gettop(L) != 1)
    return luaL_error(L, "expecting exactly 1 argument");
  luaL_checktype(L, 1, LUA_TSTRING);

  const char* ident = lua_tostring(L, 1);
  oms_status_enu_t status = oms_terminate(ident);
  lua_pushinteger(L, status);
  return 1;
}

//oms_status_enu_t oms_reset(const char* ident);
static int OMSimulatorLua_oms_reset(lua_State *L)
{
  if (lua_gettop(L) != 1)
    return luaL_error(L, "expecting exactly 1 argument");
  luaL_checktype(L, 1, LUA_TSTRING);

  const char* ident = lua_tostring(L, 1);
  oms_status_enu_t status = oms_reset(ident);
  lua_pushinteger(L, status);
  return 1;
}

//oms_status_enu_t oms_simulate(const char* cref);
static int OMSimulatorLua_oms_simulate(lua_State *L)
{
  if (lua_gettop(L) != 1)
    return luaL_error(L, "expecting exactly 1 argument");
  luaL_checktype(L, 1, LUA_TSTRING);

  const char* cref = lua_tostring(L, 1);
  oms_status_enu_t status = oms_simulate(cref);
  lua_pushinteger(L, status);
  return 1;
}

//oms_status_enu_t oms_stepUntil(const char* cref, double stopTime);
static int OMSimulatorLua_oms_stepUntil(lua_State *L)
{
  if (lua_gettop(L) != 2)
    return luaL_error(L, "expecting exactly 2 arguments");
  luaL_checktype(L, 1, LUA_TSTRING);
  luaL_checktype(L, 2, LUA_TNUMBER);

  const char* cref = lua_tostring(L, 1);
  double stopTime = lua_tonumber(L, 2);
  oms_status_enu_t status = oms_stepUntil(cref, stopTime);
  lua_pushinteger(L, status);
  return 1;
}

//oms_status_enu_t oms_getModelState(const char* cref, oms_modelState_enu_t* modelState);
static int OMSimulatorLua_oms_getModelState(lua_State *L)
{
  if (lua_gettop(L) != 1)
    return luaL_error(L, "expecting exactly 1 argument");
  luaL_checktype(L, 1, LUA_TSTRING);

  const char* cref = lua_tostring(L, 1);
  oms_modelState_enu_t modelState = oms_modelState_error;

  oms_status_enu_t status = oms_getModelState(cref, &modelState);
  lua_pushinteger(L, modelState);
  lua_pushinteger(L, status);
  return 2;
}

//oms_status_enu_t oms_setTLMSocketData(const char* cref, const char* address, int managerPort, int monitorPort)
static int OMSimulatorLua_oms_setTLMSocketData(lua_State *L)
{
  if (lua_gettop(L) != 4)
    return luaL_error(L, "expecting exactly 4 arguments");
  luaL_checktype(L, 1, LUA_TSTRING);
  luaL_checktype(L, 2, LUA_TSTRING);
  luaL_checktype(L, 3, LUA_TNUMBER);
  luaL_checktype(L, 4, LUA_TNUMBER);

  const char *cref =    lua_tostring(L, 1);
  const char *address = lua_tostring(L, 2);
  int managerPort =     lua_tonumber(L, 3);
  int monitorPort =     lua_tonumber(L, 4);

  oms_status_enu_t status = oms_setTLMSocketData(cref, address, managerPort, monitorPort);
  lua_pushinteger(L, status);
  return 1;
}

//oms_status_enu_t oms_setTLMPositionAndOrientation(const char *cref, double x1, double x2, double x3, double A11, double A12, double A13, double A21, double A22, double A23, double A31, double A32, double A33)
static int OMSimulatorLua_oms_setTLMPositionAndOrientation(lua_State *L)
{
  if (lua_gettop(L) != 13)
    return luaL_error(L, "expecting exactly 13 arguments");

  luaL_checktype(L, 1, LUA_TSTRING);
  const char *cref =  lua_tostring(L, 1);
  int i;

  //Position
  double x[3];
  for(i=0; i<3; ++i) {
    luaL_checktype(L, i+2, LUA_TNUMBER);
    x[i] = lua_tonumber(L, i+2);
  }

  //Orientation (3x3 matrix, stored as 1x9 vector)
  double A[9];
  for(i=0; i<9; ++i) {
    luaL_checktype(L, i+5, LUA_TNUMBER);
    A[i] = lua_tonumber(L, i+5);
  }

  oms_status_enu_t status = oms_setTLMPositionAndOrientation(cref,
                                                              x[0], x[1], x[2],
                                                              A[0], A[1], A[2],
                                                              A[3], A[4], A[5],
                                                              A[6], A[7], A[8]);

  lua_pushinteger(L, status);
  return 1;
}

//oms_status_enu_t oms_setResultFile(const char* cref, const char* filename, unsigned int bufferSize);
static int OMSimulatorLua_oms_setResultFile(lua_State *L)
{
  if (lua_gettop(L) != 2 && lua_gettop(L) != 3)
    return luaL_error(L, "expecting 2 or 3 arguments");
  luaL_checktype(L, 1, LUA_TSTRING);
  luaL_checktype(L, 2, LUA_TSTRING);

  const char* cref = lua_tostring(L, 1);
  const char* filename = lua_tostring(L, 2);

  unsigned int bufferSize = 1;
  if (lua_gettop(L) == 3)
  {
    luaL_checktype(L, 3, LUA_TNUMBER);
    bufferSize = lua_tonumber(L, 3);
  }
  oms_status_enu_t status = oms_setResultFile(cref, filename, bufferSize);
  lua_pushinteger(L, status);
  return 1;
}

//oms_status_enu_t oms_setSignalFilter(const char* cref, const char* regex)
static int OMSimulatorLua_oms_setSignalFilter(lua_State *L)
{
  if (lua_gettop(L) != 2)
    return luaL_error(L, "expecting exactly 2 argument");
  luaL_checktype(L, 1, LUA_TSTRING);
  luaL_checktype(L, 2, LUA_TSTRING);

  const char* cref = lua_tostring(L, 1);
  const char* regex = lua_tostring(L, 2);
  oms_status_enu_t status = oms_setSignalFilter(cref, regex);
  lua_pushinteger(L, status);
  return 1;
}

//OMSAPI oms_status_enu_t OMSCALL oms_setSolver(const char* cref, oms_solver_enu_t solver);
static int OMSimulatorLua_oms_setSolver(lua_State *L)
{
  if (lua_gettop(L) != 2)
    return luaL_error(L, "expecting exactly 2 argument");
  luaL_checktype(L, 1, LUA_TSTRING);
  luaL_checktype(L, 2, LUA_TNUMBER);

  const char* cref = lua_tostring(L, 1);
  oms_solver_enu_t solver = (oms_solver_enu_t)lua_tointeger(L, 2);
  oms_status_enu_t status = oms_setSolver(cref, solver);
  lua_pushinteger(L, status);
  return 1;
}

//oms_status_enu_t oms_addSignalsToResults(const char* cref, const char* regex);
static int OMSimulatorLua_oms_addSignalsToResults(lua_State *L)
{
  if (lua_gettop(L) != 2)
    return luaL_error(L, "expecting exactly 2 argument");
  luaL_checktype(L, 1, LUA_TSTRING);

  const char* cref = lua_tostring(L, 1);
  const char* regex = lua_tostring(L, 2);
  oms_status_enu_t status = oms_addSignalsToResults(cref, regex);
  lua_pushinteger(L, status);
  return 1;
}

//oms_status_enu_t oms_removeSignalsFromResults(const char* cref, const char* regex);
static int OMSimulatorLua_oms_removeSignalsFromResults(lua_State *L)
{
  if (lua_gettop(L) != 2)
    return luaL_error(L, "expecting exactly 2 argument");
  luaL_checktype(L, 1, LUA_TSTRING);

  const char* cref = lua_tostring(L, 1);
  const char* regex = lua_tostring(L, 2);
  oms_status_enu_t status = oms_removeSignalsFromResults(cref, regex);
  lua_pushinteger(L, status);
  return 1;
}

//oms_status_enu_t oms_getStartTime(const char* cref, double* startTime);
static int OMSimulatorLua_oms_getStartTime(lua_State *L)
{
  if (lua_gettop(L) != 1)
    return luaL_error(L, "expecting exactly 1 argument");
  luaL_checktype(L, 1, LUA_TSTRING);

  const char* cref = lua_tostring(L, 1);
  double startTime = 0.0;
  oms_status_enu_t status = oms_getStartTime(cref, &startTime);
  lua_pushnumber(L, startTime);
  lua_pushinteger(L, status);
  return 2;
}

//oms_status_enu_t oms_setStartTime(const char* cref, double startTime);
static int OMSimulatorLua_oms_setStartTime(lua_State *L)
{
  if (lua_gettop(L) != 2)
    return luaL_error(L, "expecting exactly 2 arguments");
  luaL_checktype(L, 1, LUA_TSTRING);
  luaL_checktype(L, 2, LUA_TNUMBER);

  const char* cref = lua_tostring(L, 1);
  double startTime = lua_tonumber(L, 2);
  oms_status_enu_t status = oms_setStartTime(cref, startTime);
  lua_pushinteger(L, status);
  return 1;
}

//oms_status_enu_t oms_getStopTime(const char* cref, double* stopTime);
static int OMSimulatorLua_oms_getStopTime(lua_State *L)
{
  if (lua_gettop(L) != 1)
    return luaL_error(L, "expecting exactly 1 argument");
  luaL_checktype(L, 1, LUA_TSTRING);

  const char* cref = lua_tostring(L, 1);
  double stopTime = 1.0;
  oms_status_enu_t status = oms_getStopTime(cref, &stopTime);
  lua_pushnumber(L, stopTime);
  lua_pushinteger(L, status);
  return 2;
}

//oms_status_enu_t oms_setStopTime(const char* cref, double stopTime);
static int OMSimulatorLua_oms_setStopTime(lua_State *L)
{
  if (lua_gettop(L) != 2)
    return luaL_error(L, "expecting exactly 2 arguments");
  luaL_checktype(L, 1, LUA_TSTRING);
  luaL_checktype(L, 2, LUA_TNUMBER);

  const char* cref = lua_tostring(L, 1);
  double startTime = lua_tonumber(L, 2);
  oms_status_enu_t status = oms_setStopTime(cref, startTime);
  lua_pushinteger(L, status);
  return 1;
}

//oms_status_enu_t oms_setFixedStepSize(const char* cref, double stepSize);
static int OMSimulatorLua_oms_setFixedStepSize(lua_State *L)
{
  if (lua_gettop(L) != 2)
    return luaL_error(L, "expecting exactly 2 arguments");
  luaL_checktype(L, 1, LUA_TSTRING);
  luaL_checktype(L, 2, LUA_TNUMBER);

  const char* cref = lua_tostring(L, 1);
  double stepSize = lua_tonumber(L, 2);
  oms_status_enu_t status = oms_setFixedStepSize(cref, stepSize);
  lua_pushinteger(L, status);
  return 1;
}

DLLEXPORT int luaopen_OMSimulatorLua(lua_State *L)
{
  REGISTER_LUA_CALL(oms_addBus);
  REGISTER_LUA_CALL(oms_addConnection);
  REGISTER_LUA_CALL(oms_addConnector);
  REGISTER_LUA_CALL(oms_addConnectorToBus);
  REGISTER_LUA_CALL(oms_addConnectorToTLMBus);
  REGISTER_LUA_CALL(oms_addDynamicValueIndicator);
  REGISTER_LUA_CALL(oms_addEventIndicator);
  REGISTER_LUA_CALL(oms_addExternalModel);
  REGISTER_LUA_CALL(oms_addSignalsToResults);
  REGISTER_LUA_CALL(oms_addStaticValueIndicator);
  REGISTER_LUA_CALL(oms_addSubModel);
  REGISTER_LUA_CALL(oms_addSystem);
  REGISTER_LUA_CALL(oms_addTimeIndicator);
  REGISTER_LUA_CALL(oms_addTLMBus);
  REGISTER_LUA_CALL(oms_addTLMConnection);
  REGISTER_LUA_CALL(oms_compareSimulationResults);
  REGISTER_LUA_CALL(oms_copySystem);
  REGISTER_LUA_CALL(oms_delete);
  REGISTER_LUA_CALL(oms_deleteConnection);
  REGISTER_LUA_CALL(oms_deleteConnectorFromBus);
  REGISTER_LUA_CALL(oms_deleteConnectorFromTLMBus);
  REGISTER_LUA_CALL(oms_export);
  REGISTER_LUA_CALL(oms_exportDependencyGraphs);
  REGISTER_LUA_CALL(oms_exportSnapshot);
  REGISTER_LUA_CALL(oms_exportSSVTemplate);
  REGISTER_LUA_CALL(oms_exportSSMTemplate);
  REGISTER_LUA_CALL(oms_faultInjection);
  REGISTER_LUA_CALL(oms_getBoolean);
  REGISTER_LUA_CALL(oms_getFixedStepSize);
  REGISTER_LUA_CALL(oms_getInteger);
  REGISTER_LUA_CALL(oms_getModelState);
  REGISTER_LUA_CALL(oms_getReal);
  REGISTER_LUA_CALL(oms_getSolver);
  REGISTER_LUA_CALL(oms_getStartTime);
  REGISTER_LUA_CALL(oms_getStopTime);
  REGISTER_LUA_CALL(oms_getSystemType);
  REGISTER_LUA_CALL(oms_getVariableStepSize);
  REGISTER_LUA_CALL(oms_getVersion);
  REGISTER_LUA_CALL(oms_importFile);
  REGISTER_LUA_CALL(oms_importSnapshot);
  REGISTER_LUA_CALL(oms_initialize);
  REGISTER_LUA_CALL(oms_instantiate);
  REGISTER_LUA_CALL(oms_listUnconnectedConnectors);
  REGISTER_LUA_CALL(oms_newModel);
  REGISTER_LUA_CALL(oms_removeSignalsFromResults);
  REGISTER_LUA_CALL(oms_rename);
  REGISTER_LUA_CALL(oms_reset);
  REGISTER_LUA_CALL(oms_setBoolean);
  REGISTER_LUA_CALL(oms_setCommandLineOption);
  REGISTER_LUA_CALL(oms_setFixedStepSize);
  REGISTER_LUA_CALL(oms_setInteger);
  REGISTER_LUA_CALL(oms_setLogFile);
  REGISTER_LUA_CALL(oms_setLoggingInterval);
  REGISTER_LUA_CALL(oms_setLoggingLevel);
  REGISTER_LUA_CALL(oms_setMaxLogFileSize);
  REGISTER_LUA_CALL(oms_setReal);
  REGISTER_LUA_CALL(oms_setRealInputDerivative);
  REGISTER_LUA_CALL(oms_setResultFile);
  REGISTER_LUA_CALL(oms_setSignalFilter);
  REGISTER_LUA_CALL(oms_setSolver);
  REGISTER_LUA_CALL(oms_setStartTime);
  REGISTER_LUA_CALL(oms_setStopTime);
  REGISTER_LUA_CALL(oms_setTempDirectory);
  REGISTER_LUA_CALL(oms_setTLMPositionAndOrientation);
  REGISTER_LUA_CALL(oms_setTLMSocketData);
  REGISTER_LUA_CALL(oms_setTolerance);
  REGISTER_LUA_CALL(oms_setVariableStepSize);
  REGISTER_LUA_CALL(oms_setWorkingDirectory);
  REGISTER_LUA_CALL(oms_simulate);
  REGISTER_LUA_CALL(oms_stepUntil);
  REGISTER_LUA_CALL(oms_terminate);

  // oms_tlm_interpolation_t
  REGISTER_LUA_ENUM(oms_tlm_no_interpolation);
  REGISTER_LUA_ENUM(oms_tlm_coarse_grained);
  REGISTER_LUA_ENUM(oms_tlm_fine_grained);

  // oms_causality_enu_t;
  REGISTER_LUA_ENUM(oms_causality_input);
  REGISTER_LUA_ENUM(oms_causality_output);
  REGISTER_LUA_ENUM(oms_causality_parameter);
  REGISTER_LUA_ENUM(oms_causality_bidir);
  REGISTER_LUA_ENUM(oms_causality_undefined);

  // oms_solver_enu_t
  REGISTER_LUA_ENUM(oms_solver_sc_explicit_euler);
  REGISTER_LUA_ENUM(oms_solver_sc_cvode);
  REGISTER_LUA_ENUM(oms_solver_wc_ma);
  REGISTER_LUA_ENUM(oms_solver_wc_mav);
  REGISTER_LUA_ENUM(oms_solver_wc_assc);
  REGISTER_LUA_ENUM(oms_solver_wc_mav2);

  // oms_system_enu_t
  REGISTER_LUA_ENUM(oms_system_none);
  REGISTER_LUA_ENUM(oms_system_tlm);
  REGISTER_LUA_ENUM(oms_system_wc);
  REGISTER_LUA_ENUM(oms_system_sc);

  // oms_status_enu_t
  REGISTER_LUA_ENUM(oms_status_ok);
  REGISTER_LUA_ENUM(oms_status_warning);
  REGISTER_LUA_ENUM(oms_status_discard);
  REGISTER_LUA_ENUM(oms_status_error);
  REGISTER_LUA_ENUM(oms_status_fatal);
  REGISTER_LUA_ENUM(oms_status_pending);

  // oms_modelState_enu_t
  REGISTER_LUA_ENUM(oms_modelState_virgin);
  REGISTER_LUA_ENUM(oms_modelState_enterInstantiation);
  REGISTER_LUA_ENUM(oms_modelState_instantiated);
  REGISTER_LUA_ENUM(oms_modelState_initialization);
  REGISTER_LUA_ENUM(oms_modelState_simulation);
  REGISTER_LUA_ENUM(oms_modelState_error);

  // oms_signal_type_enu_t
  REGISTER_LUA_ENUM(oms_signal_type_real);
  REGISTER_LUA_ENUM(oms_signal_type_integer);
  REGISTER_LUA_ENUM(oms_signal_type_boolean);
  REGISTER_LUA_ENUM(oms_signal_type_string);

  // oms_tlm_domain_t
  REGISTER_LUA_ENUM(oms_tlm_domain_input);
  REGISTER_LUA_ENUM(oms_tlm_domain_output);
  REGISTER_LUA_ENUM(oms_tlm_domain_mechanical);
  REGISTER_LUA_ENUM(oms_tlm_domain_rotational);
  REGISTER_LUA_ENUM(oms_tlm_domain_hydraulic);
  REGISTER_LUA_ENUM(oms_tlm_domain_electric);

  // oms_fault_type_enu_t
  REGISTER_LUA_ENUM(oms_fault_type_bias);
  REGISTER_LUA_ENUM(oms_fault_type_gain);
  REGISTER_LUA_ENUM(oms_fault_type_const);

  return 0;
}
