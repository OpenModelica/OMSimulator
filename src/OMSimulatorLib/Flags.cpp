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

#include "Flags.h"

#include "Component.h"
#include "ComRef.h"
#include "Logging.h"
#include "Model.h"
#include "Scope.h"
#include "System.h"
#include <RegEx.h>

oms3::Flags::Flags()
  : suppressPath(false)
{
}

oms3::Flags::~Flags()
{
}

oms3::Flags& oms3::Flags::GetInstance()
{
  // the only instance
  static Flags flags;
  return flags;
}

bool isOption(const std::string& cmd, const std::string& name)
{
  return (0 == cmd.compare(name));
}

bool isOptionAndValue(const std::string& cmd, const std::string& name, std::string& value, oms_regex re)
{
  if (0 == cmd.compare(0, name.length()+1, name + "="))
  {
    value = cmd.substr(name.length()+1);
    return regex_match(value, re);
  }

  return false;
}

oms_status_enu_t oms3::Flags::SetCommandLineOption(const std::string& cmd)
{
  const oms_regex re_bool("true|false");
  const oms_regex re_default(".+");
  std::string value;

  if (isOptionAndValue(cmd, "--suppressPath", value, re_bool))
    oms3::Flags::SuppressPath(value == "true");
  else if (isOptionAndValue(cmd, "--fetchAllVars", value, re_default))
  {
    oms3::ComRef tail(value);
    oms3::ComRef front = tail.pop_front();

    oms3::Model* model = oms3::Scope::GetInstance().getModel(front);
    if (!model)
      return logError_ModelNotInScope(front);

    front = tail.pop_front();
    oms3::System* system = model->getSystem(front);
    if (!system)
      return logError_SystemNotInModel(model->getCref(), front);

    oms3::Component* component = system->getComponent(tail);
    if (!component)
      return logError_ComponentNotInSystem(system, tail);

    if (component->getType() != oms_component_fmu)
      return oms_status_error;

    component->fetchAllVars();
  }
  else
    return logError("Unknown flag or option: \"" + cmd + "\"");

  return oms_status_ok;
}
