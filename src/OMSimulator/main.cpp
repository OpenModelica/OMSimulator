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

#include <cstring>
#include <iostream>
#include <Logging.h>
#include <OMSimulator.h>

extern "C"
{
  #include <OMSimulatorLua.c>
}

bool isLuaScript(const char* filename)
{
  size_t len = strlen(filename);
  if (len < 5)
    return false;

  if (filename[0] != '-' &&
      filename[len-4] == '.' &&
      filename[len-3] == 'l' &&
      filename[len-2] == 'u' &&
      filename[len-1] == 'a')
    return true;
  return false;
}

int main(int argc, char *argv[])
{
  for (int i=1; i<argc; ++i)
  {
    if (0 == strcmp(argv[i], "--help") || 0 == strcmp(argv[i], "-h"))
    {
      logInfo("Usage: OMSimulator [Options|Lua script]");
      oms_setCommandLineOption(argv[i]);
      return 0;
    }
  }

  for (int i=1; i<argc; ++i)
  {
    if (isLuaScript(argv[i]))
    {
      lua_State *L = luaL_newstate();
      luaL_openlibs(L);
      luaopen_OMSimulatorLua(L);
      if (luaL_loadfile(L, argv[i]))
      {
        logError(lua_tostring(L, -1));
        return 1;
      }

      if (lua_pcall(L, 0, 0, 0))
      {
        logError(lua_tostring(L, -1));
        return 1;
      }

      lua_close(L);
    }
    else if (oms_status_ok != oms_setCommandLineOption(argv[i]))
      return 1;
  }

  return 0;
}
