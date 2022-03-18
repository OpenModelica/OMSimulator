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

  // update start values in ssv template
  auto realValue = modelDescriptionRealStartValues.find(cref);
  if (realValue != modelDescriptionRealStartValues.end())
    modelDescriptionRealStartValues[cref] = value;

  setUnitDefinitions(cref);

  return oms_status_ok;
}

/*
function which sets the unitdefinitions <ssv:Units> or <ssd:Units> which will be exported in ssp
*/
void oms::Values::setUnitDefinitions(const ComRef& cref)
{
  // search in user provided map set
  std::string unitname = "";
  auto unitValue = variableUnits.find(cref);
  if (unitValue != variableUnits.end())
    unitname = unitValue->second;
  else
  {
    auto unitValue = modelDescriptionVariableUnits.find(cref);
    if (unitValue != modelDescriptionVariableUnits.end())
      unitname = unitValue->second;
  }
  if (!unitname.empty())
  {
    unitDefinitionsToExport unitToExport = {cref, unitname, modeldescriptionUnitDefinitions[unitname], true};
    unitDefinitionsToExportInSSP.push_back(unitToExport);
  }
}

void oms::Values::getFilteredUnitDefinitionsToSSD(std::map<std::string, std::map<std::string, std::string>>& unitDefinitions)
{
  if (unitDefinitionsToExportInSSP.empty())
    return;

  for(const auto &it: unitDefinitionsToExportInSSP)
  {
    auto unitvalue = unitDefinitions.find(it.unitValue);
    if (unitvalue == unitDefinitions.end())
      unitDefinitions[it.unitValue] = it.baseUnit;
  }
}

oms_status_enu_t oms::Values::setInteger(const ComRef& cref, int value)
{
  integerStartValues[cref] = value;

  // update start values in ssv template
  auto integerValue = modelDescriptionIntegerStartValues.find(cref);
  if (integerValue != modelDescriptionIntegerStartValues.end())
    modelDescriptionIntegerStartValues[cref] = value;

  return oms_status_ok;
}

oms_status_enu_t oms::Values::setBoolean(const ComRef& cref, bool value)
{
  booleanStartValues[cref] = value;

  // update start values in ssv template
  auto boolValue = modelDescriptionBooleanStartValues.find(cref);
  if (boolValue != modelDescriptionBooleanStartValues.end())
    modelDescriptionBooleanStartValues[cref] = value;

  return oms_status_ok;
}

oms_status_enu_t oms::Values::setString(const ComRef& cref, const std::string& value)
{
  stringStartValues[cref] = value;

  // update start values in ssv template
  auto stringValue = modelDescriptionStringStartValues.find(cref);
  if (stringValue != modelDescriptionStringStartValues.end())
    modelDescriptionStringStartValues[cref] = value;

  return oms_status_ok;
}

oms_status_enu_t oms::Values::setUnit(const ComRef& cref, const std::string& value)
{
  variableUnits[cref] = value;
  // update unit values in ssv template
  modelDescriptionVariableUnits[cref] = value; // override if exists otherwise make a new entry
  // update unit Definitions
  for (auto & it : unitDefinitionsToExportInSSP)
  {
    if (it.unitName == cref.c_str())
    {
      //std::cout << "\n override the unit:" << it.unitName << "=>" << cref.c_str() << "==>" << value;
      it.unitValue = value;
      it.baseUnit = {};
      break;
    }
  }

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

oms_status_enu_t oms::Values::getString(const ComRef& cref, std::string& value)
{
  auto stringValue = stringStartValues.find(cref);
  if (stringValue != stringStartValues.end())
  {
    value = stringValue->second;
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

oms_status_enu_t oms::Values::setStringResources(const ComRef& cref, const std::string& value, const ComRef& fullCref, bool externalInput, oms_modelState_enu_t modelState)
{
  bool resourceAvailable = false;
  for (auto &it : parameterResources)
  {
    for (auto &res : it.allresources)
    {
      //update the value in all resources, so that same cref in multiple ssv can be updated, this can result in duplication
      auto stringValue = res.second.stringStartValues.find(cref);
      if (stringValue != res.second.stringStartValues.end())
      {
        if (oms_modelState_simulation == modelState && externalInput)
          res.second.stringValues[cref] = value;
        else
          res.second.setString(cref, value);
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
      res.second.setString(cref, value);
      break;
    }
  }
  return oms_status_ok;
}

oms_status_enu_t oms::Values::setUnitResources(const ComRef& cref, const std::string& value, const ComRef& fullCref)
{
  bool resourceAvailable = false;
  for (auto &it : parameterResources)
  {
    for (auto &res : it.allresources)
    {
      //update the value in all resources, so that same cref in multiple ssv can be updated, this can result in duplication
      auto unitValue = res.second.variableUnits.find(cref);
      if (unitValue != res.second.variableUnits.end())
      {
        res.second.setUnit(cref, value);
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
      res.second.setUnit(cref, value);
      break;
    }
  }
  return oms_status_ok;
}

oms_status_enu_t oms::Values::setIntegerResources(const ComRef& cref, int value, const ComRef& fullCref, bool externalInput, oms_modelState_enu_t modelState)
{
  bool resourceAvailable = false;
  for (auto &it : parameterResources)
  {
    for (auto &res : it.allresources)
    {
      //update the value in all resources, so that same cref in multiple ssv can be updated, this can result in duplication
      auto integerValue = res.second.integerStartValues.find(cref);
      if (integerValue != res.second.integerStartValues.end())
      {
        if (oms_modelState_simulation == modelState && externalInput)
        {
          res.second.integerValues[cref] = value;
        }
        else
        {
          res.second.setInteger(cref, value);
        }
        resourceAvailable = true;
      }
    }
  }

  if (!resourceAvailable)
  {
    auto &it = parameterResources.front();
    for (auto &res : it.allresources)
    {
      // insert the new signal at the first resource available
      res.second.setInteger(cref, value);
      break;
    }
  }
  return oms_status_ok;
}

oms_status_enu_t oms::Values::setBooleanResources(const ComRef& cref, bool value, const ComRef& fullCref, bool externalInput, oms_modelState_enu_t modelState)
{
  bool resourceAvailable = false;
  for (auto &it : parameterResources)
  {
    for (auto &res : it.allresources)
    {
      //update the value in all resources, so that same cref in multiple ssv can be updated, this can result in duplication
      auto boolValue = res.second.booleanStartValues.find(cref);
      if (boolValue != res.second.booleanStartValues.end())
      {
        if (oms_modelState_simulation == modelState && externalInput)
        {
          res.second.booleanValues[cref] = value;
        }
        else
        {
          res.second.setBoolean(cref, value);
        }
        resourceAvailable = true;
      }
    }
  }

  if (!resourceAvailable)
  {
    auto &it = parameterResources.front();
    for (auto &res : it.allresources)
    {
      // insert the new signal at the first resource available
      res.second.setBoolean(cref, value);
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
        if (res.second.linkResources)
        {
          value = res.second.realValues[cref];
          return oms_status_ok;
        }
      }
      auto realValue = res.second.realStartValues.find(cref);
      if (realValue != res.second.realStartValues.end())
      {
        if (res.second.linkResources)
        {
          value = realValue->second;
          return oms_status_ok;
        }
      }
    }
  }

  return oms_status_error;
}

oms_status_enu_t oms::Values::getIntegerResources(const ComRef& cref, int& value, bool externalInput, oms_modelState_enu_t modelState)
{
  for (auto &it: parameterResources)
  {
    for (auto &res: it.allresources)
    {
      if (externalInput && oms_modelState_simulation == modelState && res.second.integerValues[cref] != 0)
      {
        if (res.second.linkResources)
        {
          value = res.second.integerValues[cref];
          return oms_status_ok;
        }
      }
      auto integerValue = res.second.integerStartValues.find(cref);
      if (integerValue != res.second.integerStartValues.end())
      {
        if (res.second.linkResources)
        {
          value = integerValue->second;
          return oms_status_ok;
        }
      }
    }
  }

  return oms_status_error;
}

oms_status_enu_t oms::Values::getBooleanResources(const ComRef& cref, bool& value, bool externalInput, oms_modelState_enu_t modelState)
{
  for (auto &it: parameterResources)
  {
    for (auto &res: it.allresources)
    {
      if (externalInput && oms_modelState_simulation == modelState && res.second.booleanValues[cref] != false)
      {
        if (res.second.linkResources)
        {
          value = res.second.booleanValues[cref];
          return oms_status_ok;
        }
      }
      auto booleanValue = res.second.booleanStartValues.find(cref);
      if (booleanValue != res.second.booleanStartValues.end())
      {
        if (res.second.linkResources)
        {
          value = booleanValue->second;
          return oms_status_ok;
        }
      }
    }
  }

  return oms_status_error;
}

oms_status_enu_t oms::Values::getStringResources(const ComRef& cref, std::string& value, bool externalInput, oms_modelState_enu_t modelState)
{
  for (auto &it: parameterResources)
  {
    for (auto &res: it.allresources)
    {
      if (externalInput && oms_modelState_simulation == modelState && res.second.stringValues[cref] != "")
      {
        if (res.second.linkResources)
        {
          value = res.second.stringValues[cref];
          return oms_status_ok;
        }
      }
      auto stringValue = res.second.stringStartValues.find(cref);
      if (stringValue != res.second.stringStartValues.end())
      {
        if (res.second.linkResources)
        {
          value = stringValue->second;
          return oms_status_ok;
        }
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

oms_status_enu_t oms::Values::getStringFromModeldescription(const ComRef& cref, std::string& value)
{
  // search in modelDescription.xml
  auto stringValue = modelDescriptionStringStartValues.find(cref);
  if (stringValue != modelDescriptionStringStartValues.end())
  {
    value = stringValue->second;
    return oms_status_ok;
  }

  return oms_status_error;
}

std::string oms::Values::getUnit(ComRef& cref) const
{
  // search in user provided map set
  auto unitValue = variableUnits.find(cref);
  if (unitValue != variableUnits.end())
    return unitValue->second;

  return "";
}

std::string oms::Values::getUnitFromModeldescription(ComRef& cref) const
{
  // search in modelDescription.xml
  auto unitValue = modelDescriptionVariableUnits.find(cref);
  if (unitValue != modelDescriptionVariableUnits.end())
    return unitValue->second;

  return "";
}

oms_status_enu_t oms::Values::getIntegerFromModeldescription(const ComRef& cref, int& value)
{
  // search in modelDescription.xml
  auto integerValue = modelDescriptionIntegerStartValues.find(cref);
  if (integerValue != modelDescriptionIntegerStartValues.end())
  {
    value = integerValue->second;
    return oms_status_ok;
  }

  return oms_status_error;
}

oms_status_enu_t oms::Values::getBooleanFromModeldescription(const ComRef& cref, bool& value)
{
  // search in modelDescription.xml
  auto boolValue = modelDescriptionBooleanStartValues.find(cref);
  if (boolValue != modelDescriptionBooleanStartValues.end())
  {
    value = boolValue->second;
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

oms_status_enu_t oms::Values::deleteReferencesInSSD(const std::string &filename)
{
  for (auto &it : parameterResources)
  {
    for (auto &res : it.allresources)
    {
      if (res.first.c_str() == "resources/"+ filename)
      {
        res.second.linkResources = false;  //unlink the references
        return oms_status_ok;
      }
    }
  }

  return oms_status_error;
}

oms_status_enu_t oms::Values::deleteResourcesInSSP(const std::string &filename)
{
  for (auto &it : parameterResources)
  {
    for (auto &res : it.allresources)
    {
      if (res.first.c_str() == "resources/"+ filename)
      {
        res.second.linkResources = false; //unlink the references
        it.allresources.erase("resources/"+ filename); // delete the resouces
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

oms_status_enu_t oms::Values::importFromSnapshot(const Snapshot &snapshot, const std::string& ssvFilename, const std::string& ssmFilename)
{
  // import new ssm references
  if (!ssmFilename.empty())
  {
    pugi::xml_node ssm_parameterMapping = snapshot.getResourceNode(ssmFilename);
    if (!ssm_parameterMapping)
      return logError("loading <oms:file> \"" + ssmFilename + "\" from <oms:snapshot> failed");

    importParameterMapping(ssm_parameterMapping);
  }

  // import new ssv references
  pugi::xml_node parameterSet = snapshot.getResourceNode(ssvFilename);
  if (!parameterSet)
    return logError("loading <oms:file> \"" + ssvFilename + "\" from <oms:snapshot> failed");

  pugi::xml_node parameters = parameterSet.child(oms::ssp::Version1_0::ssv::parameters);

  importStartValuesHelper(parameters);

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
          resources.ssmFile = ssmFileSource;
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

oms_status_enu_t oms::Values::exportUnitDefinitions(Snapshot &snapshot, std::string filename) const
{
  if (unitDefinitionsToExportInSSP.empty())
    return oms_status_ok;

  pugi::xml_node parameterSet = snapshot.getResourceNode(filename);
  if (!parameterSet)
    return logError("loading <oms:file> \"" + filename + "\" from <oms:snapshot> failed");

  pugi::xml_node node_units = parameterSet.append_child(oms::ssp::Version1_0::ssv::units);

  std::vector<std::string> unitList;
  for (const auto &it : unitDefinitionsToExportInSSP)
  {
    if (it.exportUnit &&  std::find(unitList.begin(), unitList.end(), it.unitValue) == unitList.end())
    {
      pugi::xml_node ssc_unit = node_units.append_child(oms::ssp::Version1_0::ssc::unit);
      ssc_unit.append_attribute("name") = it.unitValue.c_str();
      pugi::xml_node ssc_base_unit = ssc_unit.append_child(oms::ssp::Version1_0::ssc::base_unit);
      for (const auto &baseunit : it.baseUnit)
      {
        ssc_base_unit.append_attribute(baseunit.first.c_str()) = baseunit.second.c_str();
      }
      unitList.push_back(it.unitValue);
    }
  }
  return oms_status_ok;
}

oms_status_enu_t oms::Values::exportUnitDefinitionsToSSVTemplate(Snapshot &snapshot, std::string filename)
{
  if (modelDescriptionVariableUnits.empty())
    return oms_status_ok;

  pugi::xml_node parameterSet = snapshot.getResourceNode(filename);
  if (!parameterSet)
    return logError("loading <oms:file> \"" + filename + "\" from <oms:snapshot> failed");

  pugi::xml_node node_units;
  if (!parameterSet.child(oms::ssp::Version1_0::ssv::units))
    node_units = parameterSet.append_child(oms::ssp::Version1_0::ssv::units);
  else
    node_units = parameterSet.last_child();

  for (auto &it : modelDescriptionVariableUnits)
  {
    pugi::xml_node nodeExists = node_units.find_child_by_attribute(oms::ssp::Version1_0::ssc::unit, "name", it.second.c_str());
    // check if node already exist to avoid duplicates of units
    if (!nodeExists)
    {
      auto unitvalue = modeldescriptionUnitDefinitions.find(it.second);
      if (unitvalue != modeldescriptionUnitDefinitions.end())
      {
        pugi::xml_node ssc_unit = node_units.append_child(oms::ssp::Version1_0::ssc::unit);
        ssc_unit.append_attribute("name") = unitvalue->first.c_str();
        pugi::xml_node ssc_base_unit = ssc_unit.append_child(oms::ssp::Version1_0::ssc::base_unit);
        for (const auto &baseunit : unitvalue->second)
        {
          ssc_base_unit.append_attribute(baseunit.first.c_str()) = baseunit.second.c_str();
        }
      }
      else
      {
        // set user defined units
        pugi::xml_node ssc_unit = node_units.append_child(oms::ssp::Version1_0::ssc::unit);
        ssc_unit.append_attribute("name") = it.second.c_str();
        pugi::xml_node ssc_base_unit = ssc_unit.append_child(oms::ssp::Version1_0::ssc::base_unit);
      }
    }
  }

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

      // check for units set by user, priority over modeldescription.xml
      if (!getUnit(cref).empty())
        node_parameter_type.append_attribute("unit") = getUnit(cref).c_str();
      else if (!getUnitFromModeldescription(cref).empty()) // get unit from modelDescription.xml if available
        node_parameter_type.append_attribute("unit") = getUnitFromModeldescription(cref).c_str();

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
      if (!it.allresources.empty())
      {
        bool nodeSet = true;
        pugi::xml_node node_parameters_bindings;
        for (const auto &res : it.allresources)
        {
          if (nodeSet && res.second.linkResources)
          {
            node_parameters_bindings = node.append_child(oms::ssp::Version1_0::ssd::parameter_bindings);
            nodeSet = false;
          }
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
            pugi::xml_node node_parameter_binding;
            if (res.second.linkResources)
            {
              node_parameter_binding = node_parameters_bindings.append_child(oms::ssp::Version1_0::ssd::parameter_binding);
              node_parameter_binding.append_attribute("source") = res.first.c_str();
            }
            //std::cout << "\n export To SSV file :" << res.first.c_str() << "=" << res.second.ssmFile;
            pugi::xml_node ssvNode = snapshot.getTemplateResourceNodeSSV(res.first, "parameters");
            res.second.exportToSSV(ssvNode);
            res.second.exportUnitDefinitions(snapshot, res.first);

            // export SSM file if exist
            if (!res.second.ssmFile.empty())
            {
              if (res.second.linkResources)
              {
                pugi::xml_node ssd_parameter_mapping = node_parameter_binding.append_child(oms::ssp::Version1_0::ssd::parameter_mapping);
                ssd_parameter_mapping.append_attribute("source") = res.second.ssmFile.c_str();
              }
              pugi::xml_node ssmNode = snapshot.getTemplateResourceNodeSSM(res.second.ssmFile);
              res.second.exportParameterMappingToSSM(ssmNode);
            }
          }
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

void oms::Values::exportParameterMappingToSSM(pugi::xml_node& node) const
{
  if (mappedEntry.empty())
    return;

  for (const auto& it : mappedEntry)
  {
    pugi::xml_node ssm_mapping_entry = node.append_child(oms::ssp::Version1_0::ssm::parameter_mapping_entry);
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
    // check for units available and export to ssv template
    auto unitValue = modelDescriptionVariableUnits.find(r.first);
    if (unitValue != modelDescriptionVariableUnits.end())
      node_parameter_type.append_attribute("unit") = unitValue->second.c_str();
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
    if (name == "UnitDefinitions")
    {
      //std::cout << "\nParse Unit Definitions";
      pugi::xml_node units = node.child("UnitDefinitions");
      if (units)
      {

        for (pugi::xml_node unit = units.child("Unit"); unit; unit = unit.next_sibling("Unit"))
        {
          std::string unitName = unit.attribute("name").as_string();
          pugi::xml_node baseUnitNode = unit.child("BaseUnit");
          std::map<std::string, std::string> baseUnits;
          for (pugi::xml_attribute attr = baseUnitNode.first_attribute(); attr; attr = attr.next_attribute())
          {
            baseUnits[attr.name()] = attr.value();
          }
          modeldescriptionUnitDefinitions[unitName] = baseUnits;
          //unitToExport_1 unit_to_export = {"", unitName, baseUnits, false};
          //exportunitdefinitionToSSp_1.push_back(unit_to_export);
        }
      }
    }
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
          if (strlen(scalarVariable.child("Real").attribute("unit").as_string()) != 0)
            modelDescriptionVariableUnits[ComRef(scalarVariable.attribute("name").as_string())] = scalarVariable.child("Real").attribute("unit").as_string();
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
    if (name == "ModelStructure")
    {
      //ModelStructure-Outputs
      pugi::xml_node modelStructureOutputNode = it->child("Outputs");
      if (modelStructureOutputNode)
      {
        for (pugi::xml_node output = modelStructureOutputNode.child("Unknown"); output; output = output.next_sibling("Unknown"))
        {
          int index = output.attribute("index").as_int();
          std::string dependencies = output.attribute("dependencies").as_string();
          std::vector<int> dependenciesList;
          parseModelStructureDependencies(dependencies, dependenciesList);
          modelStructureOutputs[index] = dependenciesList;
        }
      }
      // ModelStructure-derivatives
      pugi::xml_node modelStructureDerivativeNode = it->child("Derivatives");
      if (modelStructureDerivativeNode)
      {
        for (pugi::xml_node derivative = modelStructureDerivativeNode.child("Unknown"); derivative; derivative = derivative.next_sibling("Unknown"))
        {
          int index = derivative.attribute("index").as_int();
          std::string dependencies = derivative.attribute("dependencies").as_string();
          std::vector<int> dependenciesList;
          parseModelStructureDependencies(dependencies, dependenciesList);
          modelStructureDerivatives[index] = dependenciesList;
        }
      }
      // ModelStructure-initialUnknowns
      pugi::xml_node modelStructureInitialUnknownsNode = it->child("InitialUnknowns");
      if (modelStructureInitialUnknownsNode)
      {
        for (pugi::xml_node initialUnknowns = modelStructureInitialUnknownsNode.child("Unknown"); initialUnknowns; initialUnknowns = initialUnknowns.next_sibling("Unknown"))
        {
          int index = initialUnknowns.attribute("index").as_int();
          std::string dependencies = initialUnknowns.attribute("dependencies").as_string();
          std::vector<int> dependenciesList;
          parseModelStructureDependencies(dependencies, dependenciesList);
          modelStructureInitialUnknowns[index] = dependenciesList;
        }
      }
    }
  }
  return oms_status_ok;
}

void oms::Values::parseModelStructureDependencies(std::string &dependencies, std::vector<int> &dependencyList)
{
  std::stringstream ss(dependencies);
  std::string temp;
  while (std::getline(ss, temp, ' '))
  {
    if (!temp.empty())
      dependencyList.push_back(std::stoi(temp));
  }
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
  for (auto it=realStartValues.cbegin(); it != realStartValues.cend() /* not hoisted */; /* no increment */)
  {
    // renaming the keys
    ComRef tail(it->first);
    ComRef front = tail.pop_front();
    if (oldCref == front)
    {
      //logInfo("old:   " + std::string(it->first));
      //logInfo("new:   " + std::string(newCref + tail));
      //logInfo("value: " + std::to_string(it->second));
      realStartValues[newCref + tail] = it->second;  // update the newCref
      it = realStartValues.erase(it);                // delete the old cref
    }
    else
    {
      ++it;
    }
  }

  for (auto it=integerStartValues.cbegin(); it != integerStartValues.cend() /* not hoisted */; /* no increment */)
  {
    // renaming the keys
    ComRef tail(it->first);
    ComRef front = tail.pop_front();
    if (oldCref == front)
    {
      //logInfo("old:   " + std::string(it->first));
      //logInfo("new:   " + std::string(newCref + tail));
      //logInfo("value: " + std::to_string(it->second));
      integerStartValues[newCref + tail] = it->second;  // update the newCref
      it = integerStartValues.erase(it);                // delete the old cref
    }
    else
    {
      ++it;
    }
  }

  for (auto it=booleanStartValues.cbegin(); it != booleanStartValues.cend() /* not hoisted */; /* no increment */)
  {
    // renaming the keys
    ComRef tail(it->first);
    ComRef front = tail.pop_front();
    if (oldCref == front)
    {
      //logInfo("old:   " + std::string(it->first));
      //logInfo("new:   " + std::string(newCref + tail));
      //logInfo("value: " + std::to_string(it->second));
      booleanStartValues[newCref + tail] = it->second;  // update the newCref
      it = booleanStartValues.erase(it);                // delete the old cref
    }
    else
    {
      ++it;
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
      for (auto it=res.second.realStartValues.cbegin(); it != res.second.realStartValues.cend() /* not hoisted */; /* no increment */)
      {
        // renaming the keys
        ComRef tail(it->first);
        ComRef front = tail.pop_front();
        if (oldCref == front)
        {
          //logInfo("old:   " + std::string(it->first));
          //logInfo("new:   " + std::string(newCref + tail));
          //logInfo("value: " + std::to_string(it->second));
          res.second.realStartValues[newCref + tail] = it->second;  // update the newCref
          it = res.second.realStartValues.erase(it);                // delete the old cref
        }
        else
        {
          ++it;
        }
      }

      for (auto it=res.second.integerStartValues.cbegin(); it != res.second.integerStartValues.cend() /* not hoisted */; /* no increment */)
      {
        // renaming the keys
        ComRef tail(it->first);
        ComRef front = tail.pop_front();
        if (oldCref == front)
        {
          //logInfo("old:   " + std::string(it->first));
          //logInfo("new:   " + std::string(newCref + tail));
          //logInfo("value: " + std::to_string(it->second));
          res.second.integerStartValues[newCref + tail] = it->second;  // update the newCref
          it = res.second.integerStartValues.erase(it);                // delete the old cref
        }
        else
        {
          ++it;
        }
      }

      for (auto it=res.second.booleanStartValues.cbegin(); it != res.second.booleanStartValues.cend() /* not hoisted */; /* no increment */)
      {
        // renaming the keys
        ComRef tail(it->first);
        ComRef front = tail.pop_front();
        if (oldCref == front)
        {
          //logInfo("old:   " + std::string(it->first));
          //logInfo("new:   " + std::string(newCref + tail));
          //logInfo("value: " + std::to_string(it->second));
          res.second.booleanStartValues[newCref + tail] = it->second; // update the newCref
          it = res.second.booleanStartValues.erase(it);               // delete the old cref
        }
        else
        {
          ++it;
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
