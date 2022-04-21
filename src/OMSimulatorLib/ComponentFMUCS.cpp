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

#include "ComponentFMUCS.h"

#include "Flags.h"
#include "Logging.h"
#include "Model.h"
#include "OMSFileSystem.h"
#include "ssd/Tags.h"
#include "System.h"
#include "SystemTLM.h"
#include "SystemWC.h"

#include <fmilib.h>
#include <JM/jm_portability.h>
#include <RegEx.h>
#include <unordered_set>
#include <cmath>


oms::ComponentFMUCS::ComponentFMUCS(const ComRef& cref, System* parentSystem, const std::string& fmuPath)
  : oms::Component(cref, oms_component_fmu, parentSystem, fmuPath), fmuInfo(fmuPath)
{
}

oms::ComponentFMUCS::~ComponentFMUCS()
{
  fmi2_import_free_instance(fmu);
  fmi2_import_destroy_dllfmu(fmu);
  fmi2_import_free(fmu);
  fmi_import_free_context(context);
}

oms::Component* oms::ComponentFMUCS::NewComponent(const oms::ComRef& cref, oms::System* parentSystem, const std::string& fmuPath)
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

  filesystem::path temp_root(parentSystem->getModel().getTempDirectory());
  filesystem::path temp_temp = temp_root / "temp";
  filesystem::path relFMUPath = parentSystem->copyResources() ? (filesystem::path("resources") / (parentSystem->getUniqueID() + "_" + std::string(cref) + ".fmu")) : filesystem::path(fmuPath);
  filesystem::path absFMUPath = temp_root / relFMUPath;

  ComponentFMUCS* component = new ComponentFMUCS(cref, parentSystem, relFMUPath.generic_string());

  component->callbacks.malloc = malloc;
  component->callbacks.calloc = calloc;
  component->callbacks.realloc = realloc;
  component->callbacks.free = free;
  component->callbacks.logger = oms::fmiLogger;
  component->callbacks.log_level = jm_log_level_all;
  component->callbacks.context = 0;

  // Copy the resource to the temp directory of the model? We don't want have
  // to copy resources if importing an SSP file or snapshot.
  if (parentSystem->copyResources())
    oms_copy_file(filesystem::path(fmuPath), absFMUPath);

  // set temp directory
  filesystem::path tempDir = temp_temp / relFMUPath.stem();
  component->setTempDir(tempDir.string());
  if (!filesystem::is_directory(tempDir) && !filesystem::create_directory(tempDir))
  {
    logError("Creating temp directory for component \"" + std::string(cref) + "\" failed");
    return NULL;
  }

  component->context = fmi_import_allocate_context(&component->callbacks);

  // check version of FMU
  fmi_version_enu_t version = fmi_import_get_fmi_version(component->context, absFMUPath.string().c_str(), component->getTempDir().c_str());
  if (fmi_version_2_0_enu != version)
  {
    logError("Unsupported FMI version: " + std::string(fmi_version_to_string(version)));
    delete component;
    return NULL;
  }

  // parse modelDescription.xml
  component->fmu = fmi2_import_parse_xml(component->context, component->getTempDir().c_str(), 0);
  if (!component->fmu)
  {
    logError("Error parsing modelDescription.xml");
    delete component;
    return NULL;
  }

  fmi2_fmu_kind_enu_t fmuKind = fmi2_import_get_fmu_kind(component->fmu);
  if (!(fmi2_fmu_kind_cs == fmuKind || fmi2_fmu_kind_me_and_cs == fmuKind))
  {
    logError("FMU \"" + std::string(cref) + "\" doesn't support co-simulation mode.");
    delete component;
    return NULL;
  }

  // update FMU info
  component->fmuInfo.update(version, component->fmu);

  component->callbackFunctions.logger = oms::fmi2logger;
  component->callbackFunctions.allocateMemory = calloc;
  component->callbackFunctions.freeMemory = free;
  component->callbackFunctions.componentEnvironment = component->fmu;
  component->callbackFunctions.stepFinished = NULL;

  // create a list of all variables
  fmi2_import_variable_list_t *varList = fmi2_import_get_variable_list(component->fmu, 0);
  size_t varListSize = fmi2_import_get_variable_list_size(varList);
  component->allVariables.reserve(varListSize);
  component->exportVariables.reserve(varListSize);
  for (size_t i = 0; i < varListSize; ++i)
  {
    fmi2_import_variable_t* var = fmi2_import_get_variable(varList, i);
    oms::Variable v(var);
    if (v.getIndex() != i)
    {
      logError("Index mismatch " + std::to_string(v.getIndex()) + " != " + std::to_string(i) + ".\nPlease report the problem to the dev team: https://github.com/OpenModelica/OMSimulator/issues/new?assignees=&labels=&template=bug_report.md");
      fmi2_import_free_variable_list(varList);
      delete component;
      return NULL;
    }
    component->allVariables.push_back(v);
    component->exportVariables.push_back(true);
  }
  fmi2_import_free_variable_list(varList);

  // extract continuous-time derivatives
  varList = fmi2_import_get_derivatives_list(component->fmu);
  varListSize = fmi2_import_get_variable_list_size(varList);
  for (size_t i = 0; i < varListSize; ++i)
  {
    fmi2_import_variable_t* var = fmi2_import_get_variable(varList, i);
    size_t originalIndex = fmi2_import_get_variable_original_order(var);
    component->allVariables[originalIndex].markAsContinuousTimeDer();
    component->derivatives.push_back(originalIndex);
  }
  fmi2_import_free_variable_list(varList);

  // mark states and continuous-time states
  for (size_t i = 0; i < component->allVariables.size(); ++i)
  {
    if (component->allVariables[i].isContinuousTimeDer())
      component->allVariables[component->allVariables[i].getStateIndex()].markAsContinuousTimeState(i);
    else if (component->allVariables[i].isDer())
      component->allVariables[component->allVariables[i].getStateIndex()].markAsState(i);
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

  // parse modelDescription.xml to get start values before instantiating fmu's
  component->values.parseModelDescription(tempDir);

  // set units to connector
  for (auto &connector : component->connectors)
  {
    if (connector)
    {
      oms::ComRef connectorCref = connector->getName();
      std::string unitName = component->values.getUnitFromModeldescription(connectorCref);
      if (!unitName.empty())
        connector->connectorUnits[unitName] = component->values.modeldescriptionUnitDefinitions[unitName];
    }
  }
  return component;
}

oms::Component* oms::ComponentFMUCS::NewComponent(const pugi::xml_node& node, oms::System* parentSystem, const std::string& sspVersion, const Snapshot& snapshot)
{
  ComRef cref = ComRef(node.attribute("name").as_string());
  std::string type = node.attribute("type").as_string();
  std::string source = node.attribute("source").as_string();

  if (type != "application/x-fmu-sharedlibrary" && !type.empty())
  {
    logError("Unexpected component type: " + type);
    return NULL;
  }

  oms::ComponentFMUCS* component = dynamic_cast<oms::ComponentFMUCS*>(oms::ComponentFMUCS::NewComponent(cref, parentSystem, source));
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
      pugi::xml_node ssdNode = snapshot.getResourceNode("SystemStructure.ssd");
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
      resources.importFromSnapshot(*it, sspVersion, snapshot);
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

oms_status_enu_t oms::ComponentFMUCS::exportToSSD(pugi::xml_node& node, Snapshot& snapshot) const
{
#if !defined(NO_TLM)
  if (tlmbusconnectors[0])
  {
    pugi::xml_node annotations_node = node.append_child(oms::ssp::Draft20180219::ssd::annotations);
    pugi::xml_node annotation_node = annotations_node.append_child(oms::ssp::Version1_0::ssc::annotation);
    annotation_node.append_attribute("type") = oms::ssp::Draft20180219::annotation_type;
    for (const auto& tlmbusconnector : tlmbusconnectors)
      if (tlmbusconnector)
        tlmbusconnector->exportToSSD(annotation_node);
  }
#endif

  node.append_attribute("name") = this->getCref().c_str();
  node.append_attribute("type") = "application/x-fmu-sharedlibrary";
  node.append_attribute("source") = getPath().c_str();
  pugi::xml_node node_connectors = node.append_child(oms::ssp::Draft20180219::ssd::connectors);

  if (element.getGeometry())
    element.getGeometry()->exportToSSD(node);

  for (const auto& connector : connectors)
    if (connector)
      if (oms_status_ok != connector->exportToSSD(node_connectors))
        return oms_status_error;

  // export ParameterBindings at component level
  values.exportParameterBindings(node, snapshot);

  return oms_status_ok;
}

void oms::ComponentFMUCS::getFilteredUnitDefinitionsToSSD(std::map<std::string, std::map<std::string, std::string>>& unitDefinitions)
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

oms_status_enu_t oms::ComponentFMUCS::exportToSSV(pugi::xml_node& ssvNode)
{
  return values.exportToSSV(ssvNode);
}

oms_status_enu_t oms::ComponentFMUCS::exportToSSVTemplate(pugi::xml_node& ssvNode, Snapshot& snapshot)
{
  values.exportToSSVTemplate(ssvNode, getCref());
  values.exportUnitDefinitionsToSSVTemplate(snapshot, "template.ssv");
  return oms_status_ok;
}

oms_status_enu_t oms::ComponentFMUCS::exportToSSMTemplate(pugi::xml_node& ssmNode)
{
  values.exportToSSMTemplate(ssmNode, getCref());
  return oms_status_ok;
}

void oms::ComponentFMUCS::dumpInitialUnknowns()
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

oms_status_enu_t oms::ComponentFMUCS::initializeDependencyGraph_initialUnknowns()
{
  if (initialUnknownsGraph.getEdges().connections.size() > 0)
    return logError(std::string(getCref()) + ": " + getPath() + " is already initialized");

  //dumpInitialUnknowns();

  // Check if initial unknowns from modelDescription.xml are the same as in initialUnknownsGraph
  fmi2_import_variable_list_t* initialUnknowns_ModelStructure;
  initialUnknowns_ModelStructure = fmi2_import_get_initial_unknowns_list(fmu);
  size_t N_ModelStructure = fmi2_import_get_variable_list_size(initialUnknowns_ModelStructure);
  size_t N = initialUnknownsGraph.getNodes().size();

  bool badInitialUnknowns = false;

  std::unordered_set <unsigned int> setA_ModelStructure;
  for (size_t i=0; i < N_ModelStructure; ++i)
  {
    fmi2_xml_variable_t* var_ModelStructure = fmi2_import_get_variable(initialUnknowns_ModelStructure, i);
    size_t originalIndex = fmi2_import_get_variable_original_order(var_ModelStructure);
    setA_ModelStructure.insert(originalIndex);
  }

  std::string missing_unknowns = "";
  for (auto &v : allVariables)
  {
    if (v.isInitialUnknown() && setA_ModelStructure.find(v.getIndex()) == setA_ModelStructure.end())
    {
      badInitialUnknowns = true;
      missing_unknowns += "\n  * " + std::to_string(v.getIndex()+1) + ": " + std::string(v.getCref()) + " is missing";
    }
  }

  for (size_t i=0; i < N_ModelStructure; ++i)
  {
    fmi2_xml_variable_t* var_ModelStructure = fmi2_import_get_variable(initialUnknowns_ModelStructure, i);
    size_t originalIndex = fmi2_import_get_variable_original_order(var_ModelStructure);
    const Variable& var_oms = allVariables[originalIndex];
    oms::ComRef name_fmilib(fmi2_import_get_variable_name(var_ModelStructure));

    if (var_oms.getCref() != name_fmilib)
    {
      missing_unknowns += "\n  * " + std::to_string(originalIndex+1) + ": " + std::string(var_oms.getCref()) + " could not be found";
      badInitialUnknowns = true;
    }
    else if (!var_oms.isInitialUnknown())
    {
      missing_unknowns += "\n  * " + std::to_string(originalIndex+1) + ": " + std::string(var_oms.getCref()) + " is wrongly listed";
      badInitialUnknowns = true;
    }
  }

  if (badInitialUnknowns && !Flags::IgnoreInitialUnknowns() && N_ModelStructure > 0)
    logWarning("[" + std::string(getCref()) + ": " + getPath() + "] The FMU lists " + std::to_string(N_ModelStructure) + " initial unknowns and exposes " + std::to_string(N) + " initial unknowns." + missing_unknowns);

  fmi2_import_free_variable_list(initialUnknowns_ModelStructure);

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

  size_t *startIndex=NULL, *dependency=NULL;
  char* factorKind;

  fmi2_import_get_initial_unknowns_dependencies(fmu, &startIndex, &dependency, &factorKind);

  if (!startIndex)
  {
    logDebug(std::string(getCref()) + ": " + getPath() + " no dependencies");
    return oms_status_ok;
  }

  //logInfo(std::string(getCref()) + ": " + getPath());
  //std::string buffer = "startIndex(" + std::to_string(N+1) + ")\n";
  //for (int i=0; i < N; i++)
  //  buffer += "startIndex[" + std::to_string(i) + "] (" + std::string(initialUnknownsGraph.getNodes()[i]) + ") = " + std::to_string(startIndex[i]) + "\n";
  //buffer += "startIndex[" + std::to_string(N) + "] = " + std::to_string(startIndex[N]) + "\n";
  //logInfo(buffer);

  //buffer = "dependency(" + std::to_string(startIndex[N]) + ")\n";
  //for (int i=0; i < startIndex[N]; i++)
  //  buffer += "dependency[" + std::to_string(i) + "] = " + std::to_string(dependency[i]) + "\n";
  //logInfo(buffer);

  //buffer = "factorKind(" + std::to_string(startIndex[N]) + ")\n";
  //for (int i=0; i < startIndex[N]; i++)
  //  buffer += "factorKind[" + std::to_string(i) + "] = " + std::string(fmi2_dependency_factor_kind_to_string((fmi2_dependency_factor_kind_enu_t)factorKind[i])) + "\n";
  //logInfo(buffer);

  for (int i=0; i < N_ModelStructure; i++)
  {
    if (startIndex[i] == startIndex[i+1])
    {
      logDebug(std::string(getCref()) + ": " + getPath() + " initial unknown " + std::string(initialUnknownsGraph.getNodes()[i]) + " has no dependencies");
    }
    else if ((startIndex[i] + 1 == startIndex[i+1]) && (dependency[startIndex[i]] == 0))
    {
      logDebug(std::string(getCref()) + ": " + getPath() + " initial unknown " + std::string(initialUnknownsGraph.getNodes()[i]) + " depends on all inputs");
      for (const auto& j : inputs)
        initialUnknownsGraph.addEdge(allVariables[j].makeConnector(this->getFullCref()), initialUnknownsGraph.getNodes()[i]);
    }
    else
    {
      for (size_t j=startIndex[i]; j < startIndex[i+1]; j++)
      {
        if (dependency[j] < 1 || dependency[j] > allVariables.size())
        {
          logWarning("Initial unknown " + std::string(initialUnknownsGraph.getNodes()[i]) + " has bad dependency on variable with index " + std::to_string(dependency[j]) + " which couldn't be resolved");
          return logError(std::string(getCref()) + ": Erroneous initial unknowns detected in modelDescription.xml\nUse flag --ignoreInitialUnknowns=true to ignore all initial unknowns, but this can cause inflated loop size.");
        }
        logDebug(std::string(getCref()) + ": " + getPath() + " initial unknown " + std::string(initialUnknownsGraph.getNodes()[i]) + " depends on " + std::string(allVariables[dependency[j] - 1]));
        initialUnknownsGraph.addEdge(allVariables[dependency[j] - 1].makeConnector(this->getFullCref()), initialUnknownsGraph.getNodes()[i]);
      }
    }
  }

  return oms_status_ok;
}

oms_status_enu_t oms::ComponentFMUCS::initializeDependencyGraph_outputs()
{
  if (outputsGraph.getEdges().connections.size() > 0)
  {
    logError(std::string(getCref()) + ": " + getPath() + " is already initialized.");
    return oms_status_error;
  }

  size_t *startIndex=NULL, *dependency=NULL;
  char* factorKind;

  fmi2_import_get_outputs_dependencies(fmu, &startIndex, &dependency, &factorKind);

  if (!startIndex)
  {
    logDebug(std::string(getCref()) + ": " + getPath() + " no dependencies");
    return oms_status_ok;
  }

  for (int i=0; i < outputs.size(); i++)
  {
    Variable& output = allVariables[outputs[i]];
    if (startIndex[i] == startIndex[i + 1])
    {
      logDebug(std::string(getCref()) + ": " + getPath() + " output " + std::string(output) + " has no dependencies");
    }
    else if ((startIndex[i] + 1 == startIndex[i + 1]) && (dependency[startIndex[i]] == 0))
    {
      logDebug(std::string(getCref()) + ": " + getPath() + " output " + std::string(output) + " depends on all");
      for (const auto& j : inputs)
        outputsGraph.addEdge(allVariables[j].makeConnector(this->getFullCref()), output.makeConnector(this->getFullCref()));
    }
    else
    {
      for (size_t j = startIndex[i]; j < startIndex[i + 1]; j++)
      {
        if (dependency[j] < 1 || dependency[j] > allVariables.size())
        {
          logWarning("Output " + std::string(output) + " has bad dependency on variable with index " + std::to_string(dependency[j]) + " which couldn't be resolved");
          return logError(std::string(getCref()) + ": erroneous dependencies detected in modelDescription.xml");
        }
        logDebug(std::string(getCref()) + ": " + getPath() + " output " + std::string(output) + " depends on " + std::string(allVariables[dependency[j] - 1]));
        outputsGraph.addEdge(allVariables[dependency[j] - 1].makeConnector(this->getFullCref()), output.makeConnector(this->getFullCref()));
      }
    }
  }

  return oms_status_ok;
}

oms_status_enu_t oms::ComponentFMUCS::instantiate()
{
  jm_status_enu_t jmstatus;
  fmi2_status_t fmistatus;

  // load the FMU shared library
  jmstatus = fmi2_import_create_dllfmu(fmu, fmi2_fmu_kind_cs, &callbackFunctions);
  if (jm_status_error == jmstatus)
    return logError("Could not load \"" + getPath() + "\" which is associated with \"" + std::string(getFullCref()) + "\"; it may be corrupted or may not support your platform");

  jmstatus = fmi2_import_instantiate(fmu, getCref().c_str(), fmi2_cosimulation, NULL, fmi2_false);
  if (jm_status_error == jmstatus)
    return logError_FMUCall("fmi2_import_instantiate", this);

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
  dynamic_cast<SystemWC*>(getParentSystem())->getTolerance(NULL, &relativeTolerance);
  fmistatus = fmi2_import_setup_experiment(fmu, fmi2_true, relativeTolerance, time, fmi2_false, 1.0);
  if (fmi2_status_ok != fmistatus) return logError_FMUCall("fmi2_import_setup_experiment", this);

  fmistatus = fmi2_import_enter_initialization_mode(fmu);
  if (fmi2_status_ok != fmistatus) return logError_FMUCall("fmi2_import_enter_initialization_mode", this);

  return oms_status_ok;
}

oms_status_enu_t oms::ComponentFMUCS::setResourcesHelper1(Values values)
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

oms_status_enu_t oms::ComponentFMUCS::setResourcesHelper2(Values values)
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
oms::ComRef oms::ComponentFMUCS::getValidCref(ComRef cref)
{
  oms::ComRef tail(cref);
  oms::ComRef head = tail.pop_front();

  if (tail.isEmpty() || head != getCref()) // check for inline parameter crefs, (e.g.) P => P or C.mChassis => C.mChassis
    tail = cref;
  return tail;
}

oms_status_enu_t oms::ComponentFMUCS::newResources(const std::string& ssvFilename, const std::string& ssmFilename, bool externalResources)
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
    values.parameterResources[0].allresources["resources/" + ssvFilename] = resources;
  }

  return oms_status_ok;
}

oms_status_enu_t oms::ComponentFMUCS::deleteReferencesInSSD(const std::string& filename)
{
  if (values.hasResources())
    return values.deleteReferencesInSSD(filename);

  return oms_status_error;
}

oms_status_enu_t oms::ComponentFMUCS::deleteResourcesInSSP(const std::string& filename)
{
  if (values.hasResources())
    return values.deleteResourcesInSSP(filename);

  return oms_status_error;
}

oms_status_enu_t oms::ComponentFMUCS::initialize()
{
  clock.reset();
  CallClock callClock(clock);

  fmi2_status_t fmistatus;

  // exitInitialization
  fmistatus = fmi2_import_exit_initialization_mode(fmu);
  if (fmi2_status_ok != fmistatus) return logError_FMUCall("fmi2_import_exit_initialization_mode", this);

  return oms_status_ok;
}

oms_status_enu_t oms::ComponentFMUCS::terminate()
{
  freeState();

  fmi2_status_t fmistatus = fmi2_import_terminate(fmu);
  if (fmi2_status_ok != fmistatus)
    return logError_Termination(getCref());

  fmi2_import_free_instance(fmu);
  fmi2_import_destroy_dllfmu(fmu);
  return oms_status_ok;
}

oms_status_enu_t oms::ComponentFMUCS::reset()
{
  fmi2_status_t fmistatus = fmi2_import_reset(fmu);
  if (fmi2_status_ok != fmistatus)
    return logError_ResetFailed(getCref());

  // enterInitialization
  time = getModel().getStartTime();
  double relativeTolerance = 0.0;
  dynamic_cast<SystemWC*>(getParentSystem())->getTolerance(NULL, &relativeTolerance);
  fmistatus = fmi2_import_setup_experiment(fmu, fmi2_true, relativeTolerance, time, fmi2_false, 1.0);
  if (fmi2_status_ok != fmistatus) return logError_FMUCall("fmi2_import_setup_experiment", this);

  fmistatus = fmi2_import_enter_initialization_mode(fmu);
  if (fmi2_status_ok != fmistatus) return logError_FMUCall("fmi2_import_enter_initialization_mode", this);

  return oms_status_ok;
}

oms_status_enu_t oms::ComponentFMUCS::stepUntil(double stopTime)
{
  CallClock callClock(clock);
  System *topLevelSystem = getModel().getTopLevelSystem();

  fmi2_status_t fmistatus;
  double hdef = (stopTime-time) / 1.0;

  while (time < stopTime)
  {
#if !defined(NO_TLM)
    //Read from TLM sockets if top level system is of TLM type
    if(topLevelSystem->getType() == oms_system_tlm)
      reinterpret_cast<SystemTLM*>(topLevelSystem)->readFromSockets(reinterpret_cast<SystemWC*>(getParentSystem()), time, this);
#endif

    // HACK for certain FMUs
    if (fetchAllVars_)
    {
      for (auto &v : allVariables)
      {
        if (v.isTypeReal())
        {
          double realValue;
          if (oms_status_ok != getReal(v.getCref(), realValue))
            logError("failed to fetch variable " + std::string(v));
        }
      }
    }

    fmistatus = fmi2_import_do_step(fmu, time, hdef, fmi2_true);
    time += hdef;

#if !defined(NO_TLM)
    //Write to TLM sockets if top level system is of TLM type
    if(topLevelSystem->getType() == oms_system_tlm)
      reinterpret_cast<SystemTLM*>(topLevelSystem)->writeToSockets(reinterpret_cast<SystemWC*>(getParentSystem()), time, this);
#endif
  }
  time = stopTime;
  return oms_status_ok;
}

oms_status_enu_t oms::ComponentFMUCS::getBoolean(const fmi2_value_reference_t& vr, bool& value)
{
  CallClock callClock(clock);

  int value_;
  if (fmi2_status_ok != fmi2_import_get_boolean(fmu, &vr, 1, &value_))
    return oms_status_error;

  value = value_ ? true : false;
  return oms_status_ok;
}

oms_status_enu_t oms::ComponentFMUCS::getBoolean(const ComRef& cref, bool& value)
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

  fmi2_value_reference_t vr = allVariables[j].getValueReference();
  return getBoolean(vr, value);
}

oms_status_enu_t oms::ComponentFMUCS::getInteger(const fmi2_value_reference_t& vr, int& value)
{
  CallClock callClock(clock);

  if (fmi2_status_ok != fmi2_import_get_integer(fmu, &vr, 1, &value))
    return oms_status_error;

  return oms_status_ok;
}

oms_status_enu_t oms::ComponentFMUCS::getInteger(const ComRef& cref, int& value)
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

  fmi2_value_reference_t vr = allVariables[j].getValueReference();
  return getInteger(vr, value);
}

oms::Variable* oms::ComponentFMUCS::getVariable(const ComRef& cref)
{
  CallClock callClock(clock);
  for (size_t i=0; i < allVariables.size(); i++)
    if (allVariables[i].getCref() == cref)
      return &allVariables[i];

  logError_UnknownSignal(getFullCref() + cref);
  return NULL;
}

oms_status_enu_t oms::ComponentFMUCS::getReal(const fmi2_value_reference_t& vr, double& value)
{
  CallClock callClock(clock);

  if (fmi2_status_ok != fmi2_import_get_real(fmu, &vr, 1, &value))
    return oms_status_error;

  if (std::isnan(value))
    return logError("getReal returned NAN");
  if (std::isinf(value))
    return logError("getReal returned +/-inf");

  // Check for FIB
  auto block = fib.find(vr);
  if (block != fib.end())
  {
    switch(block->second.faultType)
    {
      case oms_fault_type_bias:   // y = y.$original + faultValue
        value += block->second.faultValue;
        break;

      case oms_fault_type_gain:   // y = y.$original * faultValue
        value *= block->second.faultValue;
        break;

      case oms_fault_type_const:  // y = faultValue
        value = block->second.faultValue;
        break;

      default:
        return logError("Unhandled fault injection block");
    }
  }

  return oms_status_ok;
}

oms_status_enu_t oms::ComponentFMUCS::getReal(const ComRef& cref, double& value)
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

  fmi2_value_reference_t vr = allVariables[j].getValueReference();
  return getReal(vr, value);
}

oms_status_enu_t oms::ComponentFMUCS::getString(const fmi2_value_reference_t& vr, std::string& value)
{
  CallClock callClock(clock);
  fmi2_string_t str;
  if (fmi2_status_ok != fmi2_import_get_string(fmu, &vr, 1, &str))
    return oms_status_error;

  value = std::string(str);

  return oms_status_ok;
}

oms_status_enu_t oms::ComponentFMUCS::getString(const ComRef& cref, std::string& value)
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

  fmi2_value_reference_t vr = allVariables[j].getValueReference();
  return getString(vr, value);
}

oms_status_enu_t oms::ComponentFMUCS::getDirectionalDerivative(const ComRef& unknownCref, const ComRef& knownCref, double& value)
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

  // fmi2_value_reference_t vr_unknown[1] = {5};
  // fmi2_value_reference_t vr_known[4] = {0, 2, 3, 4};
  // fmi2_real_t dvknown[4] = {1.0, 1.0, 1.0, 1.0};
  // fmi2_real_t val;
  // std::cout << "Get directional derivative_static_1: " << val << std::endl;
  // fmi2_import_get_directional_derivative(fmu, vr_unknown, 1, vr_known, 4, dvknown, &val);
  // std::cout << "\nGet directional derivative_static_2: " << val << std::endl;

  return oms_status_ok;
}

oms_status_enu_t oms::ComponentFMUCS::getDirectionalDerivativeHeper(const int unknownIndex, const int knownIndex, const std::vector<int> &dependencyList, double &value)
{
  fmi2_value_reference_t vr_unknown = allVariables[unknownIndex].getValueReference();
  fmi2_value_reference_t *vr_known = (fmi2_value_reference_t *)calloc(dependencyList.size(), sizeof(fmi2_value_reference_t *));
  fmi2_real_t *dvknown = (fmi2_real_t *)calloc(dependencyList.size(), sizeof(fmi2_real_t *));

  for (int i = 0; i < dependencyList.size(); i++)
  {
    vr_known[i] = allVariables[dependencyList[i] - 1].getValueReference();

    // The knownIndex is < 0 if not specified. In this case, we
    // calculate the sum of the row, which means we set all seed
    // values to 1.0. Otherwise we just set the explicitly provided
    // element to 1.0.
    if (knownIndex < 0 || (dependencyList[i] == knownIndex + 1))
      dvknown[i] = 1.0;
    else
      dvknown[i] = 0.0;
  }

  fmi2_import_get_directional_derivative(fmu, &vr_unknown, 1, vr_known, dependencyList.size(), dvknown, &value);

  free(vr_known);
  free(dvknown);

  return oms_status_ok;
}

oms_status_enu_t oms::ComponentFMUCS::getRealOutputDerivative(const ComRef& cref, SignalDerivative& der)
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

  der = SignalDerivative(getFMUInfo()->getMaxOutputDerivativeOrder(), fmu, allVariables[j].getValueReference());
  return oms_status_ok;
}

oms_status_enu_t oms::ComponentFMUCS::setRealInputDerivative(const ComRef& cref, const SignalDerivative& der)
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

  fmi2_value_reference_t vr = allVariables[j].getValueReference();
  return der.setRealInputDerivatives(fmu, vr);
}

oms_status_enu_t oms::ComponentFMUCS::setBoolean(const ComRef& cref, bool value)
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
    fmi2_value_reference_t vr = allVariables[j].getValueReference();
    int value_ = value ? 1 : 0;
    if (fmi2_status_ok != fmi2_import_set_boolean(fmu, &vr, 1, &value_))
      return oms_status_error;
  }

  return oms_status_ok;
}

oms_status_enu_t oms::ComponentFMUCS::setInteger(const ComRef& cref, int value)
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
    fmi2_value_reference_t vr = allVariables[j].getValueReference();
    if (fmi2_status_ok != fmi2_import_set_integer(fmu, &vr, 1, &value))
      return oms_status_error;
  }

  return oms_status_ok;
}

oms_status_enu_t oms::ComponentFMUCS::setReal(const ComRef& cref, double value)
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
    // check for local resources available
    if (values.hasResources())
    {
      return values.setRealResources(cref, value, getFullCref(), false, oms_modelState_virgin);
    }
    // check for resources in root
    else if (getParentSystem() && getParentSystem()->getValues().hasResources())
    {
      return getParentSystem()->getValues().setRealResources(getCref()+cref, value, getParentSystem()->getFullCref(), false, oms_modelState_virgin);
    }
    // check for resources in top level root
    else if (getParentSystem()->getParentSystem() && getParentSystem()->getParentSystem()->getValues().hasResources())
    {
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
    fmi2_value_reference_t vr = allVariables[j].getValueReference();
    if (fmi2_status_ok != fmi2_import_set_real(fmu, &vr, 1, &value))
      return oms_status_error;
  }

  return oms_status_ok;
}

oms_status_enu_t oms::ComponentFMUCS::setString(const ComRef& cref, const std::string& value)
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
    fmi2_value_reference_t vr = allVariables[j].getValueReference();
    fmi2_string_t value_ = value.c_str();
    if (fmi2_status_ok != fmi2_import_set_string(fmu, &vr, 1, &value_))
      return oms_status_error;
  }

  return oms_status_ok;
}

oms_status_enu_t oms::ComponentFMUCS::setUnit(const ComRef &cref, const std::string &value)
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

oms_status_enu_t oms::ComponentFMUCS::deleteStartValue(const ComRef& cref)
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

oms_status_enu_t oms::ComponentFMUCS::registerSignalsForResultFile(ResultWriter& resultFile)
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

oms_status_enu_t oms::ComponentFMUCS::updateSignals(ResultWriter& resultWriter)
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
    fmi2_value_reference_t vr = var.getValueReference();
    SignalValue_t value;
    if (var.isTypeReal())
    {
        if (oms_status_ok != getReal(vr, value.realValue))
        return logError("failed to fetch variable " + std::string(var.getCref()));
      resultWriter.updateSignal(ID, value);
    }
    else if (var.isTypeInteger())
    {
      if (oms_status_ok != getInteger(vr, value.intValue))
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

oms_status_enu_t oms::ComponentFMUCS::addSignalsToResults(const char* regex)
{
  oms_regex exp(regex);
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

oms_status_enu_t oms::ComponentFMUCS::removeSignalsFromResults(const char* regex)
{
  oms_regex exp(regex);
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

oms_status_enu_t oms::ComponentFMUCS::saveState()
{
  fmi2_status_t fmistatus = fmi2_import_get_fmu_state(fmu, &fmuState);
  if (fmi2_status_ok != fmistatus) return logError_FMUCall("fmi2_import_get_fmu_state", this);
  fmuStateTime = time;

  return oms_status_ok;
}

oms_status_enu_t oms::ComponentFMUCS::freeState()
{
  if (!fmuState)
    return oms_status_warning;

  fmi2_status_t fmistatus = fmi2_import_free_fmu_state(fmu, &fmuState);
  fmuState = NULL;
  if (fmi2_status_ok != fmistatus) return logError_FMUCall("fmi2_import_free_fmu_state", this);

  return oms_status_ok;
}

oms_status_enu_t oms::ComponentFMUCS::restoreState()
{
  fmi2_status_t fmistatus = fmi2_import_set_fmu_state(fmu, fmuState);
  if (fmi2_status_ok != fmistatus) return logError_FMUCall("fmi2_import_set_fmu_state", this);
  time = fmuStateTime;

  return oms_status_ok;
}

oms_status_enu_t oms::ComponentFMUCS::setFaultInjection(const oms::ComRef& signal, oms_fault_type_enu_t faultType, double faultValue)
{
  Variable* var = getVariable(signal);

  if (!var || !var->isOutput())
    return oms_status_error;

  // remove fib?
  if ((faultType == oms_fault_type_bias && faultValue == 0.0) ||
      (faultType == oms_fault_type_gain && faultValue == 1.0))
  {
    fib.erase(var->getValueReference());
  }
  else
  {
    oms_fault_type_t ft;
    ft.faultType = faultType;
    ft.faultValue = faultValue;
    fib[var->getValueReference()] = ft;
  }
  return oms_status_ok;
}

void oms::ComponentFMUCS::getFilteredSignals(std::vector<Connector>& filteredSignals) const
{
  for (unsigned int i = 0; i < allVariables.size(); ++i)
  {
    if (exportVariables[i])
      filteredSignals.push_back(allVariables[i].makeConnector(this->getFullCref()));
  }
}

oms_status_enu_t oms::ComponentFMUCS::renameValues(const ComRef& oldCref, const ComRef& newCref)
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
