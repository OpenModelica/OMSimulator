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
  logDebug(std::to_string(varListSize) + " variables");
  component->allVariables.reserve(varListSize);
  component->exportVariables.reserve(varListSize);
  for (size_t i = 0; i < varListSize; ++i)
  {
    fmi2_import_variable_t* var = fmi2_import_get_variable(varList, i);
    oms::Variable v(var, i + 1);
    component->allVariables.push_back(v);
    component->exportVariables.push_back(true);
  }
  fmi2_import_free_variable_list(varList);

  // mark states
  varList = fmi2_import_get_derivatives_list(component->fmu);
  varListSize = fmi2_import_get_variable_list_size(varList);
  logDebug(std::to_string(varListSize) + " states");
  for (size_t i = 0; i < varListSize; ++i)
  {
    fmi2_import_variable_t* var = fmi2_import_get_variable(varList, i);
    fmi2_import_real_variable_t* varReal = fmi2_import_get_variable_as_real(var);
    fmi2_import_variable_t* varState = (fmi2_import_variable_t*)fmi2_import_get_real_variable_derivative_of(varReal);
    if (varState)
    {
      fmi2_value_reference_t state_vr = fmi2_import_get_variable_vr(varState);
      bool found = false;
      for (size_t i = 0; i < component->allVariables.size(); i++)
      {
        if (state_vr == component->allVariables[i].getValueReference())
        {
          component->allVariables[i].markAsState();
          found = true;
          break;
        }
      }
      if (!found)
      {
        logError("Couldn't find " + std::string(fmi2_import_get_variable_name(varState)));
        fmi2_import_free_variable_list(varList);
        delete component;
        return NULL;
      }
    }
    else
    {
      logError("Couldn't map " + std::string(fmi2_import_get_variable_name(var)) + " to the corresponding state variable");
      fmi2_import_free_variable_list(varList);
      delete component;
      return NULL;
    }
  }
  fmi2_import_free_variable_list(varList);

  // create some special variable maps
  for (auto const& v : component->allVariables)
  {
    if (v.isInput())
      component->inputs.push_back(v);
    else if (v.isOutput())
    {
      component->outputs.push_back(v);
      component->outputsGraph.addNode(Connector(oms_causality_output, v.getType(), v.getCref(), component->getFullCref()));
    }
    else if (v.isParameter())
      component->parameters.push_back(v);
    else if (v.isCalculatedParameter())
      component->calculatedParameters.push_back(v);

    if (v.isInitialUnknown())
      component->initialUnknownsGraph.addNode(Connector(v.getCausality(), v.getType(), v.getCref(), component->getFullCref()));

    component->exportVariables.push_back(v.isInput() || v.isOutput());
  }

  // create connectors
  while (component->connectors.size() > 0 && NULL == component->connectors.back())
    component->connectors.pop_back();

  int i = 1;
  int size = 1 + component->inputs.size();
  for (const auto& v : component->inputs)
    component->connectors.push_back(new Connector(oms_causality_input, v.getType(), v.getCref(), component->getFullCref(), i++/(double)size));
  i = 1;
  size = 1 + component->outputs.size();
  for (const auto& v : component->outputs)
    component->connectors.push_back(new Connector(oms_causality_output, v.getType(), v.getCref(), component->getFullCref(), i++/(double)size));
  for (const auto& v : component->parameters)
    component->connectors.push_back(new Connector(oms_causality_parameter, v.getType(), v.getCref(), component->getFullCref()));
  for (const auto& v : component->calculatedParameters)
    component->connectors.push_back(new Connector(oms_causality_calculatedParameter, v.getType(), v.getCref(), component->getFullCref()));
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
      // import connectors
      for(pugi::xml_node_iterator itConnectors = (*it).begin(); itConnectors != (*it).end(); ++itConnectors)
      {
        component->connectors.push_back(oms::Connector::NewConnector(*itConnectors, sspVersion, component->getFullCref()));
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
      std::string tempdir = parentSystem->getModel().getTempDirectory();
      component->values.importFromSnapshot(*it, sspVersion, snapshot);
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

oms_status_enu_t oms::ComponentFMUCS::exportToSSD(pugi::xml_node& node, pugi::xml_node& ssvNode, Snapshot& snapshot) const
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
  if (Flags::ExportParametersInline()) // export as inline
  {
    values.exportToSSD(node);
  }
  else
  {
    values.exportToSSV(ssvNode); // export to ssv file
  }

  return oms_status_ok;
}

oms_status_enu_t oms::ComponentFMUCS::exportToSSVTemplate(pugi::xml_node& ssvNode)
{
  values.exportToSSVTemplate(ssvNode, getCref());
  return oms_status_ok;
}

oms_status_enu_t oms::ComponentFMUCS::exportToSSMTemplate(pugi::xml_node& ssmNode)
{
  values.exportToSSMTemplate(ssmNode, getCref());
  return oms_status_ok;
}

oms_status_enu_t oms::ComponentFMUCS::initializeDependencyGraph_initialUnknowns()
{
  if (initialUnknownsGraph.getEdges().size() > 0)
  {
    logError(std::string(getCref()) + ": " + getPath() + " is already initialized");
    return oms_status_error;
  }

  int N=initialUnknownsGraph.getNodes().size();

  if (Flags::IgnoreInitialUnknowns())
  {
    for (int i = 0; i < N; i++)
    {
      logDebug(std::string(getCref()) + ": " + getPath() + " initial unknown " + std::string(initialUnknownsGraph.getNodes()[i]) + " depends on all");
      for (int j = 0; j < inputs.size(); j++)
        initialUnknownsGraph.addEdge(inputs[j].makeConnector(this->getFullCref()), initialUnknownsGraph.getNodes()[i]);
    }
    return oms_status_ok;
  }

  // Check if initial unknowns from modelDescription.xml are the same as in initialUnknownsGraph
  fmi2_import_variable_list_t* initialUnknowns;
  initialUnknowns = fmi2_import_get_initial_unknowns_list(fmu);
  int numInitialUnknowns = fmi2_import_get_variable_list_size(initialUnknowns);
  bool initialUnknownsCorrect = true;
  for (int i = 0; i < numInitialUnknowns; i++)
  {
    fmi2_xml_variable_t* tmpVar;
    tmpVar = fmi2_import_get_variable(initialUnknowns, i);
    int originalIndex = fmi2_import_get_variable_original_order(tmpVar);

    // Check if variable with valueReference is initialUnknown
    Variable var = allVariables[originalIndex];
    if (! var.isInitialUnknown())
    {
      logWarning(std::string(getCref()) + ": Variable " + std::string(var.getCref()) + " with index " + std::to_string(originalIndex+1) + " is not an initial unknown.");
      initialUnknownsCorrect = false;
    }
  }
  fmi2_import_free_variable_list(initialUnknowns);
  if (N < numInitialUnknowns || initialUnknownsCorrect==false)
  {
    logDebug("Found " +  std::to_string(numInitialUnknowns) + " initial unknown variables in modelDescription.xml (expected max. " + std::to_string(N) + ")");
    logError(std::string(getCref()) + ": Erroneous initial unknowns detected in modelDescription.xml.");
    logInfo("Use flag --ignoreInitialUnknowns=true to ignore all initial unknowns, but this can cause inflated loop size.");
    return oms_status_error;
  }


  size_t *startIndex=NULL, *dependency=NULL;
  char* factorKind;

  fmi2_import_get_initial_unknowns_dependencies(fmu, &startIndex, &dependency, &factorKind);

  if (!startIndex)
  {
    logDebug(std::string(getCref()) + ": " + getPath() + " no dependencies");
    return oms_status_ok;
  }

  for (int i = 0; i < N; i++)
  {
    if (startIndex[i] == startIndex[i + 1])
    {
      logDebug(std::string(getCref()) + ": " + getPath() + " initial unknown " + std::string(initialUnknownsGraph.getNodes()[i]) + " has no dependencies");
    }
    else if ((startIndex[i] + 1 == startIndex[i + 1]) && (dependency[startIndex[i]] == 0))
    {
      logDebug(std::string(getCref()) + ": " + getPath() + " initial unknown " + std::string(initialUnknownsGraph.getNodes()[i]) + " depends on all");
      for (int j = 0; j < inputs.size(); j++)
        initialUnknownsGraph.addEdge(inputs[j].makeConnector(this->getFullCref()), initialUnknownsGraph.getNodes()[i]);
    }
    else
    {
      for (size_t j = startIndex[i]; j < startIndex[i + 1]; j++)
      {
        if (dependency[j] < 1 || dependency[j] > allVariables.size())
        {
          logWarning("Initial unknown " + std::string(initialUnknownsGraph.getNodes()[i]) + " has bad dependency on variable with index " + std::to_string(dependency[j]) + " which couldn't be resolved");
          logError(std::string(getCref()) + ": erroneous dependencies detected in modelDescription.xml");
          logInfo("Use flag --ignoreInitialUnknowns=true to ignore all dependencies, but this can cause inflated loop size.");
          return oms_status_error;
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
  if (outputsGraph.getEdges().size() > 0)
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

  for (int i = 0; i < outputs.size(); i++)
  {
    if (startIndex[i] == startIndex[i + 1])
    {
      logDebug(std::string(getCref()) + ": " + getPath() + " output " + std::string(outputs[i]) + " has no dependencies");
    }
    else if ((startIndex[i] + 1 == startIndex[i + 1]) && (dependency[startIndex[i]] == 0))
    {
      logDebug(std::string(getCref()) + ": " + getPath() + " output " + std::string(outputs[i]) + " depends on all");
      for (int j = 0; j < inputs.size(); j++)
        outputsGraph.addEdge(inputs[j].makeConnector(this->getFullCref()), outputs[i].makeConnector(this->getFullCref()));
    }
    else
    {
      for (size_t j = startIndex[i]; j < startIndex[i + 1]; j++)
      {
        if (dependency[j] < 1 || dependency[j] > allVariables.size())
        {
          logWarning("Output " + std::string(outputs[i]) + " has bad dependency on variable with index " + std::to_string(dependency[j]) + " which couldn't be resolved");
          return logError(std::string(getCref()) + ": erroneous dependencies detected in modelDescription.xml");
        }
        logDebug(std::string(getCref()) + ": " + getPath() + " output " + std::string(outputs[i]) + " depends on " + std::string(allVariables[dependency[j] - 1]));
        outputsGraph.addEdge(allVariables[dependency[j] - 1].makeConnector(this->getFullCref()), outputs[i].makeConnector(this->getFullCref()));
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

  // set start values
  for (const auto& v : values.booleanStartValues)
  {
    oms::ComRef cref = getValidCref(v.first);
    if (oms_status_ok != setBoolean(cref, v.second))
      return logError("Failed to set start value for " + std::string(v.first));
  }
  for (const auto& v : values.integerStartValues)
  {
    oms::ComRef cref = getValidCref(v.first);
    if (oms_status_ok != setInteger(cref, v.second))
      return logError("Failed to set start value for " + std::string(v.first));
  }
  for (const auto& v : values.realStartValues)
  {
    oms::ComRef cref = getValidCref(v.first);
    if (oms_status_ok != setReal(cref, v.second))
      return logError("Failed to set start value for " + std::string(v.first));
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

/**
 * @brief Get boolean value(s) of FMU component.
 *
 * @param vr                      Array with value references.
 * @param value                   Array containing values on output.
 * @param nvr                     Length of arrays vr and value.
 * @return oms_status_enu_t       Return oms_status_ok on success.
 */
oms_status_enu_t oms::ComponentFMUCS::getBoolean(const fmi2_value_reference_t vr[], bool value[], size_t nvr)
{
  CallClock callClock(clock);

  int stupidIntBoolArray[nvr];

  if (fmi2_status_ok != fmi2_import_get_boolean(fmu, vr, nvr, stupidIntBoolArray))
    return oms_status_error;

  for (int i=0; i<nvr; i++)
  {
    value[i] = stupidIntBoolArray[i] ? true : false;
  }
  return oms_status_ok;
}

oms_status_enu_t oms::ComponentFMUCS::getBoolean(const ComRef& cref, bool& value)
{
  CallClock callClock(clock);

  if (oms_modelState_virgin == getModel().getModelState())
  {
    // check for start values exist, priority over modeldescription.xml start values
    auto booleanValue = values.booleanStartValues.find(cref);
    if (booleanValue != values.booleanStartValues.end())
    {
      value = booleanValue->second;
      return oms_status_ok;
    }
    else
    {
      // search in modelDescription.xml
      auto booleanValue = values.modelDescriptionBooleanStartValues.find(cref);
      if (booleanValue != values.modelDescriptionBooleanStartValues.end())
      {
        value = booleanValue->second;
        return oms_status_ok;
      }
    }
    return logError("no start value provided or available for signal: " + std::string(getFullCref() + cref));
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


/**
 * @brief Get integer value(s) of FMU component.
 *
 * @param vr                      Array with value references.
 * @param value                   Array containing values on output.
 * @param nvr                     Length of arrays vr and value.
 * @return oms_status_enu_t       Return oms_status_ok on success.
 */
oms_status_enu_t oms::ComponentFMUCS::getInteger(const fmi2_value_reference_t vr[], int value[], size_t nvr)
{
  CallClock callClock(clock);

  if (fmi2_status_ok != fmi2_import_get_integer(fmu, vr, nvr, value))
    return oms_status_error;

  return oms_status_ok;
}


oms_status_enu_t oms::ComponentFMUCS::getInteger(const ComRef& cref, int& value)
{
  CallClock callClock(clock);

  if (oms_modelState_virgin == getModel().getModelState())
  {
    // check for start values exist, priority over modeldescription.xml start values
    auto integerValue = values.integerStartValues.find(cref);
    if (integerValue != values.integerStartValues.end())
    {
      value = integerValue->second;
      return oms_status_ok;
    }
    else
    {
      // search in modelDescription.xml
      auto integerValue = values.modelDescriptionIntegerStartValues.find(cref);
      if (integerValue != values.modelDescriptionIntegerStartValues.end())
      {
        value = integerValue->second;
        return oms_status_ok;
      }
    }
    return logError("no start value set or available for signal: " + std::string(getFullCref() + cref));
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


/**
 * @brief Get real value(s) of FMU component.
 *
 * @param vr                      Array with value references.
 * @param value                   Array containing values on output.
 * @param nvr                     Length of arrays vr and value.
 * @return oms_status_enu_t       Return oms_status_ok on success.
 */
oms_status_enu_t oms::ComponentFMUCS::getReal(const fmi2_value_reference_t vr[], double value[], size_t nvr)
{
  CallClock callClock(clock);

  if (fmi2_status_ok != fmi2_import_get_real(fmu, vr, nvr, value))
    return oms_status_error;

  for(int i=0; i<nvr; i++)
  {
    if (std::isnan(value[i]))
      return logError("getReal returned NAN");
    if (std::isinf(value[i]))
      return logError("getReal returned +/-inf");
  }

  // Check for FIB
  if(!fib.empty())
  {
    for(int i=0; i<nvr; i++)
    {
      auto block = fib.find(vr[i]);
      if (block != fib.end())
      {
        switch(block->second.faultType)
        {
          case oms_fault_type_bias:   // y = y.$original + faultValue
            value[i] += block->second.faultValue;
            break;

          case oms_fault_type_gain:   // y = y.$original * faultValue
            value[i] *= block->second.faultValue;
            break;

          case oms_fault_type_const:  // y = faultValue
            value[i] = block->second.faultValue;
            break;

          default:
            return logError("Unhandled fault injection block");
        }
      }
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
    auto realValue = values.realStartValues.find(cref);
    if (realValue != values.realStartValues.end())
    {
      value = realValue->second;
      return oms_status_ok;
    }
    else
    {
      // search in modelDescription.xml
      auto realValue = values.modelDescriptionRealStartValues.find(cref);
      if (realValue != values.modelDescriptionRealStartValues.end())
      {
        value = realValue->second;
        return oms_status_ok;
      }
    }
    return logError("no start value set or available for signal: " + std::string(getFullCref() + cref));
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
    if (Flags::ExportParametersInline())
    {
      values.setBoolean(allVariables[j].getCref(), value);
    }
    else
    {
      // append startValues with prefix (e.g) addP.K1
      values.setBoolean(getCref()+allVariables[j].getCref(), value);
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
    if (Flags::ExportParametersInline())
    {
      values.setInteger(allVariables[j].getCref(), value);
    }
    else
    {
      // append startValues with prefix (e.g) addP.K1
      values.setInteger(getCref()+allVariables[j].getCref(), value);
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
    if (Flags::ExportParametersInline())
    {
      values.setReal(allVariables[j].getCref(), value);
    }
    else
    {
      // append startValues with prefix (e.g) addP.K1
      values.setReal(getCref()+allVariables[j].getCref(), value);
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

oms_status_enu_t oms::ComponentFMUCS::deleteStartValue(const ComRef& cref)
{
  return values.deleteStartValue(cref);
}

oms_status_enu_t oms::ComponentFMUCS::registerSignalsForResultFile(ResultWriter& resultFile)
{
  unsigned int jRealVars = 0;
  unsigned int jIntVars = 0;
  unsigned int jBoolVars = 0;

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
        jRealVars++;
      }
      else if (var.isTypeInteger())
      {
        getInteger(var.getCref(), value.intValue);
        resultFile.addParameter(name, description, SignalType_INT, value);
        jIntVars++;
      }
      else if (var.isTypeBoolean())
      {
        getBoolean(var.getCref(), value.boolValue);
        resultFile.addParameter(name, description, SignalType_BOOL, value);
        jBoolVars++;
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
        jRealVars++;
      }
      else if (var.isTypeInteger())
      {
        unsigned int ID = resultFile.addSignal(name, description, SignalType_INT);
        resultFileMapping[ID] = i;
        jIntVars++;
      }
      else if (var.isTypeBoolean())
      {
        unsigned int ID = resultFile.addSignal(name, description, SignalType_BOOL);
        resultFileMapping[ID] = i;
        jBoolVars++;
      }
      else
        logInfo("Variable " + name + " will not be stored in the result file, because the signal type is not supported");
    }
  }

  // Allocate memroy for var arrays
  resultVariables.allocVarArrays(jRealVars, jIntVars, jBoolVars);

  // Register result variables
  resultVariables.registerVariables(allVariables, exportVariables);

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

  // Get all result variables
  oms_status_enu_t status;
  status = getReal(resultVariables.realResultVars.vr, resultVariables.realResultVars.value, resultVariables.realResultVars.nvr);
  if (status != oms_status_ok)
    return logError("failed to fetch real variables");
  status = getInteger(resultVariables.intResultVars.vr, resultVariables.intResultVars.value, resultVariables.intResultVars.nvr);
  if (status != oms_status_ok)
    return logError("failed to fetch integer variables");
  status = getBoolean(resultVariables.boolResultVars.vr, resultVariables.boolResultVars.value, resultVariables.boolResultVars.nvr);
  if (status != oms_status_ok)
    return logError("failed to fetch boolean variables");

  // Update signals
  resultWriter.updateSignals(resultVariables.realResultVars.id, resultVariables.realResultVars.value, resultVariables.realResultVars.nvr);
  resultWriter.updateSignals(resultVariables.intResultVars.id, resultVariables.intResultVars.value, resultVariables.intResultVars.nvr);
  resultWriter.updateSignals(resultVariables.boolResultVars.id, resultVariables.boolResultVars.value, resultVariables.boolResultVars.nvr);

  /*
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
  */

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
