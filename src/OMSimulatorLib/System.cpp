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

#include "System.h"

#include "Component.h"
#include "ComponentFMUCS.h"
#include "ComponentFMUME.h"
#include "ComponentTable.h"
#include "ExternalModel.h"
#include "Model.h"
#include "ResultWriter.h"
#include "ssd/Tags.h"
#include "SystemSC.h"
#include "SystemTLM.h"
#include "SystemWC.h"
#include "Types.h"

oms3::System::System(const oms3::ComRef& cref, oms_system_enu_t type, oms3::Model* parentModel, oms3::System* parentSystem)
  : element(oms_element_system, cref), cref(cref), type(type), parentModel(parentModel), parentSystem(parentSystem)
{
  connections.push_back(NULL);

  connectors.push_back(NULL);
  element.setConnectors(&connectors[0]);

  busconnectors.push_back(NULL);
  element.setBusConnectors(&busconnectors[0]);

  tlmbusconnectors.push_back(NULL);
  element.setTLMBusConnectors(&tlmbusconnectors[0]);

  subelements.push_back(NULL);
  element.setSubElements(&subelements[0]);
}

oms3::System::~System()
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

  for (const auto tlmbusconnector : tlmbusconnectors)
    if (tlmbusconnector)
      delete tlmbusconnector;
}

oms3::System* oms3::System::NewSystem(const oms3::ComRef& cref, oms_system_enu_t type, oms3::Model* parentModel, oms3::System* parentSystem)
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
    return SystemTLM::NewSystem(cref, parentModel, parentSystem);

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
  }

  logError_InternalError;
  return NULL;
}

oms3::ComRef oms3::System::getFullCref() const
{
  if (parentSystem)
    return parentSystem->getFullCref() + this->getCref();
  if (parentModel)
    return ComRef(parentModel->getCref()) + this->getCref();

  logError_InternalError;
  return this->getCref();
}

oms3::System* oms3::System::getSystem(const oms3::ComRef& cref)
{
  oms3::System* system = NULL;

  oms3::ComRef tail(cref);
  oms3::ComRef front = tail.pop_front();

  auto it = subsystems.find(front);
  if (it == subsystems.end())
    return NULL;

  if (tail.isEmpty())
    return it->second;

  return it->second->getSystem(tail);
}

oms3::Component* oms3::System::getComponent(const oms3::ComRef& cref)
{
  oms3::System* system = NULL;

  oms3::ComRef tail(cref);
  oms3::ComRef front = tail.pop_front();

  auto subsystem = subsystems.find(front);
  if (subsystem != subsystems.end())
    return subsystem->second->getComponent(tail);

  auto component = components.find(cref);
  if (component == components.end())
    return NULL;

  return component->second;
}

oms3::System *oms3::System::getSubSystem(const oms3::ComRef &cref)
{
  auto it = subsystems.find(cref);
  if (it == subsystems.end())
    return NULL;

  return it->second;
}

bool oms3::System::validCref(const oms3::ComRef& cref)
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

  if (getTLMBusConnector(cref))
    return false;

  return true;
}

oms_status_enu_t oms3::System::addSubSystem(const oms3::ComRef& cref, oms_system_enu_t type)
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
      subelements.back() = reinterpret_cast<oms3_element_t*>(system->getElement());
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

oms_status_enu_t oms3::System::addSubModel(const oms3::ComRef& cref, const std::string& path)
{
  if (cref.isValidIdent())
  {
    if (!validCref(cref))
      return logError_AlreadyInScope(getFullCref() + cref);

    Component* component = NULL;

    std::string extension = "";
    if (path.length() > 5)
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
    subelements.back() = reinterpret_cast<oms3_element_t*>(component->getElement());
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

oms_status_enu_t oms3::System::list(const oms3::ComRef& cref, char** contents)
{
  return logError("not implemented");
}

oms_status_enu_t oms3::System::listUnconnectedConnectors(char** contents) const
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
      }
    }
  }

  for (const auto& busconnector : busconnectors)
    if (busconnector && !isConnected(busconnector->getName()))
      unconnectedBuses.push_back(getFullCref() + busconnector->getName());

  for (const auto& tlmbusconnector : tlmbusconnectors)
    if (tlmbusconnector && !isConnected(tlmbusconnector->getName()))
      unconnectedBuses.push_back(getFullCref() + tlmbusconnector->getName());

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
    strcpy(*contents, msg.c_str());
  }
  return oms_status_ok;
}

oms_status_enu_t oms3::System::exportToSSD(pugi::xml_node& node) const
{
  node.append_attribute("name") = this->getCref().c_str();

  // export ssd:SimulationInformation
  if (oms_status_ok != this->exportToSSD_SimulationInformation(node))
    return logError("export of system SimulationInformation failed");

  if (oms_status_ok != element.getGeometry()->exportToSSD(node))
    return logError("export of system ElementGeometry failed");

  pugi::xml_node elements_node = node.append_child(oms2::ssd::ssd_elements);

  for (const auto& subsystem : subsystems)
  {
    pugi::xml_node system_node = elements_node.append_child(oms2::ssd::ssd_system);
    if (oms_status_ok != subsystem.second->exportToSSD(system_node))
      return logError("export of system failed");
  }

  for (const auto& component : components)
  {
    pugi::xml_node component_node = elements_node.append_child(oms2::ssd::ssd_component);
    if (oms_status_ok != component.second->exportToSSD(component_node))
      return logError("export of component failed");
  }

  pugi::xml_node connectors_node = node.append_child(oms2::ssd::ssd_connectors);
  for(const auto& connector : connectors)
    if (connector)
      connector->exportToSSD(connectors_node);

  std::vector<oms3::Connection*> busconnections;
  pugi::xml_node connections_node = node.append_child(oms2::ssd::ssd_connections);
  for (const auto& connection : connections)
    if (connection && connection->getType() == oms3_connection_single)
      connection->exportToSSD(connections_node);
    else if (connection)
      busconnections.push_back(connection);

  if (busconnectors[0] || tlmbusconnectors[0] || !busconnections.empty())
  {
    pugi::xml_node annotations_node = node.append_child(oms2::ssd::ssd_annotations);
    pugi::xml_node annotation_node = annotations_node.append_child(oms2::ssd::ssd_annotation);
    annotation_node.append_attribute("type") = oms::annotation_type;
    for (const auto& busconnector : busconnectors)
      if (busconnector)
        busconnector->exportToSSD(annotation_node);
    for (const auto& tlmbusconnector : tlmbusconnectors)
      if (tlmbusconnector)
        tlmbusconnector->exportToSSD(annotation_node);
    if (!busconnections.empty())
    {
      pugi::xml_node busconnections_node = annotation_node.append_child(oms::bus_connections);
      for (const auto& busconnection : busconnections)
        busconnection->exportToSSD(busconnections_node);
    }
  }

  return oms_status_ok;
}

oms_status_enu_t oms3::System::importFromSSD(const pugi::xml_node& node)
{
  for(pugi::xml_node_iterator it = node.begin(); it != node.end(); ++it)
  {
    std::string name = it->name();
    if (name == oms2::ssd::ssd_simulation_information)
    {
      if (oms_status_ok != importFromSSD_SimulationInformation(*it))
        return logError("Failed to import " + std::string(oms2::ssd::ssd_simulation_information));
    }
    else if (name == oms2::ssd::ssd_element_geometry)
    {
      oms3::ssd::ElementGeometry geometry;
      geometry.importFromSSD(*it);
      setGeometry(geometry);
    }
    else if (name == oms2::ssd::ssd_connections)
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
          return logError("Failed to import " + std::string(oms2::ssd::ssd_connection));
        else
        {
          // Load connection geometry
          if (oms_status_ok != importFromSSD_ConnectionGeometry(*itConnectors, crefA, crefB))
            return logError("Failed to import " + std::string(oms2::ssd::ssd_connection_geometry));
        }
      }
    }
    else if (name == oms2::ssd::ssd_connectors)
    {
      for(pugi::xml_node_iterator itConnectors = (*it).begin(); itConnectors != (*it).end(); ++itConnectors)
      {
        connectors.back() = oms3::Connector::NewConnector(*itConnectors);
        if (connectors.back())
          connectors.push_back(NULL);
        else
          return logError("Failed to import ssd:connector");
        element.setConnectors(&connectors[0]);
      }
    }
    else if (name == oms2::ssd::ssd_elements)
    {
      for(pugi::xml_node_iterator itElements = (*it).begin(); itElements != (*it).end(); ++itElements)
      {
        name = itElements->name();
        if (name == oms2::ssd::ssd_system)
        {
          ComRef systemCref = ComRef(itElements->attribute("name").as_string());

          // lochel: I guess that can somehow be improved
          oms_system_enu_t systemType = oms_system_tlm;
          if (itElements->child(oms2::ssd::ssd_simulation_information).child("VariableStepSolver").attribute("description").as_string() != "")
            systemType = oms_system_sc;
          if (itElements->child(oms2::ssd::ssd_simulation_information).child("FixedStepSolver").attribute("description").as_string() != "")
            systemType = oms_system_sc;
          if (itElements->child(oms2::ssd::ssd_simulation_information).child("VariableStepMaster").attribute("description").as_string() != "")
            systemType = oms_system_wc;
          if (itElements->child(oms2::ssd::ssd_simulation_information).child("FixedStepMaster").attribute("description").as_string() != "")
            systemType = oms_system_wc;

          if (oms_status_ok != addSubSystem(systemCref, systemType))
            return oms_status_error;

          System* system = getSystem(systemCref);
          if (!system)
            return oms_status_error;

          if (oms_status_ok != system->importFromSSD(*itElements))
            return oms_status_error;
        }
        else if (name == oms2::ssd::ssd_component)
        {
          Component* component = NULL;
          std::string type = itElements->attribute("type").as_string();
          if ("application/x-fmu-sharedlibrary" == type)
            component = ComponentFMUCS::NewComponent(*itElements, this);
          else if ("application/table" == type)
            component = ComponentTable::NewComponent(*itElements, this);

          if (component)
          {
            components[component->getCref()] = component;
            subelements.back() = reinterpret_cast<oms3_element_t*>(component->getElement());
            subelements.push_back(NULL);
            element.setSubElements(&subelements[0]);
          }
          else
            return oms_status_error;
        }
        else
          return logError("wrong xml schema detected: " + name);
      }
    }
    else if (name == oms2::ssd::ssd_annotations)
    {
      pugi::xml_node annotation_node = it->child(oms2::ssd::ssd_annotation);
      if (annotation_node && std::string(annotation_node.attribute("type").as_string()) == oms::annotation_type)
      {
        for(pugi::xml_node_iterator itAnnotations = annotation_node.begin(); itAnnotations != annotation_node.end(); ++itAnnotations)
        {
          name = itAnnotations->name();
          if (std::string(name) == oms::bus)
          {
            //Load bus connector
            std::string busname = itAnnotations->attribute("name").as_string();
            if (std::string(itAnnotations->attribute("type").as_string()) == "tlm")
            {
              std::string domain = itAnnotations->attribute("domain").as_string();
              int dimensions = itAnnotations->attribute("dimensions").as_int();
              std::string interpolationStr = itAnnotations->attribute("interpolation").as_string();
              oms_tlm_interpolation_t interpolation;
              if (interpolationStr == "none")
                interpolation = oms_tlm_no_interpolation;
              else if (interpolationStr == "coarsegrained")
                interpolation = oms_tlm_coarse_grained;
              else if (interpolationStr == "finegrained")
                interpolation = oms_tlm_fine_grained;
              else
                logError("Unsupported interpolation type: "+interpolationStr);

              if (oms_status_ok != addTLMBus(busname,domain,dimensions,interpolation))
                return oms_status_error;
            }
            else {
              if (oms_status_ok != addBus(busname))
                return oms_status_error;
            }

            //Load bus connector signals
            pugi::xml_node signals_node = itAnnotations->child(oms::signals);
            if (signals_node)
            {
              for(pugi::xml_node_iterator itSignals = signals_node.begin(); itSignals != signals_node.end(); ++itSignals)
              {
                name = itSignals->name();
                if (name == oms::signal)
                {
                  std::string signalname = itSignals->attribute("name").as_string();
                  if (std::string(itAnnotations->attribute("type").as_string()) == "tlm")
                  {
                    std::string signaltype = itSignals->attribute("type").as_string();
                    addConnectorToTLMBus(busname, signalname, signaltype);
                  }
                  else
                    addConnectorToBus(busname, signalname);
                }
              }
            }

            // Load bus connector geometry
            pugi::xml_node connectorGeometryNode = itAnnotations->child(oms2::ssd::ssd_connector_geometry);
            if (connectorGeometryNode)
            {
              oms2::ssd::ConnectorGeometry geometry(0.0, 0.0);
              geometry.setPosition(connectorGeometryNode.attribute("x").as_double(), connectorGeometryNode.attribute("y").as_double());
              if (std::string(itAnnotations->attribute("type").as_string()) == "tlm")
              {
                oms3::TLMBusConnector* tlmBusConnector = getTLMBusConnector(busname);
                if (tlmBusConnector)
                  tlmBusConnector->setGeometry(&geometry);
              }
              else
              {
                oms3::BusConnector* busConnector = getBusConnector(busname);
                if (busConnector)
                  busConnector->setGeometry(&geometry);
              }
            }
          }
          else if (std::string(name) == oms::bus_connections)
          {
            //Load bus connections
            for(pugi::xml_node_iterator itConnection = itAnnotations->begin(); itConnection != itAnnotations->end(); ++itConnection)
            {
              //Load bus connection
              oms3::ComRef element1(itConnection->attribute("startElement").as_string());
              oms3::ComRef connector1(itConnection->attribute("startConnector").as_string());
              oms3::ComRef element2(itConnection->attribute("endElement").as_string());
              oms3::ComRef connector2(itConnection->attribute("endConnector").as_string());

              oms_status_enu_t status;
              // Load TLM bus connection
              if (itConnection->attribute("delay") || itConnection->attribute("alpha")
                  || itConnection->attribute("linearimpedance") || itConnection->attribute("angularimpedance"))
              {
                double delay = itConnection->attribute("delay").as_double();
                double alpha = itConnection->attribute("alpha").as_double();
                double linearimpedance = itConnection->attribute("linearimpedance").as_double();
                double angularimpedance = itConnection->attribute("angularimpedance").as_double();

                status = addTLMConnection(element1+connector1, element2+connector2, delay, alpha, linearimpedance, angularimpedance);
              }
              else  // Load bus connection
              {
                status = addConnection(element1+connector1, element2+connector2);
              }
              // Load the bus connection geometry
              if (oms_status_ok != status)
                return logError("Failed to import " + std::string(oms::bus_connection));
              else
              {
                // Load connection geometry
                if (oms_status_ok != importFromSSD_ConnectionGeometry(*itConnection, element1+connector1, element2+connector2))
                  return logError("Failed to import " + std::string(oms2::ssd::ssd_connection_geometry));
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

oms_status_enu_t oms3::System::addConnector(const oms3::ComRef& cref, oms_causality_enu_t causality, oms_signal_type_enu_t type)
{
  oms3::ComRef tail(cref);
  oms3::ComRef head = tail.pop_front();
  auto subsystem = subsystems.find(head);
  if (subsystem != subsystems.end())
    return subsystem->second->addConnector(tail, causality, type);

  if (this->type == oms_system_tlm)
    return logError_NotForTlmSystem;
  if (!cref.isValidIdent())
    return logError_InvalidIdent(cref);
  if (!validCref(cref))
    return logError_AlreadyInScope(getFullCref() + cref);

  connectors.back() = new oms3::Connector(causality, type, cref);
  connectors.push_back(NULL);
  element.setConnectors(&connectors[0]);

  return oms_status_ok;
}

oms3::Connector* oms3::System::getConnector(const oms3::ComRef& cref)
{
  oms3::ComRef tail(cref);
  oms3::ComRef head = tail.pop_front();

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

oms3::BusConnector* oms3::System::getBusConnector(const oms3::ComRef& cref)
{
  oms3::ComRef tail(cref);
  oms3::ComRef head = tail.pop_front();
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

oms3::TLMBusConnector* oms3::System::getTLMBusConnector(const oms3::ComRef& cref)
{
  oms3::ComRef tail(cref);
  oms3::ComRef head = tail.pop_front();
  auto subsystem = subsystems.find(head);
  if (subsystem != subsystems.end())
    return subsystem->second->getTLMBusConnector(tail);

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

oms3::Connection** oms3::System::getConnections(const oms3::ComRef& cref)
{
  if (!cref.isEmpty())
  {
    oms3::ComRef tail(cref);
    oms3::ComRef head = tail.pop_front();
    auto subsystem = subsystems.find(head);
    if (subsystem != subsystems.end())
      return subsystem->second->getConnections(tail);
  }

  return &connections[0];
}

oms_status_enu_t oms3::System::addConnection(const oms3::ComRef& crefA, const oms3::ComRef& crefB)
{
  oms3::ComRef tailA(crefA);
  oms3::ComRef headA = tailA.pop_front();

  oms3::ComRef tailB(crefB);
  oms3::ComRef headB = tailB.pop_front();

  // if both A and B references the same subsystem, recurse into that subsystem
  if (headA == headB)
  {
    auto subsystem = subsystems.find(headA);
    if (subsystem != subsystems.end())
      return subsystem->second->addConnection(tailA,tailB);
  }

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
    connections.back() = new oms3::Connection(crefA, crefB, oms3_connection_bus);
    connections.push_back(NULL);
    return oms_status_ok;
  }

  // not a bus connection, attempt normal connection
  oms3::Connector* conA = this->getConnector(crefA);
  if (!conA) return logError_ConnectorNotInSystem(crefA, this);
  oms3::Connector* conB = this->getConnector(crefB);
  if (!conB) return logError_ConnectorNotInSystem(crefB, this);

  if (getConnection(crefA, crefB))
    return logError_ConnectionExistsAlready(crefA, crefB, this);

  if (conA->getType() != conB->getType())
    return logError("Type mismatch in connection: " + std::string(crefA) + " -> " + std::string(crefB));

  for (auto& connection : connections)
  {
    if (connection && conA->getCausality() == oms_causality_input && connection->containsSignal(crefA))
      return logError("Connector is already connected: " + std::string(crefA));
    if (connection && conB->getCausality() == oms_causality_input && connection->containsSignal(crefB))
      return logError("Connector is already connected: " + std::string(crefB));
  }

  // system inputs/outputs
  if (crefA.isValidIdent() && crefB.isValidIdent())
    return logError("Connections between inputs and outputs of the same system are forbidden: " + std::string(crefA) + " -> " + std::string(crefB));
  else if (crefA.isValidIdent() || crefB.isValidIdent())
  {
    // flipped causality check
    if (!((conA->getCausality() == oms_causality_output && conB->getCausality() == oms_causality_output) ||
      (conB->getCausality() == oms_causality_input && conA->getCausality() == oms_causality_input)))
      return logError("Causality mismatch in connection: " + std::string(crefA) + " -> " + std::string(crefB));
  }
  else
  {
    if (!((conA->getCausality() == oms_causality_output && conB->getCausality() == oms_causality_input) ||
      (conB->getCausality() == oms_causality_output && conA->getCausality() == oms_causality_input)))
      return logError("Causality mismatch in connection: " + std::string(crefA) + " -> " + std::string(crefB));
  }
  if ((crefA.isValidIdent() && conA->getCausality() == oms_causality_input) ||
      (!crefA.isValidIdent() && conA->getCausality() == oms_causality_output))
    connections.back() = new oms3::Connection(crefA, crefB);
  else
    connections.back() = new oms3::Connection(crefB, crefA);

  connections.push_back(NULL);
  return oms_status_ok;
}

oms_status_enu_t oms3::System::deleteConnection(const oms3::ComRef& crefA, const oms3::ComRef& crefB)
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

  oms3::ComRef tailA(crefA);
  oms3::ComRef headA = tailA.pop_front();

  oms3::ComRef tailB(crefB);
  oms3::ComRef headB = tailB.pop_front();

  //If both A and B references the same subsystem, recurse into that subsystem
  if (headA == headB)
  {
    auto subsystem = subsystems.find(headA);
    if (subsystem != subsystems.end())
      return subsystem->second->deleteConnection(tailA, tailB);
  }

  return logError_ConnectionNotInSystem(crefA, crefB, this);
}

oms_status_enu_t oms3::System::addTLMConnection(const oms3::ComRef& crefA, const oms3::ComRef& crefB, double delay, double alpha, double linearimpedance, double angularimpedance)
{
  if (type != oms_system_tlm)
    return logError_OnlyForTlmSystem;

  oms3::ComRef tailA(crefA);
  oms3::ComRef headA = tailA.pop_front();

  oms3::ComRef tailB(crefB);
  oms3::ComRef headB = tailB.pop_front();

  //If both A and B references the same subsystem, recurse into that subsystem
  if (headA == headB)
  {
    auto subsystem = subsystems.find(headA);
    if (subsystem != subsystems.end())
      return subsystem->second->addTLMConnection(tailA,tailB,delay,alpha,linearimpedance,angularimpedance);
  }

  TLMBusConnector *busA=0, *busB=0;
  auto subsystemA = subsystems.find(headA);
  if (subsystemA != subsystems.end())
    busA = subsystemA->second->getTLMBusConnector(tailA);
  auto subsystemB = subsystems.find(headB);
  if (subsystemB != subsystems.end())
    busB = subsystemB->second->getTLMBusConnector(tailB);
  if (busA && busB)
  {
    //Create bus connection
    connections.back() = new oms3::Connection(crefA, crefB, oms3_connection_tlm);
    connections.back()->setTLMParameters(delay, alpha, linearimpedance, angularimpedance);
    connections.push_back(NULL);
    busA->setDelay(delay);
    busB->setDelay(delay);
    return oms_status_ok;
  }

  return logError("TLM bus connector(s) not found in system");
}

oms_status_enu_t oms3::System::addBus(const oms3::ComRef& cref)
{
  oms3::ComRef tail(cref);
  oms3::ComRef head = tail.pop_front();

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

  oms3::BusConnector* bus = new oms3::BusConnector(cref);
  busconnectors.back() = bus;
  busconnectors.push_back(NULL);
  element.setBusConnectors(&busconnectors[0]);
  return oms_status_ok;
}

oms_status_enu_t oms3::System::addTLMBus(const oms3::ComRef& cref, const std::string domain, const int dimensions, const oms_tlm_interpolation_t interpolation)
{
  oms3::ComRef tail(cref);
  oms3::ComRef head = tail.pop_front();

  auto subsystem = subsystems.find(head);
  if (subsystem != subsystems.end())
    return subsystem->second->addTLMBus(tail, domain, dimensions, interpolation);

  auto externalmodel = components.find(head);
  if (externalmodel != components.end())
  {
    if (oms_component_external != externalmodel->second->getType())
      return logError_OnlyForExternalModels;
    return dynamic_cast<ExternalModel*>(externalmodel->second)->addTLMBus(tail, domain, dimensions, interpolation);
  }

  if (type == oms_system_tlm)
    return logError_NotForTlmSystem;
  if (!cref.isValidIdent())
    return logError_InvalidIdent(cref);
  if (!validCref(cref))
    return logError_AlreadyInScope(getFullCref() + cref);

  oms3::TLMBusConnector* bus = new oms3::TLMBusConnector(cref, domain, dimensions, interpolation, this);
  tlmbusconnectors.back() = bus;
  tlmbusconnectors.push_back(NULL);
  element.setTLMBusConnectors(&tlmbusconnectors[0]);
  return oms_status_ok;
}

oms_status_enu_t oms3::System::addConnectorToBus(const oms3::ComRef& busCref, const oms3::ComRef& connectorCref)
{
  oms3::ComRef busTail(busCref);
  oms3::ComRef busHead = busTail.pop_front();
  oms3::ComRef connectorTail(connectorCref);
  oms3::ComRef connectorHead = connectorTail.pop_front();
  //If both bus and connector references the same subsystem, recurse into that subsystem
  if (busHead == connectorHead)
  {
    auto subsystem = subsystems.find(busHead);
    if (subsystem != subsystems.end())
      return subsystem->second->addConnectorToBus(busTail,connectorTail);
  }

  if (!busTail.isEmpty() && !connectorTail.isEmpty() && busHead != connectorHead)
    return logError_BusAndConnectorNotSameSystem;
  if (type == oms_system_tlm)
    return logError_NotForTlmSystem;

  for(auto& bus : busconnectors)
    if (bus && bus->getName() == busCref)
      bus->addConnector(connectorCref);

  return oms_status_ok;
}

oms_status_enu_t oms3::System::deleteConnectorFromBus(const oms3::ComRef& busCref, const oms3::ComRef& connectorCref)
{
  oms3::ComRef busTail(busCref);
  oms3::ComRef busHead = busTail.pop_front();
  oms3::ComRef connectorTail(connectorCref);
  oms3::ComRef connectorHead = connectorTail.pop_front();
  //If both bus and connector references the same subsystem, recurse into that subsystem
  if(busHead == connectorHead)
  {
    auto subsystem = subsystems.find(busHead);
    if(subsystem != subsystems.end())
      return subsystem->second->deleteConnectorFromBus(busTail,connectorTail);
  }

  if(!busTail.isEmpty() && !connectorTail.isEmpty() && busHead != connectorHead)
    return logError_BusAndConnectorNotSameSystem;
  if(type == oms_system_tlm)
    return logError_NotForTlmSystem;

  for(auto& bus : busconnectors)
    if(bus && bus->getName() == busCref)
      if (oms_status_ok != bus->deleteConnector(connectorCref))
        return logError_ConnectorNotInSystem(connectorCref, this);

  return oms_status_ok;
}

oms_status_enu_t oms3::System::addConnectorToTLMBus(const oms3::ComRef& busCref, const oms3::ComRef& connectorCref, const std::string type)
{
  oms3::ComRef busTail(busCref);
  oms3::ComRef busHead = busTail.pop_front();
  oms3::ComRef connectorTail(connectorCref);
  oms3::ComRef connectorHead = connectorTail.pop_front();
  //If both bus and connector references the same subsystem, recurse into that subsystem
  if (busHead == connectorHead)
  {
    auto subsystem = subsystems.find(busHead);
    if (subsystem != subsystems.end())
      return subsystem->second->addConnectorToTLMBus(busTail,connectorTail,type);
  }

  if (this->type == oms_system_tlm)
    return logError_NotForTlmSystem;

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
    {
      oms_status_enu_t status = bus->addConnector(connectorCref,type);
      if (oms_status_ok != status)
        return status;
    }
  }
  return oms_status_ok;
}

oms_status_enu_t oms3::System::deleteConnectorFromTLMBus(const oms3::ComRef& busCref, const oms3::ComRef& connectorCref)
{
  oms3::ComRef busTail(busCref);
  oms3::ComRef busHead = busTail.pop_front();
  oms3::ComRef connectorTail(connectorCref);
  oms3::ComRef connectorHead = connectorTail.pop_front();
  //If both bus and connector references the same subsystem, recurse into that subsystem
  if(busHead == connectorHead)
  {
    auto subsystem = subsystems.find(busHead);
    if(subsystem != subsystems.end())
      return subsystem->second->deleteConnectorFromTLMBus(busTail,connectorTail);
  }

  if(!busTail.isEmpty() && !connectorTail.isEmpty() && busHead != connectorHead)
    return logError_BusAndConnectorNotSameSystem;
  if(type == oms_system_tlm)
    return logError_NotForTlmSystem;

  for(auto& bus : tlmbusconnectors)
    if(bus && bus->getName() == busCref)
      if (oms_status_ok != bus->deleteConnector(connectorCref))
        return logError_ConnectorNotInSystem(connectorCref, this);

  return oms_status_ok;
}

oms_status_enu_t oms3::System::addExternalModel(const oms3::ComRef& cref, std::string path, std::string startscript)
{
  if (type != oms_system_tlm)
    return logError_OnlyForTlmSystem;

  if (!cref.isValidIdent())
    return oms_status_error;

  oms3::ExternalModel* externalmodel = oms3::ExternalModel::NewComponent(cref, this, path, startscript);
  if (!externalmodel)
    return oms_status_error;

  components[cref] = externalmodel;
  subelements.back() = reinterpret_cast<oms3_element_t*>(externalmodel->getElement());
  subelements.push_back(NULL);
  element.setSubElements(&subelements[0]);

  return oms_status_ok;
}

oms_status_enu_t oms3::System::setConnectorGeometry(const oms3::ComRef& cref, const oms2::ssd::ConnectorGeometry *geometry)
{
  oms3::ComRef tail(cref);
  oms3::ComRef head = tail.pop_front();
  auto subsystem = subsystems.find(head);
  if (subsystem != subsystems.end())
    return subsystem->second->setConnectorGeometry(tail,geometry);

  auto component = components.find(head);
  if (component != components.end())
  {
    oms3::Connector *connector = component->second->getConnector(tail);
    if (connector)
    {
      connector->setGeometry(geometry);
      return oms_status_ok;
    }
    else {
      return logError("Connector " + std::string(tail) + " not found in component " + std::string(head));
    }
  }

  oms3::Connector* connector = this->getConnector(cref);
  if (connector)
  {
    connector->setGeometry(geometry);
    return oms_status_ok;
  }
  return logError("Connector " + std::string(cref) + " not found in system " + std::string(getCref()));
}

oms_status_enu_t oms3::System::setConnectionGeometry(const oms3::ComRef& crefA, const oms3::ComRef& crefB, const oms3::ssd::ConnectionGeometry *geometry)
{
  oms3::ComRef tailA(crefA);
  oms3::ComRef headA = tailA.pop_front();

  oms3::ComRef tailB(crefB);
  oms3::ComRef headB = tailB.pop_front();

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


oms_status_enu_t oms3::System::setTLMConnectionParameters(const ComRef &crefA, const ComRef &crefB, const oms3_tlm_connection_parameters_t* parameters)
{
  oms3::ComRef tailA(crefA);
  oms3::ComRef headA = tailA.pop_front();

  oms3::ComRef tailB(crefB);
  oms3::ComRef headB = tailB.pop_front();

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
}

oms_status_enu_t oms3::System::setBusGeometry(const oms3::ComRef& cref, const oms2::ssd::ConnectorGeometry *geometry)
{
  oms3::ComRef tail(cref);
  oms3::ComRef head = tail.pop_front();
  auto subsystem = subsystems.find(head);
  if (subsystem != subsystems.end())
    return subsystem->second->setBusGeometry(tail,geometry);

  oms3::BusConnector* busConnector = this->getBusConnector(cref);
  if (busConnector)
  {
    busConnector->setGeometry(geometry);
    return oms_status_ok;
  }
  return logError("Bus " + std::string(cref)+" not found in system " + std::string(getCref()));
}

oms_status_enu_t oms3::System::setTLMBusGeometry(const oms3::ComRef& cref, const oms2::ssd::ConnectorGeometry *geometry)
{
  oms3::ComRef tail(cref);
  oms3::ComRef head = tail.pop_front();
  auto subsystem = subsystems.find(head);
  if (subsystem != subsystems.end())
    return subsystem->second->setTLMBusGeometry(tail,geometry);

  oms3::TLMBusConnector* tlmBusConnector = this->getTLMBusConnector(cref);
  if (tlmBusConnector)
  {
    tlmBusConnector->setGeometry(geometry);
    return oms_status_ok;
  }
  return logError("TLM Bus " + std::string(cref)+" not found in system " + std::string(getCref()));
}

oms3::Connection* oms3::System::getConnection(const oms3::ComRef& crefA, const oms3::ComRef& crefB)
{
  for (auto& connection : connections)
    if (connection && connection->isEqual(crefA, crefB))
      return connection;
  return NULL;
}

oms3::Model* oms3::System::getModel()
{
  if (parentSystem)
    return parentSystem->getModel();
  return parentModel;
}

oms_status_enu_t oms3::System::deleteAllConectionsTo(const oms3::ComRef& cref)
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

bool oms3::System::isConnected(const ComRef& cref) const
{
  for (int i=0; i<connections.size(); ++i)
    if (connections[i] && connections[i]->containsSignal(cref))
      return true;

  return false;
}

oms_status_enu_t oms3::System::delete_(const oms3::ComRef& cref)
{
  oms3::ComRef tail(cref);
  oms3::ComRef front = tail.pop_front();

  if (tail.isEmpty())
  {
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
      deleteAllConectionsTo(front);
      delete component->second;
      components.erase(component);
      return oms_status_ok;
    }

    for (int i=0; i<connectors.size()-1; ++i)
      if (connectors[i]->getName() == front)
      {
        delete connectors[i];
        connectors.pop_back();   // last element is always NULL
        connectors[i] = connectors.back();
        connectors.back() = NULL;
        return oms_status_ok;
      }
  }
  else
  {
    auto subsystem = subsystems.find(front);
    if (subsystem != subsystems.end())
      return subsystem->second->delete_(tail);

    auto component = components.find(front);
    if (component != components.end())
      return component->second->deleteConnector(tail);
  }

  return oms_status_error;
}

bool oms3::System::copyResources()
{
  if (parentSystem)
    return parentSystem->copyResources();
  return parentModel->copyResources();
}

oms_status_enu_t oms3::System::getAllResources(std::vector<std::string>& resources)
{
  for (const auto& component : components)
    if (oms_status_ok != component.second->getAllResources(resources))
      return oms_status_error;

  for (const auto& subsystem : subsystems)
    if (oms_status_ok != subsystem.second->getAllResources(resources))
      return oms_status_error;

  return oms_status_ok;
}

oms_status_enu_t oms3::System::exportDependencyGraphs(const std::string& pathInitialization, const std::string& pathSimulation)
{
  oms_status_enu_t status = updateDependencyGraphs();

  initialUnknownsGraph.dotExport(pathInitialization);
  outputsGraph.dotExport(pathSimulation);

  return status;
}

oms_status_enu_t oms3::System::updateDependencyGraphs()
{
  initialUnknownsGraph.clear();
  outputsGraph.clear();

  for (const auto& subsystem : subsystems)
  {
    if (oms_status_ok != subsystem.second->updateDependencyGraphs())
      return oms_status_error;

    initialUnknownsGraph.includeGraph(subsystem.second->getInitialUnknownsGraph(), subsystem.first);
    outputsGraph.includeGraph(subsystem.second->getOutputsGraph(), subsystem.first);
  }

  for (const auto& component : components)
  {
    initialUnknownsGraph.includeGraph(component.second->getInitialUnknownsGraph(), component.first);
    outputsGraph.includeGraph(component.second->getOutputsGraph(), component.first);
  }

  for (const auto& connection : connections)
  {
    if (!connection || connection->getType() != oms3_connection_single)
      continue;

    Connector* varA = getConnector(connection->getSignalA());
    Connector* varB = getConnector(connection->getSignalB());
    if (varA && varB)
    {
      // flip causality checks for connectors (top-level crefs)
      bool outA = connection->getSignalA().isValidIdent() ? varA->isInput() : varA->isOutput();
      bool inB = connection->getSignalB().isValidIdent() ? varB->isOutput() : varB->isInput();

      if (outA && inB)
      {
        initialUnknownsGraph.addEdge(Connector(varA->getCausality(), varA->getType(), connection->getSignalA()), Connector(varB->getCausality(), varB->getType(), connection->getSignalB()));
        outputsGraph.addEdge(Connector(varA->getCausality(), varA->getType(), connection->getSignalA()), Connector(varB->getCausality(), varB->getType(), connection->getSignalB()));
      }
      else
        return logError("failed for " + std::string(connection->getSignalA()) + " -> " + std::string(connection->getSignalB()));
    }
    else
      return logError("invalid connection");
  }

  return oms_status_ok;
}

oms_status_enu_t oms3::System::getBoolean(const ComRef& cref, bool& value)
{
  switch (getModel()->getModelState())
  {
    case oms_modelState_instantiated:
    case oms_modelState_initialization:
    case oms_modelState_simulation:
      break;
    default:
      return logError_ModelInWrongState(getModel());
  }

  oms3::ComRef tail(cref);
  oms3::ComRef head = tail.pop_front();

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
      auto booleanValue = booleanValues.find(cref);
      if (booleanValue != booleanValues.end())
        value = booleanValue->second;
      else
        value = 0; // default value
      return oms_status_ok;
    }
  }

  return oms_status_error;
}

oms_status_enu_t oms3::System::getInteger(const ComRef& cref, int& value)
{
  switch (getModel()->getModelState())
  {
    case oms_modelState_instantiated:
    case oms_modelState_initialization:
    case oms_modelState_simulation:
      break;
    default:
      return logError_ModelInWrongState(getModel());
  }

  oms3::ComRef tail(cref);
  oms3::ComRef head = tail.pop_front();

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
      auto integerValue = integerValues.find(cref);
      if (integerValue != integerValues.end())
        value = integerValue->second;
      else
        value = 0; // default value
      return oms_status_ok;
    }
  }

  return oms_status_error;
}

oms_status_enu_t oms3::System::getReal(const ComRef& cref, double& value)
{
  switch (getModel()->getModelState())
  {
    case oms_modelState_instantiated:
    case oms_modelState_initialization:
    case oms_modelState_simulation:
      break;
    default:
      return logError_ModelInWrongState(getModel());
  }

  oms3::ComRef tail(cref);
  oms3::ComRef head = tail.pop_front();

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
      auto realValue = realValues.find(cref);
      if (realValue != realValues.end())
        value = realValue->second;
      else
        value = 0.0; // default value
      return oms_status_ok;
    }
  }

  return oms_status_error;
}

oms_status_enu_t oms3::System::setBoolean(const ComRef& cref, bool value)
{
  switch (getModel()->getModelState())
  {
    case oms_modelState_instantiated:
    case oms_modelState_initialization:
    case oms_modelState_simulation:
      break;
    default:
      return logError_ModelInWrongState(getModel());
  }

  oms3::ComRef tail(cref);
  oms3::ComRef head = tail.pop_front();

  auto subsystem = subsystems.find(head);
  if (subsystem != subsystems.end())
    return subsystem->second->setBoolean(tail, value);

  auto component = components.find(head);
  if (component != components.end())
    return component->second->setBoolean(tail, value);

  for (auto& connector : connectors)
    if (connector && connector->getName() == cref && connector->isTypeBoolean())
    {
      booleanValues[cref] = value;
      return oms_status_ok;
    }

  return oms_status_error;
}

oms_status_enu_t oms3::System::setInteger(const ComRef& cref, int value)
{
  switch (getModel()->getModelState())
  {
    case oms_modelState_instantiated:
    case oms_modelState_initialization:
    case oms_modelState_simulation:
      break;
    default:
      return logError_ModelInWrongState(getModel());
  }

  oms3::ComRef tail(cref);
  oms3::ComRef head = tail.pop_front();

  auto subsystem = subsystems.find(head);
  if (subsystem != subsystems.end())
    return subsystem->second->setInteger(tail, value);

  auto component = components.find(head);
  if (component != components.end())
    return component->second->setInteger(tail, value);

  for (auto& connector : connectors)
    if (connector && connector->getName() == cref && connector->isTypeInteger())
    {
      integerValues[cref] = value;
      return oms_status_ok;
    }

  return oms_status_error;
}

oms_status_enu_t oms3::System::setReal(const ComRef& cref, double value)
{
  switch (getModel()->getModelState())
  {
    case oms_modelState_instantiated:
    case oms_modelState_initialization:
    case oms_modelState_simulation:
      break;
    default:
      return logError_ModelInWrongState(getModel());
  }

  oms3::ComRef tail(cref);
  oms3::ComRef head = tail.pop_front();

  auto subsystem = subsystems.find(head);
  if (subsystem != subsystems.end())
    return subsystem->second->setReal(tail, value);

  auto component = components.find(head);
  if (component != components.end())
    return component->second->setReal(tail, value);

  for (auto& connector : connectors)
    if (connector && connector->getName() == cref && connector->isTypeReal())
    {
      realValues[cref] = value;
      return oms_status_ok;
    }

  return oms_status_error;
}

oms_status_enu_t oms3::System::getReals(const std::vector<oms3::ComRef> &sr, std::vector<double> &values) const
{
  return logError_NotImplemented;
}

oms_status_enu_t oms3::System::setReals(const std::vector<oms3::ComRef> &crefs, std::vector<double> values)
{
  return logError_NotImplemented;
}

oms_status_enu_t oms3::System::setRealInputDerivatives(const oms3::ComRef &cref, int order, double value)
{
  return logError_NotImplemented;
}

oms_status_enu_t oms3::System::importFromSSD_ConnectionGeometry(const pugi::xml_node& node, const ComRef& crefA, const ComRef& crefB)
{
  pugi::xml_node connectionGeometryNode = node.child(oms2::ssd::ssd_connection_geometry);
  if (connectionGeometryNode)
  {
    oms3::Connection *connection = getConnection(crefA, crefB);
    if (connection)
    {
      oms3::ssd::ConnectionGeometry geometry;
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

oms_status_enu_t oms3::System::registerSignalsForResultFile(ResultWriter& resultFile)
{
  for (const auto& component : components)
    if (oms_status_ok != component.second->registerSignalsForResultFile(resultFile))
      return oms_status_error;

  for (const auto& subsystem : subsystems)
    if (oms_status_ok != subsystem.second->registerSignalsForResultFile(resultFile))
      return oms_status_error;

  resultFileMapping.clear();
  for (unsigned int i=0; i<connectors.size(); ++i)
  {
    //if (!exportVariables[i])
    //  continue;

    if (!connectors[i])
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

oms_status_enu_t oms3::System::updateSignals(ResultWriter& resultFile, double time)
{
  for (const auto& component : components)
    if (oms_status_ok != component.second->updateSignals(resultFile, time))
      return oms_status_error;

  for (const auto& subsystem : subsystems)
    if (oms_status_ok != subsystem.second->updateSignals(resultFile, time))
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
