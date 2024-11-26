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
 * even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE, EXCEPT AS EXPRESSLY SET FORTH
 * IN THE BY RECIPIENT SELECTED SUBSIDIARY LICENSE CONDITIONS OF OSMC-PL.
 *
 * See the full OSMC Public License conditions for more details.
 *
 */

#include "OMSimulator/OMSimulator.h"
#include <string>

#include "Application.h"
#include "DemoLayer.h"
#include "Layer.h"

namespace oms
{
  int EntryPoint(int argc, char *argv[])
  {
    std::string arg;
    for (int i = 1; i < argc; ++i)
    {
      if (!arg.empty())
        arg += " ";
      arg += argv[i];
    }

    oms_status_enu_t status = oms_status_ok;
    if (arg.empty())
    {
      Application app(oms_getVersion(), 800, 600);
      app.PushLayer(std::make_shared<DemoLayer>(app));
      app.Run();
    }
    else
      status = oms_setCommandLineOption(arg.c_str());

    if (oms_status_ok != status)
      return 1;

    return 0;
  }
}

#if defined(_MSC_VER)
#include <windows.h>
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
  bool console = false;
  if (AttachConsole(ATTACH_PARENT_PROCESS))
  {
    console = true;
    FILE *fp;
    freopen_s(&fp, "CONOUT$", "w", stdout); // Redirect stdout
    freopen_s(&fp, "CONOUT$", "w", stderr); // Redirect stderr
    freopen_s(&fp, "CONIN$", "r", stdin);   // Redirect stdin
  }

  FILE *fp;
  freopen_s(&fp, "CONOUT$", "w", stdout);

  int return_value = oms::EntryPoint(__argc, __argv);

  if (console)
    FreeConsole();

  return return_value;
}
#else
int main(int argc, char *argv[])
{
  return oms::EntryPoint(argc, argv);
}
#endif
