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
#include "ScopeLayer.h"
#include "Layer.h"

namespace oms
{
  int EntryPoint(const std::vector<std::string> &args)
  {
    Application app(oms_getVersion(), 800, 600);
    const std::shared_ptr<ScopeLayer> &layer = std::make_shared<ScopeLayer>(app);

    for (const auto &arg : args)
      layer->LoadModel(arg);

    app.PushLayer(layer);
#if !defined(NDEBUG)
    app.PushLayer(std::make_shared<DemoLayer>(app));
#endif
    app.Run();

    return 0;
  }
}

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
  std::vector<std::string> args;
  int argc;
  LPWSTR *argv = CommandLineToArgvW(GetCommandLineW(), &argc);
  for (int i = 0; i < argc; ++i)
  {
    std::wstring ws(argv[i]);
    std::string str(ws.begin(), ws.end());
    args.push_back(str);
  }
  LocalFree(argv);

  return oms::EntryPoint(args);
}
#else
int main(int argc, char *argv[])
{
  std::vector<std::string> args;
  for (int i = 0; i < argc; ++i)
    args.push_back(argv[i]);

  return oms::EntryPoint(args);
}
#endif
