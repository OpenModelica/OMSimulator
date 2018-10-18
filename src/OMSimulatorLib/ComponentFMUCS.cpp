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
#include <OMSBoost.h>
#include <fmilib.h>
#include <JM/jm_portability.h>

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
    boost::filesystem::copy_file(fmuPath, absFMUPath, boost::filesystem::copy_option::overwrite_if_exists);

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
    oms3::Variable v(cref, var, i + 1);
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
      component->realParameters[v.getName()] = oms3::Option<double>();
    else if (v.isParameter() && v.isTypeInteger())
      component->integerParameters[v.getName()] = oms3::Option<int>();
    else if (v.isParameter() && v.isTypeBoolean())
      component->booleanParameters[v.getName()] = oms3::Option<bool>();

    if (v.isInput())
    {
      component->inputs.push_back(v);
    }
    else if (v.isOutput())
    {
      component->outputs.push_back(v);
      component->outputsGraph.addVariable(v);
    }
    else if (v.isParameter())
      component->parameters.push_back(v);

    if (v.isInitialUnknown())
      component->initialUnknownsGraph.addVariable(v);

    component->exportVariables.push_back(v.isInput() || v.isOutput());
  }

  // create connectors
  int i = 1;
  int size = 1 + component->inputs.size();
  for (const auto& v : component->inputs)
    component->connectors.push_back(Connector::NewConnector(oms_causality_input, v.getType(), ComRef(v.getName()), i++/(double)size));
  i = 1;
  size = 1 + component->outputs.size();
  for (const auto& v : component->outputs)
    component->connectors.push_back(Connector::NewConnector(oms_causality_output, v.getType(), ComRef(v.getName()), i++/(double)size));
  for (const auto& v : component->parameters)
    component->connectors.push_back(Connector::NewConnector(oms_causality_parameter, v.getType(), ComRef(v.getName())));
  component->connectors.push_back(NULL);
  component->element.setConnectors(&component->connectors[0]);

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
  node.append_attribute("name") = this->getName().c_str();
  node.append_attribute("type") = "application/x-fmu-sharedlibrary";
  node.append_attribute("source") = getPath().c_str();
  pugi::xml_node node_connectors = node.append_child(oms2::ssd::ssd_connectors);

  for (const auto& connector : connectors)
    if (connector)
      if (oms_status_ok != connector->exportToSSD(node_connectors))
        return oms_status_error;
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

  jmstatus = fmi2_import_instantiate(fmu, getName().c_str(), fmi2_cosimulation, NULL, fmi2_false);
  if (jm_status_error == jmstatus)
    return logError("fmi2_import_instantiate failed");

  // enterInitialization
  fmistatus = fmi2_import_setup_experiment(fmu, fmi2_true, getParentSystem()->getTolerance(), getParentSystem()->getModel()->getStartTime(), fmi2_false, 1.0);
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
    return logError_Termination(getName());
  return oms_status_ok;
}
