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

#include "FMUWrapper.h"

#include "Connector.h"
#include "Logging.h"
#include "Option.h"
#include "Scope.h"
#include "Variable.h"
#include "ssd/Tags.h"
#include "Solver.h"
#include "FMICompositeModel.h"

#include <cmath>
#include <RegEx.h>
#include <fmilib.h>
#include <JM/jm_portability.h>

#include <boost/filesystem.hpp>

namespace oms2
{
  void fmiLogger(jm_callbacks* c, jm_string module, jm_log_level_enu_t log_level, jm_string message);
  void fmi2logger(fmi2_component_environment_t env, fmi2_string_t instanceName, fmi2_status_t status, fmi2_string_t category, fmi2_string_t message, ...);
}

void oms2::fmiLogger(jm_callbacks* c, jm_string module, jm_log_level_enu_t log_level, jm_string message)
{
  switch (log_level)
  {
  case jm_log_level_info:    // Informative messages
    logDebug("module " + std::string(module) + ": " + std::string(message));
    break;
  case jm_log_level_warning: // Non-critical issues
    logWarning("module " + std::string(module) + ": " + std::string(message));
    break;
  case jm_log_level_error:   // Errors that may be not critical for some FMUs
  case jm_log_level_fatal:   // Unrecoverable errors
    logError("module " + std::string(module) + ": " + std::string(message));
    break;
  case jm_log_level_verbose: // Verbose messages
  case jm_log_level_debug:   // Debug messages. Only enabled if library is configured with FMILIB_ENABLE_LOG_LEVEL_DEBUG
    logDebug("[log level " + std::string(jm_log_level_to_string(log_level)) + "] module " + std::string(module) + ": " + std::string(message));
  }
}

void oms2::fmi2logger(fmi2_component_environment_t env, fmi2_string_t instanceName, fmi2_status_t status, fmi2_string_t category, fmi2_string_t message, ...)
{
  int len;
  char msg[1000];
  va_list argp;
  va_start(argp, message);
  len = vsnprintf(msg, 1000, message, argp);

  switch (status)
  {
  case fmi2_status_ok:
  case fmi2_status_pending:
    logDebug(std::string(instanceName) + " (" + category + "): " + msg);
    break;
  case fmi2_status_warning:
    logWarning(std::string(instanceName) + " (" + category + "): " + msg);
    break;
  case fmi2_status_discard:
  case fmi2_status_error:
  case fmi2_status_fatal:
    logError(std::string(instanceName) + " (" + category + "): " + msg);
    break;
  default:
    logWarning("fmiStatus = " + std::string(fmi2_status_to_string(status)) + "; " + instanceName + " (" + category + "): " + msg);
  }
}

oms2::FMUWrapper::FMUWrapper(const oms2::ComRef& cref, const std::string& filename, const ComRef &parent)
  : oms2::FMISubModel(oms_component_fmu, cref), fmuInfo(filename)
{
  this->parent = parent;
}

oms2::FMUWrapper::~FMUWrapper()
{
  logTrace();

  reset(true);
  fmi2_import_free_instance(fmu);
  fmi2_import_destroy_dllfmu(fmu);
  fmi2_import_free(fmu);
  fmi_import_free_context(context);
  if (!tempDir.empty() && boost::filesystem::is_directory(tempDir))
  {
    /* Use boost::filesystem::remove_all to remove the tempDir instead of fmi_import_rmdir
     * to avoid showing terminal windows
     */
    //fmi_import_rmdir(&callbacks, tempDir.c_str());
    boost::filesystem::remove_all(tempDir);
    logDebug("removed working directory: \"" + tempDir + "\"");
  }
}

oms2::FMUWrapper* oms2::FMUWrapper::newSubModel(const oms2::ComRef& cref, const std::string& filename, const oms2::ComRef& parent)
{
  if (!cref.isValidQualified())
  {
    logError("\"" + cref + "\" is not a valid model name.");
    return NULL;
  }

  if (!boost::filesystem::exists(filename))
  {
    logError("Specified file name does not exist: \"" + filename + "\"");
    return NULL;
  }

  oms2::FMUWrapper *model = new oms2::FMUWrapper(cref, filename, parent);

  model->callbacks.malloc = malloc;
  model->callbacks.calloc = calloc;
  model->callbacks.realloc = realloc;
  model->callbacks.free = free;
  model->callbacks.logger = oms2::fmiLogger;
  model->callbacks.log_level = jm_log_level_all;
  model->callbacks.context = 0;

  // set temp directory
  char* temp_tempDir = fmi_import_mk_temp_dir(&model->callbacks, oms2::Scope::GetInstance().getTempDirectory().c_str(), "temp_");
  model->tempDir = std::string(temp_tempDir);
  free(temp_tempDir);

  logDebug("Using \"" + model->tempDir + "\" as temp directory for " + cref);

  model->context = fmi_import_allocate_context(&model->callbacks);

  // check version of FMU
  fmi_version_enu_t version = fmi_import_get_fmi_version(model->context, filename.c_str(), model->tempDir.c_str());
  if (fmi_version_2_0_enu != version)
  {
    logError("Unsupported FMI version: " + std::string(fmi_version_to_string(version)));
    delete model;
    return NULL;
  }

  // parse modelDescription.xml
  model->fmu = fmi2_import_parse_xml(model->context, model->tempDir.c_str(), 0);
  if (!model->fmu)
  {
    logError("Error parsing modelDescription.xml");
    delete model;
    return NULL;
  }

  model->fmuInfo.setKind(model->fmu);

  model->callbackFunctions.logger = oms2::fmi2logger;
  model->callbackFunctions.allocateMemory = calloc;
  model->callbackFunctions.freeMemory = free;
  model->callbackFunctions.componentEnvironment = model->fmu;
  model->callbackFunctions.stepFinished = NULL;

  if (oms_fmi_kind_me == model->fmuInfo.getKind())
  {
    jm_status_enu_t jmstatus;

    // load the FMU's shared library
    jmstatus = fmi2_import_create_dllfmu(model->fmu, fmi2_fmu_kind_me, &model->callbackFunctions);
    if (jm_status_error == jmstatus)
    {
      logError("Could not create the DLL loading mechanism (C-API). Error: " + std::string(fmi2_import_get_last_error(model->fmu)));
      delete model;
      return NULL;
    }

    jmstatus = fmi2_import_instantiate(model->fmu, cref.toString().c_str(), fmi2_model_exchange, NULL, fmi2_false);
    if (jm_status_error == jmstatus)
    {
      logError("fmi2_import_instantiate failed");
      delete model;
      return NULL;
    }

    // update FMU info
    if (oms_status_ok != model->fmuInfo.update(model->fmu))
    {
      logError("Error importing FMU attributes");
      delete model;
      return NULL;
    }
  }
  else if (oms_fmi_kind_cs == model->fmuInfo.getKind() || oms_fmi_kind_me_and_cs == model->fmuInfo.getKind())
  {
    jm_status_enu_t jmstatus;

    // load the FMU shared library
    jmstatus = fmi2_import_create_dllfmu(model->fmu, fmi2_fmu_kind_cs, &model->callbackFunctions);
    if (jm_status_error == jmstatus)
    {
      logError("Could not create the DLL loading mechanism (C-API). Error: " + std::string(fmi2_import_get_last_error(model->fmu)));
      delete model;
      return NULL;
    }

    jmstatus = fmi2_import_instantiate(model->fmu, cref.toString().c_str(), fmi2_cosimulation, NULL, fmi2_false);
    if (jm_status_error == jmstatus)
    {
      logError("fmi2_import_instantiate failed");
      delete model;
      return NULL;
    }

    // update FMU info
    if (oms_status_ok != model->fmuInfo.update(model->fmu))
    {
      logError("Error importing FMU attributes");
      delete model;
      return NULL;
    }
  }

  // create a list of all variables
  fmi2_import_variable_list_t *varList = fmi2_import_get_variable_list(model->fmu, 0);
  size_t varListSize = fmi2_import_get_variable_list_size(varList);
  logDebug(std::to_string(varListSize) + " variables");
  model->allVariables.reserve(varListSize);
  for (size_t i = 0; i < varListSize; ++i)
  {
    fmi2_import_variable_t* var = fmi2_import_get_variable(varList, i);
    oms2::Variable v(cref, var, i + 1);
    model->allVariables.push_back(v);
  }
  fmi2_import_free_variable_list(varList);

  // mark states
  varList = fmi2_import_get_derivatives_list(model->fmu);
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
      oms2::Variable* state_var = NULL;
      for (size_t i = 0; i < model->allVariables.size() && !state_var; i++)
        if (state_vr == model->allVariables[i].getValueReference())
          state_var = &model->allVariables[i];
      if (state_var)
        state_var->markAsState();
      else
      {
        logError("Couldn't find " + std::string(fmi2_import_get_variable_name(varState)));
        delete model;
        return NULL;
      }
    }
    else
    {
      logError("Couldn't map " + std::string(fmi2_import_get_variable_name(var)) + " to the corresponding state variable");
      delete model;
      return NULL;
    }
  }
  fmi2_import_free_variable_list(varList);

  // create some special variable maps
  for (auto const &v : model->allVariables)
  {
    if (v.isParameter() && v.isTypeReal())
      model->realParameters[v.getName()] = oms2::Option<double>();
    else if (v.isParameter() && v.isTypeInteger())
      model->integerParameters[v.getName()] = oms2::Option<int>();
    else if (v.isParameter() && v.isTypeBoolean())
      model->booleanParameters[v.getName()] = oms2::Option<bool>();

    if (v.isInput())
    {
      model->inputs.push_back(v);
    }
    else if (v.isOutput())
    {
      model->outputs.push_back(v);
      model->outputsGraph.addVariable(v);
    }
    else if (v.isParameter())
      model->parameters.push_back(v);

    if (v.isInitialUnknown())
      model->initialUnknownsGraph.addVariable(v);

    model->exportVariables.push_back(v.isInput() || v.isOutput());
  }

  std::vector<oms2::Connector> connectors;
  int i = 1;
  int size = 1 + model->inputs.size();
  for (auto const &v : model->inputs)
  {
    oms2::Connector c(oms_causality_input, v.getType(), v.getSignalRef(), i++/(double)size);
    connectors.push_back(c);
  }
  i = 1;
  size = 1 + model->outputs.size();
  for (auto const &v : model->outputs)
  {
    oms2::Connector c(oms_causality_output, v.getType(), v.getSignalRef(), i++/(double)size);
    connectors.push_back(c);
  }
  for (auto const &v : model->parameters)
  {
    oms2::Connector c(oms_causality_parameter, v.getType(), v.getSignalRef());
    connectors.push_back(c);
  }
  model->element.setConnectors(connectors);

  model->initializeDependencyGraph_initialUnknowns();
  model->initializeDependencyGraph_outputs();

  return model;
}

oms_status_enu_t oms2::FMUWrapper::initializeDependencyGraph_initialUnknowns()
{
  if (initialUnknownsGraph.edges.size() > 0)
  {
    logError("oms2::FMUWrapper::initializeDependencyGraph_initialUnknowns: [" + getName() + ": " + getFMUPath() + "] is already initialized.");
    return oms_status_error;
  }

  size_t *startIndex=NULL, *dependency=NULL;
  char* factorKind;

  fmi2_import_get_initial_unknowns_dependencies(fmu, &startIndex, &dependency, &factorKind);

  if (!startIndex)
  {
    logDebug("oms2::FMUWrapper::initializeDependencyGraph_initialUnknowns: [" + getName() + ": " + getFMUPath() + "] no dependencies");
    return oms_status_ok;
  }

  int N=initialUnknownsGraph.nodes.size();
  for (int i = 0; i < N; i++)
  {
    if (startIndex[i] == startIndex[i + 1])
    {
      logDebug("oms2::FMUWrapper::initializeDependencyGraph_initialUnknowns: [" + getName() + ": " + getFMUPath() + "] initial unknown " + initialUnknownsGraph.nodes[i].getName() + " has no dependencies");
    }
    else if ((startIndex[i] + 1 == startIndex[i + 1]) && (dependency[startIndex[i]] == 0))
    {
      logDebug("oms2::FMUWrapper::initializeDependencyGraph_initialUnknowns: [" + getName() + ": " + getFMUPath() + "] initial unknown " + initialUnknownsGraph.nodes[i].getName() + " depends on all");
      for (int j = 0; j < inputs.size(); j++)
        initialUnknownsGraph.addEdge(inputs[j], initialUnknownsGraph.nodes[i]);
    }
    else
    {
      for (size_t j = startIndex[i]; j < startIndex[i + 1]; j++)
      {
        logDebug("oms2::FMUWrapper::initializeDependencyGraph_initialUnknowns: [" + getName() + ": " + getFMUPath() + "] initial unknown " + initialUnknownsGraph.nodes[i].getName() + " depends on " + allVariables[dependency[j] - 1].getName());
        initialUnknownsGraph.addEdge(allVariables[dependency[j] - 1], initialUnknownsGraph.nodes[i]);
      }
    }
  }

  return oms_status_ok;
}

oms_status_enu_t oms2::FMUWrapper::initializeDependencyGraph_outputs()
{
  if (outputsGraph.edges.size() > 0)
  {
    logError("oms2::FMUWrapper::initializeDependencyGraph_outputs: [" + getName() + ": " + getFMUPath() + "] is already initialized.");
    return oms_status_error;
  }

  size_t *startIndex=NULL, *dependency=NULL;
  char* factorKind;

  fmi2_import_get_outputs_dependencies(fmu, &startIndex, &dependency, &factorKind);

  if (!startIndex)
  {
    logDebug("oms2::FMUWrapper::initializeDependencyGraph_outputs: [" + getName() + ": " + getFMUPath() + "] no dependencies");
    return oms_status_ok;
  }

  for (int i = 0; i < outputs.size(); i++)
  {
    if (startIndex[i] == startIndex[i + 1])
    {
      logDebug("oms2::FMUWrapper::initializeDependencyGraph_outputs: [" + getName() + ": " + getFMUPath() + "] output " + outputs[i].getName() + " has no dependencies");
    }
    else if ((startIndex[i] + 1 == startIndex[i + 1]) && (dependency[startIndex[i]] == 0))
    {
      logDebug("oms2::FMUWrapper::initializeDependencyGraph_outputs: [" + getName() + ": " + getFMUPath() + "] output " + outputs[i].getName() + " depends on all");
      for (int j = 0; j < inputs.size(); j++)
        outputsGraph.addEdge(inputs[j], outputs[i]);
    }
    else
    {
      for (size_t j = startIndex[i]; j < startIndex[i + 1]; j++)
      {
        logDebug("oms2::FMUWrapper::initializeDependencyGraph_outputs: [" + getName() + ": " + getFMUPath() + "] output " + outputs[i].getName() + " depends on " + allVariables[dependency[j] - 1].getName());
        outputsGraph.addEdge(allVariables[dependency[j] - 1], outputs[i]);
      }
    }
  }

  return oms_status_ok;
}

oms_status_enu_t oms2::FMUWrapper::enterInitialization(const double startTime)
{
  fmi2_status_t fmistatus;

  // check if a solver instance is assigned to this FMU
  if (!solver)
    return logError("[oms2::FMUWrapper::exitInitialization] No solver assigned to FMU \"" + getName() + "\"");

  if (oms_fmi_kind_me == fmuInfo.getKind())
  {
    fmistatus = fmi2_import_setup_experiment(fmu, fmi2_true, solver->getTolerance(), solver->getTime(), fmi2_false, 1.0);
    if (fmi2_status_ok != fmistatus) return logError("fmi2_import_setup_experiment failed");

    fmistatus = fmi2_import_enter_initialization_mode(fmu);
    if (fmi2_status_ok != fmistatus) return logError("fmi2_import_enter_initialization_mode failed");
  }
  else if (oms_fmi_kind_cs == fmuInfo.getKind() || oms_fmi_kind_me_and_cs == fmuInfo.getKind())
  {
    fmistatus = fmi2_import_setup_experiment(fmu, fmi2_true, solver->getTolerance(), solver->getTime(), fmi2_false, 1.0);
    if (fmi2_status_ok != fmistatus) return logError("fmi2_import_setup_experiment failed");

    fmistatus = fmi2_import_enter_initialization_mode(fmu);
    if (fmi2_status_ok != fmistatus) return logError("fmi2_import_enter_initialization_mode failed");
  }
  else
    return logError("Unsupported FMU kind");

  return oms_status_ok;
}

oms_status_enu_t oms2::FMUWrapper::exitInitialization()
{
  fmi2_status_t fmistatus;

  // check if a solver instance is assigned to this FMU
  if (!solver)
    return logError("[oms2::FMUWrapper::exitInitialization] No solver assigned to FMU \"" + getName() + "\"");

  if (oms_solver_internal == solver->getMethod())
  {
    // initialize co-simulation
    if (oms_fmi_kind_cs != fmuInfo.getKind() && oms_fmi_kind_me_and_cs != fmuInfo.getKind())
      return logError("[oms2::FMUWrapper::exitInitialization] FMU \"" + getName() + "\" doesn't provide any internal solver (no co-simulation)");
  }
  else
  {
    // initialize me-simulation
    if (oms_fmi_kind_me != fmuInfo.getKind() && oms_fmi_kind_me_and_cs != fmuInfo.getKind())
      return logError("[oms2::FMUWrapper::exitInitialization] FMU \"" + getName() + "\" doesn't provide me-simulation functionality");
  }

  fmistatus = fmi2_import_exit_initialization_mode(fmu);
  if (fmi2_status_ok != fmistatus) return logError("fmi2_import_exit_initialization_mode failed");

  solver->initializeFMU(this);

  initialized = true;
  return oms_status_ok;
}

oms_status_enu_t oms2::FMUWrapper::reset(bool terminate)
{
  if (!initialized)
    return oms_status_ok;

  if (terminate)
  {
    fmi2_status_t fmistatus = fmi2_import_terminate(fmu);
    if (fmi2_status_ok != fmistatus)
      return logError("fmi2_import_terminate failed");
  }
  else
  {
    fmi2_status_t fmistatus = fmi2_import_reset(fmu);
    if (fmi2_status_ok != fmistatus)
      return logError("fmi2_import_reset failed");
  }

  initialized = false;
  return oms_status_ok;
}

oms_status_enu_t oms2::FMUWrapper::exportToSSD(pugi::xml_node& root) const
{
  oms_status_enu_t status = oms_status_ok;

  oms2::ComRef cref = element.getName();
  pugi::xml_node subModel = root.append_child(oms2::ssd::ssd_component);
  subModel.append_attribute("name") = cref.last().toString().c_str();

  subModel.append_attribute("type") = "application/x-fmu-sharedlibrary";

  const std::string& fmuPath = getFMUPath();
  subModel.append_attribute("source") = fmuPath.c_str();

  // export ssd:ElementGeometry
  status = element.getGeometry()->exportToSSD(subModel);
  if (oms_status_ok != status)
    return status;

  // export ssd:Connectors
  oms2::Connector** connectors = element.getConnectors();
  if (connectors)
  {
    pugi::xml_node connectorsNode = subModel.append_child(oms2::ssd::ssd_connectors);
    for (int i=0; connectors[i]; ++i)
    {
      status = connectors[i]->exportToSSD(connectorsNode);
      if (oms_status_ok != status)
        return status;
    }
  }

  // export ssd:ParameterBindings
  const std::map<std::string, oms2::Option<double>>& realParameters = getRealParameters();
  for (auto it=realParameters.begin(); it != realParameters.end(); it++)
  {
    if (it->second.isSome())
    {
      pugi::xml_node parameter = subModel.append_child("Parameter");
      parameter.append_attribute("Type") = "Real";
      parameter.append_attribute("Name") = it->first.c_str();
      parameter.append_attribute("Value") = std::to_string(it->second.getValue()).c_str();
    }
  }

  const std::map<std::string, oms2::Option<int>>& integerParameters = getIntegerParameters();
  for (auto it=integerParameters.begin(); it != integerParameters.end(); it++)
  {
    if (it->second.isSome())
    {
      pugi::xml_node parameter = subModel.append_child("Parameter");
      parameter.append_attribute("Type") = "Integer";
      parameter.append_attribute("Name") = it->first.c_str();
      parameter.append_attribute("Value") = std::to_string(it->second.getValue()).c_str();
    }
  }

  const std::map<std::string, oms2::Option<bool>>& booleanParameters = getBooleanParameters();
  for (auto it=booleanParameters.begin(); it != booleanParameters.end(); it++)
  {
    if (it->second.isSome())
    {
      pugi::xml_node parameter = subModel.append_child("Parameter");
      parameter.append_attribute("Type") = "Boolean";
      parameter.append_attribute("Name") = it->first.c_str();
      parameter.append_attribute("Value") = it->second.getValue() ? "true" : "false";
    }
  }

  return status;
}

void oms2::FMUWrapper::readFromTLMSockets(double time)
{
#if !defined(NO_TLM)
  FMICompositeModel *model = oms2::Scope::GetInstance().getFMICompositeModel(parent);
  model->readFromTLMSockets(time, element.getName().toString());
#else
  throw std::runtime_error("[oms2::FMUWrapper::readFromTLMSockets] Compiled without TLM support");
#endif
}

void oms2::FMUWrapper::writeToTLMSockets(double time)
{
#if !defined(NO_TLM)
  FMICompositeModel *model = oms2::Scope::GetInstance().getFMICompositeModel(parent);
  model->writeToTLMSockets(time, element.getName().toString());
#else
  throw std::runtime_error("[oms2::FMUWrapper::writeToTLMSockets] Compiled without TLM support");
#endif
}

oms2::Variable* oms2::FMUWrapper::getVariable(const std::string& var)
{
  for (auto &v : allVariables)
    if (v.getName() == var)
      return &v;

  return NULL;
}

oms_status_enu_t oms2::FMUWrapper::setRealParameter(const std::string& var, double value)
{
  auto it = realParameters.find(var);
  if (realParameters.end() == it)
    return logError("No such parameter: " + var);

  it->second = value;

  oms2::Variable* v = getVariable(var);
  if (!v)
    return oms_status_error;
  return setReal(*v, value);
}

oms_status_enu_t oms2::FMUWrapper::getRealParameter(const std::string& var, double& value)
{
  auto it = realParameters.find(var);
  if (realParameters.end() == it)
    return logError("No such parameter: " + var);

  if (it->second.isNone())
  {
    oms2::Variable *v = getVariable(var);
    if (!v)
      return oms_status_error;
    if (oms_status_ok != getReal(*v, value))
      return oms_status_error;
    it->second = value;
  }
  else
    value = it->second.getValue();

  return oms_status_ok;
}

oms_status_enu_t oms2::FMUWrapper::getReal(const std::string& var, double& value)
{
  oms2::Variable* v = getVariable(var);
  if (!v)
    return oms_status_error;
  return getReal(*v, value);
}

oms_status_enu_t oms2::FMUWrapper::setIntegerParameter(const std::string& var, int value)
{
  auto it = integerParameters.find(var);
  if (integerParameters.end() == it)
    return logError("No such parameter: " + var);

  it->second = value;

  oms2::Variable* v = getVariable(var);
  if (!v)
    return oms_status_error;
  return setInteger(*v, value);
}

oms_status_enu_t oms2::FMUWrapper::getIntegerParameter(const std::string& var, int& value)
{
  auto it = integerParameters.find(var);
  if (integerParameters.end() == it)
    return logError("No such parameter: " + var);

  if (it->second.isNone())
  {
    oms2::Variable *v = getVariable(var);
    if (!v)
      return oms_status_error;
    if (oms_status_ok != getInteger(*v, value))
      return oms_status_error;
    it->second = value;
  }
  else
    value = it->second.getValue();

  return oms_status_ok;
}

oms_status_enu_t oms2::FMUWrapper::setBooleanParameter(const std::string& var, bool value)
{
  auto it = booleanParameters.find(var);
  if (booleanParameters.end() == it)
    return logError("No such parameter: " + var);

  it->second = value;

  oms2::Variable* v = getVariable(var);
  if (!v)
    return oms_status_error;
  return setBoolean(*v, value);
}

oms_status_enu_t oms2::FMUWrapper::getBooleanParameter(const std::string& var, bool& value)
{
  auto it = booleanParameters.find(var);
  if (booleanParameters.end() == it)
    return logError("No such parameter: " + var);

  if (it->second.isNone())
  {
    oms2::Variable *v = getVariable(var);
    if (!v)
      return oms_status_error;
    if (oms_status_ok != getBoolean(*v, value))
      return oms_status_error;
    it->second = value;
  }
  else
    value = it->second.getValue();

  return oms_status_ok;
}

oms_status_enu_t oms2::FMUWrapper::setReal(const oms2::Variable& var, double realValue)
{
  logTrace();
  if (!fmu || !var.isTypeReal())
    return logError("oms2::FMUWrapper::setReal failed");

  fmi2_value_reference_t vr = var.getValueReference();
  if (fmi2_status_ok == fmi2_import_set_real(fmu, &vr, 1, &realValue))
    return oms_status_ok;

  return oms_status_error;
}

oms_status_enu_t oms2::FMUWrapper::getReal(const oms2::Variable& var, double& realValue)
{
  logTrace();
  if (!fmu || !var.isTypeReal())
    return logError("oms2::FMUWrapper::getReal failed");

  fmi2_value_reference_t vr = var.getValueReference();
  if (fmi2_status_ok == fmi2_import_get_real(fmu, &vr, 1, &realValue))
  {
    if (std::isnan(realValue))
      return logError("getReal returned NAN");
    if (std::isinf(realValue))
      return logError("getReal returned +/-inf");
    return oms_status_ok;
  }

  return oms_status_error;
}

oms_status_enu_t oms2::FMUWrapper::setRealInputDerivatives(const oms2::Variable &var, int order, double realValue)
{
  logTrace();
  if (!fmu || !var.isTypeReal())
    return logError("oms2::FMUWrapper::setRealInputDerivatives failed");

  fmi2_value_reference_t vr = var.getValueReference();
  if (fmi2_status_ok == fmi2_import_set_real_input_derivatives(fmu, &vr, 1, &order, &realValue))
    return oms_status_ok;

  return oms_status_error;
}

oms_status_enu_t oms2::FMUWrapper::setInteger(const oms2::Variable& var, int integerValue)
{
  logTrace();
  if (!fmu || !var.isTypeInteger())
    return logError("oms2::FMUWrapper::setInteger failed");

  fmi2_value_reference_t vr = var.getValueReference();
  if (fmi2_status_ok == fmi2_import_set_integer(fmu, &vr, 1, &integerValue))
    return oms_status_ok;

  return oms_status_error;
}

oms_status_enu_t oms2::FMUWrapper::getInteger(const oms2::Variable& var, int& integerValue)
{
  logTrace();
  if (!fmu || !var.isTypeInteger())
    return logError("oms2::FMUWrapper::getInteger failed");

  fmi2_value_reference_t vr = var.getValueReference();
  if (fmi2_status_ok == fmi2_import_get_integer(fmu, &vr, 1, &integerValue))
    return oms_status_ok;

  return oms_status_error;
}

oms_status_enu_t oms2::FMUWrapper::setBoolean(const oms2::Variable& var, bool booleanValue)
{
  logTrace();
  if (!fmu || !var.isTypeBoolean())
    return logError("oms2::FMUWrapper::setBoolean failed");

  fmi2_value_reference_t vr = var.getValueReference();
  int value = booleanValue;
  if (fmi2_status_ok == fmi2_import_set_boolean(fmu, &vr, 1, &value))
    return oms_status_ok;

  return oms_status_error;
}

oms_status_enu_t oms2::FMUWrapper::getBoolean(const oms2::Variable& var, bool& booleanValue)
{
  logTrace();
  if (!fmu || !var.isTypeBoolean())
    return logError("oms2::FMUWrapper::getBoolean failed");

  fmi2_value_reference_t vr = var.getValueReference();
  int value = 0;
  if (fmi2_status_ok == fmi2_import_get_boolean(fmu, &vr, 1, &value))
  {
    booleanValue = value;
    return oms_status_ok;
  }

  return oms_status_error;
}

oms_status_enu_t oms2::FMUWrapper::setReal(const oms2::SignalRef& sr, double value)
{
  oms2::Variable* var = getVariable(sr.getVar());
  if (!var)
    return oms_status_error;
  return setReal(*var, value);
}

oms_status_enu_t oms2::FMUWrapper::getReal(const oms2::SignalRef& sr, double& value)
{
  oms2::Variable* var = getVariable(sr.getVar());
  if (!var)
    return oms_status_error;
  return getReal(*var, value);
}

oms_status_enu_t oms2::FMUWrapper::setInteger(const oms2::SignalRef& sr, int value)
{
  oms2::Variable* var = getVariable(sr.getVar());
  if (!var)
    return oms_status_error;
  return setInteger(*var, value);
}

oms_status_enu_t oms2::FMUWrapper::getInteger(const oms2::SignalRef& sr, int& value)
{
  oms2::Variable* var = getVariable(sr.getVar());
  if (!var)
    return oms_status_error;
  return getInteger(*var, value);
}

oms_status_enu_t oms2::FMUWrapper::setBoolean(const oms2::SignalRef& sr, bool value)
{
  oms2::Variable* var = getVariable(sr.getVar());
  if (!var)
    return oms_status_error;
  return setBoolean(*var, value);
}

oms_status_enu_t oms2::FMUWrapper::getBoolean(const oms2::SignalRef& sr, bool& value)
{
  oms2::Variable* var = getVariable(sr.getVar());
  if (!var)
    return oms_status_error;
  return getBoolean(*var, value);
}

oms_status_enu_t oms2::FMUWrapper::setRealInputDerivatives(const oms2::SignalRef &sr, int order, double value)
{
  oms2::Variable* var = getVariable(sr.getVar());
  if(!var)
    return oms_status_error;
  return setRealInputDerivatives(*var, order, value);
}

oms_status_enu_t oms2::FMUWrapper::registerSignalsForResultFile(ResultWriter& resultWriter)
{
  for (unsigned int i=0; i<allVariables.size(); ++i)
  {
    if (!exportVariables[i])
      continue;

    auto const &var = allVariables[i];
    std::string name = var.getCref().toString() + "." + var.getName();
    const std::string& description = var.getDescription();
    if (var.isParameter())
    {
      SignalValue_t value;
      if (var.isTypeReal())
      {
        getReal(var, value.realValue);
        resultWriter.addParameter(name, description, SignalType_REAL, value);
      }
      else if (var.isTypeInteger())
      {
        getInteger(var, value.intValue);
        resultWriter.addParameter(name, description, SignalType_INT, value);
      }
      else if (var.isTypeBoolean())
      {
        getBoolean(var, value.boolValue);
        resultWriter.addParameter(name, description, SignalType_BOOL, value);
      }
      else
        logInfo("Parameter " + name + " will not be stored in the result file, because the signal type is not supported");
    }
    else
    {
      if (var.isTypeReal())
      {
        unsigned int ID = resultWriter.addSignal(name, description, SignalType_REAL);
        resultFileMapping[ID] = i;
      }
      else if (var.isTypeInteger())
      {
        unsigned int ID = resultWriter.addSignal(name, description, SignalType_INT);
        resultFileMapping[ID] = i;
      }
      else if (var.isTypeBoolean())
      {
        unsigned int ID = resultWriter.addSignal(name, description, SignalType_BOOL);
        resultFileMapping[ID] = i;
      }
      else
        logInfo("Variable " + name + " will not be stored in the result file, because the signal type is not supported");
    }
  }

  return oms_status_ok;
}

oms_status_enu_t oms2::FMUWrapper::emit(ResultWriter& resultWriter)
{
  for (auto const &it : resultFileMapping)
  {
    unsigned int ID = it.first;
    oms2::Variable& var = allVariables[it.second];
    SignalValue_t value;
    if (var.isTypeReal())
    {
      if (oms_status_ok != getReal(var, value.realValue))
        return logError("failed to fetch variable " + var.toString());
      resultWriter.updateSignal(ID, value);
    }
    else if (var.isTypeInteger())
    {
      if (oms_status_ok != getInteger(var, value.intValue))
        return logError("failed to fetch variable " + var.toString());
      resultWriter.updateSignal(ID, value);
    }
    else if (var.isTypeBoolean())
    {
      if (oms_status_ok != getBoolean(var, value.boolValue))
        return logError("failed to fetch variable " + var.toString());
      resultWriter.updateSignal(ID, value);
    }
  }

  return oms_status_ok;
}

void oms2::FMUWrapper::addSignalsToResults(const std::string& regex)
{
  oms_regex exp(regex);
  for (unsigned int i=0; i<allVariables.size(); ++i)
  {
    if (exportVariables[i])
      continue;

    auto const &var = allVariables[i];
    if(regex_match(var.toString(), exp))
    {
      logInfo("added \"" + var.toString() + "\" to results");
      exportVariables[i] = true;
    }
  }
}

void oms2::FMUWrapper::removeSignalsFromResults(const std::string& regex)
{
  oms_regex exp(regex);
  for (unsigned int i=0; i<allVariables.size(); ++i)
  {
    if (!exportVariables[i])
      continue;

    auto const &var = allVariables[i];
    if(regex_match(var.toString(), exp))
    {
      logInfo("removed \"" + var.toString() + "\" from results");
      exportVariables[i] = false;
    }
  }
}

void oms2::FMUWrapper::fetchAllVars()
{
  // HACK for certain FMUs
  if (FMISubModel::fetchAllVars)
  {
    for (auto &v : allVariables)
    {
      if (v.isTypeReal())
      {
        double realValue;
        if (oms_status_ok != getReal(v, realValue))
          logError("failed to fetch variable " + v.toString());
      }
    }
  }
}
