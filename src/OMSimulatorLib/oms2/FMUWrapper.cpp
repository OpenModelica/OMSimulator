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

oms2::FMUWrapper::FMUWrapper(const oms2::ComRef& cref, const std::string& filename)
  : oms2::FMISubModel(oms_component_fmu, cref)
{
  this->filename = filename;

  this->context = NULL;
  this->fmu = NULL;
}

oms2::FMUWrapper::~FMUWrapper()
{
  logTrace();

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

oms2::FMUWrapper* oms2::FMUWrapper::newSubModel(const oms2::ComRef& cref, const std::string& filename)
{
  if (!cref.isValidQualified())
  {
    logError("\"" + cref + "\" is not a valid model name.");
    return NULL;
  }

  oms2::FMUWrapper *model = new oms2::FMUWrapper(cref, filename);

  // instantiate FMU
  if (!boost::filesystem::exists(filename))
  {
    logError("Specified file name does not exist: \"" + filename + "\"");
    delete model;
    return NULL;
  }

  model->callbacks.malloc = malloc;
  model->callbacks.calloc = calloc;
  model->callbacks.realloc = realloc;
  model->callbacks.free = free;
  model->callbacks.logger = oms2::fmiLogger;
  model->callbacks.log_level = jm_log_level_all;
  model->callbacks.context = 0;

  // set temp directory
  model->tempDir = fmi_import_mk_temp_dir(&model->callbacks, oms2::Scope::GetInstance().getTempDirectory().c_str(), "temp_");
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

  // check FMU kind (CS or ME)
  model->fmuKind = fmi2_import_get_fmu_kind(model->fmu);
  if (fmi2_fmu_kind_me == model->fmuKind)
    logDebug("FMU ME");
  else if (fmi2_fmu_kind_cs == model->fmuKind)
    logDebug("FMU CS");
  else if (fmi2_fmu_kind_me_and_cs == model->fmuKind)
    logDebug("FMU ME & CS");
  else
  {
    logError("Unsupported FMU kind: " + std::string(fmi2_fmu_kind_to_string(model->fmuKind)));
    delete model;
    return NULL;
  }

  model->callBackFunctions.logger = oms2::fmi2logger;
  model->callBackFunctions.allocateMemory = calloc;
  model->callBackFunctions.freeMemory = free;
  model->callBackFunctions.componentEnvironment = model->fmu;
  model->callBackFunctions.stepFinished = NULL;

  if (fmi2_fmu_kind_me == model->fmuKind)
  {
    jm_status_enu_t jmstatus;

    // load the FMU's shared library
    jmstatus = fmi2_import_create_dllfmu(model->fmu, fmi2_fmu_kind_me, &model->callBackFunctions);
    if (jm_status_error == jmstatus)
    {
      logError("Could not create the DLL loading mechanism (C-API). Error: " + std::string(fmi2_import_get_last_error(model->fmu)));
      delete model;
      return NULL;
    }

    //logDebug("Version returned from FMU: " + std::string(fmi2_import_get_version(model->fmu)));
    //logDebug("Platform type returned: " + std::string(fmi2_import_get_types_platform(model->fmu)));
    //logDebug("GUID: " + std::string(fmi2_import_get_GUID(model->fmu)));

    jmstatus = fmi2_import_instantiate(model->fmu, cref.toString().c_str(), fmi2_model_exchange, NULL, fmi2_false);
    if (jm_status_error == jmstatus)
    {
      logError("fmi2_import_instantiate failed");
      delete model;
      return NULL;
    }
  }
  else if (fmi2_fmu_kind_cs == model->fmuKind || fmi2_fmu_kind_me_and_cs == model->fmuKind)
  {
    jm_status_enu_t jmstatus;

    // load the FMU shared library
    jmstatus = fmi2_import_create_dllfmu(model->fmu, fmi2_fmu_kind_cs, &model->callBackFunctions);
    if (jm_status_error == jmstatus)
    {
      logError("Could not create the DLL loading mechanism (C-API). Error: " + std::string(fmi2_import_get_last_error(model->fmu)));
      delete model;
      return NULL;
    }

    //logDebug("Version returned from FMU: " + std::string(fmi2_import_get_version(model->fmu)));
    //logDebug("Platform type returned: " + std::string(fmi2_import_get_types_platform(model->fmu)));
    //logDebug("GUID: " + std::string(fmi2_import_get_GUID(model->fmu)));

    jmstatus = fmi2_import_instantiate(model->fmu, cref.toString().c_str(), fmi2_cosimulation, NULL, fmi2_false);
    if (jm_status_error == jmstatus)
    {
      logError("fmi2_import_instantiate failed");
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

    if (v.isInput())
      model->inputs.push_back(v);
    else if (v.isOutput())
      model->outputs.push_back(v);
    else if (v.isParameter())
      model->parameters.push_back(v);
  }

  std::vector<oms2::Connector> connectors;
  for (auto const &v : model->inputs)
  {
    oms2::Connector c(oms_causality_input, v.getType(), v.getSignalRef());
    connectors.push_back(c);
  }
  for (auto const &v : model->outputs)
  {
    oms2::Connector c(oms_causality_output, v.getType(), v.getSignalRef());
    connectors.push_back(c);
  }
  for (auto const &v : model->parameters)
  {
    oms2::Connector c(oms_causality_parameter, v.getType(), v.getSignalRef());
    connectors.push_back(c);
  }
  model->element.setInterfaces(connectors);

  return model;
}

oms_status_enu_t oms2::FMUWrapper::exportToSSD(pugi::xml_node& root) const
{
  oms_status_enu_t status = oms_status_ok;

  oms2::ComRef cref = element.getName();
  pugi::xml_node subModel = root.append_child("ssd:Component");
  subModel.append_attribute("name") = cref.last().toString().c_str();

  // export ssd:ElementGeometry
  status = element.getGeometry()->exportToSSD(subModel);
  if (oms_status_ok != status)
    return status;

  subModel.append_attribute("type") = "application/x-fmu-sharedlibrary";

  const std::string& fmuPath = getFMUPath();
  subModel.append_attribute("source") = fmuPath.c_str();

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

  return status;
}

oms2::Variable* oms2::FMUWrapper::getVar(const std::string& var)
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
  {
    logError("No such parameter: " + var);
    return oms_status_error;
  }

  it->second = value;
  return oms_status_ok;
}

oms_status_enu_t oms2::FMUWrapper::getRealParameter(const std::string& var, double& value)
{
  auto it = realParameters.find(var);
  if (realParameters.end() == it)
  {
    logError("No such parameter: " + var);
    return oms_status_error;
  }

  if (it->second.isNone())
  {
    oms2::Variable *v = getVar(var);
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

oms_status_enu_t oms2::FMUWrapper::setReal(const oms2::Variable& var, double realValue)
{
  logTrace();
  if (!fmu || !var.isTypeReal())
  {
    logError("oms2::FMUWrapper::setReal failed");
    return oms_status_error;
  }

  fmi2_value_reference_t vr = var.getValueReference();
  if (fmi2_status_ok != fmi2_import_set_real(fmu, &vr, 1, &realValue))
    return oms_status_ok;

  return oms_status_error;
}

oms_status_enu_t oms2::FMUWrapper::getReal(const oms2::Variable& var, double& realValue)
{
  logTrace();
  if (!fmu || !var.isTypeReal())
  {
    logError("oms2::FMUWrapper::getReal failed");
    return oms_status_error;
  }

  fmi2_value_reference_t vr = var.getValueReference();
  if (fmi2_status_ok != fmi2_import_get_real(fmu, &vr, 1, &realValue))
    return oms_status_ok;

  return oms_status_error;
}

