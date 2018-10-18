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

#include "ComponentFMUME.h"

#include "ssd/Tags.h"
#include "Logging.h"

oms3::ComponentFMUME::ComponentFMUME(const ComRef& cref, System* parentSystem, const std::string& fmuPath)
  : oms3::Component(cref, oms_component_fmu, parentSystem, fmuPath)
{
}

oms3::ComponentFMUME::~ComponentFMUME()
{
}

oms3::Component* oms3::ComponentFMUME::NewComponent(const oms3::ComRef& cref, oms3::System* parentSystem, const std::string& fmuPath)
{
  if (!cref.isValidIdent())
  {
    logError("\"" + std::string(cref) + "\" is not a valid ident");
    return NULL;
  }

  if (!parentSystem)
  {
    logError("Internal error");
    return NULL;
  }

  Component* component = new ComponentFMUME(cref, parentSystem, fmuPath);
  return component;
}

oms_status_enu_t oms3::ComponentFMUME::exportToSSD(pugi::xml_node& node) const
{
  node.append_attribute("name") = this->getName().c_str();
  node.append_attribute("type") = "application/x-fmu-sharedlibrary";
  node.append_attribute("source") = getPath().c_str();
  pugi::xml_node node_connectors = node.append_child(oms2::ssd::ssd_connectors);

  for (const auto& connector : connectors)
    if (connector)
      if (oms_status_ok != connector->exportToSSD(node_connectors))
        return oms_status_error;
  return oms_status_ok;
}

oms_status_enu_t oms3::ComponentFMUME::instantiate()
{
  return logError_NotImplemented;
}

oms_status_enu_t oms3::ComponentFMUME::initialize()
{
  return logError_NotImplemented;
}

oms_status_enu_t oms3::ComponentFMUME::terminate()
{
  return logError_NotImplemented;
}
