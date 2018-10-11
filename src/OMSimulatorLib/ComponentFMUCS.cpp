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
  : oms3::Component(cref, oms_component_fmu, parentSystem, fmuPath)
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
    component->connectors.push_back(new Connector(oms_causality_input, v.getType(), v.getCref() + ComRef(v.getName()), i++/(double)size));
  i = 1;
  size = 1 + component->outputs.size();
  for (const auto& v : component->outputs)
    component->connectors.push_back(new Connector(oms_causality_output, v.getType(), v.getCref() + ComRef(v.getName()), i++/(double)size));
  for (const auto& v : component->parameters)
    component->connectors.push_back(new Connector(oms_causality_parameter, v.getType(), v.getCref() + ComRef(v.getName())));
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
        ComRef cref = ComRef(itConnectors->attribute("name").as_string());
        std::string causalityString = itConnectors->attribute("kind").as_string();
        std::string typeString = itConnectors->attribute("type").as_string();
        oms_causality_enu_t causality = oms_causality_undefined;
        if (causalityString == "input")
          causality = oms_causality_input;
        else if (causalityString == "output")
          causality = oms_causality_output;
        else if (causalityString == "parameter")
          causality = oms_causality_parameter;
        else
        {
          logError("Failed to import " + std::string(oms2::ssd::ssd_connector) + ":causality");
          delete component;
          return NULL;
        }
        oms_signal_type_enu_t type = oms_signal_type_real;
        if (typeString == "Real")
          type = oms_signal_type_real;
        else if (typeString == "Integer")
          type = oms_signal_type_integer;
        else if (typeString == "Boolean")
          type = oms_signal_type_boolean;
        else
        {
          logError("Failed to import " + std::string(oms2::ssd::ssd_connector) + ":type");
          delete component;
          return NULL;
        }

        Connector* connector = new Connector(causality, type, cref);

        if (!connector)
        {
          logError("Failed to import " + std::string(oms2::ssd::ssd_connector));
          delete component;
          return NULL;
        }
        else
        {
          // Load connector geometry
          pugi::xml_node connectorGeometryNode = itConnectors->child(oms2::ssd::ssd_connector_geometry);
          if (connectorGeometryNode)
          {
            oms2::ssd::ConnectorGeometry geometry(0.0, 0.0);
            geometry.setPosition(connectorGeometryNode.attribute("x").as_double(), connectorGeometryNode.attribute("y").as_double());
            connector->setGeometry(&geometry);
          }
        }

        component->connectors.push_back(connector);
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
