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

#include "ComponentFMUCS.h"

#include "Logging.h"
#include "Model.h"
#include "ssd/Tags.h"
#include "System.h"
#include "SystemWC.h"
#include "SystemTLM.h"
#include <fmilib.h>
#include <JM/jm_portability.h>
#include <OMSBoost.h>

oms3::ComponentFMUCS::ComponentFMUCS(const ComRef& cref, System* parentSystem, const std::string& fmuPath)
  : oms3::Component(cref, oms_component_fmu, parentSystem, fmuPath), fmuInfo(fmuPath, oms_fmi_kind_cs)
{
}

oms3::ComponentFMUCS::~ComponentFMUCS()
{
  fmi2_import_free_instance(fmu);
  fmi2_import_destroy_dllfmu(fmu);
  fmi2_import_free(fmu);
  fmi_import_free_context(context);

  if (!tempDir.empty() && boost::filesystem::is_directory(tempDir))
  {
    boost::filesystem::remove_all(tempDir);
    logDebug("removed working directory: \"" + tempDir + "\"");
  }
}

oms3::Component* oms3::ComponentFMUCS::NewComponent(const oms3::ComRef& cref, oms3::System* parentSystem, const std::string& fmuPath)
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

  boost::filesystem::path temp_root(parentSystem->getModel()->getTempDirectory());
  boost::filesystem::path temp_temp = temp_root / "temp";
  boost::filesystem::path temp_resources = temp_root / "resources";

  boost::filesystem::path relFMUPath = boost::filesystem::path("resources") / (std::string(cref) + ".fmu");
  boost::filesystem::path absFMUPath = temp_root / relFMUPath;

  ComponentFMUCS* component = new ComponentFMUCS(cref, parentSystem, "resources/" + std::string(cref) + ".fmu");

  component->callbacks.malloc = malloc;
  component->callbacks.calloc = calloc;
  component->callbacks.realloc = realloc;
  component->callbacks.free = free;
  component->callbacks.logger = oms3::fmiLogger;
  component->callbacks.log_level = jm_log_level_all;
  component->callbacks.context = 0;

  if (parentSystem->copyResources())
    boost::filesystem::copy_file(boost::filesystem::path(fmuPath), absFMUPath, boost::filesystem::copy_option::overwrite_if_exists);

  // set temp directory
  boost::filesystem::path tempDir = temp_temp / std::string(cref);
  component->tempDir = tempDir.string();
  if (!boost::filesystem::is_directory(tempDir) && !boost::filesystem::create_directory(tempDir))
  {
    logError("Creating temp directory for component \"" + std::string(cref) + "\" failed");
    return NULL;
  }

  component->context = fmi_import_allocate_context(&component->callbacks);

  // check version of FMU
  fmi_version_enu_t version = fmi_import_get_fmi_version(component->context, absFMUPath.string().c_str(), component->tempDir.c_str());
  if (fmi_version_2_0_enu != version)
  {
    logError("Unsupported FMI version: " + std::string(fmi_version_to_string(version)));
    delete component;
    return NULL;
  }

  // parse modelDescription.xml
  component->fmu = fmi2_import_parse_xml(component->context, component->tempDir.c_str(), 0);
  if (!component->fmu)
  {
    logError("Error parsing modelDescription.xml");
    delete component;
    return NULL;
  }

  // update FMU info
  if (oms_status_ok != component->fmuInfo.update(version, component->fmu))
  {
    logError("Error importing FMU attributes");
    delete component;
    return NULL;
  }

  component->callbackFunctions.logger = oms3::fmi2logger;
  component->callbackFunctions.allocateMemory = calloc;
  component->callbackFunctions.freeMemory = free;
  component->callbackFunctions.componentEnvironment = component->fmu;
  component->callbackFunctions.stepFinished = NULL;

  // create a list of all variables
  fmi2_import_variable_list_t *varList = fmi2_import_get_variable_list(component->fmu, 0);
  size_t varListSize = fmi2_import_get_variable_list_size(varList);
  logDebug(std::to_string(varListSize) + " variables");
  component->allVariables.reserve(varListSize);
  for (size_t i = 0; i < varListSize; ++i)
  {
    fmi2_import_variable_t* var = fmi2_import_get_variable(varList, i);
    oms3::Variable v(var, i + 1);
    component->allVariables.push_back(v);
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
    if (v.isParameter() && v.isTypeReal())
      component->realParameters[std::string(v)] = oms3::Option<double>();
    else if (v.isParameter() && v.isTypeInteger())
      component->integerParameters[std::string(v)] = oms3::Option<int>();
    else if (v.isParameter() && v.isTypeBoolean())
      component->booleanParameters[std::string(v)] = oms3::Option<bool>();

    if (v.isInput())
      component->inputs.push_back(v);
    else if (v.isOutput())
    {
      component->outputs.push_back(v);
      component->outputsGraph.addNode(Connector(oms_causality_output, v.getType(), v.getCref()));
    }
    else if (v.isParameter())
      component->parameters.push_back(v);

    if (v.isInitialUnknown())
      component->initialUnknownsGraph.addNode(Connector(v.getCausality(), v.getType(), v.getCref()));

    component->exportVariables.push_back(v.isInput() || v.isOutput());
  }

  // create connectors
  while (component->connectors.size() > 0 && NULL == component->connectors.back())
    component->connectors.pop_back();

  int i = 1;
  int size = 1 + component->inputs.size();
  for (const auto& v : component->inputs)
    component->connectors.push_back(new Connector(oms_causality_input, v.getType(), v.getCref(), i++/(double)size));
  i = 1;
  size = 1 + component->outputs.size();
  for (const auto& v : component->outputs)
    component->connectors.push_back(new Connector(oms_causality_output, v.getType(), v.getCref(), i++/(double)size));
  for (const auto& v : component->parameters)
    component->connectors.push_back(new Connector(oms_causality_parameter, v.getType(), v.getCref()));
  component->connectors.push_back(NULL);
  component->element.setConnectors(&component->connectors[0]);

  component->initializeDependencyGraph_initialUnknowns();
  component->initializeDependencyGraph_outputs();

  return component;
}

oms3::Component* oms3::ComponentFMUCS::NewComponent(const pugi::xml_node& node, oms3::System* parentSystem)
{
  ComRef cref = ComRef(node.attribute("name").as_string());
  std::string type = node.attribute("type").as_string();
  std::string source = node.attribute("source").as_string();

  if (type != "application/x-fmu-sharedlibrary")
  {
    logError("Unexpected component type: " + type);
    return NULL;
  }

  oms3::ComponentFMUCS* component = dynamic_cast<oms3::ComponentFMUCS*>(oms3::ComponentFMUCS::NewComponent(cref, parentSystem, source));
  if (!component)
    return NULL;

  for (const auto& connector : component->connectors)
    if (connector)
      delete connector;
  component->connectors.clear();
  for(pugi::xml_node_iterator it = node.begin(); it != node.end(); ++it)
  {
    std::string name = it->name();
    if(name == oms2::ssd::ssd_connectors)
    {
      // import connectors
      for(pugi::xml_node_iterator itConnectors = (*it).begin(); itConnectors != (*it).end(); ++itConnectors)
      {
        component->connectors.push_back(oms3::Connector::NewConnector(*itConnectors));
      }
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

oms_status_enu_t oms3::ComponentFMUCS::exportToSSD(pugi::xml_node& node) const
{
  node.append_attribute("name") = this->getCref().c_str();
  node.append_attribute("type") = "application/x-fmu-sharedlibrary";
  node.append_attribute("source") = getPath().c_str();
  pugi::xml_node node_connectors = node.append_child(oms2::ssd::ssd_connectors);

  if (element.getGeometry())
    element.getGeometry()->exportToSSD(node);

  for (const auto& connector : connectors)
    if (connector)
      if (oms_status_ok != connector->exportToSSD(node_connectors))
        return oms_status_error;
  return oms_status_ok;
}

oms_status_enu_t oms3::ComponentFMUCS::initializeDependencyGraph_initialUnknowns()
{
  if (initialUnknownsGraph.getEdges().size() > 0)
  {
    logError(std::string(getCref()) + ": " + getPath() + " is already initialized");
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

  int N=initialUnknownsGraph.getNodes().size();
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
        initialUnknownsGraph.addEdge(inputs[j].makeConnector(), initialUnknownsGraph.getNodes()[i]);
    }
    else
    {
      for (size_t j = startIndex[i]; j < startIndex[i + 1]; j++)
      {
        logDebug(std::string(getCref()) + ": " + getPath() + " initial unknown " + std::string(initialUnknownsGraph.getNodes()[i]) + " depends on " + std::string(allVariables[dependency[j] - 1]));
        initialUnknownsGraph.addEdge(allVariables[dependency[j] - 1].makeConnector(), initialUnknownsGraph.getNodes()[i]);
      }
    }
  }

  return oms_status_ok;
}

oms_status_enu_t oms3::ComponentFMUCS::initializeDependencyGraph_outputs()
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
        outputsGraph.addEdge(inputs[j].makeConnector(), outputs[i].makeConnector());
    }
    else
    {
      for (size_t j = startIndex[i]; j < startIndex[i + 1]; j++)
      {
        logDebug(std::string(getCref()) + ": " + getPath() + " output " + std::string(outputs[i]) + " depends on " + std::string(allVariables[dependency[j] - 1]));
        outputsGraph.addEdge(allVariables[dependency[j] - 1].makeConnector(), outputs[i].makeConnector());
      }
    }
  }

  return oms_status_ok;
}

oms_status_enu_t oms3::ComponentFMUCS::instantiate()
{
  jm_status_enu_t jmstatus;
  fmi2_status_t fmistatus;

  // load the FMU shared library
  jmstatus = fmi2_import_create_dllfmu(fmu, fmi2_fmu_kind_cs, &callbackFunctions);
  if (jm_status_error == jmstatus)
    return logError("Could not create the DLL loading mechanism (C-API). Error: " + std::string(fmi2_import_get_last_error(fmu)));

  jmstatus = fmi2_import_instantiate(fmu, getCref().c_str(), fmi2_cosimulation, NULL, fmi2_false);
  if (jm_status_error == jmstatus)
    return logError("fmi2_import_instantiate failed");

  // enterInitialization
  time = getParentSystem()->getModel()->getStartTime();
  double tolerance = dynamic_cast<SystemWC*>(getParentSystem())->getTolerance();
  fmistatus = fmi2_import_setup_experiment(fmu, fmi2_true, tolerance, time, fmi2_false, 1.0);
  if (fmi2_status_ok != fmistatus) return logError("fmi2_import_setup_experiment failed");

  fmistatus = fmi2_import_enter_initialization_mode(fmu);
  if (fmi2_status_ok != fmistatus) return logError("fmi2_import_enter_initialization_mode failed");

  return oms_status_ok;
}

oms_status_enu_t oms3::ComponentFMUCS::initialize()
{
  fmi2_status_t fmistatus;

  // exitInitialization
  fmistatus = fmi2_import_exit_initialization_mode(fmu);
  if (fmi2_status_ok != fmistatus) return logError("fmi2_import_exit_initialization_mode failed");

  return oms_status_ok;
}

oms_status_enu_t oms3::ComponentFMUCS::terminate()
{
  fmi2_status_t fmistatus = fmi2_import_terminate(fmu);
  if (fmi2_status_ok != fmistatus)
    return logError_Termination(getCref());

  fmi2_import_free_instance(fmu);
  fmi2_import_destroy_dllfmu(fmu);
  return oms_status_ok;
}

oms_status_enu_t oms3::ComponentFMUCS::stepUntil(double stopTime)
{
  System *topLevelSystem = getModel()->getTopLevelSystem();

  fmi2_status_t fmistatus;
  double hdef = (stopTime-time) / 1.0;

  while (time < stopTime)
  {
    //Read from TLM sockets if top level system is of TLM type
    if(topLevelSystem->getType() == oms_system_tlm)
      reinterpret_cast<SystemTLM*>(topLevelSystem)->readFromSockets(reinterpret_cast<SystemWC*>(getParentSystem()),time,this);

    fmistatus = fmi2_import_do_step(fmu, time, hdef, fmi2_true);
    time += hdef;

    //Write to TLM sockets if top level system is of TLM type
    if(topLevelSystem->getType() == oms_system_tlm)
      reinterpret_cast<SystemTLM*>(topLevelSystem)->writeToSockets(reinterpret_cast<SystemWC*>(getParentSystem()),time,this);
  }
  time = stopTime;
  return oms_status_ok;
}

oms_status_enu_t oms3::ComponentFMUCS::getBoolean(const ComRef& cref, bool& value)
{
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
    return oms_status_error;

  fmi2_value_reference_t vr = allVariables[j].getValueReference();
  int value_;
  if (fmi2_status_ok != fmi2_import_get_boolean(fmu, &vr, 1, &value_))
    return oms_status_error;

  value = value_ ? true : false;
  return oms_status_ok;
}

oms_status_enu_t oms3::ComponentFMUCS::getInteger(const ComRef& cref, int& value)
{
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
    return oms_status_error;

  fmi2_value_reference_t vr = allVariables[j].getValueReference();
  if (fmi2_status_ok != fmi2_import_get_integer(fmu, &vr, 1, &value))
    return oms_status_error;

  return oms_status_ok;
}

oms_status_enu_t oms3::ComponentFMUCS::getReal(const ComRef& cref, double& value)
{
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
    return oms_status_error;

  fmi2_value_reference_t vr = allVariables[j].getValueReference();
  if (fmi2_status_ok != fmi2_import_get_real(fmu, &vr, 1, &value))
    return oms_status_error;

  if (std::isnan(value))
    return logError("getReal returned NAN");
  if (std::isinf(value))
    return logError("getReal returned +/-inf");
  return oms_status_ok;
}

oms_status_enu_t oms3::ComponentFMUCS::setBoolean(const ComRef& cref, bool value)
{
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
    return oms_status_error;

  fmi2_value_reference_t vr = allVariables[j].getValueReference();
  int value_ = value ? 1 : 0;
  if (fmi2_status_ok != fmi2_import_set_boolean(fmu, &vr, 1, &value_))
    return oms_status_error;

  return oms_status_ok;
}

oms_status_enu_t oms3::ComponentFMUCS::setInteger(const ComRef& cref, int value)
{
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
    return oms_status_error;

  fmi2_value_reference_t vr = allVariables[j].getValueReference();
  if (fmi2_status_ok != fmi2_import_set_integer(fmu, &vr, 1, &value))
    return oms_status_error;

  return oms_status_ok;
}

oms_status_enu_t oms3::ComponentFMUCS::setReal(const ComRef& cref, double value)
{
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
    return oms_status_error;

  fmi2_value_reference_t vr = allVariables[j].getValueReference();
  if (fmi2_status_ok != fmi2_import_set_real(fmu, &vr, 1, &value))
    return oms_status_error;

  return oms_status_ok;
}

oms_status_enu_t oms3::ComponentFMUCS::registerSignalsForResultFile(ResultWriter& resultFile)
{
  resultFileMapping.clear();

  for (unsigned int i=0; i<allVariables.size(); ++i)
  {
    //if (!exportVariables[i])
    //  continue;

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

oms_status_enu_t oms3::ComponentFMUCS::updateSignals(ResultWriter& resultWriter, double time)
{
  for (auto const &it : resultFileMapping)
  {
    unsigned int ID = it.first;
    Variable& var = allVariables[it.second];
    SignalValue_t value;
    if (var.isTypeReal())
    {
        if (oms_status_ok != getReal(var.getCref(), value.realValue))
        return logError("failed to fetch variable " + std::string(var.getCref()));
      resultWriter.updateSignal(ID, value);
    }
    else if (var.isTypeInteger())
    {
      if (oms_status_ok != getInteger(var.getCref(), value.intValue))
        return logError("failed to fetch variable " + std::string(var.getCref()));
      resultWriter.updateSignal(ID, value);
    }
    else if (var.isTypeBoolean())
    {
      if (oms_status_ok != getBoolean(var.getCref(), value.boolValue))
        return logError("failed to fetch variable " + std::string(var.getCref()));
      resultWriter.updateSignal(ID, value);
    }
  }

  return oms_status_ok;
}
