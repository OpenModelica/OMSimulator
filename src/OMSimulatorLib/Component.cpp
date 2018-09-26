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

#include "Component.h"

#include "Model.h"
#include "ssd/Tags.h"
#include "System.h"
#include <OMSBoost.h>

oms3::Component::Component(const ComRef& cref, oms_component_enu_t type, System* parentSystem, const std::string& path)
  : element(oms_element_component, cref), cref(cref), type(type), parentSystem(parentSystem), path(path)
{
  connectors.push_back(NULL);
  element.setConnectors(&connectors[0]);
}

oms3::Component::~Component()
{
  for (const auto& connector : connectors)
    if (connector)
      delete connector;
}

oms_status_enu_t oms3::Component::exportToSSD(pugi::xml_node& node) const
{
  node.append_attribute("name") = this->getName().c_str();
  node.append_attribute("type") = "application/x-fmu-sharedlibrary";
  node.append_attribute("source") = path.c_str();
  return oms_status_ok;
}

oms3::Connector* oms3::Component::getConnector(const oms3::ComRef &cref)
{
  for(auto &connector : connectors) {
    if(connector && connector->getName() == cref)
      return connector;
  }
  return NULL;
}

oms_status_enu_t oms3::Component::deleteResources()
{
  boost::filesystem::path temp_root(parentSystem->getModel()->getTempDirectory());
  boost::filesystem::path temp_temp = temp_root / "temp";
  boost::filesystem::path temp_resources = temp_root / "resources";

  boost::filesystem::path relFMUPath = boost::filesystem::path("resources") / (std::string(cref) + ".fmu");
  boost::filesystem::path absFMUPath = temp_root / relFMUPath;

  // delete resources
  boost::filesystem::remove(absFMUPath);

  // delete temp directory
  boost::filesystem::remove(temp_temp / std::string(cref));

  return oms_status_ok;
}
