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


#include "Util.h"
#include "Logging.h"
#include "Parameters.h"
#include "Types.h"
#include "ComRef.h"
#include "ssd/Tags.h"

#include <pugixml.hpp>
#include <map>
#include <string>
#include <iostream>


oms::Parameters::Parameters()
{
}

oms::Parameters::~Parameters()
{
}

oms_status_enu_t oms::Parameters::setReal(const ComRef& cref, double value)
{
  realStartValues[cref] = value;
}

oms_status_enu_t oms::Parameters::setInteger(const ComRef& cref, int value)
{
  integerStartValues[cref] = value;
}

oms_status_enu_t oms::Parameters::setBoolean(const ComRef& cref, bool value)
{
  booleanStartValues[cref] = value;
}

oms_status_enu_t oms::Parameters::exportToSSD(pugi::xml_node& node) const
{
  // skip this if there is nothing to export
  if (realStartValues.empty() && integerStartValues.empty() && booleanStartValues.empty())
    return oms_status_ok;

  // Top level Parameter nodes
  pugi::xml_node node_parameters_bindings = node.append_child(oms::ssp::Version1_0::ssd::parameter_bindings);
  pugi::xml_node node_parameter_binding  = node_parameters_bindings.append_child(oms::ssp::Version1_0::ssd::parameter_binding);
  pugi::xml_node node_parameterset = node_parameter_binding.append_child(oms::ssp::Version1_0::ssv::parameter_set);
  node_parameterset.append_attribute("version") = "1.0";
  node_parameterset.append_attribute("name") = "parameters";
  pugi::xml_node node_parameters = node_parameterset.append_child(oms::ssp::Version1_0::ssv::parameters);

  // realStartValues
  for (const auto& r : realStartValues)
  {
    pugi::xml_node node_parameter = node_parameters.append_child(oms::ssp::Version1_0::ssv::parameter);
    node_parameter.append_attribute("name") = r.first.c_str();
    pugi::xml_node node_parameter_type = node_parameter.append_child(oms::ssp::Version1_0::ssv::real_type);
    node_parameter_type.append_attribute("value") = r.second;
  }

  // integerStartValues
  for (const auto& i : integerStartValues)
  {
    pugi::xml_node node_parameter = node_parameters.append_child(oms::ssp::Version1_0::ssv::parameter);
    node_parameter.append_attribute("name") = i.first.c_str();
    pugi::xml_node node_parameter_type = node_parameter.append_child(oms::ssp::Version1_0::ssv::integer_type);
    node_parameter_type.append_attribute("value") = i.second;
  }

  // boolStartValues
  for (const auto& b : booleanStartValues)
  {
    pugi::xml_node node_parameter = node_parameters.append_child(oms::ssp::Version1_0::ssv::parameter);
    node_parameter.append_attribute("name") = b.first.c_str();
    pugi::xml_node node_parameter_type = node_parameter.append_child(oms::ssp::Version1_0::ssv::boolean_type);
    node_parameter_type.append_attribute("value") = b.second;
  }

  return oms_status_ok;
}
