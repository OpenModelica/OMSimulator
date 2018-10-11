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

#ifndef _OMS_COMPONENT_FMU_CS_H_
#define _OMS_COMPONENT_FMU_CS_H_

#include "Component.h"
#include "ComRef.h"
#include "Option.h"
#include "Variable.h"
#include <fmilib.h>
#include <map>
#include <pugixml.hpp>
#include <string>
#include <unordered_map>
#include <vector>

namespace oms3
{
  class System;

  class ComponentFMUCS : public Component
  {
  public:
    ~ComponentFMUCS();

    static Component* NewComponent(const oms3::ComRef& cref, System* parentSystem, const std::string& fmuPath);
    static Component* NewComponent(const pugi::xml_node& node, oms3::System* parentSystem);

  protected:
    ComponentFMUCS(const ComRef& cref, System* parentSystem, const std::string& fmuPath);

    // stop the compiler generating methods copying the object
    ComponentFMUCS(ComponentFMUCS const& copy);            ///< not implemented
    ComponentFMUCS& operator=(ComponentFMUCS const& copy); ///< not implemented

  private:
    jm_callbacks callbacks;
    fmi2_callback_functions_t callbackFunctions;
    fmi_import_context_t* context = NULL;
    fmi2_import_t* fmu = NULL;
    std::string tempDir;

    std::vector<Variable> allVariables;
    std::vector<Variable> inputs;
    std::vector<Variable> outputs;
    std::vector<Variable> parameters;
    std::vector<bool> exportVariables;

    std::map<std::string, Option<double>> realParameters;
    std::map<std::string, Option<int>> integerParameters;
    std::map<std::string, Option<bool>> booleanParameters;

  };
}

#endif
