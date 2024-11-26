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
  int EntryPoint(int argc, char *argv[])
  {
    Application app(oms_getVersion(), 800, 600);
    const std::shared_ptr<ScopeLayer> &layer = std::make_shared<ScopeLayer>(app);

    for (int i = 1; i < argc; ++i)
      layer->LoadModel(argv[i]);

    app.PushLayer(std::make_shared<ScopeLayer>(app));
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
  return oms::EntryPoint(__argc, __argv);
}
#else
int main(int argc, char *argv[])
{
  return oms::EntryPoint(argc, argv);
}
#endif
