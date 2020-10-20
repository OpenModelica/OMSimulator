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
#include "Values.h"
#include "Types.h"
#include "ComRef.h"
#include "ssd/Tags.h"
#include <OMSFileSystem.h>

#include <pugixml.hpp>
#include <map>
#include <string>
#include <iostream>


oms::Values::Values()
{
}

oms::Values::~Values()
{
}

oms_status_enu_t oms::Values::setReal(const ComRef& cref, double value)
{
  realStartValues[cref] = value;
  return oms_status_ok;
}

oms_status_enu_t oms::Values::setInteger(const ComRef& cref, int value)
{
  integerStartValues[cref] = value;
  return oms_status_ok;
}

oms_status_enu_t oms::Values::setBoolean(const ComRef& cref, bool value)
{
  booleanStartValues[cref] = value;
  return oms_status_ok;
}

oms_status_enu_t oms::Values::deleteStartValue(const ComRef& cref)
{
  oms::ComRef signal(cref);
  if (signal.hasSuffixStart())
    signal = cref.popSuffix();

  // reals
  auto realValue = realStartValues.find(signal);
  if (realValue != realStartValues.end())
  {
    realStartValues.erase(realValue);
    return oms_status_ok;
  }

  // integers
  auto integerValue = integerStartValues.find(signal);
  if (integerValue != integerStartValues.end())
  {
    integerStartValues.erase(integerValue);
    return oms_status_ok;
  }

  // booleans
  auto boolValue = booleanStartValues.find(signal);
  if (boolValue != booleanStartValues.end())
  {
    booleanStartValues.erase(boolValue);
    return oms_status_ok;
  }

  return oms_status_error;
}

oms_status_enu_t oms::Values::exportToSSD(pugi::xml_node& node) const
{
  // skip this if there is nothing to export
  if (realStartValues.empty() && integerStartValues.empty() && booleanStartValues.empty())
    return oms_status_ok;

  // Top level Parameter nodes
  pugi::xml_node node_parameters_bindings = node.append_child(oms::ssp::Version1_0::ssd::parameter_bindings);
  pugi::xml_node node_parameter_binding  = node_parameters_bindings.append_child(oms::ssp::Version1_0::ssd::parameter_binding);
  pugi::xml_node node_parameter_values  = node_parameter_binding.append_child(oms::ssp::Version1_0::ssd::parameter_values);

  pugi::xml_node node_parameterset = node_parameter_values.append_child(oms::ssp::Version1_0::ssv::parameter_set);
  node_parameterset.append_attribute("version") = "1.0";
  node_parameterset.append_attribute("name") = "parameters";
  pugi::xml_node node_parameters = node_parameterset.append_child(oms::ssp::Version1_0::ssv::parameters);

  exportStartValuesHelper(node_parameters);

  return oms_status_ok;
}

oms_status_enu_t oms::Values::importFromSSD(const pugi::xml_node& node, const std::string& sspVersion, const std::string& tempdir)
{
  for (pugi::xml_node parameterBindingNode = node.child(oms::ssp::Version1_0::ssd::parameter_binding); parameterBindingNode; parameterBindingNode = parameterBindingNode.next_sibling(oms::ssp::Version1_0::ssd::parameter_binding))
  {
    std::string ssvFile = parameterBindingNode.attribute("source").as_string() ;
    if (!ssvFile.empty())
    {
      filesystem::path temp_root(tempdir);
      pugi::xml_document ssvdoc;
      pugi::xml_parse_result result = ssvdoc.load_file((temp_root / ssvFile).string().c_str());
      if (!result)
        return logError("loading \"" + std::string(ssvFile) + "\" failed (" + std::string(result.description()) + ")");

      pugi::xml_node parameterSet = ssvdoc.document_element(); // ssd:SystemStructureDescription
      pugi::xml_node parameters = parameterSet.child(oms::ssp::Version1_0::ssv::parameters);
      importStartValuesHelper(parameters);
    }
    else
    {
      // inline ParameterBindings
      if (parameterBindingNode.child(oms::ssp::Version1_0::ssv::parameter_set))
      {
        logWarning_deprecated;
      }
      pugi::xml_node parameterValues = parameterBindingNode.child(oms::ssp::Version1_0::ssd::parameter_values);
      pugi::xml_node parameterSet = parameterValues.child(oms::ssp::Version1_0::ssv::parameter_set);
      std::string paramsetVersion = parameterSet.attribute("version").as_string();
      pugi::xml_node parameters = parameterSet.child(oms::ssp::Version1_0::ssv::parameters);
      importStartValuesHelper(parameters);
    }
  }

  return oms_status_ok;
}

oms_status_enu_t oms::Values::exportToSSV(pugi::xml_node& node) const
{
  // skip this if there is nothing to export
  if (realStartValues.empty() && integerStartValues.empty() && booleanStartValues.empty())
    return oms_status_ok;

  exportStartValuesHelper(node);

  return oms_status_ok;
}


oms_status_enu_t oms::Values::exportStartValuesHelper(pugi::xml_node& node) const
{
  // realStartValues
  for (const auto& r : realStartValues)
  {
    //std::cout << "\n Start Values : " << std::string(r.first) << " = " << r.second ;
    pugi::xml_node node_parameter = node.append_child(oms::ssp::Version1_0::ssv::parameter);
    node_parameter.append_attribute("name") = r.first.c_str();
    pugi::xml_node node_parameter_type = node_parameter.append_child(oms::ssp::Version1_0::ssv::real_type);
    node_parameter_type.append_attribute("value") = r.second;
  }

  // integerStartValues
  for (const auto& i : integerStartValues)
  {
    pugi::xml_node node_parameter = node.append_child(oms::ssp::Version1_0::ssv::parameter);
    node_parameter.append_attribute("name") = i.first.c_str();
    pugi::xml_node node_parameter_type = node_parameter.append_child(oms::ssp::Version1_0::ssv::integer_type);
    node_parameter_type.append_attribute("value") = i.second;
  }

  // boolStartValues
  for (const auto& b : booleanStartValues)
  {
    pugi::xml_node node_parameter = node.append_child(oms::ssp::Version1_0::ssv::parameter);
    node_parameter.append_attribute("name") = b.first.c_str();
    pugi::xml_node node_parameter_type = node_parameter.append_child(oms::ssp::Version1_0::ssv::boolean_type);
    node_parameter_type.append_attribute("value") = b.second;
  }

  return oms_status_ok;
}

/*
 * exports the start values read from modeldescription.xml to ssv template
 */
oms_status_enu_t oms::Values::exportToSSVTemplate(pugi::xml_node& node, const ComRef& cref)
{
  // skip this if there is nothing to export
  if (modelDescriptionRealStartValues.empty() && modelDescriptionIntegerStartValues.empty() && modelDescriptionBooleanStartValues.empty())
    return oms_status_ok;

  // realStartValues
  for (const auto& r : modelDescriptionRealStartValues)
  {
    //std::cout << "\n Start Values : " << std::string(r.first) << " = " << r.second ;
    pugi::xml_node node_parameter = node.append_child(oms::ssp::Version1_0::ssv::parameter);
    node_parameter.append_attribute("name") = std::string(cref + r.first).c_str();
    pugi::xml_node node_parameter_type = node_parameter.append_child(oms::ssp::Version1_0::ssv::real_type);
    node_parameter_type.append_attribute("value") = r.second;
  }

  // integerStartValues
  for (const auto& i : modelDescriptionIntegerStartValues)
  {
    pugi::xml_node node_parameter = node.append_child(oms::ssp::Version1_0::ssv::parameter);
    node_parameter.append_attribute("name") = std::string(cref + i.first).c_str();
    pugi::xml_node node_parameter_type = node_parameter.append_child(oms::ssp::Version1_0::ssv::integer_type);
    node_parameter_type.append_attribute("value") = i.second;
  }

  // boolStartValues
  for (const auto& b : modelDescriptionBooleanStartValues)
  {
    pugi::xml_node node_parameter = node.append_child(oms::ssp::Version1_0::ssv::parameter);
    node_parameter.append_attribute("name") = std::string(cref + b.first).c_str();
    pugi::xml_node node_parameter_type = node_parameter.append_child(oms::ssp::Version1_0::ssv::boolean_type);
    node_parameter_type.append_attribute("value") = b.second;
  }

  return oms_status_ok;
}

oms_status_enu_t oms::Values::importStartValuesHelper(pugi::xml_node& parameters)
{
  if (parameters)
  {
    for(pugi::xml_node_iterator itparameters = parameters.begin(); itparameters != parameters.end(); ++itparameters)
    {
      std::string name = itparameters->name();
      if (name == oms::ssp::Version1_0::ssv::parameter)
      {
        ComRef cref = ComRef(itparameters->attribute("name").as_string());
        if (itparameters->child(oms::ssp::Version1_0::ssv::real_type))
        {
          double value = itparameters->child(oms::ssp::Version1_0::ssv::real_type).attribute("value").as_double();
          setReal(cref, value);
        }
        else if(itparameters->child(oms::ssp::Version1_0::ssv::integer_type))
        {
          int value = itparameters->child(oms::ssp::Version1_0::ssv::integer_type).attribute("value").as_int();
          setInteger(cref, value);
        }
        else if(itparameters->child(oms::ssp::Version1_0::ssv::boolean_type))
        {
          bool value = itparameters->child(oms::ssp::Version1_0::ssv::boolean_type).attribute("value").as_bool();
          setBoolean(cref, value);
        }
        else
        {
          logError("Failed to import " + std::string(oms::ssp::Version1_0::ssv::parameter) + ":Unknown ParameterBinding-type");
        }
      }
    }
  }

  return oms_status_ok;
}

oms_status_enu_t oms::Values::parseModelDescription(const char *filename)
{
  pugi::xml_document doc;
  pugi::xml_parse_result result = doc.load_file(filename);
  pugi::xml_node node = doc.document_element(); // modelDescription.xml

  if (!result)
  {
    return logError("Failed to load modelDescription.xml");
  }

  //std::string fmiVersion = node.attribute("fmiVersion").as_string();

  for(pugi::xml_node_iterator it = node.begin(); it != node.end(); ++it)
  {
    std::string name = it->name();
    if(name == "ModelVariables")
    {
      pugi::xml_node scalarVariableNode = node.child("ModelVariables");
      if (!scalarVariableNode)
      {
        return logError("Error parsing modelDescription.xml");
      }
      for (pugi::xml_node scalarVariable = scalarVariableNode.child("ScalarVariable"); scalarVariable; scalarVariable = scalarVariable.next_sibling("ScalarVariable"))
      {
        //std::string startValue = "";
        if (scalarVariable.child("Real").attribute("start").as_string() != "")
        {
          //startValue = scalarVariable.child("Real").attribute("start").as_string();
          modelDescriptionRealStartValues[ComRef(scalarVariable.attribute("name").as_string())] = scalarVariable.child("Real").attribute("start").as_double();
        }
        if (scalarVariable.child("Integer").attribute("start").as_string() != "")
        {
          modelDescriptionIntegerStartValues[scalarVariable.attribute("name").as_string()] = scalarVariable.child("Integer").attribute("start").as_int();
        }
        if (scalarVariable.child("Boolean").attribute("start").as_string() != "")
        {
          modelDescriptionBooleanStartValues[scalarVariable.attribute("name").as_string()] = scalarVariable.child("Boolean").attribute("start").as_bool();
        }
      }
    }
  }
  return oms_status_ok;
}

