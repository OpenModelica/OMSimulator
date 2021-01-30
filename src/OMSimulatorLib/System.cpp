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

#include "System.h"

#include "AlgLoop.h"
#include "Component.h"
#include "ComponentFMUCS.h"
#include "ComponentFMUME.h"
#include "ComponentTable.h"
#include "ExternalModel.h"
#include "Flags.h"
#include "Model.h"
#include "ResultWriter.h"
#include "ssd/Tags.h"
#include "SystemSC.h"
#include "SystemTLM.h"
#include "SystemWC.h"
#include "Types.h"
#include "Variable.h"
#include <OMSFileSystem.h>
#include <RegEx.h>

oms::System::System(const oms::ComRef& cref, oms_system_enu_t type, oms::Model* parentModel, oms::System* parentSystem, oms_solver_enu_t solverMethod)
  : element(oms_element_system, cref), cref(cref), type(type), parentModel(parentModel), parentSystem(parentSystem), solverMethod(solverMethod)
{
  connections.push_back(NULL);

  connectors.push_back(NULL);
  element.setConnectors(&connectors[0]);

  busconnectors.push_back(NULL);
  element.setBusConnectors(&busconnectors[0]);

#if !defined(NO_TLM)
  tlmbusconnectors.push_back(NULL);
  element.setTLMBusConnectors(&tlmbusconnectors[0]);
#endif

  subelements.push_back(NULL);
  element.setSubElements(&subelements[0]);
}

oms::System::~System()
{
  for (const auto& connector : connectors)
    if (connector)
      delete connector;

  for (const auto& connection : connections)
    if (connection)
      delete connection;

  for (const auto& component : components)
    delete component.second;

  for (const auto& subsystem : subsystems)
    delete subsystem.second;

  for (const auto& busconnector : busconnectors)
    if (busconnector)
      delete busconnector;

#if !defined(NO_TLM)
  for (const auto tlmbusconnector : tlmbusconnectors)
    if (tlmbusconnector)
      delete tlmbusconnector;
#endif
}

oms::System* oms::System::NewSystem(const oms::ComRef& cref, oms_system_enu_t type, oms::Model* parentModel, oms::System* parentSystem)
{
  if (!cref.isValidIdent())
  {
    logError_InvalidIdent(cref);
    return NULL;
  }

  if (parentSystem && parentSystem->getSystem(cref))
  {
    logError("Name is already in use");
    return NULL;
  }

  if (parentSystem && parentSystem->getComponent(cref))
  {
    logError("Name is already in use");
    return NULL;
  }

  if ((parentModel && parentSystem) || (!parentModel && !parentSystem))
  {
    logError_InternalError;
    return NULL;
  }

  switch (type)
  {
  case oms_system_tlm:
    if (parentSystem)
    {
      logError("A TLM system must be the the root system of a model.");
      return NULL;
    }
#if !defined(NO_TLM)
    return SystemTLM::NewSystem(cref, parentModel, parentSystem);
#else
    return nullptr;
#endif

  case oms_system_wc:
    if (parentSystem && oms_system_tlm != parentSystem->getType())
    {
      logError("A WC system must be the root system or a subsystem of a TLM system.");
      return NULL;
    }
    return SystemWC::NewSystem(cref, parentModel, parentSystem);

  case oms_system_sc:
    if (parentSystem && oms_system_wc != parentSystem->getType())
    {
      logError("A SC system must be the root system or a subsystem of a WC system.");
      return NULL;
    }
    return SystemSC::NewSystem(cref, parentModel, parentSystem);
  default:
      logError_InternalError;
  return NULL;
  }
}

oms::ComRef oms::System::getFullCref() const
{
  if (parentSystem)
    return parentSystem->getFullCref() + this->getCref();
  if (parentModel)
    return ComRef(parentModel->getCref()) + this->getCref();

  logError_InternalError;
  return this->getCref();
}

oms::System* oms::System::getSystem(const oms::ComRef& cref)
{
  if (cref.isEmpty())
    return this;

  oms::ComRef tail(cref);
  oms::ComRef front = tail.pop_front();

  auto it = subsystems.find(front);
  if (it == subsystems.end())
    return NULL;

  return it->second->getSystem(tail);
}

oms::Component* oms::System::getComponent(const oms::ComRef& cref)
{
  oms::System* system = NULL;

  oms::ComRef tail(cref);
  oms::ComRef front = tail.pop_front();

  auto subsystem = subsystems.find(front);
  if (subsystem != subsystems.end())
    return subsystem->second->getComponent(tail);

  auto component = components.find(cref);
  if (component == components.end())
    return NULL;

  return component->second;
}

oms::System *oms::System::getSubSystem(const oms::ComRef &cref)
{
  auto it = subsystems.find(cref);
  if (it == subsystems.end())
    return NULL;

  return it->second;
}

bool oms::System::validCref(const oms::ComRef& cref)
{
  if (!cref.isValidIdent())
    return false;

  if (getSystem(cref))
    return false;

  if (getComponent(cref))
    return false;

  if (getConnector(cref))
    return false;

  if (getBusConnector(cref))
    return false;

#if !defined(NO_TLM)
  if (getTLMBusConnector(cref))
    return false;
#endif

  return true;
}

oms_status_enu_t oms::System::addSubSystem(const oms::ComRef& cref, oms_system_enu_t type)
{
  if (cref.isEmpty())
    return logError_AlreadyInScope(getFullCref());

  if (cref.isValidIdent())
  {
    if (this->type == oms_system_sc)
      return logError_NotForScSystem;

    System* system = System::NewSystem(cref, type, NULL, this);
    if (system)
    {
      subsystems[cref] = system;
      subelements.back() = reinterpret_cast<oms_element_t*>(system->getElement());
      subelements.push_back(NULL);
      element.setSubElements(&subelements[0]);
      return oms_status_ok;
    }
    return oms_status_error;
  }

  ComRef tail(cref);
  ComRef front = tail.pop_front();

  System* system = this->getSystem(front);
  if (!system)
    return logError("System \"" + std::string(getFullCref()) + "\" does not contain system \"" + std::string(front) + "\"");

  return system->addSubSystem(tail, type);
}

oms_status_enu_t oms::System::addSubModel(const oms::ComRef& cref, const std::string& path)
{
  if (cref.isValidIdent())
  {
    if (!validCref(cref))
      return logError_AlreadyInScope(getFullCref() + cref);

    if (!filesystem::exists(path))
      return logError("file does not exist: \"" + path + "\"");

    Component* component = NULL;

    std::string extension = "";
    if (path.length() > 4)
      extension = path.substr(path.length() - 4);

    if (extension == ".fmu" && oms_system_wc == type)
      component = ComponentFMUCS::NewComponent(cref, this, path);
    else if (extension == ".fmu" && oms_system_sc == type)
      component = ComponentFMUME::NewComponent(cref, this, path);
    else if (extension == ".csv" || extension == ".mat")
      component = ComponentTable::NewComponent(cref, this, path);
    else
      return logError("supported sub-model formats are \".fmu\", \".csv\", \".mat\"");

    if (!component)
      return oms_status_error;

    components[cref] = component;
    subelements.back() = reinterpret_cast<oms_element_t*>(component->getElement());
    subelements.push_back(NULL);
    element.setSubElements(&subelements[0]);
    return oms_status_ok;
  }

  ComRef tail(cref);
  ComRef front = tail.pop_front();

  System* system = this->getSystem(front);
  if (!system)
    return logError("System \"" + std::string(getFullCref()) + "\" does not contain system \"" + std::string(front) + "\"");

  return system->addSubModel(tail, path);
}

oms_status_enu_t oms::System::listUnconnectedConnectors(char** contents) const
{
  if (!contents)
    return oms_status_error;

  std::vector<ComRef> unconnectedInputs;
  std::vector<ComRef> unconnectedOutputs;
  std::vector<ComRef> unconnectedBuses;

  for (const auto& component : components)
  {
    Element* element = component.second->getElement();
    Connector** connectors = element->getConnectors();
    for (int i=0; connectors[i]; ++i)
    {
      ComRef cref(component.second->getCref() + connectors[i]->getName());
      if (!isConnected(cref))
      {
        if (connectors[i]->isInput())
          unconnectedInputs.push_back(getFullCref() + cref);
        if (connectors[i]->isOutput())
          unconnectedOutputs.push_back(getFullCref() + cref);
        if (connectors[i]->isParameter())
          unconnectedOutputs.push_back(getFullCref() + cref);
        if (connectors[i]->isCalculatedParameter())
          unconnectedOutputs.push_back(getFullCref() + cref);
      }
    }
  }

  for (const auto& connector : connectors)
  {
    if (connector && !isConnected(connector->getName()))
    {
      const ComRef& cref = connector->getName();
      if (!isConnected(cref))
      {
        if (connector->isInput())
          unconnectedInputs.push_back(getFullCref() + cref);
        if (connector->isOutput())
          unconnectedOutputs.push_back(getFullCref() + cref);
        if (connector->isParameter())
          unconnectedOutputs.push_back(getFullCref() + cref);
        if (connector->isCalculatedParameter())
          unconnectedOutputs.push_back(getFullCref() + cref);
      }
    }
  }

  for (const auto& busconnector : busconnectors)
    if (busconnector && !isConnected(busconnector->getName()))
      unconnectedBuses.push_back(getFullCref() + busconnector->getName());

#if !defined(NO_TLM)
  for (const auto& tlmbusconnector : tlmbusconnectors)
    if (tlmbusconnector && !isConnected(tlmbusconnector->getName()))
      unconnectedBuses.push_back(getFullCref() + tlmbusconnector->getName());
#endif

  std::string msg;
  for (const auto& unconnectedInput : unconnectedInputs)
    msg += "input " + std::string(unconnectedInput) + "\n";
  for (const auto& unconnectedOutput : unconnectedOutputs)
    msg += "output " + std::string(unconnectedOutput) + "\n";
  for (const auto& unconnectedBus : unconnectedBuses)
    msg += "bus " + std::string(unconnectedBus) + "\n";

  if (!msg.empty())
  {
    *contents = (char*) malloc(msg.length() + 1);
    if (!*contents)
    {
      logError("Out of memory");
      return oms_status_fatal;
    }
    strcpy(*contents, msg.c_str());
  }
  return oms_status_ok;
}

oms_status_enu_t oms::System::exportToSSD(pugi::xml_node& node, pugi::xml_node& ssvNode) const
{
  node.append_attribute("name") = this->getCref().c_str();

  if (oms_status_ok != element.getGeometry()->exportToSSD(node))
    return logError("export of system ElementGeometry failed");

  // export top level system connectors
  if (connectors.size() > 1)
  {
    pugi::xml_node connectors_node = node.append_child(oms::ssp::Draft20180219::ssd::connectors);
    for(const auto& connector : connectors)
    {
      if (connector)
        connector->exportToSSD(connectors_node);
    }
  }

  // export top level parameter bindings
  if (Flags::ExportParametersInline()) // export as inline
  {
    values.exportToSSD(node);
  }
  else
  {
    values.exportToSSV(ssvNode); // export to ssv file
  }

  if (subelements.size() > 1)
  {
    pugi::xml_node elements_node = node.append_child(oms::ssp::Draft20180219::ssd::elements);
    for (const auto& subsystem : subsystems)
    {
      pugi::xml_node system_node = elements_node.append_child(oms::ssp::Draft20180219::ssd::system);
      if (oms_status_ok != subsystem.second->exportToSSD(system_node, ssvNode))
        return logError("export of system failed");
    }
    for (const auto& component : components)
    {
      pugi::xml_node component_node = elements_node.append_child(oms::ssp::Draft20180219::ssd::component);
      if (oms_status_ok != component.second->exportToSSD(component_node, ssvNode))
        return logError("export of component failed");
    }
  }

  std::vector<oms::Connection*> busconnections, ssdconnections;

  for (const auto& connection : connections)
  {
    if (connection && connection->getType() == oms_connection_single)
      ssdconnections.push_back(connection);
    else if (connection)
      busconnections.push_back(connection);
  }

  if (!ssdconnections.empty())
  {
    pugi::xml_node connections_node = node.append_child(oms::ssp::Draft20180219::ssd::connections);
    for (const auto& ssdconnection : ssdconnections)
    {
      ssdconnection->exportToSSD(connections_node);
    }
  }

  pugi::xml_node annotations_node = node.append_child(oms::ssp::Draft20180219::ssd::annotations);
  pugi::xml_node annotation_node = annotations_node.append_child(oms::ssp::Version1_0::ssc::annotation);
  annotation_node.append_attribute("type") = oms::ssp::Draft20180219::annotation_type;
  pugi::xml_node oms_annotation_node = annotation_node.append_child(oms::ssp::Version1_0::oms_annotations);

#if !defined(NO_TLM)
  if (busconnectors[0] || tlmbusconnectors[0] || !busconnections.empty())
#else
  if (busconnectors[0] || !busconnections.empty())
#endif
  {
    if (busconnectors.size() > 1)
    {
      pugi::xml_node oms_buses_node = oms_annotation_node.append_child(oms::ssp::Version1_0::oms_buses);
      for (const auto& busconnector : busconnectors)
      {
        if (busconnector)
          busconnector->exportToSSD(oms_buses_node);
      }
    }
#if !defined(NO_TLM)
    if (tlmbusconnectors.size() > 1)
    {
      pugi::xml_node oms_buses_node = oms_annotation_node.append_child(oms::ssp::Version1_0::oms_buses);
      for (const auto& tlmbusconnector : tlmbusconnectors)
      {
        if (tlmbusconnector)
          tlmbusconnector->exportToSSD(oms_buses_node);
      }
    }
#endif
    if (!busconnections.empty())
    {
      pugi::xml_node busconnections_node = oms_annotation_node.append_child(oms::ssp::Draft20180219::bus_connections);
      for (const auto& busconnection : busconnections)
        busconnection->exportToSSD(busconnections_node);
    }
  }

  //export ssd:SimulationInformation to end, in order to make it valid with easy-ssp
  if (oms_status_ok != this->exportToSSD_SimulationInformation(oms_annotation_node))
    return logError("export of system SimulationInformation failed");

  return oms_status_ok;
}

oms_status_enu_t oms::System::importFromSSD(const pugi::xml_node& node, const std::string& sspVersion)
{
  for(pugi::xml_node_iterator it = node.begin(); it != node.end(); ++it)
  {
    std::string name = it->name();
    if (name == oms::ssp::Draft20180219::ssd::simulation_information && sspVersion == "Draft20180219")
    {
      if (oms_status_ok != importFromSSD_SimulationInformation(*it, sspVersion))
        return logError("Failed to import " + std::string(oms::ssp::Draft20180219::ssd::simulation_information));
    }
    else if (name == oms::ssp::Draft20180219::ssd::element_geometry)
    {
      oms::ssd::ElementGeometry geometry;
      geometry.importFromSSD(*it);
      setGeometry(geometry);
    }
    else if(name == oms::ssp::Version1_0::ssd::parameter_bindings)
    {
      std::string parent_node = it->parent().parent().name();
      // top level parameter bindings belonging to <ssd:SystemStructureDescription> provided either as inline or .ssv files
      if (parent_node == oms::ssp::Draft20180219::ssd::system_structure_description)
      {
        // check for multiple ssv files or parameter bindings
        for (pugi::xml_node parameterBindingNode = it->child(oms::ssp::Version1_0::ssd::parameter_binding); parameterBindingNode; parameterBindingNode = parameterBindingNode.next_sibling(oms::ssp::Version1_0::ssd::parameter_binding))
        {
          std::string ssvFileSource = parameterBindingNode.attribute("source").as_string();

          // set parameter bindings associated with the system
          if (ssvFileSource.empty()) // inline parameterBinding
          {
            std::string tempdir = getModel()->getTempDirectory();
            if (oms_status_ok != values.importFromSSD(*it, sspVersion, tempdir))
              return logError("Failed to import " + std::string(oms::ssp::Version1_0::ssd::parameter_bindings));
          }
          else
          {
            // store the ssv and ssm files and process it later while handling the connection, so all the components are loaded
            pugi::xml_node parameterMapping = parameterBindingNode.child(oms::ssp::Version1_0::ssd::parameter_mapping);

            // check for parameterMapping (e.g) <ssd:ParameterMapping>
            if (parameterMapping)
            {
              // parameterMapping provided
              std::string ssmFileSource = parameterMapping.attribute("source").as_string();
              startValuesFileSources[ssvFileSource] = ssmFileSource;
            }
            else
            {
              // no parameterMapping
              startValuesFileSources[ssvFileSource] = "";
            }
          }
        }
      }
      else
      // heirarchial level parameter bindings belonging to <ssd:Elements> provided either as inline or .ssv files
      {
        if (oms_status_ok !=  values.importFromSSD(*it, sspVersion, getModel()->getTempDirectory()))
            return logError("Failed to import " + std::string(oms::ssp::Version1_0::ssd::parameter_bindings));
      }
    }
    else if (name == oms::ssp::Draft20180219::ssd::connections)
    {
      for(pugi::xml_node_iterator itConnectors = (*it).begin(); itConnectors != (*it).end(); ++itConnectors)
      {
        ComRef startElement = ComRef(itConnectors->attribute("startElement").as_string());
        ComRef startConnector = ComRef(itConnectors->attribute("startConnector").as_string());
        ComRef endElement = ComRef(itConnectors->attribute("endElement").as_string());
        ComRef endConnector = ComRef(itConnectors->attribute("endConnector").as_string());
        ComRef crefA = startConnector;
        if (!startElement.isEmpty())
          crefA = startElement + startConnector;
        ComRef crefB = endConnector;
        if (!endElement.isEmpty())
          crefB = endElement + endConnector;
        if (oms_status_ok != addConnection(crefA, crefB))
          return logError("Failed to import " + std::string(oms::ssp::Draft20180219::ssd::connection));
        else
        {
          // Load connection geometry
          if (oms_status_ok != importFromSSD_ConnectionGeometry(*itConnectors, crefA, crefB))
            return logError("Failed to import " + std::string(oms::ssp::Draft20180219::ssd::connection_geometry));
        }
      }
    }
    else if (name == oms::ssp::Draft20180219::ssd::connectors)
    {
      for(pugi::xml_node_iterator itConnectors = (*it).begin(); itConnectors != (*it).end(); ++itConnectors)
      {
        connectors.back() = oms::Connector::NewConnector(*itConnectors, sspVersion);
        if (connectors.back())
        {
          //save the connectors with full cref
          exportConnectors[getFullCref() + connectors.back()->getName()] = true;
          connectors.push_back(NULL);
        }
        else
          return logError("Failed to import ssd:connector");
        element.setConnectors(&connectors[0]);
      }
    }
    else if (name == oms::ssp::Draft20180219::ssd::elements)
    {
      for(pugi::xml_node_iterator itElements = (*it).begin(); itElements != (*it).end(); ++itElements)
      {
        name = itElements->name();
        if (name == oms::ssp::Draft20180219::ssd::system)
        {
          ComRef systemCref = ComRef(itElements->attribute("name").as_string());

          // lochel: I guess that can somehow be improved
          oms_system_enu_t systemType = getModel()->getSystemType(*itElements, sspVersion);

          if (oms_status_ok != addSubSystem(systemCref, systemType))
            return oms_status_error;

          System* system = getSystem(systemCref);
          if (!system)
            return oms_status_error;

          if (oms_status_ok != system->importFromSSD(*itElements, sspVersion))
            return oms_status_error;
        }
        else if (name == oms::ssp::Draft20180219::ssd::component)
        {
          Component* component = NULL;
          std::string type = itElements->attribute("type").as_string();
          // allow component type to be empty, as type is optional according to SSP-1.0 and default type is application/x-fmu-sharedlibrary
          if ("application/x-fmu-sharedlibrary" == type || type.empty() && getType() != oms_system_tlm)
          {
            if (getType() == oms_system_wc)
              component = ComponentFMUCS::NewComponent(*itElements, this, sspVersion);
            else if (getType() == oms_system_sc)
              component = ComponentFMUME::NewComponent(*itElements, this, sspVersion);
            else
              return logError("wrong xml schema detected: " + name);
          }
          else if ("application/table" == type)
            component = ComponentTable::NewComponent(*itElements, this, sspVersion);
#if !defined(NO_TLM)
          else if (itElements->attribute("type") == nullptr && getType() == oms_system_tlm) {
            std::string name = itElements->attribute("name").as_string();
            std::string source = itElements->attribute("source").as_string();

            // parse older <ssd:SimulationInformation> <ssd:annotations> </ssd:annotations> </ssd:SimulationInformation>
            if (sspVersion == "Draft20180219")
            {
              pugi::xml_node simulationInformationNode = itElements->child(oms::ssp::Draft20180219::ssd::simulation_information);
              if (simulationInformationNode && sspVersion == "Draft20180219")
              {
                pugi::xml_node annotationsNode = simulationInformationNode.child(oms::ssp::Draft20180219::ssd::annotations);
                if (annotationsNode)
                {
                  if (annotationsNode.child(oms::ssp::Draft20180219::ssd::annotation))
                  {
                    logWarning_deprecated;
                  }
                  for (pugi::xml_node annotationNode = annotationsNode.child(oms::ssp::Draft20180219::ssd::annotation); annotationNode; annotationNode = annotationsNode.next_sibling(oms::ssp::Draft20180219::ssd::annotation))
                  {
                    std::string type = annotationNode.attribute("type").as_string();
                    if (oms::ssp::Draft20180219::annotation_type == type)
                    {
                      pugi::xml_node externalModelNode = annotationNode.child(oms::ssp::Draft20180219::external_model);
                      if (externalModelNode)
                      {
                        std::string startScript = externalModelNode.attribute("startscript").as_string();
                        component = oms::ExternalModel::NewComponent(name, this, source, startScript);
                      }
                    }
                  }
                }
              }
            }

            // sspVersion-1.0
            pugi::xml_node annotationsNode = itElements->child(oms::ssp::Draft20180219::ssd::annotations);
            pugi::xml_node annotation_node = annotationsNode.child(oms::ssp::Version1_0::ssc::annotation);

            // check for ssd:annotation to support older version, which is a bug
            if(!annotation_node)
            {
              annotation_node = annotationsNode.child(oms::ssp::Draft20180219::ssd::annotation);
              logWarning_deprecated;
            }

            if (annotation_node && std::string(annotation_node.attribute("type").as_string()) == oms::ssp::Draft20180219::annotation_type)
            {
              pugi::xml_node oms_annotation_node = annotation_node.child(oms::ssp::Version1_0::oms_annotations);
              // support older <ssc:annotation>
              if (!oms_annotation_node)
              {
                oms_annotation_node = annotation_node;
                logWarning_deprecated;
              }

              if (oms_annotation_node)
              {
                pugi::xml_node oms_simulation_information = oms_annotation_node.child(oms::ssp::Version1_0::simulation_information);
                if (oms_simulation_information)
                {
                  pugi::xml_node externalModelNode = oms_simulation_information.child(oms::ssp::Draft20180219::external_model);
                  if (externalModelNode)
                  {
                    std::string startScript = externalModelNode.attribute("startscript").as_string();
                    component = oms::ExternalModel::NewComponent(name, this, source, startScript);
                  }
                }
                // parse <oms:Buses>
                for (pugi::xml_node_iterator itAnnotations = oms_annotation_node.begin(); itAnnotations != oms_annotation_node.end(); ++itAnnotations)
                {
                  std::string nodeName = itAnnotations->name();
                  // support older <oms:bus>
                  if (nodeName == oms::ssp::Draft20180219::bus)
                  {
                    importTLMBus(*itAnnotations, component);
                  }
                  // <oms:buses>
                  if (nodeName == oms::ssp::Version1_0::oms_buses)
                  {
                    for (pugi::xml_node_iterator itbuses = itAnnotations->begin(); itbuses != itAnnotations->end(); ++itbuses)
                    {
                      if (std::string(itbuses->name()) == oms::ssp::Draft20180219::bus)
                      {
                        importTLMBus(*itbuses, component);
                      }
                    }
                  }
                }
              }
            }
          }
#endif
          if (component)
          {
            components[component->getCref()] = component;
            subelements.back() = reinterpret_cast<oms_element_t*>(component->getElement());
            subelements.push_back(NULL);
            element.setSubElements(&subelements[0]);
          }
          else
            return oms_status_error;
        }
        else
          return logError("wrong xml schema detected: " + name);
      }

      // check for ssv FileSource exist and set the values before the connections
      if (!startValuesFileSources.empty())
      {
        importStartValuesFromSSV();
      }
    }
    else if (name == oms::ssp::Draft20180219::ssd::annotations)
    {
      pugi::xml_node annotation_node = it->child(oms::ssp::Version1_0::ssc::annotation);

      // check for ssd:annotation to support older version, which is a bug
      if(!annotation_node)
      {
        annotation_node = it->child(oms::ssp::Draft20180219::ssd::annotation);
        logWarning_deprecated;
      }

      if (annotation_node && std::string(annotation_node.attribute("type").as_string()) == oms::ssp::Draft20180219::annotation_type)
      {
        pugi::xml_node oms_annotation_node = annotation_node.child(oms::ssp::Version1_0::oms_annotations);

        // support older <ssc:annotation>
        if (!oms_annotation_node)
        {
          oms_annotation_node = annotation_node;
          logWarning_deprecated;
        }

        // parse <oms:annotations>
        for(pugi::xml_node_iterator itAnnotations = oms_annotation_node.begin(); itAnnotations != oms_annotation_node.end(); ++itAnnotations)
        {
          name = itAnnotations->name();

          // check for oms:simulationInformation from version 1.0
          if (std::string(name) == oms::ssp::Version1_0::simulation_information && sspVersion == "1.0")
          {
            if (oms_status_ok != importFromSSD_SimulationInformation(*itAnnotations, sspVersion))
              return logError("Failed to import " + std::string(oms::ssp::Version1_0::simulation_information));
          }

          // support older <ssc:annotation> <oms:bus> </oms:bus> </ssc:annotation>
          if (std::string(name) == oms::ssp::Draft20180219::bus)
          {
            //Load bus connector
            std::string busname = itAnnotations->attribute("name").as_string();
            if (std::string(itAnnotations->attribute("type").as_string()) == "tlm")
            {
              importTLMBus(*itAnnotations, NULL);
            }
            else
            {
              if (oms_status_ok != addBus(busname))
                return oms_status_error;
            }
            //Load bus connector signals
            importBusConnectorSignals (*itAnnotations);

            // Load bus connector geometry
            importBusConnectorGeometry(*itAnnotations);
          }

          // support <ssc:annotation> <oms:buses> <oms:bus> </oms:bus> </oms:buses></ssc:annotation>
          if (std::string(name) == oms::ssp::Version1_0::oms_buses)
          {
            for(pugi::xml_node_iterator itbuses = itAnnotations->begin(); itbuses != itAnnotations->end(); ++itbuses)
            {
              if (std::string(itbuses->name()) == oms::ssp::Draft20180219::bus)
              {
                //Load bus connector
                std::string busname = itbuses->attribute("name").as_string();
                if (std::string(itbuses->attribute("type").as_string()) == "tlm")
                {
                  importTLMBus(*itbuses, NULL);
                }
                else
                {
                  if (oms_status_ok != addBus(busname))
                    return oms_status_error;
                }
                //Load bus connector signals
                importBusConnectorSignals(*itbuses);

                // Load bus connector geometry
                importBusConnectorGeometry(*itbuses);
              }
            }
          }
          else if (std::string(name) == oms::ssp::Draft20180219::bus_connections)
          {
            //Load bus connections
            for(pugi::xml_node_iterator itConnection = itAnnotations->begin(); itConnection != itAnnotations->end(); ++itConnection)
            {
              //Load bus connection
              oms::ComRef element1(itConnection->attribute("startElement").as_string());
              oms::ComRef connector1(itConnection->attribute("startConnector").as_string());
              oms::ComRef element2(itConnection->attribute("endElement").as_string());
              oms::ComRef connector2(itConnection->attribute("endConnector").as_string());

              oms_status_enu_t status;
              // Load TLM bus connection
              if (itConnection->attribute("delay") || itConnection->attribute("alpha")
                  || itConnection->attribute("linearimpedance") || itConnection->attribute("angularimpedance"))
              {
#if !defined(NO_TLM)
                double delay = itConnection->attribute("delay").as_double();
                double alpha = itConnection->attribute("alpha").as_double();
                double linearimpedance = itConnection->attribute("linearimpedance").as_double();
                double angularimpedance = itConnection->attribute("angularimpedance").as_double();

                status = addTLMConnection(element1+connector1, element2+connector2, delay, alpha, linearimpedance, angularimpedance);
#else
                return LOG_NO_TLM();
#endif
              }
              else  // Load bus connection
              {
                status = addConnection(element1+connector1, element2+connector2);
              }
              // Load the bus connection geometry
              if (oms_status_ok != status)
                return logError("Failed to import " + std::string(oms::ssp::Draft20180219::bus_connection));
              else
              {
                // Load connection geometry
                if (oms_status_ok != importFromSSD_ConnectionGeometry(*itConnection, element1+connector1, element2+connector2))
                  return logError("Failed to import " + std::string(oms::ssp::Draft20180219::ssd::connection_geometry));
              }
            }
          }
        }
      }
    }
    else
      return logError_WrongSchema(name);
  }

  return oms_status_ok;
}

oms_status_enu_t oms::System::addConnector(const oms::ComRef& cref, oms_causality_enu_t causality, oms_signal_type_enu_t type)
{
  oms::ComRef tail(cref);
  oms::ComRef head = tail.pop_front();
  auto subsystem = subsystems.find(head);
  if (subsystem != subsystems.end())
    return subsystem->second->addConnector(tail, causality, type);

  if (this->type == oms_system_tlm)
    return logError_NotForTlmSystem;
  if (!cref.isValidIdent())
    return logError_InvalidIdent(cref);
  if (!validCref(cref))
    return logError_AlreadyInScope(getFullCref() + cref);

  connectors.back() = new oms::Connector(causality, type, cref);
  exportConnectors[getFullCref() + connectors.back()->getName()] = true;
  connectors.push_back(NULL);
  element.setConnectors(&connectors[0]);

  return oms_status_ok;
}

oms::Connector* oms::System::getConnector(const oms::ComRef& cref)
{
  oms::ComRef tail(cref);
  oms::ComRef head = tail.pop_front();

  auto subsystem = subsystems.find(head);
  if (subsystem != subsystems.end())
    return subsystem->second->getConnector(tail);

  auto component = components.find(head);
  if (component != components.end())
    return component->second->getConnector(tail);

  for (auto& connector : connectors)
    if (connector && connector->getName() == cref)
      return connector;

  return NULL;
}

std::string oms::System::getConnectorOwner(const oms::ComRef& cref) const
{
  return cref.isValidIdent() ? "System" : "Element";
}

oms::BusConnector* oms::System::getBusConnector(const oms::ComRef& cref)
{
  oms::ComRef tail(cref);
  oms::ComRef head = tail.pop_front();
  auto subsystem = subsystems.find(head);
  if (subsystem != subsystems.end())
    return subsystem->second->getBusConnector(tail);

  if (!cref.isValidIdent())
  {
    logError_InvalidIdent(cref);
    return NULL;
  }

  for(auto& busconnector : busconnectors)
    if (busconnector && busconnector->getName() == cref)
      return busconnector;

  return NULL;
}

#if !defined(NO_TLM)
oms::TLMBusConnector* oms::System::getTLMBusConnector(const oms::ComRef& cref)
{
  oms::ComRef tail(cref);
  oms::ComRef head = tail.pop_front();
  auto subsystem = subsystems.find(head);
  if (subsystem != subsystems.end())
    return subsystem->second->getTLMBusConnector(tail);
  auto component = components.find(head);
  if (component != components.end())
    return component->second->getTLMBusConnector(tail);

  if (!cref.isValidIdent())
  {
    logError_InvalidIdent(cref);
    return NULL;
  }

  for(auto& tlmbusconnector : tlmbusconnectors)
    if (tlmbusconnector && tlmbusconnector->getName() == cref)
      return tlmbusconnector;

  return NULL;
}
#endif

oms::Connection** oms::System::getConnections(const oms::ComRef& cref)
{
  if (!cref.isEmpty())
  {
    oms::ComRef tail(cref);
    oms::ComRef head = tail.pop_front();
    auto subsystem = subsystems.find(head);
    if (subsystem != subsystems.end())
      return subsystem->second->getConnections(tail);
  }

  return &connections[0];
}

oms_status_enu_t oms::System::addConnection(const oms::ComRef& crefA, const oms::ComRef& crefB)
{
  oms::ComRef tailA(crefA);
  oms::ComRef headA = tailA.pop_front();

  oms::ComRef tailB(crefB);
  oms::ComRef headB = tailB.pop_front();

  // if both A and B references the same subsystem, recurse into that subsystem
  if (headA == headB)
  {
    auto subsystem = subsystems.find(headA);
    if (subsystem != subsystems.end())
      return subsystem->second->addConnection(tailA,tailB);
  }

#if !defined(NO_TLM)
  // check if it is an internal connection between TLM buses
  TLMBusConnector* tlmA = getTLMBusConnector(crefA);
  TLMBusConnector* tlmB = getTLMBusConnector(crefB);
  if(tlmA && tlmB)
  {
    if (getConnection(crefA, crefB))
      return logError_ConnectionExistsAlready(crefA, crefB, this);

    // create connection between TLM buses (NOT a TLM connection)
    connections.back() = new oms::Connection(crefA, crefB, oms_connection_bus);
    connections.push_back(NULL);
    return oms_status_ok;
  }
#endif

  // first check if it is a bus connection
  BusConnector* busA = NULL;
  auto subsystemA = subsystems.find(headA);
  if (subsystemA != subsystems.end())
    busA = subsystemA->second->getBusConnector(tailA);

  BusConnector* busB = NULL;
  auto subsystemB = subsystems.find(headB);
  if (subsystemB != subsystems.end())
    busB = subsystemB->second->getBusConnector(tailB);

  if (busA && busB)
  {
    if (getConnection(crefA, crefB))
      return logError_ConnectionExistsAlready(crefA, crefB, this);

    // create bus connection
    connections.back() = new oms::Connection(crefA, crefB, oms_connection_bus);
    connections.push_back(NULL);
    return oms_status_ok;
  }

  // not a bus connection, attempt normal connection
  oms::Connector* conA = this->getConnector(crefA);
  if (!conA) return logError_ConnectorNotInSystem(crefA, this);
  oms::Connector* conB = this->getConnector(crefB);
  if (!conB) return logError_ConnectorNotInSystem(crefB, this);

  if (getConnection(crefA, crefB))
    return logError_ConnectionExistsAlready(crefA, crefB, this);

  auto componentA = getComponent(headA);
  if(componentA && componentA->getType() == oms_component_table)
  {
    // allow non-real connections to tables
  }
  else if(conA->getType() == oms_signal_type_integer && conB->getType() == oms_signal_type_enum)
  {
    // allow integer connection to enum types
  }
  else if (conA->getType() != conB->getType())
  {
    return logError("Type mismatch in connection: " + std::string(crefA) + " -> " + std::string(crefB));
  }

  // Do not allow multiple connections to same 'input' connector
  // (signal B). The 'input' connector (signal B) can actually be an
  // output connector, e.g. if connecting a component to a system
  // connector.
  for (auto& connection : connections)
  {
    if (connection && connection->containsSignalB(crefB))
      return logError("Connector " + std::string(crefB) + " is already connected to " + std::string(connection->getSignalA()));
  }

  // check if the connections are valid, according to the SSP-1.0 allowed connection table
  if (oms::Connection::isValid(crefA, crefB, *conA, *conB))
  {
    connections.back() = new oms::Connection(crefA, crefB);
    connections.push_back(NULL);
  }
  // flipped causality check
  else if (oms::Connection::isValid(crefB, crefA, *conB, *conA))
  {
    // ! Flipped connection checks !
    // Do not allow multiple connections to same 'input' connector
    // (signal A!). The 'input' connector (signal A!) can actually be
    // an output connector, e.g. if connecting a component to a system
    // connector.
    for (auto &connection : connections)
    {
      if (connection && connection->containsSignalB(crefA))
        return logError("Connector " + std::string(crefA) + " is already connected to " + std::string(connection->getSignalA()));
    }
    connections.back() = new oms::Connection(crefB, crefA);
    connections.push_back(NULL);
  }
  else
    return logError("Causality mismatch in connection: " + std::string(crefA) + " -> " + std::string(crefB));

  return oms_status_ok;
}

oms_status_enu_t oms::System::deleteConnection(const oms::ComRef& crefA, const oms::ComRef& crefB)
{
  for (auto& connection : connections)
  {
    if (connection && connection->isEqual(crefA, crefB))
    {
      delete connection;

      connections.pop_back();   // last element is always NULL
      connection = connections.back();
      connections.back() = NULL;

      return oms_status_ok;
    }
  }

  oms::ComRef tailA(crefA);
  oms::ComRef headA = tailA.pop_front();

  oms::ComRef tailB(crefB);
  oms::ComRef headB = tailB.pop_front();

  //If both A and B references the same subsystem, recurse into that subsystem
  if (headA == headB)
  {
    auto subsystem = subsystems.find(headA);
    if (subsystem != subsystems.end())
      return subsystem->second->deleteConnection(tailA, tailB);
  }

  return logError_ConnectionNotInSystem(crefA, crefB, this);
}

oms_status_enu_t oms::System::addTLMConnection(const oms::ComRef& crefA, const oms::ComRef& crefB, double delay, double alpha, double linearimpedance, double angularimpedance)
{
#if !defined(NO_TLM)
  if (type != oms_system_tlm)
    return logError_OnlyForSystemTLM;

  oms::ComRef tailA(crefA);
  oms::ComRef headA = tailA.pop_front();

  oms::ComRef tailB(crefB);
  oms::ComRef headB = tailB.pop_front();

  //If both A and B references the same subsystem, recurse into that subsystem
  if (headA == headB)
  {
    auto subsystem = subsystems.find(headA);
    if (subsystem != subsystems.end())
      return subsystem->second->addTLMConnection(tailA,tailB,delay,alpha,linearimpedance,angularimpedance);
  }

  TLMBusConnector *busA=0, *busB=0;
  auto subsystemA = getSubSystem(headA);
  if(subsystemA)
    busA = subsystemA->getTLMBusConnector(tailA);
  if(!busA) {
    auto componentA = getComponent(headA);
    if(componentA)
      busA = componentA->getTLMBusConnector(tailA);
  }

  auto subsystemB = getSubSystem(headB);
  if(subsystemB)
    busB = subsystemB->getTLMBusConnector(tailB);
  if(!busB) {
    auto componentB = getComponent(headB);
    if(componentB)
      busB = componentB->getTLMBusConnector(tailB);
  }

  if (busA && busB)
  {
    //Create bus connection
    connections.back() = new oms::Connection(crefA, crefB, oms_connection_tlm);
    connections.back()->setTLMParameters(delay, alpha, linearimpedance, angularimpedance);
    connections.push_back(NULL);
    busA->setDelay(delay);
    busB->setDelay(delay);
    return oms_status_ok;
  }

  return logError("TLM bus connector(s) not found in system");
#else
    return LOG_NO_TLM();
#endif
}

oms_status_enu_t oms::System::addBus(const oms::ComRef& cref)
{
  oms::ComRef tail(cref);
  oms::ComRef head = tail.pop_front();

  auto subsystem = subsystems.find(head);
  if (subsystem != subsystems.end())
    return subsystem->second->addBus(tail);

  auto component = components.find(head);
  if (component != components.end())
    return logError("Bus connectors cannot be added to components"); ///< TODO: Implement bus connectors for components

  if (type == oms_system_tlm)
    return logError_NotForTlmSystem;
  if (!cref.isValidIdent())
    return logError_InvalidIdent(cref);
  if (!validCref(cref))
    return logError_AlreadyInScope(getFullCref() + cref);

  oms::BusConnector* bus = new oms::BusConnector(cref);
  busconnectors.back() = bus;
  busconnectors.push_back(NULL);
  element.setBusConnectors(&busconnectors[0]);
  return oms_status_ok;
}

oms_status_enu_t oms::System::addTLMBus(const oms::ComRef& cref, oms_tlm_domain_t domain, const int dimensions, const oms_tlm_interpolation_t interpolation)
{
#if !defined(NO_TLM)
  oms::ComRef tail(cref);
  oms::ComRef head = tail.pop_front();

  auto subsystem = subsystems.find(head);
  if (subsystem != subsystems.end())
    return subsystem->second->addTLMBus(tail, domain, dimensions, interpolation);

  auto component = components.find(head);
  if (component != components.end())
  {
    return component->second->addTLMBus(tail, domain, dimensions, interpolation);
  }

  if (type == oms_system_tlm)
    return logError_NotForTlmSystem;
  if (!cref.isValidIdent())
    return logError_InvalidIdent(cref);
  if (!validCref(cref))
    return logError_AlreadyInScope(getFullCref() + cref);

  oms::TLMBusConnector* bus = new oms::TLMBusConnector(cref, domain, dimensions, interpolation, this);
  tlmbusconnectors.back() = bus;
  tlmbusconnectors.push_back(NULL);
  element.setTLMBusConnectors(&tlmbusconnectors[0]);
  return oms_status_ok;
#else
    return LOG_NO_TLM();
#endif
}

oms_status_enu_t oms::System::addConnectorToBus(const oms::ComRef& busCref, const oms::ComRef& connectorCref)
{
  oms::ComRef busTail(busCref);
  oms::ComRef busHead = busTail.pop_front();
  oms::ComRef connectorTail(connectorCref);
  oms::ComRef connectorHead = connectorTail.pop_front();
  //If both bus and connector references the same subsystem, recurse into that subsystem
  if (busHead == connectorHead)
  {
    auto subsystem = subsystems.find(busHead);
    if (subsystem != subsystems.end())
      return subsystem->second->addConnectorToBus(busTail,connectorTail);
  }

  if (!busTail.isEmpty() && !connectorTail.isEmpty() && busHead != connectorHead)
    return logError_BusAndConnectorNotSameSystem(busCref, connectorCref);

  for(auto& bus : busconnectors)
    if (bus && bus->getName() == busCref)
      return bus->addConnector(connectorCref);

  return logError_BusNotInSystem(busCref, this);
}

oms_status_enu_t oms::System::deleteConnectorFromBus(const oms::ComRef& busCref, const oms::ComRef& connectorCref)
{
  oms::ComRef busTail(busCref);
  oms::ComRef busHead = busTail.pop_front();
  oms::ComRef connectorTail(connectorCref);
  oms::ComRef connectorHead = connectorTail.pop_front();
  //If both bus and connector references the same subsystem, recurse into that subsystem
  if(busHead == connectorHead)
  {
    auto subsystem = subsystems.find(busHead);
    if(subsystem != subsystems.end())
      return subsystem->second->deleteConnectorFromBus(busTail,connectorTail);
  }

  if(!busTail.isEmpty() && !connectorTail.isEmpty() && busHead != connectorHead)
    return logError_BusAndConnectorNotSameSystem(busCref, connectorCref);

  for(auto& bus : busconnectors)
    if(bus && bus->getName() == busCref)
      return bus->deleteConnector(connectorCref);

  return logError_BusNotInSystem(busCref, this);
}

oms_status_enu_t oms::System::addConnectorToTLMBus(const oms::ComRef& busCref, const oms::ComRef& connectorCref, const std::string type)
{
#if !defined(NO_TLM)
  oms::ComRef busTail(busCref);
  oms::ComRef busHead = busTail.pop_front();
  oms::ComRef connectorTail(connectorCref);
  oms::ComRef connectorHead = connectorTail.pop_front();
  //If both bus and connector references the same subsystem, recurse into that subsystem
  if (busHead == connectorHead)
  {
    auto subsystem = subsystems.find(busHead);
    if (subsystem != subsystems.end())
      return subsystem->second->addConnectorToTLMBus(busTail,connectorTail,type);

    auto component = components.find(busHead);
    if (component != components.end())
      return component->second->addConnectorToTLMBus(busTail,connectorTail,type);
  }

  //Check that connector exists in system
  bool found = false;
  for(auto& connector : connectors)
    if (connector && connector->getName() == connectorCref)
      found = true;
  if (!found)
    return logError_ConnectorNotInSystem(connectorCref, this);

  for(auto& bus : tlmbusconnectors)
  {
    if (bus && bus->getName() == busCref)
      return bus->addConnector(connectorCref,type);
  }

  return logError_TlmBusNotInSystem(busCref, this);
#else
    return LOG_NO_TLM();
#endif
}

oms_status_enu_t oms::System::deleteConnectorFromTLMBus(const oms::ComRef& busCref, const oms::ComRef& connectorCref)
{
#if !defined(NO_TLM)
  oms::ComRef busTail(busCref);
  oms::ComRef busHead = busTail.pop_front();
  oms::ComRef connectorTail(connectorCref);
  oms::ComRef connectorHead = connectorTail.pop_front();
  //If both bus and connector references the same subsystem, recurse into that subsystem
  if(busHead == connectorHead)
  {
    auto subsystem = subsystems.find(busHead);
    if(subsystem != subsystems.end())
      return subsystem->second->deleteConnectorFromTLMBus(busTail,connectorTail);

    auto component = components.find(busHead);
    if(component != components.end())
      return component->second->deleteConnectorFromTLMBus(busTail,connectorTail);
  }

  if(!busTail.isEmpty() && !connectorTail.isEmpty() && busHead != connectorHead)
    return logError_BusAndConnectorNotSameSystem(busCref, connectorCref);
  for(auto& bus : tlmbusconnectors)
    if(bus && bus->getName() == busCref)
      return bus->deleteConnector(connectorCref);

  return logError_TlmBusNotInSystem(busCref, this);
#else
    return LOG_NO_TLM();
#endif
}

oms_status_enu_t oms::System::addExternalModel(const oms::ComRef& cref, std::string path, std::string startscript)
{
#if !defined(NO_TLM)
  if (type != oms_system_tlm)
    return logError_OnlyForSystemTLM;

  if (!cref.isValidIdent())
    return oms_status_error;

  oms::ExternalModel* externalmodel = oms::ExternalModel::NewComponent(cref, this, path, startscript);
  if (!externalmodel)
    return oms_status_error;

  components[cref] = externalmodel;
  subelements.back() = reinterpret_cast<oms_element_t*>(externalmodel->getElement());
  subelements.push_back(NULL);
  element.setSubElements(&subelements[0]);

  return oms_status_ok;
#else
    return LOG_NO_TLM();
#endif
}

oms_status_enu_t oms::System::setConnectorGeometry(const oms::ComRef& cref, const oms::ssd::ConnectorGeometry *geometry)
{
  oms::ComRef tail(cref);
  oms::ComRef head = tail.pop_front();
  auto subsystem = subsystems.find(head);
  if (subsystem != subsystems.end())
    return subsystem->second->setConnectorGeometry(tail,geometry);

  auto component = components.find(head);
  if (component != components.end())
  {
    oms::Connector *connector = component->second->getConnector(tail);
    if (connector)
    {
      connector->setGeometry(geometry);
      return oms_status_ok;
    }
    else {
      return logError("Connector " + std::string(tail) + " not found in component " + std::string(head));
    }
  }

  oms::Connector* connector = this->getConnector(cref);
  if (connector)
  {
    connector->setGeometry(geometry);
    return oms_status_ok;
  }
  return logError("Connector " + std::string(cref) + " not found in system " + std::string(getCref()));
}

oms_status_enu_t oms::System::setConnectionGeometry(const oms::ComRef& crefA, const oms::ComRef& crefB, const oms::ssd::ConnectionGeometry *geometry)
{
  oms::ComRef tailA(crefA);
  oms::ComRef headA = tailA.pop_front();

  oms::ComRef tailB(crefB);
  oms::ComRef headB = tailB.pop_front();

  //If both A and B references the same subsystem, recurse into that subsystem
  if (headA == headB)
  {
    auto subsystem = subsystems.find(headA);
    if (subsystem != subsystems.end())
      return subsystem->second->setConnectionGeometry(tailA,tailB,geometry);
  }

  for (auto& connection : connections)
    if (connection && connection->isEqual(crefA, crefB))
    {
      connection->setGeometry(geometry);
      return oms_status_ok;
    }

  return logError_ConnectionNotInSystem(crefA, crefB, this);
}


oms_status_enu_t oms::System::setTLMConnectionParameters(const ComRef &crefA, const ComRef &crefB, const oms_tlm_connection_parameters_t* parameters)
{
#if !defined(NO_TLM)
  oms::ComRef tailA(crefA);
  oms::ComRef headA = tailA.pop_front();

  oms::ComRef tailB(crefB);
  oms::ComRef headB = tailB.pop_front();

  //If both A and B references the same subsystem, recurse into that subsystem
  if (headA == headB)
  {
    auto subsystem = subsystems.find(headA);
    if (subsystem != subsystems.end())
      return subsystem->second->setTLMConnectionParameters(tailA, tailB, parameters);
  }

  for (auto& connection : connections)
    if (connection && connection->isEqual(crefA, crefB))
    {
      connection->setTLMParameters(parameters);
      return oms_status_ok;
    }

  return logError_ConnectionNotInSystem(crefA, crefB, this);
#else
    return LOG_NO_TLM();
#endif
}

oms_status_enu_t oms::System::setBusGeometry(const oms::ComRef& cref, const oms::ssd::ConnectorGeometry *geometry)
{
  oms::ComRef tail(cref);
  oms::ComRef head = tail.pop_front();
  auto subsystem = subsystems.find(head);
  if (subsystem != subsystems.end())
    return subsystem->second->setBusGeometry(tail,geometry);

  oms::BusConnector* busConnector = this->getBusConnector(cref);
  if (busConnector)
  {
    busConnector->setGeometry(geometry);
    return oms_status_ok;
  }
  return logError("Bus " + std::string(cref)+" not found in system " + std::string(getCref()));
}

oms_status_enu_t oms::System::setTLMBusGeometry(const oms::ComRef& cref, const oms::ssd::ConnectorGeometry *geometry)
{
#if !defined(NO_TLM)
  oms::ComRef tail(cref);
  oms::ComRef head = tail.pop_front();
  auto subsystem = subsystems.find(head);
  if (subsystem != subsystems.end())
    return subsystem->second->setTLMBusGeometry(tail,geometry);

  oms::TLMBusConnector* tlmBusConnector = this->getTLMBusConnector(cref);
  if (tlmBusConnector)
  {
    tlmBusConnector->setGeometry(geometry);
    return oms_status_ok;
  }
  return logError("TLM Bus " + std::string(cref)+" not found in system " + std::string(getCref()));
#else
    return LOG_NO_TLM();
#endif
}

oms::Connection* oms::System::getConnection(const oms::ComRef& crefA, const oms::ComRef& crefB)
{
  for (auto& connection : connections)
    if (connection && connection->isEqual(crefA, crefB))
      return connection;
  return NULL;
}

oms::Model* oms::System::getModel()
{
  if (parentSystem)
    return parentSystem->getModel();
  return parentModel;
}

oms_status_enu_t oms::System::deleteAllConectionsTo(const oms::ComRef& cref)
{
  for (int i=0; i<connections.size(); ++i)
  {
    while (connections[i] && connections[i]->containsSignal(cref))
    {
      delete connections[i];

      connections.pop_back();   // last element is always NULL
      connections[i] = connections.back();
      connections.back() = NULL;
    }
  }

  return oms_status_ok;
}

bool oms::System::isConnected(const ComRef& cref) const
{
  for (int i=0; i<connections.size(); ++i)
    if (connections[i] && connections[i]->containsSignal(cref))
      return true;

  return false;
}

oms_status_enu_t oms::System::delete_(const oms::ComRef& cref)
{
  oms::ComRef tail(cref);
  oms::ComRef front = tail.pop_front();

  if (tail.isEmpty())
  {
    // check cref has ":start" suffix at the end to delete only start values
    if (front.hasSuffixStart())
    {
      if (oms_status_ok != values.deleteStartValue(getValidCref(front)))
        return logWarning("failed to delete start value \"" + std::string(getFullCref()+front) + "\"" + " because the identifier couldn't be resolved to any system signal");
      return oms_status_ok;
    }

    auto subsystem = subsystems.find(front);
    if (subsystem != subsystems.end())
    {
      deleteAllConectionsTo(front);
      delete subsystem->second;
      subsystems.erase(subsystem);
      return oms_status_ok;
    }

    auto component = components.find(front);
    if (component != components.end())
    {
      logInfo("Delete " + std::string(front));
      deleteAllConectionsTo(front);
      delete component->second;
      components.erase(component);
      return oms_status_ok;
    }

    for (int i=0; i<connectors.size()-1; ++i)
      if (connectors[i]->getName() == front)
      {
        // delete startValues associated with the Connector
        values.deleteStartValue(getValidCref(front));
        deleteAllConectionsTo(front);
        exportConnectors.erase(front);
        delete connectors[i];
        connectors.pop_back();   // last element is always NULL
        connectors[i] = connectors.back();
        connectors.back() = NULL;
        return oms_status_ok;
      }

    for (int i=0; i<busconnectors.size()-1; ++i)
      if (busconnectors[i]->getName() == front)
      {
        // delete startValues associated with the Connector
        values.deleteStartValue(getValidCref(front));
        deleteAllConectionsTo(front);
        exportConnectors.erase(front);
        delete busconnectors[i];
        busconnectors.pop_back();   // last element is always NULL
        busconnectors[i] = busconnectors.back();
        busconnectors.back() = NULL;
        return oms_status_ok;
      }
#if !defined(NO_TLM)
    for (int i=0; i<tlmbusconnectors.size()-1; ++i)
      if (tlmbusconnectors[i]->getName() == front)
      {
        // delete startValues associated with the Connector
        values.deleteStartValue(getValidCref(front));
        deleteAllConectionsTo(front);
        exportConnectors.erase(front);
        delete tlmbusconnectors[i];
        tlmbusconnectors.pop_back();   // last element is always NULL
        tlmbusconnectors[i] = tlmbusconnectors.back();
        tlmbusconnectors.back() = NULL;
        return oms_status_ok;
      }
#endif
  }
  else
  {
    auto subsystem = subsystems.find(front);
    if (subsystem != subsystems.end())
    {
      deleteAllConectionsTo(cref);
      return subsystem->second->delete_(tail);
    }

    auto component = components.find(front);
    if (component != components.end())
    {
      // check cref has ":start" suffix at the end to delete only start values
      if (tail.hasSuffixStart())
      {
        // add prefix to cref start values if ssv file is used (e.g) k1:start -> addP.k1:start
        if(!Flags::ExportParametersInline())
        {
          tail = cref;
        }
        if (oms_status_ok != component->second->deleteStartValue(tail))
          return logWarning("failed to delete start value \"" + std::string(getFullCref()+cref) + "\"" + " because the identifier couldn't be resolved to any component signal");
        return oms_status_ok;
      }

      deleteAllConectionsTo(cref);
      return component->second->deleteConnector(tail);
    }
  }
  logWarning("failed to delete object \"" + std::string(getFullCref()+cref) + "\"" + " because the identifier couldn't be resolved to any connector, component, system, or model");
  return oms_status_error;
}

bool oms::System::copyResources()
{
  if (parentSystem)
    return parentSystem->copyResources();
  return parentModel->copyResources();
}

oms_status_enu_t oms::System::getAllResources(std::vector<std::string>& resources)
{
  for (const auto& component : components)
    if (oms_status_ok != component.second->getAllResources(resources))
      return oms_status_error;

  for (const auto& subsystem : subsystems)
    if (oms_status_ok != subsystem.second->getAllResources(resources))
      return oms_status_error;

  return oms_status_ok;
}

oms_status_enu_t oms::System::exportDependencyGraphs(const std::string& pathInitialization, const std::string& pathEvent, const std::string& pathSimulation)
{
  oms_status_enu_t status = updateDependencyGraphs();

  initializationGraph.dotExport(pathInitialization);
  eventGraph.dotExport(pathEvent);
  simulationGraph.dotExport(pathSimulation);
  return status;
}

oms_status_enu_t oms::System::updateDependencyGraphs()
{
  initializationGraph.clear();
  eventGraph.clear();
  simulationGraph.clear();

  for (const auto& subsystem : subsystems)
  {
    if (oms_status_ok != subsystem.second->updateDependencyGraphs())
      return oms_status_error;

    initializationGraph.includeGraph(subsystem.second->getInitialUnknownsGraph(), subsystem.first);
    eventGraph.includeGraph(subsystem.second->getOutputsGraph(), subsystem.first);
    simulationGraph.includeGraph(subsystem.second->getOutputsGraph(), subsystem.first);
  }

  for (const auto& component : components)
  {
    initializationGraph.includeGraph(component.second->getInitialUnknownsGraph(), component.first);
    eventGraph.includeGraph(component.second->getOutputsGraph(), component.first);
    simulationGraph.includeGraph(component.second->getOutputsGraph(), component.first);
  }

  for (const auto& connection : connections)
  {
    if (!connection || connection->getType() != oms_connection_single)
      continue;

    Connector* varA = getConnector(connection->getSignalA());
    Connector* varB = getConnector(connection->getSignalB());
    if (varA && varB)
    {
      bool validConnection = oms::Connection::isValid(connection->getSignalA(), connection->getSignalB(), *varA, *varB);
      if (validConnection)
      {
        initializationGraph.addEdge(Connector(varA->getCausality(), varA->getType(), connection->getSignalA()), Connector(varB->getCausality(), varB->getType(), connection->getSignalB()));
        // Don't include parameter connections in simulation dependencies
        if (!varA->isParameter())
        {
          eventGraph.addEdge(Connector(varA->getCausality(), varA->getType(), connection->getSignalA()), Connector(varB->getCausality(), varB->getType(), connection->getSignalB()));
        }
        // allow only real connections in Continuous time mode
        if (varA->getType() == oms_signal_type_real && !varA->isParameter())
        {
          simulationGraph.addEdge(Connector(varA->getCausality(), varA->getType(), connection->getSignalA()), Connector(varB->getCausality(), varB->getType(), connection->getSignalB()));
        }
      }
      else
        return logError("failed for " + std::string(connection->getSignalA()) + " -> " + std::string(connection->getSignalB()));
    }
    else
      return logError("invalid connection");
  }

  return oms_status_ok;
}

oms_status_enu_t oms::System::getBoolean(const ComRef& cref, bool& value)
{
  if (!getModel()->validState(oms_modelState_virgin|oms_modelState_instantiated|oms_modelState_initialization|oms_modelState_simulation))
    return logError_ModelInWrongState(getModel());

  oms::ComRef tail(cref);
  oms::ComRef head = tail.pop_front();

  auto subsystem = subsystems.find(head);
  if (subsystem != subsystems.end())
    return subsystem->second->getBoolean(tail, value);

  auto component = components.find(head);
  if (component != components.end())
    return component->second->getBoolean(tail, value);

  for (auto& connector : connectors)
  {
    if (connector && connector->getName() == cref && connector->isTypeBoolean())
    {
      oms::ComRef ident = getValidCref(cref);
      // check any external input are set and return the value, otherwise return the startValue
      if (oms_modelState_simulation == getModel()->getModelState() && values.booleanValues[ident] != 0.0)
      {
        value = values.booleanValues[ident];
        return oms_status_ok;
      }
      auto booleanValue = values.booleanStartValues.find(ident);
      if (booleanValue != values.booleanStartValues.end())
        value = booleanValue->second;
      else
        value = 0; // default value
      return oms_status_ok;
    }
  }

  return logError_UnknownSignal(getFullCref() + cref);
}

oms_status_enu_t oms::System::getInteger(const ComRef& cref, int& value)
{
  if (!getModel()->validState(oms_modelState_virgin|oms_modelState_instantiated|oms_modelState_initialization|oms_modelState_simulation))
    return logError_ModelInWrongState(getModel());

  oms::ComRef tail(cref);
  oms::ComRef head = tail.pop_front();

  auto subsystem = subsystems.find(head);
  if (subsystem != subsystems.end())
    return subsystem->second->getInteger(tail, value);

  auto component = components.find(head);
  if (component != components.end())
    return component->second->getInteger(tail, value);

  for (auto& connector : connectors)
  {
    if (connector && connector->getName() == cref && connector->isTypeInteger())
    {
      oms::ComRef ident = getValidCref(cref);
      // check any external input are set and return the value, otherwise return the startValue
      if (oms_modelState_simulation == getModel()->getModelState() && values.integerValues[ident] != 0.0)
      {
        value = values.integerValues[ident];
        return oms_status_ok;
      }
      auto integerValue = values.integerStartValues.find(ident);
      if (integerValue != values.integerStartValues.end())
        value = integerValue->second;
      else
        value = 0; // default value
      return oms_status_ok;
    }
  }

  return logError_UnknownSignal(getFullCref() + cref);
}

oms_status_enu_t oms::System::getReal(const ComRef& cref, double& value)
{
  if (!getModel()->validState(oms_modelState_virgin|oms_modelState_instantiated|oms_modelState_initialization|oms_modelState_simulation))
    return logError_ModelInWrongState(getModel());

  oms::ComRef tail(cref);
  oms::ComRef head = tail.pop_front();

  auto subsystem = subsystems.find(head);
  if (subsystem != subsystems.end())
    return subsystem->second->getReal(tail, value);

  auto component = components.find(head);
  if (component != components.end())
    return component->second->getReal(tail, value);

  for (auto& connector : connectors)
  {
    if (connector && connector->getName() == cref && connector->isTypeReal())
    {
      oms::ComRef ident = getValidCref(cref);
      // check any external input are set and return the value, otherwise return the startValue
      if (oms_modelState_simulation == getModel()->getModelState() && values.realValues[ident] != 0.0)
      {
        value = values.realValues[ident];
        return oms_status_ok;
      }
      auto realValue = values.realStartValues.find(ident);
      if (realValue != values.realStartValues.end())
        value = realValue->second;
      else
        value = 0.0; // default value
      return oms_status_ok;
    }
  }

  return logError_UnknownSignal(getFullCref() + cref);
}

/*
 * function which returns valid cref for which are needed to set StartValues
 * if ExportParametersInline is set return the default cref
 * else when importing from ssv , check for cref belongs to toplevel System or SubSystem
 */
oms::ComRef oms::System::getValidCref(const ComRef& cref)
{
  oms::ComRef ident;
  if (Flags::ExportParametersInline())
  {
    ident = cref;
  }
  else
  {
    std::string s = std::string(getFullCref());
    int count  = std::count(s.begin(), s.end(), '.');
    // count > 1 means subsystems (e.g.) A.B.C else Top level System (e.g) A.B
    if (count > 1)
      ident = getCref() + cref;
    else
      ident = cref;
  }
  return ident;
}

oms::Variable* oms::System::getVariable(const ComRef& cref)
{
  oms::ComRef tail(cref);
  oms::ComRef head = tail.pop_front();

  auto subsystem = subsystems.find(head);
  if (subsystem != subsystems.end())
    return subsystem->second->getVariable(tail);

  auto component = components.find(head);
  if (component != components.end())
    return component->second->getVariable(tail);

  //for (auto& connector : connectors)
  //  if (connector && connector->getName() == cref)
  //    return connector->getVariable();

  logError_UnknownSignal(getFullCref() + cref);
  return NULL;
}

oms_status_enu_t oms::System::setBoolean(const ComRef& cref, bool value)
{
  if (!getModel()->validState(oms_modelState_virgin|oms_modelState_enterInstantiation|oms_modelState_instantiated|oms_modelState_initialization|oms_modelState_simulation))
    return logError_ModelInWrongState(getModel());

  oms::ComRef tail(cref);
  oms::ComRef head = tail.pop_front();

  auto subsystem = subsystems.find(head);
  if (subsystem != subsystems.end())
    return subsystem->second->setBoolean(tail, value);

  auto component = components.find(head);
  if (component != components.end())
    return component->second->setBoolean(tail, value);

  for (auto& connector : connectors)
    if (connector && connector->getName() == cref && connector->isTypeBoolean())
    {
      oms::ComRef ident = getValidCref(cref);
      // set external inputs, after initialization
      if (oms_modelState_simulation == getModel()->getModelState())
      {
        values.booleanValues[ident] = value;
      }
      else
      {
        values.setBoolean(ident, value);
      }
      return oms_status_ok;
    }

  return logError_UnknownSignal(getFullCref() + cref);
}

oms_status_enu_t oms::System::setInteger(const ComRef& cref, int value)
{
  if (!getModel()->validState(oms_modelState_virgin|oms_modelState_enterInstantiation|oms_modelState_instantiated|oms_modelState_initialization|oms_modelState_simulation))
    return logError_ModelInWrongState(getModel());

  oms::ComRef tail(cref);
  oms::ComRef head = tail.pop_front();

  auto subsystem = subsystems.find(head);
  if (subsystem != subsystems.end())
    return subsystem->second->setInteger(tail, value);

  auto component = components.find(head);
  if (component != components.end())
    return component->second->setInteger(tail, value);

  for (auto& connector : connectors)
    if (connector && connector->getName() == cref && connector->isTypeInteger())
    {
      oms::ComRef ident = getValidCref(cref);
      // set external inputs, after initialization
      if (oms_modelState_simulation == getModel()->getModelState())
      {
        values.integerValues[ident] = value;
      }
      else
      {
        values.setInteger(ident, value);
      }
      return oms_status_ok;
    }

  return logError_UnknownSignal(getFullCref() + cref);
}

oms_status_enu_t oms::System::setReal(const ComRef& cref, double value)
{
  if (!getModel()->validState(oms_modelState_virgin|oms_modelState_enterInstantiation|oms_modelState_instantiated|oms_modelState_initialization|oms_modelState_simulation))
    return logError_ModelInWrongState(getModel());

  oms::ComRef tail(cref);
  oms::ComRef head = tail.pop_front();

  auto subsystem = subsystems.find(head);
  if (subsystem != subsystems.end())
    return subsystem->second->setReal(tail, value);

  auto component = components.find(head);
  if (component != components.end())
    return component->second->setReal(tail, value);

  for (auto& connector : connectors)
    if (connector && connector->getName() == cref && connector->isTypeReal())
    {
      oms::ComRef ident = getValidCref(cref);
      // set external inputs, after initialization
      if (oms_modelState_simulation == getModel()->getModelState())
      {
        values.realValues[ident] = value;
      }
      else
      {
        values.setReal(ident, value);
      }
      return oms_status_ok;
    }
  return logError_UnknownSignal(getFullCref() + cref);
}

oms_status_enu_t oms::System::getReals(const std::vector<oms::ComRef> &sr, std::vector<double> &values)
{
  oms_status_enu_t status = oms_status_ok;
  for(int i=0; i<sr.size(); ++i) {
    status = getReal(sr[i],values[i]);
    if(status != oms_status_ok)
      break;
  }
  return status;
}

oms_status_enu_t oms::System::setReals(const std::vector<oms::ComRef> &crefs, std::vector<double> values)
{
  oms_status_enu_t status = oms_status_ok;
  for(int i=0; i<crefs.size(); ++i) {
    status = setReal(crefs[i],values[i]);
    if(status != oms_status_ok)
      break;
  }
  return status;
}

oms_status_enu_t oms::System::setRealInputDerivatives(const oms::ComRef &cref, int order, double value)
{
  return logError_NotImplemented;
}

oms_status_enu_t oms::System::importFromSSD_ConnectionGeometry(const pugi::xml_node& node, const ComRef& crefA, const ComRef& crefB)
{
  pugi::xml_node connectionGeometryNode = node.child(oms::ssp::Draft20180219::ssd::connection_geometry);
  if (connectionGeometryNode)
  {
    oms::Connection *connection = getConnection(crefA, crefB);
    if (connection)
    {
      oms::ssd::ConnectionGeometry geometry;
      geometry.importFromSSD(connectionGeometryNode);
      connection->setGeometry(&geometry);
    }
    else
    {
      return oms_status_error;
    }
  }
  return oms_status_ok;
}

oms_status_enu_t oms::System::registerSignalsForResultFile(ResultWriter& resultFile)
{
  if (Flags::WallTime())
    clock_id = resultFile.addSignal(std::string(getFullCref() + ComRef("$wallTime")), "wall-clock time [s]", SignalType_REAL);
  else
    clock_id = 0;

  for (const auto& component : components)
    if (oms_status_ok != component.second->registerSignalsForResultFile(resultFile))
      return oms_status_error;

  for (const auto& subsystem : subsystems)
    if (oms_status_ok != subsystem.second->registerSignalsForResultFile(resultFile))
      return oms_status_error;

  resultFileMapping.clear();
  for (unsigned int i=0; i<connectors.size(); ++i)
  {
    if (!connectors[i])
      continue;
    // check for full cref in exportConnectors
    if (!exportConnectors[getFullCref()+connectors[i]->getName()])
      continue;

    auto const& connector = connectors[i];

    if (oms_signal_type_real == connector->getType())
    {
      unsigned int ID = resultFile.addSignal(std::string(getFullCref() + connector->getName()), "connector", SignalType_REAL);
      resultFileMapping[ID] = i;
    }
    else if (oms_signal_type_integer == connector->getType())
    {
      unsigned int ID = resultFile.addSignal(std::string(connector->getName()), "connector", SignalType_INT);
      resultFileMapping[ID] = i;
    }
    else if (oms_signal_type_boolean == connector->getType())
    {
      unsigned int ID = resultFile.addSignal(std::string(connector->getName()), "connector", SignalType_BOOL);
      resultFileMapping[ID] = i;
    }
  }

  return oms_status_ok;
}

oms_status_enu_t oms::System::updateSignals(ResultWriter& resultFile)
{
  if (clock_id)
  {
    SignalValue_t wallTime;
    wallTime.realValue = clock.getElapsedWallTime();
    resultFile.updateSignal(clock_id, wallTime);
  }

  for (const auto& component : components)
    if (oms_status_ok != component.second->updateSignals(resultFile))
      return oms_status_error;

  for (const auto& subsystem : subsystems)
    if (oms_status_ok != subsystem.second->updateSignals(resultFile))
      return oms_status_error;

  for (auto const &it : resultFileMapping)
  {
    unsigned int ID = it.first;
    auto const& connector = connectors[it.second];
    SignalValue_t value;
    if (oms_signal_type_real == connector->getType())
    {
      if (oms_status_ok != getReal(connector->getName(), value.realValue))
        return logError("failed to fetch connector " + std::string(connector->getName()));
      resultFile.updateSignal(ID, value);
    }
    else if (oms_signal_type_integer == connector->getType())
    {
      if (oms_status_ok != getInteger(connector->getName(), value.intValue))
        return logError("failed to fetch variable " + std::string(connector->getName()));
      resultFile.updateSignal(ID, value);
    }
    else if (oms_signal_type_boolean == connector->getType())
    {
      if (oms_status_ok != getBoolean(connector->getName(), value.boolValue))
        return logError("failed to fetch variable " + std::string(connector->getName()));
      resultFile.updateSignal(ID, value);
    }
  }

  return oms_status_ok;
}

oms_status_enu_t oms::System::addSignalsToResults(const char* regex)
{
  try
  {
    oms_regex exp(regex);
    for (auto& x: exportConnectors)
    {
      if (x.second)
        continue;

      if (regex_match(std::string(x.first), exp))
      {
        //logInfo("added \"" + std::string(x.first) + "\" to results");
        x.second = true;
      }
    }
  }
  catch (std::regex_error& e)
  {
    return logError("invalid regular expression");
  }

  for (const auto& component : components)
    if (oms_status_ok != component.second->addSignalsToResults(regex))
      return oms_status_error;

  for (const auto& subsystem : subsystems)
    if (oms_status_ok != subsystem.second->addSignalsToResults(regex))
      return oms_status_error;

  return oms_status_ok;
}

oms_status_enu_t oms::System::removeSignalsFromResults(const char* regex)
{
  try
  {
    oms_regex exp(regex);
    for (auto& x: exportConnectors)
    {
      if (!x.second)
        continue;

      if (regex_match(std::string(x.first), exp))
      {
        //logInfo("removed \"" + std::string(x.first) + "\" from results");
        x.second = false;
      }
    }
  }
  catch (std::regex_error& e)
  {
    return logError("invalid regular expression");
  }

  for (const auto& component : components)
    if (oms_status_ok != component.second->removeSignalsFromResults(regex))
      return oms_status_error;

  for (const auto& subsystem : subsystems)
    if (oms_status_ok != subsystem.second->removeSignalsFromResults(regex))
      return oms_status_error;

  return oms_status_ok;
}

bool oms::System::useThreadPool()
{
  return getModel()->useThreadPool();
}

ctpl::thread_pool& oms::System::getThreadPool()
{
  return getModel()->getThreadPool();
}

std::string oms::System::getUniqueID() const
{
  static unsigned int id = 0;
  std::string str = std::to_string(++id);

  while (str.length() < 4)
    str = "0" + str;

  return str;
}

oms_status_enu_t oms::System::setFaultInjection(const oms::ComRef& signal, oms_fault_type_enu_t faultType, double faultValue)
{
  oms::System* system = NULL;

  oms::ComRef tail(signal);
  oms::ComRef front = tail.pop_front();

  auto subsystem = subsystems.find(front);
  if (subsystem != subsystems.end())
    return subsystem->second->setFaultInjection(tail, faultType, faultValue);

  auto component = components.find(front);
  if (component != components.end())
    return component->second->setFaultInjection(tail, faultType, faultValue);

  return oms_status_error;
}

oms_status_enu_t oms::System::importTLMBus(const pugi::xml_node& node, Component* component)
{
  std::string busname = node.attribute("name").as_string();
  std::string domainstr = node.attribute("domain").as_string();
  int dimensions = node.attribute("dimensions").as_int();
  std::string interpolationstr = node.attribute("interpolation").as_string();

  oms_tlm_interpolation_t interpolation;
  if (interpolationstr == "none")
    interpolation = oms_tlm_no_interpolation;
  else if (interpolationstr == "coarsegrained")
    interpolation = oms_tlm_coarse_grained;
  else if (interpolationstr == "finegrained")
    interpolation = oms_tlm_fine_grained;
  else
    return logError("Unsupported interpolation type: " + interpolationstr);

  oms_tlm_domain_t domain;
  if (domainstr == "input")
    domain = oms_tlm_domain_input;
  else if (domainstr == "output")
    domain = oms_tlm_domain_output;
  else if (domainstr == "mechanical")
    domain = oms_tlm_domain_mechanical;
  else if (domainstr == "rotational")
    domain = oms_tlm_domain_rotational;
  else if (domainstr == "hydraulic")
    domain = oms_tlm_domain_hydraulic;
  else if (domainstr == "electric")
    domain = oms_tlm_domain_electric;
  else
    return logError("Unsupported TLM domain: " + domainstr);

  if (component)
  {
    if (oms_status_ok != component->addTLMBus(busname, domain, dimensions, interpolation))
      return oms_status_error;
  }
  else
  {
    if (oms_status_ok != addTLMBus(busname, domain, dimensions, interpolation))
      return oms_status_error;
  }

  return oms_status_ok;
}


oms_status_enu_t oms::System::importBusConnectorSignals(const pugi::xml_node& node)
{
  std::string busname = node.attribute("name").as_string();
  //Load bus connector signals
  pugi::xml_node signals_node = node.child(oms::ssp::Draft20180219::signals);

  if (signals_node)
  {
    for(pugi::xml_node_iterator itSignals = signals_node.begin(); itSignals != signals_node.end(); ++itSignals)
    {
      std::string name = itSignals->name();
      if (name == oms::ssp::Draft20180219::signal)
      {
        std::string signalname = itSignals->attribute("name").as_string();
        if (std::string(node.attribute("type").as_string()) == "tlm")
        {
          std::string signaltype = itSignals->attribute("type").as_string();
          addConnectorToTLMBus(busname, signalname, signaltype);
        }
        else
          addConnectorToBus(busname, signalname);
      }
    }
  }

  return oms_status_ok;
}

oms_status_enu_t oms::System::importBusConnectorGeometry(const pugi::xml_node& node)
{
  std::string busname = node.attribute("name").as_string();
  //Load bus connector signals
  pugi::xml_node connectorGeometryNode = node.child(oms::ssp::Draft20180219::ssd::connector_geometry);

  if (connectorGeometryNode)
  {
    oms::ssd::ConnectorGeometry geometry(0.0, 0.0);
    geometry.setPosition(connectorGeometryNode.attribute("x").as_double(), connectorGeometryNode.attribute("y").as_double());
    if (std::string(node.attribute("type").as_string()) == "tlm")
    {
#if !defined(NO_TLM)
      oms::TLMBusConnector* tlmBusConnector = getTLMBusConnector(busname);
      if (tlmBusConnector)
        tlmBusConnector->setGeometry(&geometry);
#else
      return LOG_NO_TLM();
#endif
    }
    else
    {
      oms::BusConnector* busConnector = getBusConnector(busname);
      if (busConnector)
        busConnector->setGeometry(&geometry);
    }
  }

  return oms_status_ok;
}

oms::AlgLoop* oms::System::getAlgLoop(const int systemNumber)
{
  if (systemNumber > algLoops.size()-1 || systemNumber < 0)
  {
    logError("Invalid system number for algebraic loop.");
    return NULL;
  }

  return &algLoops[systemNumber];
}

oms_status_enu_t oms::System::addAlgLoop(oms_ssc_t SCC, const int algLoopNum)
{
  if (loopsNeedUpdate)
  {
    algLoops.clear();
    loopsNeedUpdate = false;
  }
  algLoops.push_back( AlgLoop( Flags::AlgLoopSolver(), absoluteTolerance, SCC, algLoopNum ));

  return oms_status_ok;
}

oms_status_enu_t oms::System::importStartValuesFromSSV()
{
  for (const auto& file : startValuesFileSources)
  {
    // mapping between a parameter in the source and a parameter of the system or component being parametrized
    std::multimap<ComRef, ComRef> mappedEntry;

    // check for parameter mapping file ".ssm file"
    if (!file.second.empty())
    {
      importParameterMappingFromSSM(file.second, mappedEntry);
      importStartValuesFromSSVHelper(file.first, mappedEntry);
    }
    else
    {
      // no mapping file provided, apply the values from ssv file
      importStartValuesFromSSVHelper(file.first, mappedEntry);
    }
  }

  return oms_status_ok;
}

oms_status_enu_t oms::System::importStartValuesFromSSVHelper(std::string fileName, std::multimap<ComRef, ComRef> &mappedEntry)
{
  std::string tempdir = getModel()->getTempDirectory();
  filesystem::path temp_root(tempdir);
  pugi::xml_document ssvdoc;
  pugi::xml_parse_result result = ssvdoc.load_file((temp_root / fileName).string().c_str());
  pugi::xml_node parameterSet, parameters;

  if (result) // check from ssv file
  {
    parameterSet = ssvdoc.document_element(); // ssv:ParameterSet
    parameters = parameterSet.child(oms::ssp::Version1_0::ssv::parameters);
  }
  else if (getModel()->getSnapshot().child(oms::ssp::Version1_0::ssv_file)) // check in memory oms:ssv_file
  {
    parameterSet = getModel()->getSnapshot().child(oms::ssp::Version1_0::ssv_file).child(oms::ssp::Version1_0::ssv::parameter_set); // ssv:ParameterSet
    parameters = parameterSet.child(oms::ssp::Version1_0::ssv::parameters);
  }
  else
  {
    return logError("loading \"" + std::string(fileName) + "\" failed (" + std::string(result.description()) + ")");
  }

  if (parameters)
  {
    for (pugi::xml_node_iterator itparameters = parameters.begin(); itparameters != parameters.end(); ++itparameters)
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
          {
            mappedcrefs.push_back(it->second);
          }
        }

        if (itparameters->child(oms::ssp::Version1_0::ssv::real_type))
        {
          double value = itparameters->child(oms::ssp::Version1_0::ssv::real_type).attribute("value").as_double();
          if (!mappedcrefs.empty())
          {
            for (const auto &mappedcref : mappedcrefs)
            {
              setReal(mappedcref, value);
            }
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
            {
              setInteger(mappedcref, value);
            }
          }
          else
          {
            // no mapping entry found, apply the default cref found in ssv file
            setInteger(cref, value);
          }
        }
        else if (itparameters->child(oms::ssp::Version1_0::ssv::boolean_type))
        {
          bool value = itparameters->child(oms::ssp::Version1_0::ssv::boolean_type).attribute("value").as_bool();
          if (!mappedcrefs.empty())
          {
            for (const auto &mappedcref : mappedcrefs)
            {
              setBoolean(mappedcref, value);
            }
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
  }

  return oms_status_ok;
}

oms_status_enu_t oms::System::updateAlgebraicLoops(const std::vector< oms_ssc_t >& sortedConnections)
{
  // Instantiate loops
  if (loopsNeedUpdate)
  {
    int systCount = 0;
    for(int i=0; i<sortedConnections.size(); i++)
    {
      if (sortedConnections[i].size() > 1)
      {
        addAlgLoop(sortedConnections[i], systCount);
        systCount++;
      }
    }
    loopsNeedUpdate = false;
  }

  return oms_status_ok;
}

oms_status_enu_t oms::System::importParameterMappingFromSSM(std::string fileName, std::multimap<ComRef, ComRef> &mappedEntry)
{
  std::string tempdir = getModel()->getTempDirectory();
  filesystem::path temp_root(tempdir);
  pugi::xml_document ssmdoc;
  pugi::xml_parse_result result = ssmdoc.load_file((temp_root / fileName).string().c_str());
  pugi::xml_node parameterMapping;

  if (result) // check from ssm file
  {
    parameterMapping = ssmdoc.document_element(); // ssm:ParameterMapping
  }
  else
  {
    return logError("loading \"" + std::string(fileName) + "\" failed (" + std::string(result.description()) + ")");
  }

  if (parameterMapping)
  {
    for (pugi::xml_node_iterator itparametermapping = parameterMapping.begin(); itparametermapping != parameterMapping.end(); ++itparametermapping)
    {
      std::string name = itparametermapping->name();
      if (name == oms::ssp::Version1_0::ssm::parameter_mapping_entry)
      {
        ComRef source = itparametermapping->attribute("source").as_string();
        if (!source.isEmpty())
        {
          mappedEntry.insert(std::make_pair(source, itparametermapping->attribute("target").as_string()));
        }
        else
        {
          // default value will be applied
        }
      }
    }
  }

  return oms_status_ok;
}

oms_status_enu_t oms::System::solveAlgLoop(DirectedGraph& graph, int loopNumber)
{
  return algLoops[loopNumber].solveAlgLoop(*this, graph);
}
