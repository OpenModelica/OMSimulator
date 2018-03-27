/*
 * This file is part of OpenModelica.
 *
 * Copyright (c) 1998-CurrentYear, Open Source Modelica Consortium (OSMC),
 * c/o Linköpings universitet, Department of Computer and Information Science,
 * SE-58183 Linköping, Sweden.
 *
 * All rights reserved.
 *
 * THIS PROGRAM IS PROVIDED UNDER THE TERMS OF GPL VERSION 3 LICENSE OR
 * THIS OSMC PUBLIC LICENSE (OSMC-PL) VERSION 1.2.
 * ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS PROGRAM CONSTITUTES
 * RECIPIENT'S ACCEPTANCE OF THE OSMC PUBLIC LICENSE OR THE GPL VERSION 3,
 * ACCORDING TO RECIPIENTS CHOICE.
 *
 * The OpenModelica software and the Open Source Modelica
 * Consortium (OSMC) Public License (OSMC-PL) are obtained
 * from OSMC, either from the above address,
 * from the URLs: http://www.ida.liu.se/projects/OpenModelica or
 * http://www.openmodelica.org, and in the OpenModelica distribution.
 * GNU version 3 is obtained from: http://www.gnu.org/copyleft/gpl.html.
 *
 * This program is distributed WITHOUT ANY WARRANTY; without
 * even the implied warranty of  MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE, EXCEPT AS EXPRESSLY SET FORTH
 * IN THE BY RECIPIENT SELECTED SUBSIDIARY LICENSE CONDITIONS OF OSMC-PL.
 *
 * See the full OSMC Public License conditions for more details.
 *
 */

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include <stdlib.h>
#include <string.h>
#include <OMSimulator.h>
#include <OMSysIdent.h>

#define REGISTER_LUA_CALL_OMFIT(name) lua_register(L, #name, OMSysIdentLua_##name)

#ifdef _WIN32
  #define DLLEXPORT __declspec(dllexport)
#else
  #define DLLEXPORT
#endif

void push_pointer2(lua_State *L, void *p)
{
  void **bp = (void**)lua_newuserdata(L, sizeof(p));
  *bp = p;
}

void* topointer2(lua_State *L, int index)
{
  void **bp = (void**)lua_touserdata(L, index);
  return *bp;
}

//void* omsi_newSysIdentModel(void* model);
static int OMSysIdentLua_omsi_newSysIdentModel(lua_State *L)
{
  if (lua_gettop(L) != 1)
    return luaL_error(L, "expecting exactly 1 argument");
  luaL_checktype(L, 1, LUA_TUSERDATA);

  void *model = topointer2(L, 1);

  void *pModel = omsi_newSysIdentModel(model);
  push_pointer2(L, pModel);
  return 1;
}

//void omsi_freeSysIdentModel(void* fitmodel);
static int OMSysIdentLua_omsi_freeSysIdentModel(lua_State *L)
{
  if (lua_gettop(L) != 1)
    return luaL_error(L, "expecting exactly 1 argument");
  luaL_checktype(L, 1, LUA_TUSERDATA);

  void *model = topointer2(L, 1);
  omsi_freeSysIdentModel(model);
  return 0;
}

//void oms_setReal(void* model, const char* var, double value);
// oms_status_enu_t omsi_initialize(void* fitmodel, size_t nSeries, const double* time, size_t nTime, char const* const* inputvars, size_t nInputvars, char const* const* measurementvars, size_t nMeasurementvars);
static int OMSysIdentLua_omsi_initialize(lua_State *L)
{
  if (lua_gettop(L) != 5)
    return luaL_error(L, "expecting exactly 5 arguments");
  luaL_checktype(L, 1, LUA_TUSERDATA); // fitmodel
  luaL_checktype(L, 2, LUA_TNUMBER);   // nSeries
  luaL_checktype(L, 3, LUA_TTABLE);    // time
  luaL_checktype(L, 4, LUA_TTABLE);    // inputvars
  luaL_checktype(L, 5, LUA_TTABLE);    // measurementvars
  // luaL_checktype(L, 2, LUA_TSTRING);

  void *model = topointer2(L, 1);
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

//oms_status_enu_t omsi_describe(void* fitmodel);
static int OMSysIdentLua_omsi_describe(lua_State *L)
{
  if (lua_gettop(L) != 1)
    return luaL_error(L, "expecting exactly 1 argument");
  luaL_checktype(L, 1, LUA_TUSERDATA);

  void *model = topointer2(L, 1);
  oms_status_enu_t returnValue = omsi_describe(model);
  lua_pushinteger(L, returnValue);
  return 1;
}

// oms_status_enu_t omsi_addMeasurement(void* fitmodel, size_t iSeries, const char* var, const double* values, size_t nValues);
static int OMSysIdentLua_omsi_addMeasurement(lua_State *L)
{
  if (lua_gettop(L) != 4)
    return luaL_error(L, "expecting exactly 4 arguments");
  luaL_checktype(L, 1, LUA_TUSERDATA); // fitmodel
  luaL_checktype(L, 2, LUA_TNUMBER);   // iSeries
  luaL_checktype(L, 3, LUA_TSTRING);   // var
  luaL_checktype(L, 4, LUA_TTABLE);    // values

  void *model = topointer2(L, 1);
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

// oms_status_enu_t omsi_addParameter(void* fitmodel, const char* var, double startvalue);
static int OMSysIdentLua_omsi_addParameter(lua_State *L)
{
  if (lua_gettop(L) != 3)
    return luaL_error(L, "expecting exactly 3 arguments");
  luaL_checktype(L, 1, LUA_TUSERDATA); // fitmodel
  luaL_checktype(L, 2, LUA_TSTRING);   // var
  luaL_checktype(L, 3, LUA_TNUMBER);   // startvalue

  void *model = topointer2(L, 1);
  const char* var = lua_tostring(L, 2);
  double startvalue = lua_tonumber(L, 3);

  oms_status_enu_t returnValue =
    omsi_addParameter(model, var, startvalue);
  lua_pushinteger(L, returnValue);
  return 1;
}

// oms_status_enu_t omsi_getParameter(void* fitmodel, const char* var, double* startvalue, double* estimatedvalue);
static int OMSysIdentLua_omsi_getParameter(lua_State *L)
{
  if (lua_gettop(L) != 2)
    return luaL_error(L, "expecting exactly 2 arguments");
  luaL_checktype(L, 1, LUA_TUSERDATA); // fitmodel
  luaL_checktype(L, 2, LUA_TSTRING);   // var

  void *model = topointer2(L, 1);
  const char* var = lua_tostring(L, 2);

  double startvalue, estimatedvalue;
  oms_status_enu_t returnValue =
    omsi_getParameter(model, var, &startvalue, &estimatedvalue);
  lua_pushinteger(L, returnValue);
  lua_pushnumber(L, startvalue);
  lua_pushnumber(L, estimatedvalue);
  return 3;
}

// oms_status_enu_t omsi_solve(void* fitmodel, const char* reporttype);
static int OMSysIdentLua_omsi_solve(lua_State *L)
{
  if (lua_gettop(L) != 2)
    return luaL_error(L, "expecting exactly 2 arguments");
  luaL_checktype(L, 1, LUA_TUSERDATA); // fitmodel
  luaL_checktype(L, 2, LUA_TSTRING);   // reporttype

  void *model = topointer2(L, 1);
  const char* reporttype = lua_tostring(L, 2);

  oms_status_enu_t returnValue =
    omsi_solve(model, reporttype);
  lua_pushinteger(L, returnValue);
  return 1;
}

// oms_status_enu_t omsi_setOptions_max_num_iterations(void* fitmodel, size_t max_num_iterations);
static int OMSysIdentLua_omsi_setOptions_max_num_iterations(lua_State *L)
{
  if (lua_gettop(L) != 2)
    return luaL_error(L, "expecting exactly 2 arguments");
  luaL_checktype(L, 1, LUA_TUSERDATA); // fitmodel
  luaL_checktype(L, 2, LUA_TNUMBER);   // max_num_iterations

  void *model = topointer2(L, 1);
  int max_num_iterations = lua_tointeger(L, 2);

  oms_status_enu_t returnValue =
    omsi_setOptions_max_num_iterations(model, max_num_iterations);
  lua_pushinteger(L, returnValue);
  return 1;
}

// oms_status_enu_t omsi_getState(void* fitmodel, omsi_fitmodelstate_t* state);
static int OMSysIdentLua_omsi_getState(lua_State *L)
{
  if (lua_gettop(L) != 1)
    return luaL_error(L, "expecting exactly 1 argument");
  luaL_checktype(L, 1, LUA_TUSERDATA); // fitmodel

  void *model = topointer2(L, 1);

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

DLLEXPORT int luaopen_OMSysIdentLua(lua_State *L)
{
  REGISTER_LUA_CALL_OMFIT(omsi_newSysIdentModel);
  REGISTER_LUA_CALL_OMFIT(omsi_freeSysIdentModel);
  REGISTER_LUA_CALL_OMFIT(omsi_initialize);
  REGISTER_LUA_CALL_OMFIT(omsi_describe);
  REGISTER_LUA_CALL_OMFIT(omsi_addMeasurement);
  REGISTER_LUA_CALL_OMFIT(omsi_addParameter);
  REGISTER_LUA_CALL_OMFIT(omsi_getParameter);
  REGISTER_LUA_CALL_OMFIT(omsi_solve);
  REGISTER_LUA_CALL_OMFIT(omsi_setOptions_max_num_iterations);
  REGISTER_LUA_CALL_OMFIT(omsi_getState);
  return 0;
}
