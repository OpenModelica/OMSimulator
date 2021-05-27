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


#include "Values.h"

#include "Logging.h"
#include "ssd/Tags.h"
#include "Util.h"

#include <iostream>
#include <map>
#include <pugixml.hpp>
#include <string>


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

oms_status_enu_t oms::Values::getReal(const ComRef& cref, double& value)
{
  auto realValue = realStartValues.find(cref);
  if (realValue != realStartValues.end())
  {
    value = realValue->second;
    return oms_status_ok;
  }

  return oms_status_error;
}

oms_status_enu_t oms::Values::getInteger(const ComRef& cref, int& value)
{
  auto integerValue = integerStartValues.find(cref);
  if (integerValue != integerStartValues.end())
  {
    value = integerValue->second;
    return oms_status_ok;
  }

  return oms_status_error;
}

oms_status_enu_t oms::Values::getBoolean(const ComRef& cref, bool& value)
{
  auto boolValue = booleanStartValues.find(cref);
  if (boolValue != booleanStartValues.end())
  {
    value = boolValue->second;
    return oms_status_ok;
  }

  return oms_status_error;
}

oms_status_enu_t oms::Values::setRealResources(const ComRef& cref, double value, const ComRef& fullCref, bool externalInput, oms_modelState_enu_t modelState)
{
  bool resourceAvailable = false;
  for (auto &it : parameterResources)
  {
    for (auto &res : it.allresources)
    {
      //update the value in all resources, so that same cref in multiple ssv can be updated, this can result in duplication
      auto realValue = res.second.realStartValues.find(cref);
      if (realValue != res.second.realStartValues.end())
      {
        if (oms_modelState_simulation == modelState && externalInput)
        {
          res.second.realValues[cref] = value;
        }
        else
        {
          res.second.setReal(cref, value);
        }
        resourceAvailable = true;
        // return oms_status_ok; return here to avoid updating the same value in different ssv file
      }
    }
  }

  if (!resourceAvailable)
  {
    auto &it = parameterResources.front();
    for (auto &res : it.allresources)
    {
      // insert the new signal at the first resource available
      res.second.setReal(cref, value);
      break;
    }
  }
  return oms_status_ok;
}

oms_status_enu_t oms::Values::getRealResources(const ComRef& cref, double& value, bool externalInput, oms_modelState_enu_t modelState)
{
  for (auto &it: parameterResources)
  {
    for (auto &res: it.allresources)
    {
      if (externalInput && oms_modelState_simulation == modelState && res.second.realValues[cref] != 0.0)
      {
        value = res.second.realValues[cref];
        return oms_status_ok;
      }
      auto realValue = res.second.realStartValues.find(cref);
      if (realValue != res.second.realStartValues.end())
      {
        value = realValue->second;
        return oms_status_ok;
      }
    }
  }

  return oms_status_error;
}

oms_status_enu_t oms::Values::getRealFromModeldescription(const ComRef& cref, double& value)
{
  // search in modelDescription.xml
  auto realValue = modelDescriptionRealStartValues.find(cref);
  if (realValue != modelDescriptionRealStartValues.end())
  {
    value = realValue->second;
    return oms_status_ok;
  }

  return oms_status_error;
}

oms_status_enu_t oms::Values::deleteStartValue(const ComRef& cref)
{
  oms::ComRef signal(cref);
  signal.pop_suffix("start");

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

oms_status_enu_t oms::Values::deleteStartValueInResources(const ComRef& cref)
{
  oms::ComRef signal(cref);
  signal.pop_suffix("start");

  for (auto &it : parameterResources)
  {
    for (auto &res : it.allresources)
    {
      // reals
      auto realValue = res.second.realStartValues.find(signal);
      if (realValue != res.second.realStartValues.end())
      {
        res.second.realStartValues.erase(realValue);
        return oms_status_ok;
      }

      // integers
      auto integerValue = res.second.integerStartValues.find(signal);
      if (integerValue != res.second.integerStartValues.end())
      {
        res.second.integerStartValues.erase(integerValue);
        return oms_status_ok;
      }

      // booleans
      auto boolValue = res.second.booleanStartValues.find(signal);
      if (boolValue != res.second.booleanStartValues.end())
      {
        res.second.booleanStartValues.erase(boolValue);
        return oms_status_ok;
      }
    }
  }

  return oms_status_error;
}

oms_status_enu_t oms::Values::exportToSSD(pugi::xml_node& node) const
{
  // skip this if there is nothing to export
  if (this->empty())
    return oms_status_ok;

  // Top level Parameter nodes
  pugi::xml_node node_parameters_bindings = node.append_child(oms::ssp::Version1_0::ssd::parameter_bindings);
  pugi::xml_node node_parameter_binding = node_parameters_bindings.append_child(oms::ssp::Version1_0::ssd::parameter_binding);
  pugi::xml_node node_parameter_values = node_parameter_binding.append_child(oms::ssp::Version1_0::ssd::parameter_values);

  pugi::xml_node node_parameterset = node_parameter_values.append_child(oms::ssp::Version1_0::ssv::parameter_set);
  node_parameterset.append_attribute("version") = "1.0";
  node_parameterset.append_attribute("name") = "parameters";
  pugi::xml_node node_parameters = node_parameterset.append_child(oms::ssp::Version1_0::ssv::parameters);

  exportStartValuesHelper(node_parameters);
  exportParameterMappingInline(node_parameter_binding);

  return oms_status_ok;
}

oms_status_enu_t oms::Values::importFromSnapshot(const pugi::xml_node& node, const std::string& sspVersion, const Snapshot& snapshot)
{
  for (pugi::xml_node parameterBindingNode = node.child(oms::ssp::Version1_0::ssd::parameter_binding); parameterBindingNode; parameterBindingNode = parameterBindingNode.next_sibling(oms::ssp::Version1_0::ssd::parameter_binding))
  {
    Values resources; // create a new value object for each parameter binding node
    std::string ssvFile = parameterBindingNode.attribute("source").as_string();
    if (!ssvFile.empty()) // parameter binding provided with .ssv file
    {
      //resourceFiles.push_back(ssvFile);
      pugi::xml_node parameterSet = snapshot.getResourceNode(ssvFile);
      if (!parameterSet)
        return logError("loading <oms:file> \"" + ssvFile + "\" from <oms:snapshot> failed");

      pugi::xml_node parameters = parameterSet.child(oms::ssp::Version1_0::ssv::parameters);

      // check for parameterMapping (e.g) <ssd:ParameterMapping>
      pugi::xml_node ssd_parameterMapping = parameterBindingNode.child(oms::ssp::Version1_0::ssd::parameter_mapping);

      if (ssd_parameterMapping)
      {
        // parameterMapping provided
        std::string ssmFileSource = ssd_parameterMapping.attribute("source").as_string();
        if (!ssmFileSource.empty())
        {
          pugi::xml_node ssm_parameterMapping = snapshot.getResourceNode(ssmFileSource);
          if (!ssm_parameterMapping)
            return logError("loading <oms:file> \"" + ssmFileSource + "\" from <oms:snapshot> failed");

          resources.importParameterMapping(ssm_parameterMapping);
        }
      }
      resources.importStartValuesHelper(parameters);
      // add the mapped ssv parameter binding node
      allresources[ssvFile] = resources;
    }
    else // inline ParameterBindings
    {
      if (parameterBindingNode.child(oms::ssp::Version1_0::ssv::parameter_set))
        logWarning_deprecated;

      pugi::xml_node parameterValues = parameterBindingNode.child(oms::ssp::Version1_0::ssd::parameter_values);
      pugi::xml_node parameterSet = parameterValues.child(oms::ssp::Version1_0::ssv::parameter_set);
      pugi::xml_node parameters = parameterSet.child(oms::ssp::Version1_0::ssv::parameters);

      // check for parameterMapping (e.g) <ssd:ParameterMapping>
      pugi::xml_node ssd_parameterMapping = parameterBindingNode.child(oms::ssp::Version1_0::ssd::parameter_mapping);
      pugi::xml_node ssm_parameterMapping = ssd_parameterMapping.child(oms::ssp::Version1_0::ssm::parameter_mapping);

      if (ssm_parameterMapping)
        resources.importParameterMapping(ssm_parameterMapping);

      resources.importStartValuesHelper(parameters);
      // add the mapped inline parameter binding node
      allresources["inline"] = resources;
    }
  }

  return oms_status_ok;
}

bool oms::Values::empty() const
{
  return realStartValues.empty() && integerStartValues.empty() && booleanStartValues.empty();
}

oms_status_enu_t oms::Values::exportToSSV(pugi::xml_node& node) const
{
  // skip this if there is nothing to export
  if (this->empty())
    return oms_status_ok;

  exportStartValuesHelper(node);
  return oms_status_ok;
}

oms_status_enu_t oms::Values::exportStartValuesHelper(pugi::xml_node& node) const
{
  // realStartValues
  std::vector<ComRef> realEntry; // list of exported cref entry
  for (const auto &r : realStartValues)
  {
    // check cref has entry associated with parameter mapping
    ComRef cref = getMappedCrefEntry(r.first);

    // export cref if entry does not exist, as cref associated with parameter mapping can have multiple entries with same value
    if (std::find(realEntry.begin(), realEntry.end(), cref) == realEntry.end())
    {
      pugi::xml_node node_parameter = node.append_child(oms::ssp::Version1_0::ssv::parameter);
      node_parameter.append_attribute("name") = cref.c_str();
      pugi::xml_node node_parameter_type = node_parameter.append_child(oms::ssp::Version1_0::ssv::real_type);
      node_parameter_type.append_attribute("value") = r.second;
      realEntry.push_back(cref);
    }
  }

  // integerStartValues
  std::vector<ComRef> integerEntry; // list of exported cref entry
  for (const auto &i : integerStartValues)
  {
    // check cref has entry associated with parameter mapping
    ComRef cref = getMappedCrefEntry(i.first);

    // export cref if entry does not exist, as cref associated with parameter mapping can have multiple entries with same value
    if (std::find(integerEntry.begin(), integerEntry.end(), cref) == integerEntry.end())
    {
      pugi::xml_node node_parameter = node.append_child(oms::ssp::Version1_0::ssv::parameter);
      node_parameter.append_attribute("name") = i.first.c_str();
      pugi::xml_node node_parameter_type = node_parameter.append_child(oms::ssp::Version1_0::ssv::integer_type);
      node_parameter_type.append_attribute("value") = i.second;
      integerEntry.push_back(cref);
    }
  }

  // boolStartValues
  std::vector<ComRef> booleanEntry; // list of exported cref entry
  for (const auto &b : booleanStartValues)
  {
    // check cref has entry associated with parameter mapping
    ComRef cref = getMappedCrefEntry(b.first);

    // export cref if entry does not exist, as cref associated with parameter mapping can have multiple entries with same value
    if (std::find(booleanEntry.begin(), booleanEntry.end(), cref) == booleanEntry.end())
    {
      pugi::xml_node node_parameter = node.append_child(oms::ssp::Version1_0::ssv::parameter);
      node_parameter.append_attribute("name") = b.first.c_str();
      pugi::xml_node node_parameter_type = node_parameter.append_child(oms::ssp::Version1_0::ssv::boolean_type);
      node_parameter_type.append_attribute("value") = b.second;
      booleanEntry.push_back(cref);
    }
  }

  return oms_status_ok;
}

void oms::Values::exportParameterBindings(pugi::xml_node &node, Snapshot &snapshot) const
{
  if (!parameterResources.empty())
  {
    for (const auto &it : parameterResources)
    {
      pugi::xml_node node_parameters_bindings = node.append_child(oms::ssp::Version1_0::ssd::parameter_bindings);
      for (const auto &res : it.allresources)
      {
        if (res.first == "inline")
        {
          // export as inline
          pugi::xml_node node_parameter_binding = node_parameters_bindings.append_child(oms::ssp::Version1_0::ssd::parameter_binding);
          pugi::xml_node node_parameter_values = node_parameter_binding.append_child(oms::ssp::Version1_0::ssd::parameter_values);
          pugi::xml_node node_parameterset = node_parameter_values.append_child(oms::ssp::Version1_0::ssv::parameter_set);
          node_parameterset.append_attribute("version") = "1.0";
          node_parameterset.append_attribute("name") = "parameters";
          pugi::xml_node node_parameters = node_parameterset.append_child(oms::ssp::Version1_0::ssv::parameters);
          res.second.exportStartValuesHelper(node_parameters);
          res.second.exportParameterMappingInline(node_parameter_binding);
        }
        else
        {
          // export to ssv file
          pugi::xml_node node_parameter_binding = node_parameters_bindings.append_child(oms::ssp::Version1_0::ssd::parameter_binding);
          node_parameter_binding.append_attribute("source") = res.first.c_str();
          pugi::xml_node ssvNode = snapshot.getTemplateResourceNodeSSV(res.first, "parameters");
          res.second.exportToSSV(ssvNode);
        }
      }
    }
  }
  else
  {
    // inline parameters
    exportToSSD(node);
  }
}

/*
 * returns mapped cref if entry found associated with parameter mapping,
 * otherwise return the default cref
 */
oms::ComRef oms::Values::getMappedCrefEntry(const ComRef& cref) const
{
  for (const auto &it : mappedEntry)
    if (it.second == cref)
      return it.first;

  return cref;
}

// export parameter mapping inline associated with parameterbinding
// <ssd:parameterBinding>
//   <ssd:parameterMapping>
//     <ssm:parameterMapping> </ssm:parameterMapping>
//   </ssd:paramterMapping>
// </ssd:parameterBinding>
void oms::Values::exportParameterMappingInline(pugi::xml_node& node) const
{
  if (mappedEntry.empty())
    return;

  pugi::xml_node ssd_parameter_mapping = node.append_child(oms::ssp::Version1_0::ssd::parameter_mapping);
  pugi::xml_node ssm_parameter_mapping = ssd_parameter_mapping.append_child(oms::ssp::Version1_0::ssm::parameter_mapping);

  for (const auto& it : mappedEntry)
  {
    pugi::xml_node ssm_mapping_entry = ssm_parameter_mapping.append_child(oms::ssp::Version1_0::ssm::parameter_mapping_entry);
    ssm_mapping_entry.append_attribute("source") = it.first.c_str();
    ssm_mapping_entry.append_attribute("target") = it.second.c_str();
  }
}

/*
 * exports the start values read from modeldescription.xml to ssv template
 */
void oms::Values::exportToSSVTemplate(pugi::xml_node& node, const ComRef& cref)
{
  // skip this if there is nothing to export
  if (modelDescriptionRealStartValues.empty() && modelDescriptionIntegerStartValues.empty() && modelDescriptionBooleanStartValues.empty())
    return;

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
}

/*
 * exports the start values read from modeldescription.xml to ssm template
 * with attributes source and target (eg.) <ssm:MappingEntry target="controller_2.bandwidth" source=""/>
 * the source attribute will be empty and needs to be manually updated by the user
 */
oms_status_enu_t oms::Values::exportToSSMTemplate(pugi::xml_node& node, const ComRef& cref)
{
  // skip this if there is nothing to export
  if (modelDescriptionRealStartValues.empty() && modelDescriptionIntegerStartValues.empty() && modelDescriptionBooleanStartValues.empty())
    return oms_status_ok;

  // realStartValues
  for (const auto& r : modelDescriptionRealStartValues)
  {
    pugi::xml_node node_parameterMappingEntry = node.append_child(oms::ssp::Version1_0::ssm::parameter_mapping_entry);
    node_parameterMappingEntry.append_attribute("source") = "";
    node_parameterMappingEntry.append_attribute("target") = std::string(cref + r.first).c_str();
  }

  // integerStartValues
  for (const auto& i : modelDescriptionIntegerStartValues)
  {
    pugi::xml_node node_parameterMappingEntry = node.append_child(oms::ssp::Version1_0::ssm::parameter_mapping_entry);
    node_parameterMappingEntry.append_attribute("source") = "";
    node_parameterMappingEntry.append_attribute("target") = std::string(cref + i.first).c_str();
  }

  // boolStartValues
  for (const auto& b : modelDescriptionBooleanStartValues)
  {
    pugi::xml_node node_parameterMappingEntry = node.append_child(oms::ssp::Version1_0::ssm::parameter_mapping_entry);
    node_parameterMappingEntry.append_attribute("source") = "";
    node_parameterMappingEntry.append_attribute("target") = std::string(cref + b.first).c_str();
  }

  return oms_status_ok;
}

oms_status_enu_t oms::Values::importStartValuesHelper(const pugi::xml_node& parameters)
{
  if (!parameters)
    return oms_status_ok;

  for(pugi::xml_node_iterator itparameters = parameters.begin(); itparameters != parameters.end(); ++itparameters)
  {
    std::string name = itparameters->name();
    std::vector<ComRef> mappedcrefs;
    if (name == oms::ssp::Version1_0::ssv::parameter)
    {
      ComRef cref = ComRef(itparameters->attribute("name").as_string());

      // check cref has any mapping entry
      if (!mappedEntry.empty())
      {
        auto mapfind = mappedEntry.equal_range(cref);
        for (auto it = mapfind.first; it != mapfind.second; ++it)
          mappedcrefs.push_back(it->second);
      }

      if (itparameters->child(oms::ssp::Version1_0::ssv::real_type))
      {
        double value = itparameters->child(oms::ssp::Version1_0::ssv::real_type).attribute("value").as_double();
        if (!mappedcrefs.empty())
        {
          for (const auto &mappedcref : mappedcrefs)
            setReal(mappedcref, value);
        }
        else
        {
          // no mapping entry found, apply the default cref found in ssv file
          setReal(cref, value);
        }
      }
      else if (itparameters->child(oms::ssp::Version1_0::ssv::integer_type))
      {
        int value = itparameters->child(oms::ssp::Version1_0::ssv::integer_type).attribute("value").as_int();
        if (!mappedcrefs.empty())
        {
          for (const auto &mappedcref : mappedcrefs)
            setInteger(mappedcref, value);
        }
        else
        {
          // no mapping entry found, apply the default cref found in ssv file
          setInteger(cref, value);
        }
      }
      else if(itparameters->child(oms::ssp::Version1_0::ssv::boolean_type))
      {
        bool value = itparameters->child(oms::ssp::Version1_0::ssv::boolean_type).attribute("value").as_bool();
        if (!mappedcrefs.empty())
        {
          for (const auto &mappedcref : mappedcrefs)
            setBoolean(mappedcref, value);
        }
        else
        {
          // no mapping entry found, apply the default cref found in ssv file
          setBoolean(cref, value);
        }
      }
      else
      {
        logError("Failed to import " + std::string(oms::ssp::Version1_0::ssv::parameter) + ":Unknown ParameterBinding-type");
      }
    }
  }

  return oms_status_ok;
}

oms_status_enu_t oms::Values::parseModelDescription(const filesystem::path& root)
{
  Snapshot snapshot;
  snapshot.importResourceFile("modelDescription.xml", root);
  const pugi::xml_node node = snapshot.getResourceNode("modelDescription.xml");

  if (!node)
    return logError("Failed to load modelDescription.xml");

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
        if (strlen(scalarVariable.child("Real").attribute("start").as_string()) != 0)
        {
          //startValue = scalarVariable.child("Real").attribute("start").as_string();
          modelDescriptionRealStartValues[ComRef(scalarVariable.attribute("name").as_string())] = scalarVariable.child("Real").attribute("start").as_double();
        }
        if (strlen(scalarVariable.child("Integer").attribute("start").as_string()) != 0)
        {
          modelDescriptionIntegerStartValues[scalarVariable.attribute("name").as_string()] = scalarVariable.child("Integer").attribute("start").as_int();
        }
        if (strlen(scalarVariable.child("Boolean").attribute("start").as_string()) != 0)
        {
          modelDescriptionBooleanStartValues[scalarVariable.attribute("name").as_string()] = scalarVariable.child("Boolean").attribute("start").as_bool();
        }
      }
    }
  }
  return oms_status_ok;
}

void oms::Values::importParameterMapping(const pugi::xml_node& parameterMapping)
{
  if (!parameterMapping)
    return;

  for (pugi::xml_node_iterator it = parameterMapping.begin(); it != parameterMapping.end(); ++it)
  {
    std::string name = it->name();
    if (oms::ssp::Version1_0::ssm::parameter_mapping_entry != name)
      continue;

    ComRef source = it->attribute("source").as_string();
    if (!source.isEmpty())
      mappedEntry.insert(std::make_pair(source, it->attribute("target").as_string()));
  }
}

oms_status_enu_t oms::Values::rename(const oms::ComRef& oldCref, const oms::ComRef& newCref)
{
  for (const auto &r : realStartValues)
  {
    ComRef tail(r.first);
    ComRef front = tail.pop_front();
    if (oldCref == front)
    {
      realStartValues[newCref + tail] = r.second; // update the newCref
      realStartValues.erase(r.first); // delete the old cref
    }
  }

  for (const auto &i : integerStartValues)
  {
    ComRef tail(i.first);
    ComRef front = tail.pop_front();
    if (oldCref == front)
    {
      integerStartValues[newCref + tail] = i.second; // update the newCref
      integerStartValues.erase(i.first);             // delete the old cref
    }
  }

  for (const auto &b : booleanStartValues)
  {
    ComRef tail(b.first);
    ComRef front = tail.pop_front();
    if (oldCref == front)
    {
      booleanStartValues[newCref + tail] = b.second; // update the newCref
      booleanStartValues.erase(b.first);             // delete the old cref
    }
  }

  return oms_status_ok;
}

oms_status_enu_t oms::Values::renameInResources(const oms::ComRef& oldCref, const oms::ComRef& newCref)
{
  for (auto &it : parameterResources)
  {
    for (auto &res : it.allresources)
    {
      for (const auto &r : res.second.realStartValues)
      {
        ComRef tail(r.first);
        ComRef front = tail.pop_front();
        if (oldCref == front)
        {
          res.second.realStartValues[newCref + tail] = r.second; // update the newCref
          res.second.realStartValues.erase(r.first);             // delete the old cref
        }
      }

      for (const auto &i : res.second.integerStartValues)
      {
        ComRef tail(i.first);
        ComRef front = tail.pop_front();
        if (oldCref == front)
        {
          res.second.integerStartValues[newCref + tail] = i.second; // update the newCref
          res.second.integerStartValues.erase(i.first);             // delete the old cref
        }
      }

      for (const auto &b : booleanStartValues)
      {
        ComRef tail(b.first);
        ComRef front = tail.pop_front();
        if (oldCref == front)
        {
          res.second.booleanStartValues[newCref + tail] = b.second; // update the newCref
          res.second.booleanStartValues.erase(b.first);             // delete the old cref
        }
      }
    }
  }
  return oms_status_ok;
}

bool oms::Values::hasResources()
{
  if (!parameterResources.empty())
    return true;

  return false;
}
