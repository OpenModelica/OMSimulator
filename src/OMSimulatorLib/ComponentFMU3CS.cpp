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

#include "ComponentFMU3CS.h"

#include "Flags.h"
#include "Logging.h"
#include "Model.h"
#include "OMSFileSystem.h"
#include "ssd/Tags.h"
#include "System.h"
#include "SystemWC.h"
#include "Scope.h"

#include <fmi4c.h>
#include <regex>
#include <unordered_set>
#include <cmath>
#include <iostream>


oms::ComponentFMU3CS::ComponentFMU3CS(const ComRef& cref, System* parentSystem, const std::string& fmuPath)
  : oms::Component(cref, oms_component_fmu3, parentSystem, fmuPath), fmuInfo(fmuPath)
{
}

oms::ComponentFMU3CS::~ComponentFMU3CS()
{
  if (oms_modelState_virgin != getModel().getModelState())
    fmi3_freeInstance(fmu);

  fmi4c_freeFmu(fmu);
}

oms::Component* oms::ComponentFMU3CS::NewComponent(const oms::ComRef& cref, oms::System* parentSystem, const std::string& fmuPath, std::string replaceComponent)
{
  if (!cref.isValidIdent())
  {
    logError_InvalidIdent(cref);
    return NULL;
  }

  if (!parentSystem)
  {
    logError_InternalError;
    return NULL;
  }
  // replaceComponent string will be used to avoid name conflicts when replacing a fmu with oms_replaceSubModel(), the default is ""

  filesystem::path temp_root(parentSystem->getModel().getTempDirectory());
  filesystem::path temp_temp = temp_root / "temp";
  filesystem::path relFMUPath = parentSystem->copyResources() ? (filesystem::path("resources") / (parentSystem->getUniqueID() + "_" + replaceComponent + std::string(cref) + ".fmu")) : filesystem::path(fmuPath);
  filesystem::path absFMUPath = temp_root / relFMUPath;

  ComponentFMU3CS* component = new ComponentFMU3CS(cref, parentSystem, relFMUPath.generic_string());

  /* parse the modeldescription.xml at top level to get the GUID to check whether instance already exist
   * so we don't need to unpack the fmu, and also parse start values before instantiating fmu's
  */
  std::string guid_ = "";
  filesystem::path modelDescriptionPath;
  /*
  * check for modeldescription path from file system or temp directory
  * because when importingSnapshot the path will be resources/0001_tank1.fmu
  */
  if (parentSystem->copyResources())
    modelDescriptionPath = fmuPath;
  else
    modelDescriptionPath = parentSystem->getModel().getTempDirectory() / filesystem::path(fmuPath);

  component->values.parseModelDescriptionFmi3(modelDescriptionPath, guid_);

  /*
   * check if instance of an fmu already exist by using guid of the fmu
   * if instance exist use the existing instance path
   * eg: tank1 => resources /0001_tank1.fmu
   *     tank2 => resources /0001_tank1.fmu
  */
  auto it = parentSystem->fmuGuid.find(guid_);
  if (it == parentSystem->fmuGuid.end())
    parentSystem->fmuGuid[guid_] = relFMUPath;
  else
  {
    // instance exists and update the FMU path to already existing instance
    relFMUPath = parentSystem->fmuGuid[guid_];
    absFMUPath = temp_root / relFMUPath;
    component->setPath(relFMUPath.generic_string());
  }

  // Copy the resource to the temp directory of the model? We don't want have
  // to copy resources if importing an SSP file or snapshot.
  if (parentSystem->copyResources() && !filesystem::exists(absFMUPath))
    oms_copy_file(filesystem::path(fmuPath), absFMUPath);

  // set temp directory
  filesystem::path tempDir = temp_temp / relFMUPath.stem();
  component->setTempDir(tempDir.string());

  bool dirExist = true;
  if (!filesystem::is_directory(tempDir))
  {
    dirExist = false;
    if (!filesystem::create_directory(tempDir))
    {
      logError("Creating temp directory for component \"" + std::string(cref) + "\" failed");
      return NULL;
    }
  }

  // unpack the fmu in temp directory
  oms::Scope::miniunz(modelDescriptionPath.generic_string().c_str(), tempDir.generic_string().c_str());

  // load the unpacked fmu and parse modelDescription.xml
  component->fmu = fmi4c_loadUnzippedFmu(cref.c_str(), tempDir.generic_string().c_str());
  if (!component->fmu)
  {
    logError("Error parsing modelDescription.xml");
    delete component;
    return NULL;
  }

  fmiVersion_t version = fmi4c_getFmiVersion(component->fmu);
  if (fmiVersion3 != version)
  {
    logError("Unsupported FMI version: " + version);
    delete component;
    return NULL;
  }

  if (!fmi3_supportsCoSimulation(component->fmu))
  {
    logError("FMU \"" + std::string(cref) + "\" doesn't support co-simulation mode.");
    delete component;
    return NULL;
  }

  // update FMU info
  component->fmuInfo.update(oms_component_fmu3, component->fmu);
  component->omsfmi3logger = oms::fmi3logger;

  //std::cout << "\n Numberof variables: " << fmi3_getNumberOfVariables(component->fmu);

  // create a list of all variables using fmi4c variable structure
  component->allVariables.reserve(fmi3_getNumberOfVariables(component->fmu));
  component->exportVariables.reserve(fmi3_getNumberOfVariables(component->fmu));
  for (unsigned int i = 0; i < fmi3_getNumberOfVariables(component->fmu); ++i)
  {
    oms::Variable v(component->fmu, i, oms_component_fmu3);
    //logInfo("vars: " + std::string(v.getCref().c_str()));
    if (v.getIndex() != i)
    {
      logError("Index mismatch " + std::to_string(v.getIndex()) + " != " + std::to_string(i) + ".\nPlease report the problem to the dev team: https://github.com/OpenModelica/OMSimulator/issues/new?assignees=&labels=&template=bug_report.md");
      delete component;
      return NULL;
    }

    // extract continuous-time derivatives
    if (v.isContinuousTimeDer())
      component->derivatives.push_back(v.getIndex());

    component->allVariables.push_back(v);
    component->exportVariables.push_back(true);
  }

  // mark states and continuous-time states
  for (unsigned int i = 0; i < fmi3_getNumberOfVariables(component->fmu); ++i)
  {
    if (component->allVariables[i].isContinuousTimeDer())
      component->allVariables[component->allVariables[i].getStateIndex()-1].markAsContinuousTimeState(i);
    else if (component->allVariables[i].isDer())
      component->allVariables[component->allVariables[i].getStateIndex()-1].markAsState(i);
  }

  // create some special variable maps
  for (auto const& v : component->allVariables)
  {
    if (v.isInput())
      component->inputs.push_back(v.getIndex());
    else if (v.isOutput())
    {
      component->outputs.push_back(v.getIndex());
      component->outputsGraph.addNode(Connector(oms_causality_output, v.getType(), v.getCref(), component->getFullCref()));
    }
    else if (v.isParameter())
      component->parameters.push_back(v.getIndex());
    else if (v.isCalculatedParameter())
      component->calculatedParameters.push_back(v.getIndex());

    if (v.isInitialUnknown())
      component->initialUnknownsGraph.addNode(Connector(v.getCausality(), v.getType(), v.getCref(), component->getFullCref()));

    component->exportVariables.push_back(v.isInput() || v.isOutput());
  }

  // create connectors
  while (component->connectors.size() > 0 && NULL == component->connectors.back())
    component->connectors.pop_back();

  int j = 1;
  int size = 1 + component->inputs.size();
  for (const auto& i : component->inputs)
    component->connectors.push_back(new Connector(oms_causality_input, component->allVariables[i].getType(), component->allVariables[i].getCref(), component->getFullCref(), j++/(double)size));
  j = 1;
  size = 1 + component->outputs.size();
  for (const auto& i : component->outputs)
    component->connectors.push_back(new Connector(oms_causality_output, component->allVariables[i].getType(), component->allVariables[i].getCref(), component->getFullCref(), j++/(double)size));
  for (const auto& i : component->parameters)
    component->connectors.push_back(new Connector(oms_causality_parameter, component->allVariables[i].getType(), component->allVariables[i].getCref(), component->getFullCref()));
  for (const auto& i : component->calculatedParameters)
    component->connectors.push_back(new Connector(oms_causality_calculatedParameter, component->allVariables[i].getType(), component->allVariables[i].getCref(), component->getFullCref()));
  component->connectors.push_back(NULL);
  component->element.setConnectors(&component->connectors[0]);

  if (oms_status_ok != component->initializeDependencyGraph_initialUnknowns())
  {
    logError(std::string(cref) + ": Couldn't initialize dependency graph for initial unknowns.");
    delete component;
    return NULL;
  }
  if (oms_status_ok != component->initializeDependencyGraph_outputs())
  {
    logError(std::string(cref) + ": Couldn't initialize dependency graph for simulation unknowns.");
    delete component;
    return NULL;
  }

  // set units to connector
  for (auto &connector : component->connectors)
  {
    if (connector)
    {
      oms::ComRef connectorCref = connector->getName();
      std::string unitName = component->values.getUnitFromModeldescription(connectorCref);
      if (!unitName.empty())
        connector->connectorUnits[unitName] = component->values.modeldescriptionUnitDefinitions[unitName];

      // get enumerationTypes
      std::string enumType = component->values.getEnumerationTypeFromModeldescription(connectorCref);
      if (!enumType.empty())
        connector->enumerationName[connectorCref] = enumType;
    }
  }
  return component;
}

oms::Component* oms::ComponentFMU3CS::NewComponent(const pugi::xml_node& node, oms::System* parentSystem, const std::string& sspVersion, const Snapshot& snapshot, std::string variantName)
{
  ComRef cref = ComRef(node.attribute("name").as_string());
  std::string type = node.attribute("type").as_string();
  std::string source = node.attribute("source").as_string();

  if (type != "application/x-fmu-sharedlibrary" && !type.empty())
  {
    logError("Unexpected component type: " + type);
    return NULL;
  }

  oms::ComponentFMU3CS* component = dynamic_cast<oms::ComponentFMU3CS*>(oms::ComponentFMU3CS::NewComponent(cref, parentSystem, source));
  if (!component)
    return NULL;

  for (const auto& connector : component->connectors)
    if (connector)
      delete connector;
  component->connectors.clear();
  for(pugi::xml_node_iterator it = node.begin(); it != node.end(); ++it)
  {
    std::string name = it->name();
    if(name == oms::ssp::Draft20180219::ssd::connectors)
    {
      // get the ssdNode to parse UnitDefinitions in "SystemStructure.ssd"
      pugi::xml_node ssdNode = snapshot.getResourceNode(variantName);
      component->values.importUnitDefinitions(ssdNode);
      // import connectors
      for(pugi::xml_node_iterator itConnectors = (*it).begin(); itConnectors != (*it).end(); ++itConnectors)
      {
        component->connectors.push_back(oms::Connector::NewConnector(*itConnectors, sspVersion, component->getFullCref()));
        // set units to connector
        if ((*itConnectors).child(oms::ssp::Version1_0::ssc::real_type))
        {
          std::string unitName = (*itConnectors).child(oms::ssp::Version1_0::ssc::real_type).attribute("unit").as_string();
          if (!unitName.empty())
            component->connectors.back()->connectorUnits[unitName] = component->values.modeldescriptionUnitDefinitions[unitName];
        }
        // set enumeration definitions
        if ((*itConnectors).child(oms::ssp::Version1_0::ssc::enumeration_type))
        {
          std::string enumTypeName = (*itConnectors).child(oms::ssp::Version1_0::ssc::enumeration_type).attribute("name").as_string();
          if (!enumTypeName.empty())
            component->connectors.back()->enumerationName[component->connectors.back()->getName().c_str()] = enumTypeName;

          // give priority to enum definitions in ssd over modeldescription.xml, it is possible the user might have manually change values in ssd file
          component->values.importEnumerationDefinitions(ssdNode, enumTypeName);
        }
      }
    }
    else if(name == oms::ssp::Draft20180219::ssd::element_geometry)
    {
      oms::ssd::ElementGeometry geometry;
      geometry.importFromSSD(*it);
      component->setGeometry(geometry);
    }
    else if(name == oms::ssp::Version1_0::ssd::parameter_bindings)
    {
      // set parameter bindings associated with the component
      Values resources;
      std::string tempdir = parentSystem->getModel().getTempDirectory();
      resources.importFromSnapshot(*it, sspVersion, snapshot, variantName);
      component->values.parameterResources.push_back(resources);
    }
    else
    {
      logError_WrongSchema(name);
      delete component;
      return NULL;
    }
  }

  component->connectors.push_back(NULL);
  component->element.setConnectors(&component->connectors[0]);

  return component;
}

oms_status_enu_t oms::ComponentFMU3CS::exportToSSD(pugi::xml_node& node, Snapshot& snapshot, std::string variantName) const
{
  node.append_attribute("name") = this->getCref().c_str();
  node.append_attribute("type") = "application/x-fmu-sharedlibrary";
  node.append_attribute("source") = getPath().c_str();

  if (element.getGeometry())
    element.getGeometry()->exportToSSD(node);

  if (connectors.size() > 1)
  {
    pugi::xml_node node_connectors = node.append_child(oms::ssp::Draft20180219::ssd::connectors);
    for (const auto& connector : connectors)
      if (connector)
        if (oms_status_ok != connector->exportToSSD(node_connectors))
          return oms_status_error;
  }

  // export ParameterBindings at component level
  values.exportParameterBindings(node, snapshot, variantName);

  return oms_status_ok;
}

void oms::ComponentFMU3CS::getFilteredUnitDefinitionsToSSD(std::map<std::string, std::map<std::string, std::string>>& unitDefinitions)
{
  // get units from connectors
  for (const auto &connector : connectors)
  {
    if (connector)
    {
      if (!connector->connectorUnits.empty())
      {
        for (auto &con : connector->connectorUnits)
        {
          auto unitvalue = unitDefinitions.find(con.first);
          if (unitvalue == unitDefinitions.end())
            unitDefinitions[con.first] = con.second;
        }
      }
    }
  }

  return values.getFilteredUnitDefinitionsToSSD(unitDefinitions);
}

void oms::ComponentFMU3CS::getFilteredEnumerationDefinitionsToSSD(std::map<std::string, std::map<std::string, std::string>>& enumerationDefinitions)
{
  return values.getFilteredEnumerationDefinitionsToSSD(enumerationDefinitions);
}

oms_status_enu_t oms::ComponentFMU3CS::exportToSSV(pugi::xml_node& ssvNode)
{
  return values.exportToSSV(ssvNode);
}

oms_status_enu_t oms::ComponentFMU3CS::exportToSSVTemplate(pugi::xml_node& ssvNode, Snapshot& snapshot)
{
  values.exportToSSVTemplate(ssvNode, getCref());
  values.exportUnitDefinitionsToSSVTemplate(snapshot, "template.ssv");
  return oms_status_ok;
}

oms_status_enu_t oms::ComponentFMU3CS::exportToSSMTemplate(pugi::xml_node& ssmNode)
{
  values.exportToSSMTemplate(ssmNode, getCref());
  return oms_status_ok;
}

void oms::ComponentFMU3CS::dumpInitialUnknowns()
{
  std::string str = "";
  int n=0;
  for (auto &v : allVariables)
  {
    if (v.isInitialUnknown())
    {
      n++;
      if (!str.empty())
        str += ", ";
      str += std::to_string(v.getIndex()+1) + ": " + std::string(v.getCref());
    }
  }
  logInfo("[" + std::string(getCref()) + ": " + getPath() + "] The FMU contains " + std::to_string(n) + " initial unknowns: " + str);
}

oms_status_enu_t oms::ComponentFMU3CS::initializeDependencyGraph_initialUnknowns()
{
  if (initialUnknownsGraph.getEdges().connections.size() > 0)
    return logError(std::string(getCref()) + ": " + getPath() + " is already initialized");

  //dumpInitialUnknowns();

  // Check if initial unknowns from modelDescription.xml are the same as in initialUnknownsGraph
  size_t N_ModelStructure = values.modelStructureInitialUnknowns.size();
  size_t N = initialUnknownsGraph.getNodes().size();

  bool badInitialUnknowns = false;

  std::unordered_set <unsigned int> setA_ModelStructure;
  for (const auto &it : values.modelStructureInitialUnknowns)
    setA_ModelStructure.insert(it.first-1);

  std::string missing_unknowns = "";
  for (auto &v : allVariables)
  {
    if (v.isInitialUnknown() && setA_ModelStructure.find(v.getIndex()) == setA_ModelStructure.end())
    {
      badInitialUnknowns = true;
      missing_unknowns += "\n  * " + std::to_string(v.getIndex()+1) + ": " + std::string(v.getCref()) + " is missing";
    }
  }

  for (const auto &it : values.modelStructureInitialUnknowns)
  {
    const Variable &var_oms = allVariables[it.first - 1];
    fmi3VariableHandle *var = fmi3_getVariableByIndex(fmu, (it.first));
    oms::ComRef name_fmilib = fmi3_getVariableName(var);
    // std::cout << "\nDebug: " << var_oms.getCref().c_str() << "===>" << name_fmilib.c_str() << "===>" << var_oms.isInitialUnknown();
    if (var_oms.getCref() != name_fmilib)
    {
      missing_unknowns += "\n  * " + std::to_string(it.first) + ": " + std::string(var_oms.getCref()) + " could not be found";
      badInitialUnknowns = true;
    }
    else if (!var_oms.isInitialUnknown())
    {
      missing_unknowns += "\n  * " + std::to_string(it.first) + ": " + std::string(var_oms.getCref()) + " is wrongly listed";
      badInitialUnknowns = true;
    }
  }


  if (badInitialUnknowns && !Flags::IgnoreInitialUnknowns() && N_ModelStructure > 0)
    logWarning("[" + std::string(getCref()) + ": " + getPath() + "] The FMU lists " + std::to_string(N_ModelStructure) + " initial unknowns and exposes " + std::to_string(N) + " initial unknowns." + missing_unknowns);

  if (badInitialUnknowns)
  {
    if(!Flags::IgnoreInitialUnknowns() && N_ModelStructure > 0)
      logInfo("[" + std::string(getCref()) + ": " + getPath() + "] The FMU contains bad initial unknowns. This might cause problems, e.g. wrong simulation results.");
    else
    {
      if (N_ModelStructure > 0)
        logWarning("[" + std::string(getCref()) + ": " + getPath() + "] The dependencies of the initial unknowns defined in the FMU are ignored because the flag --ignoreInitialUnknowns is active. Instead, all the initial unknowns will depend on all inputs.");
      for (int i=0; i < N; i++)
      {
        logDebug(std::string(getCref()) + ": " + getPath() + " initial unknown " + std::string(initialUnknownsGraph.getNodes()[i]) + " depends on all inputs");
        for (const auto& j : inputs)
          initialUnknownsGraph.addEdge(allVariables[j].makeConnector(this->getFullCref()), initialUnknownsGraph.getNodes()[i]);
      }
      return oms_status_ok;
    }
  }

  // get the initial unknowns dependencies
  int i = 0;
  for (const auto &it : values.modelStructureInitialUnknowns)
  {
    //std::cout << "\nInitialUnknowns dependencies: " << it.first << "===>" << it.second.size() << "==>" << values.modelStructureInitialUnknownsDependencyExist[it.first];
    // no dependencies
    if (it.second.empty() && values.modelStructureInitialUnknownsDependencyExist[it.first])
      logDebug(std::string(getCref()) + ": " + getPath() + " initial unknown " + std::string(initialUnknownsGraph.getNodes()[i]) + " has no dependencies");

    // dependency attribute not provided in modeldescription.xml, all output depends on all inputs
    else if (it.second.empty() && !values.modelStructureInitialUnknownsDependencyExist[it.first])
    {
      logDebug(std::string(getCref()) + ": " + getPath() + " initial unknown " + std::string(initialUnknownsGraph.getNodes()[i]) + " depends on all inputs");
      for (const auto& j : inputs)
        initialUnknownsGraph.addEdge(allVariables[j].makeConnector(this->getFullCref()), initialUnknownsGraph.getNodes()[i]);
    }
    else
    {
      //dependency exist
      for (const auto &index : it.second)
      {
        if (index < 1 || index > allVariables.size())
        {
          logWarning("Initial unknown " + std::string(initialUnknownsGraph.getNodes()[i]) + " has bad dependency on variable with index " + std::to_string(index) + " which couldn't be resolved");
          return logError(std::string(getCref()) + ": Erroneous initial unknowns detected in modelDescription.xml\nUse flag --ignoreInitialUnknowns=true to ignore all initial unknowns, but this can cause inflated loop size.");
        }
        logDebug(std::string(getCref()) + ": " + getPath() + " initial unknown " + std::string(initialUnknownsGraph.getNodes()[i]) + " depends on " + std::string(allVariables[index - 1]));
        initialUnknownsGraph.addEdge(allVariables[index - 1].makeConnector(this->getFullCref()), initialUnknownsGraph.getNodes()[i]);
      }
    }
    i = i + 1;
  }

  return oms_status_ok;
}

oms_status_enu_t oms::ComponentFMU3CS::initializeDependencyGraph_outputs()
{
  if (outputsGraph.getEdges().connections.size() > 0)
  {
    logError(std::string(getCref()) + ": " + getPath() + " is already initialized.");
    return oms_status_error;
  }

  // if (!startIndex)
  // {
  //   logDebug(std::string(getCref()) + ": " + getPath() + " no dependencies");
  //   return oms_status_ok;
  // }

  // get the output dependencies
  int i = 0;
  for (const auto &it : values.modelStructureOutputs)
  {
    Variable& output = allVariables[outputs[i]];
    // no dependencies
    if (it.second.empty() && values.modelStructureOutputDependencyExist[it.first])
    {
      logDebug(std::string(getCref()) + ": " + getPath() + " output " + std::string(output) + " has no dependencies");
    }
    // dependency attribute not provided in modeldescription.xml, all output depends on all inputs
    else if (it.second.empty() && !values.modelStructureOutputDependencyExist[it.first])
    {
      logDebug(std::string(getCref()) + ": " + getPath() + " output " + std::string(output) + " depends on all");
      for (const auto& j : inputs)
        outputsGraph.addEdge(allVariables[j].makeConnector(this->getFullCref()), output.makeConnector(this->getFullCref()));
    }
    else
    {
      for (const auto &index : it.second)
      {
        if (index < 1 || index > allVariables.size())
        {
          logWarning("Output " + std::string(output) + " has bad dependency on variable with index " + std::to_string(index) + " which couldn't be resolved");
          return logError(std::string(getCref()) + ": erroneous dependencies detected in modelDescription.xml");
        }
        logDebug(std::string(getCref()) + ": " + getPath() + " output " + std::string(output) + " depends on " + std::string(allVariables[index - 1]));
        outputsGraph.addEdge(allVariables[index - 1].makeConnector(this->getFullCref()), output.makeConnector(this->getFullCref()));
      }
    }
    i = i + 1;
  }

  return oms_status_ok;
}

oms_status_enu_t oms::ComponentFMU3CS::instantiate()
{
  // TODO investigate fmi3IntermediateUpdateCallback = NULL
  if(!fmi3_instantiateCoSimulation(fmu, fmi3False, fmi3True, fmi3False, fmi3False, NULL, 0, fmu, omsfmi3logger, NULL))
  {
    logInfo("fmi3Instantiate() failed");
    exit(1);
  }

  // set start values from local resources
  if (values.hasResources())
  {
    for (const auto &it : values.parameterResources)
    {
      for (const auto &res : it.allresources)
      {
        if (res.second.linkResources) // set values only if resources are linked in ssd
          setResourcesHelper1(res.second);
      }
    }
    /*
      check for parameter entry at system level and override the start values if exist,
      as system level parameter has highest priority, this is done after checking for
      local resources because it is possible some parameters have local entry and other
      parameters have top level system entry
    */
    if (getParentSystem() && getParentSystem()->getValues().hasResources())
      setResourcesHelper2(getParentSystem()->getValues());
  }
  // set start values from root resources
  else if (getParentSystem() && getParentSystem()->getValues().hasResources())
  {
    setResourcesHelper2(getParentSystem()->getValues());
  }
  // set start values from top level root resources
  else if (getParentSystem()->getParentSystem() && getParentSystem()->getParentSystem()->getValues().hasResources())
  {
    setResourcesHelper2(getParentSystem()->getParentSystem()->getValues());
  }
  // set start values from inline resources
  else
  {
    setResourcesHelper1(values);
  }
  // enterInitialization
  time = getModel().getStartTime();

  double relativeTolerance = 0.0;
  dynamic_cast<SystemWC*>(getParentSystem())->getTolerance(&relativeTolerance);

  fmi3Status status_ = fmi3_enterInitializationMode(fmu, fmi3False, relativeTolerance, time, fmi3True, getModel().getStopTime());

  if (fmi3OK != status_) return logError_FMUCall("fmi3_enterInitializationMode", this);

  return oms_status_ok;
}

oms_status_enu_t oms::ComponentFMU3CS::setResourcesHelper1(Values values)
{
  for (const auto &v : values.booleanStartValues)
  {
    oms::ComRef cref = getValidCref(v.first);
    if (oms_status_ok != setBoolean(cref, v.second))
      return logError("Failed to set start value for " + std::string(v.first));
  }
  for (const auto &v : values.integerStartValues)
  {
    oms::ComRef cref = getValidCref(v.first);
    if (oms_status_ok != setInteger(cref, v.second))
      return logError("Failed to set start value for " + std::string(v.first));
  }
  for (const auto &v : values.realStartValues)
  {
    oms::ComRef cref = getValidCref(v.first);
    if (oms_status_ok != setReal(cref, v.second))
      return logError("Failed to set start value for " + std::string(v.first));
  }

  return oms_status_ok;
}

oms_status_enu_t oms::ComponentFMU3CS::setResourcesHelper2(Values values)
{
  for (const auto &it : values.parameterResources)
  {
    for (const auto &res : it.allresources)
    {
      for (const auto &v : res.second.booleanStartValues)
      {
        if (res.second.linkResources) // set values only if resources are linked in ssd
        {
          oms::ComRef tail(v.first);
          oms::ComRef head = tail.pop_front();
          if (head == getCref())
          {
            if (oms_status_ok != setBoolean(tail, v.second))
              return logError("Failed to set start value for " + std::string(v.first));
          }
        }
      }
      for (const auto &v : res.second.integerStartValues)
      {
        if (res.second.linkResources) // set values only if resources are linked in ssd
        {
          oms::ComRef tail(v.first);
          oms::ComRef head = tail.pop_front();
          if (head == getCref())
          {
            if (oms_status_ok != setInteger(tail, v.second))
              return logError("Failed to set start value for " + std::string(v.first));
          }
        }
      }
      for (const auto &v : res.second.realStartValues)
      {
        if (res.second.linkResources) // set values only if resources are linked in ssd
        {
          oms::ComRef tail(v.first);
          oms::ComRef head = tail.pop_front();
          if (head == getCref())
          {
            if (oms_status_ok != setReal(tail, v.second))
              return logError("Failed to set start value for " + std::string(v.first));
          }
        }
      }
    }
  }

  return oms_status_ok;
}

/*
 * function which returns validCrefs
 * (e.g) add.P => P
 * (e.g) chassis.C.mChassis => C.mChassis
 * inline parameters should be returned as default value (e.g.) P => P or C.mChassis => C.mChassis
 */
oms::ComRef oms::ComponentFMU3CS::getValidCref(ComRef cref)
{
  oms::ComRef tail(cref);
  oms::ComRef head = tail.pop_front();

  if (tail.isEmpty() || head != getCref()) // check for inline parameter crefs, (e.g.) P => P or C.mChassis => C.mChassis
    tail = cref;
  return tail;
}

oms_status_enu_t oms::ComponentFMU3CS::newResources(const std::string& ssvFilename, const std::string& ssmFilename, bool externalResources)
{
  Values resources;
  if (externalResources) // check of external resources and override the start values with new references
  {
    Snapshot snapshot;
    snapshot.importResourceFile(ssvFilename, getModel().getTempDirectory() + "/resources");

    // import ssm file, if provided
    if (!ssmFilename.empty())
      snapshot.importResourceFile(ssmFilename, getModel().getTempDirectory() + "/resources");

    if (oms_status_ok != resources.importFromSnapshot(snapshot, ssvFilename, ssmFilename))
      return logError("referenceResources failed for \"" + std::string(getFullCref()) + ":" + ssvFilename + "\"");
  }

  if (!values.hasResources())
  {
    if(!ssmFilename.empty())
      resources.ssmFile = "resources/" + ssmFilename;
    // copy modeldescriptionVariableUnits to ssv resources which will be used to export units
    resources.modelDescriptionVariableUnits = values.modelDescriptionVariableUnits;
    // copy modeldescriptionVariableUnitDefinitions to ssv resources which will be used to export unit definitions
    resources.modeldescriptionUnitDefinitions = values.modeldescriptionUnitDefinitions;
    resources.allresources["resources/" + ssvFilename] = resources;
    values.parameterResources.push_back(resources);
  }
  else
  {
    // generate empty ssv file, if more resources are added to same level
    if(!ssmFilename.empty())
      resources.ssmFile = "resources/" + ssmFilename;
    // copy modeldescriptionVariableUnits to ssv resources which will be used to export units
    resources.modelDescriptionVariableUnits = values.modelDescriptionVariableUnits;
    // copy modeldescriptionVariableUnitDefinitions to ssv resources which will be used to export unit definitions
    resources.modeldescriptionUnitDefinitions = values.modeldescriptionUnitDefinitions;
    values.parameterResources[0].allresources["resources/" + ssvFilename] = resources;
  }

  return oms_status_ok;
}

oms_status_enu_t oms::ComponentFMU3CS::deleteReferencesInSSD(const std::string& filename)
{
  if (values.hasResources())
    return values.deleteReferencesInSSD(filename);

  return oms_status_error;
}

oms_status_enu_t oms::ComponentFMU3CS::deleteResourcesInSSP(const std::string& filename)
{
  if (values.hasResources())
    return values.deleteResourcesInSSP(filename);

  return oms_status_error;
}

oms_status_enu_t oms::ComponentFMU3CS::initialize()
{
  clock.reset();
  CallClock callClock(clock);

  // exitInitialization
  fmi3Status status = fmi3_exitInitializationMode(fmu);

  if (fmi3OK != status) return logError_FMUCall("fmi3_exitInitializationMode", this);

  //logInfo("FMI3 initialization successfull");

  return oms_status_ok;
}

oms_status_enu_t oms::ComponentFMU3CS::terminate()
{
  freeState();
  fmi3Status fmistatus = fmi3_terminate(fmu);
  if (fmi3OK != fmistatus)
    return logError_Termination(getCref());

  fmi3_freeInstance(fmu);

  return oms_status_ok;
}

oms_status_enu_t oms::ComponentFMU3CS::reset()
{
  fmi3Status fmistatus = fmi3_reset(fmu);
  if (fmi3OK != fmistatus)
    return logError_ResetFailed(getCref());

  // enterInitialization
  time = getModel().getStartTime();
  double relativeTolerance = 0.0;
  dynamic_cast<SystemWC*>(getParentSystem())->getTolerance(&relativeTolerance);

  fmi3Status status_ = fmi3_enterInitializationMode(fmu, fmi3False, relativeTolerance, time, fmi3True, getModel().getStopTime());
  if (fmi3OK != fmistatus) return logError_FMUCall("fmi3_enterInitializationMode", this);

  return oms_status_ok;
}

oms_status_enu_t oms::ComponentFMU3CS::stepUntil(double stopTime)
{
  CallClock callClock(clock);
  System *topLevelSystem = getModel().getTopLevelSystem();

  double hdef = (stopTime-time) / 1.0;
  bool eventEncountered, terminateSimulation, earlyReturn;
  double lastT;

  while (time < stopTime)
  {
    fmi3Status status = fmi3_doStep(fmu,  time, hdef, fmi3True, &eventEncountered, &terminateSimulation, &earlyReturn, &lastT);
    time += hdef;

    if (status == fmi3Discard)
    {
      getModel().setStopTime(time);
      logInfo("fmi3_doStep discarded for FMU \"" + std::string(getFullCref()) + "\"");
      return oms_status_ok;
    }
    else if (status != fmi3OK)
      return logError_FMUCall("fmi3_doStep", this);
  }
  time = stopTime;
  return oms_status_ok;
}

oms_status_enu_t oms::ComponentFMU3CS::getBoolean(const fmi3ValueReference& vr, bool& value)
{
  CallClock callClock(clock);

  // bool value_;
  if (fmi3OK != fmi3_getBoolean(fmu, &vr, 1, &value, 1))
    return oms_status_error;

  // value = value_ ? true : false;
  return oms_status_ok;
}

oms_status_enu_t oms::ComponentFMU3CS::getBoolean(const ComRef& cref, bool& value)
{
  CallClock callClock(clock);

  if (oms_modelState_virgin == getModel().getModelState())
  {
    // check for start values exist, priority over modeldescription.xml start values
    if (values.hasResources())  // search in local resources
    {
      if (oms_status_ok == values.getBooleanResources(cref, value, false, oms_modelState_virgin))
      {
        return oms_status_ok;
      }
      // search in modelDescription.xml
      else if (oms_status_ok == values.getBooleanFromModeldescription(cref, value))
      {
        return oms_status_ok;
      }

      return logError("no start value set or available for signal: " + std::string(getFullCref() + cref));
    }
    else if (getParentSystem() && getParentSystem()->getValues().hasResources())  // search in root resources
    {
      if (oms_status_ok == getParentSystem()->getValues().getBooleanResources(getCref()+cref, value, false, oms_modelState_virgin))
      {
        return oms_status_ok;
      }
      // search in modelDescription.xml
      else if(oms_status_ok == values.getBooleanFromModeldescription(cref, value))
      {
        return oms_status_ok;
      }

      return logError("no start value set or available for signal: " + std::string(getFullCref() + cref));
    }
    else if (getParentSystem()->getParentSystem() && getParentSystem()->getParentSystem()->getValues().hasResources())  // search in top level root resources
    {
      if (oms_status_ok == getParentSystem()->getParentSystem()->getValues().getBooleanResources(getCref()+cref, value, false, oms_modelState_virgin))
      {
        return oms_status_ok;
      }
      // search in modelDescription.xml
      else if(oms_status_ok == values.getBooleanFromModeldescription(cref, value))
      {
        return oms_status_ok;
      }

      return logError("no start value set or available for signal: " + std::string(getFullCref() + cref));
    }
    else // search inline
    {
      // check for start values exist, priority over modeldescription.xml start values
      if (oms_status_ok == values.getBoolean(cref, value))
      {
        return oms_status_ok;
      }
      else
      {
        return values.getBooleanFromModeldescription(cref, value);
      }
    }
  }

  int j=-1;
  for (size_t i = 0; i < allVariables.size(); i++)
  {
    if (allVariables[i].getCref() == cref && allVariables[i].isTypeBoolean())
    {
      j = i;
      break;
    }
  }

  if (!fmu || j < 0)
    return logError_UnknownSignal(getFullCref() + cref);

  fmi3ValueReference vr = allVariables[j].getValueReferenceFMI3();
  return getBoolean(vr, value);
}

oms_status_enu_t oms::ComponentFMU3CS::getInteger(const fmi3ValueReference& vr, int& value, oms_signal_numeric_type_enu_t numericType)
{
  CallClock callClock(clock);

   // Temporary variables for different types
  int64_t value64;
  int32_t value32;
  int16_t value16;
  int8_t value8;
  uint64_t valueU64;
  uint32_t valueU32;
  uint16_t valueU16;
  uint8_t valueU8;

  switch (numericType)
  {
    case oms_signal_numeric_type_INT64:
    {
      if (fmi3OK != fmi3_getInt64(fmu, &vr, 1, &value64, 1))
        return oms_status_error;
      if (value64 < INT_MIN || value64 > INT_MAX)
        return oms_status_error;  // Value out of range for int
      value = static_cast<int>(value64);  // Cast to int
      break;
    }
    case oms_signal_numeric_type_INT32:
    {
      if (fmi3OK != fmi3_getInt32(fmu, &vr, 1, &value, 1))
        return oms_status_error;
      break;
    }
    case oms_signal_numeric_type_INT16:
    {
      if (fmi3OK != fmi3_getInt16(fmu, &vr, 1, &value16, 1))
        return oms_status_error;
      value = static_cast<int>(value16);
      break;
    }
    case oms_signal_numeric_type_INT8:
    {
      if (fmi3OK != fmi3_getInt8(fmu, &vr, 1, &value8, 1))
        return oms_status_error;
      value = static_cast<int>(value8);
      break;
    }
    case oms_signal_numeric_type_UINT64:
    {
      if (fmi3OK != fmi3_getUInt64(fmu, &vr, 1, &valueU64, 1))
        return oms_status_error;
      value = static_cast<int>(valueU64);
      break;
    }
    case oms_signal_numeric_type_UINT32:
    {
      if (fmi3OK != fmi3_getUInt32(fmu, &vr, 1, &valueU32, 1))
        return oms_status_error;
      value = static_cast<int>(valueU32);
      break;
    }
    case oms_signal_numeric_type_UINT16:
    {
      if (fmi3OK != fmi3_getUInt16(fmu, &vr, 1, &valueU16, 1))
        return oms_status_error;
      value = static_cast<int>(valueU16);
      break;
    }
    case oms_signal_numeric_type_UINT8:
    {
      if (fmi3OK != fmi3_getUInt8(fmu, &vr, 1, &valueU8, 1))
        return oms_status_error;
      value = static_cast<int>(valueU8);
      break;
    }
    default :
      return logError("Unsupported Numeric Type");
  }
  return oms_status_ok;
}

oms_status_enu_t oms::ComponentFMU3CS::getInteger(const ComRef& cref, int& value)
{
  CallClock callClock(clock);

  if (oms_modelState_virgin == getModel().getModelState())
  {
    // check for start values exist, priority over modeldescription.xml start values
    if (values.hasResources())  // search in local resources
    {
      if (oms_status_ok == values.getIntegerResources(cref, value, false, oms_modelState_virgin))
      {
        return oms_status_ok;
      }
      // search in modelDescription.xml
      else if (oms_status_ok == values.getIntegerFromModeldescription(cref, value))
      {
        return oms_status_ok;
      }

      return logError("no start value set or available for signal: " + std::string(getFullCref() + cref));
    }
    else if (getParentSystem() && getParentSystem()->getValues().hasResources())  // search in root resources
    {
      if (oms_status_ok == getParentSystem()->getValues().getIntegerResources(getCref()+cref, value, false, oms_modelState_virgin))
      {
        return oms_status_ok;
      }
      // search in modelDescription.xml
      else if(oms_status_ok == values.getIntegerFromModeldescription(cref, value))
      {
        return oms_status_ok;
      }

      return logError("no start value set or available for signal: " + std::string(getFullCref() + cref));
    }
    else if (getParentSystem()->getParentSystem() && getParentSystem()->getParentSystem()->getValues().hasResources())  // search in top level root resources
    {
      if (oms_status_ok == getParentSystem()->getParentSystem()->getValues().getIntegerResources(getCref()+cref, value, false, oms_modelState_virgin))
      {
        return oms_status_ok;
      }
      // search in modelDescription.xml
      else if(oms_status_ok == values.getIntegerFromModeldescription(cref, value))
      {
        return oms_status_ok;
      }

      return logError("no start value set or available for signal: " + std::string(getFullCref() + cref));
    }
    else // search inline
    {
      // check for start values exist, priority over modeldescription.xml start values
      if (oms_status_ok == values.getInteger(cref, value))
      {
        return oms_status_ok;
      }
      else
      {
        return values.getIntegerFromModeldescription(cref, value);
      }
    }
  }

  int j=-1;
  for (size_t i = 0; i < allVariables.size(); i++)
  {
    if (allVariables[i].getCref() == cref && allVariables[i].isTypeInteger())
    {
      j = i;
      break;
    }
  }

  if (!fmu || j < 0)
    return logError_UnknownSignal(getFullCref() + cref);

  fmi3ValueReference vr = allVariables[j].getValueReferenceFMI3();
  return getInteger(vr, value, allVariables[j].getNumericType());
}

oms::Variable* oms::ComponentFMU3CS::getVariable(const ComRef& cref)
{
  CallClock callClock(clock);
  for (size_t i=0; i < allVariables.size(); i++)
    if (allVariables[i].getCref() == cref)
      return &allVariables[i];

  logError_UnknownSignal(getFullCref() + cref);
  return NULL;
}

oms_status_enu_t oms::ComponentFMU3CS::getReal(const fmi3ValueReference& vr, double& value, oms_signal_numeric_type_enu_t numericType)
{
  CallClock callClock(clock);

  switch (numericType)
  {
    case oms_signal_numeric_type_FLOAT64:
    {
      if (fmi3OK != fmi3_getFloat64(fmu, &vr, 1, &value, 1))
        return oms_status_error;
      break;
    }
    case oms_signal_numeric_type_FLOAT32:
    {
      float value_;
      if (fmi3OK != fmi3_getFloat32(fmu, &vr, 1, &value_, 1))
        return oms_status_error;
      // Convert the float to double and assign to 'value'
      value = static_cast<double>(value_);
      break;
    }
    default:
      return logError("UnSupported Numeric Type:");
  }

  if (std::isnan(value))
    return logError("getReal returned NAN");
  if (std::isinf(value))
    return logError("getReal returned +/-inf");

  return oms_status_ok;
}

oms_status_enu_t oms::ComponentFMU3CS::getReal(const ComRef& cref, double& value)
{
  CallClock callClock(clock);

  if (oms_modelState_virgin == getModel().getModelState())
  {
    // check for start values exist, priority over modeldescription.xml start values
    if (values.hasResources())  // search in local resources
    {
      if (oms_status_ok == values.getRealResources(cref, value, false, oms_modelState_virgin))
      {
        return oms_status_ok;
      }
      // search in modelDescription.xml
      else if (oms_status_ok == values.getRealFromModeldescription(cref, value))
      {
        return oms_status_ok;
      }

      return logError("no start value set or available for signal: " + std::string(getFullCref() + cref));
    }
    else if (getParentSystem() && getParentSystem()->getValues().hasResources())  // search in root resources
    {
      if (oms_status_ok == getParentSystem()->getValues().getRealResources(getCref()+cref, value, false, oms_modelState_virgin))
      {
        return oms_status_ok;
      }
      // search in modelDescription.xml
      else if(oms_status_ok == values.getRealFromModeldescription(cref, value))
      {
        return oms_status_ok;
      }

      return logError("no start value set or available for signal: " + std::string(getFullCref() + cref));
    }
    else if (getParentSystem()->getParentSystem() && getParentSystem()->getParentSystem()->getValues().hasResources())  // search in top level root resources
    {
      if (oms_status_ok == getParentSystem()->getParentSystem()->getValues().getRealResources(getCref()+cref, value, false, oms_modelState_virgin))
      {
        return oms_status_ok;
      }
      // search in modelDescription.xml
      else if(oms_status_ok == values.getRealFromModeldescription(cref, value))
      {
        return oms_status_ok;
      }

      return logError("no start value set or available for signal: " + std::string(getFullCref() + cref));
    }
    else // search inline
    {
      // check for start values exist, priority over modeldescription.xml start values
      if (oms_status_ok == values.getReal(cref, value))
      {
        return oms_status_ok;
      }
      else
      {
        return values.getRealFromModeldescription(cref, value);
      }
    }
  }

  int j=-1;
  for (size_t i = 0; i < allVariables.size(); i++)
  {
    if (allVariables[i].getCref() == cref && allVariables[i].isTypeReal())
    {
      j = i;
      break;
    }
  }

  if (!fmu || j < 0)
    return logError_UnknownSignal(getFullCref() + cref);

  fmi3ValueReference vr = allVariables[j].getValueReferenceFMI3();
  return getReal(vr, value, allVariables[j].getNumericType());
}

oms_status_enu_t oms::ComponentFMU3CS::getString(const fmi3ValueReference& vr, std::string& value)
{
  CallClock callClock(clock);
  fmi3String str;

  if (fmi3OK != fmi3_getString(fmu, &vr, 1, &str, 1))
    return oms_status_error;

  value = std::string(str);

  return oms_status_ok;
}

oms_status_enu_t oms::ComponentFMU3CS::getString(const ComRef& cref, std::string& value)
{
  CallClock callClock(clock);

  if (oms_modelState_virgin == getModel().getModelState())
  {
    // check for start values exist, priority over modeldescription.xml start values
    if (values.hasResources())  // search in local resources
    {
      if (oms_status_ok == values.getStringResources(cref, value, false, oms_modelState_virgin))
      {
        return oms_status_ok;
      }
      // search in modelDescription.xml
      else if (oms_status_ok == values.getStringFromModeldescription(cref, value))
      {
        return oms_status_ok;
      }

      return logError("no start value set or available for signal: " + std::string(getFullCref() + cref));
    }
    else if (getParentSystem() && getParentSystem()->getValues().hasResources())  // search in root resources
    {
      if (oms_status_ok == getParentSystem()->getValues().getStringResources(getCref()+cref, value, false, oms_modelState_virgin))
      {
        return oms_status_ok;
      }
      // search in modelDescription.xml
      else if(oms_status_ok == values.getStringFromModeldescription(cref, value))
      {
        return oms_status_ok;
      }

      return logError("no start value set or available for signal: " + std::string(getFullCref() + cref));
    }
    else if (getParentSystem()->getParentSystem() && getParentSystem()->getParentSystem()->getValues().hasResources())  // search in top level root resources
    {
      if (oms_status_ok == getParentSystem()->getParentSystem()->getValues().getStringResources(getCref()+cref, value, false, oms_modelState_virgin))
      {
        return oms_status_ok;
      }
      // search in modelDescription.xml
      else if(oms_status_ok == values.getStringFromModeldescription(cref, value))
      {
        return oms_status_ok;
      }

      return logError("no start value set or available for signal: " + std::string(getFullCref() + cref));
    }
    else // search inline
    {
      // check for start values exist, priority over modeldescription.xml start values
      if (oms_status_ok == values.getString(cref, value))
      {
        return oms_status_ok;
      }
      else
      {
        return values.getStringFromModeldescription(cref, value);
      }
    }
  }

  int j=-1;
  for (size_t i = 0; i < allVariables.size(); i++)
  {
    if (allVariables[i].getCref() == cref && allVariables[i].isTypeString())
    {
      j = i;
      break;
    }
  }

  if (!fmu || j < 0)
    return logError_UnknownSignal(getFullCref() + cref);

  fmi3ValueReference vr = allVariables[j].getValueReferenceFMI3();
  return getString(vr, value);
}

oms_status_enu_t oms::ComponentFMU3CS::getDirectionalDerivative(const ComRef& unknownCref, const ComRef& knownCref, double& value)
{
  if (!getModel().validState(oms_modelState_instantiated|oms_modelState_initialization|oms_modelState_simulation))
    return logError_ModelInWrongState(getModel().getCref());

  if (!getFMUInfo()->getProvidesDirectionalDerivative())
    return logError("FMU \"" + std::string(getFullCref()) + "\" doesn't support directional derivatives (providesDirectionalDerivative = false in modelDescription.xml)");

  int j = -1;
  for (size_t i = 0; i < allVariables.size(); i++)
  {
    if (allVariables[i].getCref() == unknownCref && allVariables[i].isTypeReal())
    {
      j = i;
      break;
    }
  }

  // check for knownIndex, if provided
  int knownIndex = -1;
  if (!knownCref.isEmpty())
  {
    for (size_t i = 0; i < allVariables.size(); i++)
    {
      if (allVariables[i].getCref() == knownCref && allVariables[i].isTypeReal())
      {
        knownIndex = i;
        break;
      }
    }
  }

  if (!fmu || j < 0)
    return logError_UnknownSignal(getFullCref() + unknownCref);

  if (oms_modelState_instantiated == getModel().getModelState() || oms_modelState_initialization == getModel().getModelState())
  {
    // check index exist in ModelStructure inititalUnknowns
    auto index = values.modelStructureInitialUnknowns.find(j+1);
    if (index == values.modelStructureInitialUnknowns.end())
      return logError("Signal \"" + std::string(getFullCref() + unknownCref) + "\" could not be resolved to an <InitialUnknowns> index in <ModelStructure>");

    //get dependencylist from <InitialUnknowns> in <ModelStructure>
    std::vector<int> dependencyList = values.modelStructureInitialUnknowns[j+1];
    getDirectionalDerivativeHeper(j, knownIndex, dependencyList, value);
  }

  if (oms_modelState_simulation == getModel().getModelState())
  {
    if (!allVariables[j].isOutput() && !allVariables[j].isState() && !allVariables[j].isDer())
      return logError("Signal \"" + std::string(getFullCref() + unknownCref) + "\" could not be resolved to an output or state or derivates after initalization");

    // <Outputs>
    if (allVariables[j].isOutput())
    {
      // check index exist in ModelStructure inititalUnknowns
      auto index = values.modelStructureOutputs.find(j + 1);
      if (index == values.modelStructureOutputs.end())
        return logError("Signal \"" + std::string(getFullCref() + unknownCref) + "\" could not be resolved to an <Outputs> index in <ModelStructure>");

      // get dependencylist from <Outputs> in <ModelStructure>
      std::vector<int> dependencyList = values.modelStructureOutputs[j + 1];
      getDirectionalDerivativeHeper(j, knownIndex, dependencyList, value);
    }
    // <Derivatives>
    if (allVariables[j].isState() || allVariables[j].isDer())
    {
      // check index exist in ModelStructure inititalUnknowns
      auto index = values.modelStructureDerivatives.find(j + 1);
      if (index == values.modelStructureDerivatives.end())
        return logError("Signal \"" + std::string(getFullCref() + unknownCref) + "\" could not be resolved to an <Derivatives> index in <ModelStructure>");

      // get dependencylist from <Derivatives> in <ModelStructure>
      std::vector<int> dependencyList = values.modelStructureDerivatives[j + 1];
      getDirectionalDerivativeHeper(j, knownIndex, dependencyList, value);
    }
  }

  // fmi3ValueReference vr_unknown[1] = {5};
  // fmi3ValueReference vr_known[4] = {0, 2, 3, 4};
  // fmi2Real dvknown[4] = {1.0, 1.0, 1.0, 1.0};
  // fmi2Real val;
  // std::cout << "Get directional derivative_static_1: " << val << std::endl;
  // fmi2_import_get_directional_derivative(fmu, vr_unknown, 1, vr_known, 4, dvknown, &val);
  // std::cout << "\nGet directional derivative_static_2: " << val << std::endl;

  return oms_status_ok;
}

oms_status_enu_t oms::ComponentFMU3CS::getDirectionalDerivativeHeper(const int unknownIndex, const int knownIndex, const std::vector<int> &dependencyList, double &value)
{
  fmi3ValueReference vr_unknown = allVariables[unknownIndex].getValueReferenceFMI3();
  fmi3ValueReference *vr_known = (fmi3ValueReference *)calloc(dependencyList.size(), sizeof(fmi3ValueReference *));
  fmi3Float64 *dvknown = (fmi3Float64 *)calloc(dependencyList.size(), sizeof(fmi3Float64 *));

  for (int i = 0; i < dependencyList.size(); i++)
  {
    vr_known[i] = allVariables[dependencyList[i] - 1].getValueReferenceFMI3();

    // The knownIndex is < 0 if not specified. In this case, we
    // calculate the sum of the row, which means we set all seed
    // values to 1.0. Otherwise we just set the explicitly provided
    // element to 1.0.
    if (knownIndex < 0 || (dependencyList[i] == knownIndex + 1))
      dvknown[i] = 1.0;
    else
      dvknown[i] = 0.0;
  }

  fmi3_getDirectionalDerivative(fmu, &vr_unknown, 1, vr_known, dependencyList.size(), dvknown, 1, &value, 1);

  free(vr_known);
  free(dvknown);

  return oms_status_ok;
}

oms_status_enu_t oms::ComponentFMU3CS::getRealOutputDerivative(const ComRef& cref, SignalDerivative& der)
{
  CallClock callClock(clock);

  int j=-1;
  for (size_t i = 0; i < allVariables.size(); i++)
  {
    if (allVariables[i].getCref() == cref && allVariables[i].isTypeReal() && allVariables[i].isOutput())
    {
      j = i;
      break;
    }
  }

  if (!fmu || j < 0)
    return logError_UnknownSignal(getFullCref() + cref);

  der = SignalDerivative(getFMUInfo()->getMaxOutputDerivativeOrder(), fmu, allVariables[j].getValueReferenceFMI3());
  return oms_status_ok;
}

oms_status_enu_t oms::ComponentFMU3CS::setRealInputDerivative(const ComRef& cref, const SignalDerivative& der)
{
  CallClock callClock(clock);

  if (!getFMUInfo()->getCanInterpolateInputs())
    return oms_status_ok;

  int j=-1;
  for (size_t i = 0; i < allVariables.size(); i++)
  {
    if (allVariables[i].getCref() == cref)
    {
      if (allVariables[i].isTypeReal() && allVariables[i].isInput())
      {
        j = i;
        break;
      }
      else
        return logError_OnlyForRealInputs(getFullCref() + cref);
    }
  }

  if (!fmu || j < 0)
    return logError_UnknownSignal(getFullCref() + cref);

  fmi3ValueReference vr = allVariables[j].getValueReferenceFMI3();
  return der.setRealInputDerivatives(fmu, vr);
}

oms_status_enu_t oms::ComponentFMU3CS::setBoolean(const ComRef& cref, bool value)
{
  CallClock callClock(clock);
  int j=-1;
  for (size_t i = 0; i < allVariables.size(); i++)
  {
    if (allVariables[i].getCref() == cref && allVariables[i].isTypeBoolean())
    {
      j = i;
      break;
    }
  }

  if (!fmu || j < 0)
    return logError_UnknownSignal(getFullCref() + cref);

  if (oms_modelState_virgin == getModel().getModelState())
  {
    values.updateModelDescriptionBooleanStartValue(cref, value);
    // check for local resources available
    if (values.hasResources())
    {
      return values.setBooleanResources(cref, value, getFullCref(), false, oms_modelState_virgin);
    }
    // check for resources in root
    else if (getParentSystem() && getParentSystem()->getValues().hasResources())
    {
      return getParentSystem()->getValues().setBooleanResources(getCref()+cref, value, getParentSystem()->getFullCref(), false, oms_modelState_virgin);
    }
    // check for resources in top level root
    else if (getParentSystem()->getParentSystem() && getParentSystem()->getParentSystem()->getValues().hasResources())
    {
      return getParentSystem()->getParentSystem()->getValues().setBooleanResources(getCref()+cref, value, getParentSystem()->getParentSystem()->getFullCref(), false, oms_modelState_virgin);
    }
    else
    {
      //inline parameter settings
      values.setBoolean(cref, value);
    }
  }
  else
  {
    fmi3ValueReference vr = allVariables[j].getValueReferenceFMI3();
    // int value_ = value ? 1 : 0;
    if (fmi3OK != fmi3_setBoolean(fmu, &vr, 1, &value, 1))
      return oms_status_error;
  }

  return oms_status_ok;
}

oms_status_enu_t oms::ComponentFMU3CS::setInteger(const ComRef& cref, int value)
{
  CallClock callClock(clock);
  int j=-1;
  for (size_t i = 0; i < allVariables.size(); i++)
  {
    if (allVariables[i].getCref() == cref && allVariables[i].isTypeInteger())
    {
      j = i;
      break;
    }
  }

  if (!fmu || j < 0)
    return logError_UnknownSignal(getFullCref() + cref);

  if (oms_modelState_virgin == getModel().getModelState())
  {
    // update start values in top level Modeldescription.xml to be exported in ssv templates
    values.updateModelDescriptionIntegerStartValue(cref, value);
    // check for local resources available
    if (values.hasResources())
    {
      return values.setIntegerResources(cref, value, getFullCref(), false, oms_modelState_virgin);
    }
    // check for resources in root
    else if (getParentSystem() && getParentSystem()->getValues().hasResources())
    {
      return getParentSystem()->getValues().setIntegerResources(getCref()+cref, value, getParentSystem()->getFullCref(), false, oms_modelState_virgin);
    }
    // check for resources in top level root
    else if (getParentSystem()->getParentSystem() && getParentSystem()->getParentSystem()->getValues().hasResources())
    {
      return getParentSystem()->getParentSystem()->getValues().setIntegerResources(getCref()+cref, value, getParentSystem()->getParentSystem()->getFullCref(), false, oms_modelState_virgin);
    }
    else
    {
      //inline parameter settings
      values.setInteger(cref, value);
    }
  }
  else
  {
    fmi3ValueReference vr = allVariables[j].getValueReferenceFMI3();
    int64_t value64;
    int32_t value32;
    int16_t value16;
    int8_t value8;
    uint64_t valueU64;
    uint32_t valueU32;
    uint16_t valueU16;
    uint8_t valueU8;
    switch (allVariables[j].getNumericType())
    {
      case oms_signal_numeric_type_INT64:
      {
        value64 = static_cast<int>(value); // Cast to int
        if (fmi3OK != fmi3_setInt64(fmu, &vr, 1, &value64, 1))
          return oms_status_error;
        break;
      }
      case oms_signal_numeric_type_INT32:
      {
        if (fmi3OK != fmi3_setInt32(fmu, &vr, 1, &value, 1))
          return oms_status_error;
        break;
      }
      case oms_signal_numeric_type_INT16:
      {
        value16 = static_cast<int>(value);
        if (fmi3OK != fmi3_setInt16(fmu, &vr, 1, &value16, 1))
          return oms_status_error;
        break;
      }

      case oms_signal_numeric_type_INT8:
      {
        value8 = static_cast<int>(value);
        if (fmi3OK != fmi3_setInt8(fmu, &vr, 1, &value8, 1))
          return oms_status_error;
        break;
      }
      case oms_signal_numeric_type_UINT64:
      {
        valueU64 = static_cast<int>(value);
        if (fmi3OK != fmi3_setUInt64(fmu, &vr, 1, &valueU64, 1))
          return oms_status_error;
        break;
      }
      case oms_signal_numeric_type_UINT32:
      {
        valueU32 = static_cast<int>(value);
        if (fmi3OK != fmi3_setUInt32(fmu, &vr, 1, &valueU32, 1))
          return oms_status_error;
        break;
      }
      case oms_signal_numeric_type_UINT16:
      {
        valueU16 = static_cast<int>(value);
        if (fmi3OK != fmi3_setUInt16(fmu, &vr, 1, &valueU16, 1))
          return oms_status_error;
        break;
      }
      case oms_signal_numeric_type_UINT8:
      {
        valueU8 = static_cast<int>(value);
        if (fmi3OK != fmi3_setUInt8(fmu, &vr, 1, &valueU8, 1))
          return oms_status_error;
        break;
      }
      default:
        return logError("Unsupported Numeric Type for var: \"" + std::string(cref.c_str()) + "\"");
      }
  }

  return oms_status_ok;
}

oms_status_enu_t oms::ComponentFMU3CS::setReal(const ComRef& cref, double value)
{
  CallClock callClock(clock);
  int j=-1;
  for (size_t i = 0; i < allVariables.size(); i++)
  {
    if (allVariables[i].getCref() == cref && allVariables[i].isTypeReal())
    {
      j = i;
      break;
    }
  }

  if (!fmu || j < 0)
    return logError_UnknownSignal(getFullCref() + cref);

  if (getModel().validState(oms_modelState_virgin|oms_modelState_enterInstantiation|oms_modelState_instantiated))
    if (allVariables[j].isCalculated() || allVariables[j].isIndependent())
      return logWarning("It is not allowed to provide a start value if initial=\"calculated\" or causality=\"independent\".");

  if (oms_modelState_virgin == getModel().getModelState())
  {
    // update start values in top level Modeldescription.xml to be exported in ssv templates
    values.updateModelDescriptionRealStartValue(cref, value);
    // check for local resources available
    if (values.hasResources())
    {
      values.copyModelDescriptionUnitToResources(values);
      return values.setRealResources(cref, value, getFullCref(), false, oms_modelState_virgin);
    }
    // check for resources in root
    else if (getParentSystem() && getParentSystem()->getValues().hasResources())
    {
      getParentSystem()->getValues().copyModelDescriptionUnitToResources(values);
      return getParentSystem()->getValues().setRealResources(getCref()+cref, value, getParentSystem()->getFullCref(), false, oms_modelState_virgin);
    }
    // check for resources in top level root
    else if (getParentSystem()->getParentSystem() && getParentSystem()->getParentSystem()->getValues().hasResources())
    {
      getParentSystem()->getParentSystem()->getValues().copyModelDescriptionUnitToResources(values);
      return getParentSystem()->getParentSystem()->getValues().setRealResources(getCref()+cref, value, getParentSystem()->getParentSystem()->getFullCref(), false, oms_modelState_virgin);
    }
    else
    {
      //inline parameter settings
      values.setReal(cref, value);
    }
  }
  else
  {
    fmi3ValueReference vr = allVariables[j].getValueReferenceFMI3();
    switch (allVariables[j].getNumericType())
    {
      case oms_signal_numeric_type_FLOAT64:
      {
        if (fmi3OK != fmi3_setFloat64(fmu, &vr, 1, &value, 1))
          return oms_status_error;
        break;
      }
      case oms_signal_numeric_type_FLOAT32:
      {
        float value_= static_cast<float>(value);
        if (fmi3OK != fmi3_setFloat32(fmu, &vr, 1, &value_, 1))
          return oms_status_error;
        break;
      }
      default:
        return logError("Unsupported Numeric Type for var: \"" + std::string(cref.c_str()) + "\"");
    }
  }

  return oms_status_ok;
}

oms_status_enu_t oms::ComponentFMU3CS::setString(const ComRef& cref, const std::string& value)
{
  CallClock callClock(clock);
  int j=-1;
  for (size_t i = 0; i < allVariables.size(); i++)
  {
    if (allVariables[i].getCref() == cref && allVariables[i].isTypeString())
    {
      j = i;
      break;
    }
  }

  if (!fmu || j < 0)
    return logError_UnknownSignal(getFullCref() + cref);

  if (getModel().validState(oms_modelState_virgin|oms_modelState_enterInstantiation|oms_modelState_instantiated))
    if (allVariables[j].isCalculated() || allVariables[j].isIndependent())
      return logWarning("It is not allowed to provide a start value if initial=\"calculated\" or causality=\"independent\".");

  if (oms_modelState_virgin == getModel().getModelState())
  {
    values.updateModelDescriptionStringStartValue(cref, value);
    // check for local resources available
    if (values.hasResources())
    {
      return values.setStringResources(cref, value, getFullCref(), false, oms_modelState_virgin);
    }
    // check for resources in root
    else if (getParentSystem() && getParentSystem()->getValues().hasResources())
    {
      return getParentSystem()->getValues().setStringResources(getCref()+cref, value, getParentSystem()->getFullCref(), false, oms_modelState_virgin);
    }
    // check for resources in top level root
    else if (getParentSystem()->getParentSystem() && getParentSystem()->getParentSystem()->getValues().hasResources())
    {
      return getParentSystem()->getParentSystem()->getValues().setStringResources(getCref()+cref, value, getParentSystem()->getParentSystem()->getFullCref(), false, oms_modelState_virgin);
    }
    else
    {
      //inline parameter settings
      values.setString(cref, value);
    }
  }
  else
  {
    fmi3ValueReference vr = allVariables[j].getValueReferenceFMI3();
    fmi3String value_ = value.c_str();
    if (fmi3OK != fmi3_setString(fmu, &vr, 1, &value_, 1))
      return oms_status_error;
  }

  return oms_status_ok;
}

oms_status_enu_t oms::ComponentFMU3CS::setUnit(const ComRef &cref, const std::string &value)
{
  // set units to connectors
  for (auto &connector : connectors)
  {
    if (connector)
    {
      if (connector->getName() == cref)
      {
        connector->connectorUnits.clear();
        connector->connectorUnits[value] = {};
      }
    }
  }

  // set unit in top level modeldescription.xml
  values.updateModelDescriptionVariableUnit(cref, value);

  // check for local resources available
  if (values.hasResources())
  {
    return values.setUnitResources(cref, value, getFullCref());
  }
  // check for resources in root
  else if (getParentSystem() && getParentSystem()->getValues().hasResources())
  {
    return getParentSystem()->getValues().setUnitResources(getCref() + cref, value, getParentSystem()->getFullCref());
  }
  // check for resources in top level root
  else if (getParentSystem()->getParentSystem() && getParentSystem()->getParentSystem()->getValues().hasResources())
  {
    return getParentSystem()->getParentSystem()->getValues().setUnitResources(getCref() + cref, value, getParentSystem()->getParentSystem()->getFullCref());
  }
  else
  {
    // inline unit settings
    values.setUnit(cref, value);
  }
  return oms_status_ok;
}

oms_status_enu_t oms::ComponentFMU3CS::deleteStartValue(const ComRef& cref)
{
  // check for local resources
  if (values.hasResources())
  {
    return values.deleteStartValueInResources(cref);
  }
  // check for resources in root
  else if (getParentSystem() && getParentSystem()->getValues().hasResources())
  {
    return getParentSystem()->getValues().deleteStartValueInResources(getCref()+cref);
  }
  // check for resources in top level root
  else if (getParentSystem()->getParentSystem() && getParentSystem()->getParentSystem()->getValues().hasResources())
  {
    return getParentSystem()->getParentSystem()->getValues().deleteStartValueInResources(getCref()+cref);
  }
  else
  {
    return values.deleteStartValue(cref);
  }

  return oms_status_error;
}

oms_status_enu_t oms::ComponentFMU3CS::setValuesResources(Values& values)
{
  // set all ssv and ssm resources from the old component to replacing component
  this->values.parameterResources = values.parameterResources;
  // set all user define values from the old component to replacing component as user defined values have higher priority
  // over modeldescription.xml
  this->values.realStartValues = values.realStartValues;
  this->values.integerStartValues = values.integerValues;
  this->values.booleanStartValues = values.booleanStartValues;

  return oms_status_ok;
}

oms_status_enu_t oms::ComponentFMU3CS::updateOrDeleteStartValueInReplacedComponent(std::vector<std::string>& warningList)
{
  // check for local resources available
  if (values.hasResources())
  {
    return values.updateOrDeleteStartValueInReplacedComponent(values, this->getCref(), warningList);
  }
  // check for resources in root
  else if (getParentSystem() && getParentSystem()->getValues().hasResources())
  {
    return getParentSystem()->getValues().updateOrDeleteStartValueInReplacedComponent(values, this->getCref(), warningList);
  }
  // check for resources in top level root
  else if (getParentSystem()->getParentSystem() && getParentSystem()->getParentSystem()->getValues().hasResources())
  {
    return getParentSystem()->getParentSystem()->getValues().updateOrDeleteStartValueInReplacedComponent(values, this->getCref(), warningList);
  }
  else
  {
    // inline parameter settings, no need to update the values
    return oms_status_ok;
  }

  return oms_status_error;
}

oms_status_enu_t oms::ComponentFMU3CS::registerSignalsForResultFile(ResultWriter& resultFile)
{
  resultFileMapping.clear();

  if (Flags::WallTime())
    clock_id = resultFile.addSignal(std::string(getFullCref() + ComRef("$wallTime")), "wall-clock time [s]", SignalType_REAL);
  else
    clock_id = 0;

  for (unsigned int i=0; i<allVariables.size(); ++i)
  {
    if (!exportVariables[i])
      continue;

    auto const &var = allVariables[i];
    std::string name = std::string(getFullCref() + var.getCref());
    const std::string& description = var.getDescription();
    if (var.isParameter())
    {
      SignalValue_t value;
      if (var.isTypeReal())
      {
        getReal(var.getCref(), value.realValue);
        resultFile.addParameter(name, description, SignalType_REAL, value);
      }
      else if (var.isTypeInteger())
      {
        getInteger(var.getCref(), value.intValue);
        resultFile.addParameter(name, description, SignalType_INT, value);
      }
      else if (var.isTypeBoolean())
      {
        getBoolean(var.getCref(), value.boolValue);
        resultFile.addParameter(name, description, SignalType_BOOL, value);
      }
      else
        logInfo("Parameter " + name + " will not be stored in the result file, because the signal type is not supported");
    }
    else
    {
      if (var.isTypeReal())
      {
        unsigned int ID = resultFile.addSignal(name, description, SignalType_REAL);
        resultFileMapping[ID] = i;
      }
      else if (var.isTypeInteger())
      {
        unsigned int ID = resultFile.addSignal(name, description, SignalType_INT);
        resultFileMapping[ID] = i;
      }
      else if (var.isTypeBoolean())
      {
        unsigned int ID = resultFile.addSignal(name, description, SignalType_BOOL);
        resultFileMapping[ID] = i;
      }
      else
        logInfo("Variable " + name + " will not be stored in the result file, because the signal type is not supported");
    }
  }

  return oms_status_ok;
}

oms_status_enu_t oms::ComponentFMU3CS::updateSignals(ResultWriter& resultWriter)
{
  CallClock callClock(clock);

  if (clock_id)
  {
    SignalValue_t wallTime;
    wallTime.realValue = clock.getElapsedWallTime();
    resultWriter.updateSignal(clock_id, wallTime);
  }

  for (auto const &it : resultFileMapping)
  {
    unsigned int ID = it.first;
    Variable& var = allVariables[it.second];
    fmi3ValueReference vr = var.getValueReferenceFMI3();
    SignalValue_t value;
    if (var.isTypeReal())
    {
      if (oms_status_ok != getReal(vr, value.realValue, var.getNumericType()))
        return logError("failed to fetch variable " + std::string(var.getCref()));
      resultWriter.updateSignal(ID, value);
    }
    else if (var.isTypeInteger())
    {
      if (oms_status_ok != getInteger(vr, value.intValue, var.getNumericType()))
        return logError("failed to fetch variable " + std::string(var.getCref()));
      resultWriter.updateSignal(ID, value);
    }
    else if (var.isTypeBoolean())
    {
      if (oms_status_ok != getBoolean(vr, value.boolValue))
        return logError("failed to fetch variable " + std::string(var.getCref()));
      resultWriter.updateSignal(ID, value);
    }
  }

  return oms_status_ok;
}

oms_status_enu_t oms::ComponentFMU3CS::addSignalsToResults(const char* regex)
{
  std::regex exp(regex);
  for (unsigned int i=0; i<allVariables.size(); ++i)
  {
    if (exportVariables[i])
      continue;

    auto const &var = allVariables[i];
    if(regex_match(std::string(getFullCref() + var.getCref()), exp))
    {
      //logInfo("added \"" + std::string(getFullCref() + var.getCref()) + "\" to results");
      exportVariables[i] = true;
    }
  }

  return oms_status_ok;
}

oms_status_enu_t oms::ComponentFMU3CS::removeSignalsFromResults(const char* regex)
{
  std::regex exp(regex);
  for (unsigned int i=0; i<allVariables.size(); ++i)
  {
    if (!exportVariables[i])
      continue;

    auto const &var = allVariables[i];
    if(regex_match(std::string(getFullCref() + var.getCref()), exp))
    {
      //logInfo("removed \"" + std::string(getFullCref() + var.getCref()) + "\" from results");
      exportVariables[i] = false;
    }
  }

  return oms_status_ok;
}

oms_status_enu_t oms::ComponentFMU3CS::saveState()
{
  fmi3Status fmistatus = fmi3_getFMUState(fmu, &fmuState);
  if (fmi3OK != fmistatus) return logError_FMUCall("fmi3_getFMUState", this);
  fmuStateTime = time;

  return oms_status_ok;
}

oms_status_enu_t oms::ComponentFMU3CS::freeState()
{
  if (!fmuState)
    return oms_status_warning;

  fmi3Status fmistatus = fmi3_freeFMUState(fmu, &fmuState);
  fmuState = NULL;
  if (fmi3OK != fmistatus) return logError_FMUCall("fmi3_freeFMUstate", this);

  return oms_status_ok;
}

oms_status_enu_t oms::ComponentFMU3CS::restoreState()
{
  fmi3Status fmistatus = fmi3_setFMUState(fmu, fmuState);
  if (fmi3OK != fmistatus) return logError_FMUCall("fmi3_setFMUstate", this);
  time = fmuStateTime;

  return oms_status_ok;
}

void oms::ComponentFMU3CS::getFilteredSignals(std::vector<Connector>& filteredSignals) const
{
  for (unsigned int i = 0; i < allVariables.size(); ++i)
  {
    if (exportVariables[i])
      filteredSignals.push_back(allVariables[i].makeConnector(this->getFullCref()));
  }
}

oms_status_enu_t oms::ComponentFMU3CS::renameValues(const ComRef& oldCref, const ComRef& newCref)
{
  // check for local resources
  if (values.hasResources())
  {
    return values.renameInResources(oldCref, newCref);
  }
  // check for resources in root
  else if (getParentSystem() && getParentSystem()->getValues().hasResources())
  {
    return getParentSystem()->getValues().renameInResources(oldCref, newCref);
  }
  // check for resources in top level root
  else if (getParentSystem()->getParentSystem() && getParentSystem()->getParentSystem()->getValues().hasResources())
  {
    return getParentSystem()->getParentSystem()->getValues().renameInResources(oldCref, newCref);
  }
  else
  {
    return values.rename(oldCref, newCref);
  }

  return oms_status_error;
}
