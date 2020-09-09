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
#include "Option.h"
#include "ssd/Tags.h"
#include <OMSFileSystem.h>

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
  //realValues[cref] = value;
  if (cref.hasSuffixStart())
  {
    realValues[cref.popSuffix()] = std::pair<double, oms::Option<double>>(0, value);
  }
  else
  {
    // check for start value exist and update the cref with value and old startValues eg (value, startValue)
    oms::Option<double> startValue;
    auto realValue = realValues.find(cref);
    if (realValue != realValues.end())
    {
      startValue = std::get<1>(realValue->second).getValue();
    }
    else
    {
      startValue = oms::Option<double>();
    }
    realValues[cref] = std::pair<double, oms::Option<double>>(value, startValue);
  }
  return oms_status_ok;
}

oms_status_enu_t oms::Parameters::setInteger(const ComRef& cref, int value)
{
  //integerValues[cref] = value;
  if (cref.hasSuffixStart())
  {
    integerValues[cref.popSuffix()] = std::pair<int, oms::Option<int>>(0, value);
  }
  else
  {
    // check for start value exist and update the cref with value and old startValues eg (value, startValue)
    oms::Option<int> startValue;
    auto integerValue = integerValues.find(cref);
    if (integerValue != integerValues.end())
    {
      startValue = std::get<1>(integerValue->second).getValue();
    }
    else
    {
      startValue = oms::Option<int>();
    }
    integerValues[cref] = std::pair<double, oms::Option<int>>(value, startValue);
  }
  return oms_status_ok;
}

oms_status_enu_t oms::Parameters::setBoolean(const ComRef& cref, bool value)
{
  //booleanValues[cref] = value;
  if (cref.hasSuffixStart())
  {
    booleanValues[cref.popSuffix()] = std::pair<bool, oms::Option<bool>>(false, value);
  }
  else
  {
    // check for start value exist and update the cref with value and old startValues eg (value, startValue)
    oms::Option<bool> startValue;
    auto booleanValue = booleanValues.find(cref);
    if (booleanValue != booleanValues.end())
    {
      startValue = std::get<1>(booleanValue->second).getValue();
    }
    else
    {
      startValue = oms::Option<bool>();
    }
    booleanValues[cref] = std::pair<bool, oms::Option<bool>>(value, startValue);
  }
  return oms_status_ok;
}

oms_status_enu_t oms::Parameters::deleteStartValue(const ComRef& cref)
{
  oms::ComRef signal(cref);
  if (signal.hasSuffixStart())
    signal = cref.popSuffix();

  // reals
  auto realValue = realValues.find(signal);
  if (realValue != realValues.end())
  {
    realValues.erase(realValue);
    return oms_status_ok;
  }

  // integers
  auto integerValue = integerValues.find(signal);
  if (integerValue != integerValues.end())
  {
    integerValues.erase(integerValue);
    return oms_status_ok;
  }

  // booleans
  auto boolValue = booleanValues.find(signal);
  if (boolValue != booleanValues.end())
  {
    booleanValues.erase(boolValue);
    return oms_status_ok;
  }

  return oms_status_error;
}

oms_status_enu_t oms::Parameters::exportToSSD(pugi::xml_node& node) const
{
  // skip this if there is nothing to export
  if (isStartValuesEmpty())
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

oms_status_enu_t oms::Parameters::importFromSSD(const pugi::xml_node& node, const std::string& sspVersion, const std::string& tempdir)
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

oms_status_enu_t oms::Parameters::exportToSSV(pugi::xml_node& node) const
{
  // skip this if there is nothing to export
  if (isStartValuesEmpty())
    return oms_status_ok;

  exportStartValuesHelper(node);

  return oms_status_ok;
}


oms_status_enu_t oms::Parameters::exportStartValuesHelper(pugi::xml_node& node) const
{

  // realValues
  for (const auto& r : realValues)
  {
    if (!std::get<1>(r.second).isNone())
    {
      //std::cout << "\n if real values mapping : " << std::string(r.first) << " = " << std::get<0>(r.second) << " = " <<  std::get<1>(r.second).getValue() << "= " << std::get<1>(r.second).isNone();
      pugi::xml_node node_parameter = node.append_child(oms::ssp::Version1_0::ssv::parameter);
      node_parameter.append_attribute("name") = r.first.c_str();
      pugi::xml_node node_parameter_type = node_parameter.append_child(oms::ssp::Version1_0::ssv::real_type);
      node_parameter_type.append_attribute("value") = std::get<1>(r.second).getValue();
    }
  }

  // integerValues
  for (const auto& i : integerValues)
  {
    if (!std::get<1>(i.second).isNone())
    {
      pugi::xml_node node_parameter = node.append_child(oms::ssp::Version1_0::ssv::parameter);
      node_parameter.append_attribute("name") = i.first.c_str();
      pugi::xml_node node_parameter_type = node_parameter.append_child(oms::ssp::Version1_0::ssv::integer_type);
      node_parameter_type.append_attribute("value") = std::get<1>(i.second).getValue();
    }
  }

  // boolStartValues
  for (const auto& b : booleanValues)
  {
    if (!std::get<1>(b.second).isNone())
    {
      pugi::xml_node node_parameter = node.append_child(oms::ssp::Version1_0::ssv::parameter);
      node_parameter.append_attribute("name") = b.first.c_str();
      pugi::xml_node node_parameter_type = node_parameter.append_child(oms::ssp::Version1_0::ssv::boolean_type);
      node_parameter_type.append_attribute("value") = std::get<1>(b.second).getValue();
    }
  }

  return oms_status_ok;
}

oms_status_enu_t oms::Parameters::importStartValuesHelper(pugi::xml_node& parameters)
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

bool oms::Parameters::isStartValuesEmpty() const
{
  bool realStart = true , integerStart = true , boolStart = true;

  for (const auto& r : realValues)
  {
    if (!std::get<1>(r.second).isNone())
    {
      realStart = false;
      break;
    }
  }

  for (const auto& r : integerValues)
  {
    if (!std::get<1>(r.second).isNone())
    {
      integerStart = false;
      break;
    }
  }

  for (const auto& r : booleanValues)
  {
    if (!std::get<1>(r.second).isNone())
    {
      boolStart = false;
      break;
    }
  }
  //std::cout << "\n isStartValuesEmpty : " << realStart << " " << integerStart << "  " << boolStart;

  if (realStart && integerStart && boolStart)
    return true;

  return false;
}
