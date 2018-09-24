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
#include "Model.h"
#include "SystemSC.h"
#include "SystemTLM.h"
#include "SystemWC.h"
#include "Types.h"
#include "ssd/Tags.h"

oms3::System::System(const oms3::ComRef& cref, oms_system_enu_t type, oms3::Model* parentModel, oms3::System* parentSystem)
  : element(oms_element_system, cref), cref(cref), type(type), parentModel(parentModel), parentSystem(parentSystem)
{
  connections.push_back(NULL);

  connectors.push_back(NULL);
  element.setConnectors(&connectors[0]);

  subelements.push_back(NULL);
  element.setSubElements(&subelements[0]);
}

oms3::System::~System()
{
  for (const auto& connector : connectors)
    if (connector)
      delete connector;

  for (const auto& connection : connections)
    if(connection)
      delete connection;

  for (const auto& component : components)
    delete component.second;

  for (const auto& subsystem : subsystems)
    delete subsystem.second;
}

oms3::System* oms3::System::NewSystem(const oms3::ComRef& cref, oms_system_enu_t type, oms3::Model* parentModel, oms3::System* parentSystem)
{
  if (!cref.isValidIdent())
  {
    logError("\"" + std::string(cref) + "\" is not a valid ident");
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
    logError("Internal error");
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

  logError("Internal error");
  return NULL;
}

oms3::ComRef oms3::System::getFullName()
{
  if (parentSystem)
    return parentSystem->getFullName() + this->getName();
  if (parentModel)
    return ComRef(parentModel->getName()) + this->getName();

  logError("Internal error");
  return this->getName();
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
  auto it = components.find(cref);
  if (it == components.end())
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

  return true;
}

oms_status_enu_t oms3::System::addSubSystem(const oms3::ComRef& cref, oms_system_enu_t type)
{
  if (cref.isValidIdent())
  {
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
    return logError("System \"" + std::string(getFullName()) + "\" does not contain system \"" + std::string(front) + "\"");

  return system->addSubSystem(tail, type);
}

oms_status_enu_t oms3::System::list(const oms3::ComRef& cref, char** contents)
{
  return logError("not implemented");
}

oms_status_enu_t oms3::System::exportToSSD(pugi::xml_node& node) const
{
  node.append_attribute("name") = this->getName().c_str();

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
    if(connector)
      connector->exportToSSD(connectors_node);

  pugi::xml_node connections_node = node.append_child(oms2::ssd::ssd_connections);
  for (const auto& connection : connections)
    if (connection)
      connection->exportToSSD(connections_node);

  if(!busconnectors.empty()) {
    pugi::xml_node annotations_node = node.append_child(oms2::ssd::ssd_annotations);
    pugi::xml_node annotation_node = annotations_node.append_child(oms2::ssd::ssd_annotation);
    annotation_node.append_attribute("type") = "org.openmodelica";
    for (const auto& busconnector : busconnectors) {
      busconnector->exportToSSD(annotation_node);
    }
  }

  return oms_status_ok;
}

oms_status_enu_t oms3::System::addConnector(const oms3::ComRef &cref, oms_causality_enu_t causality, oms_signal_type_enu_t type)
{
  oms3::ComRef tail(cref);
  oms3::ComRef head = tail.pop_front();
  auto subsystem = subsystems.find(head);
  if(subsystem != subsystems.end()) {
    return subsystem->second->addConnector(tail,causality,type);
  }

  if(!cref.isValidIdent()) {
    return logError("Not a valid ident: "+std::string(cref));
  }

  oms3::Connector* connector = new oms3::Connector(causality, type, cref);
  connectors.back() = connector;
  connectors.push_back(NULL);
  element.setConnectors(&connectors[0]);

  return oms_status_ok;
}

oms3::Connector *oms3::System::getConnector(const oms3::ComRef &cref)
{
  for(auto &connector : connectors) {
    if(connector && connector->getName() == cref)
      return connector;
  }
  return NULL;
}

oms3::Connection **oms3::System::getConnections(const oms3::ComRef &cref) {
  if(!cref.isEmpty()) {
    oms3::ComRef tail(cref);
    oms3::ComRef head = tail.pop_front();
    auto subsystem = subsystems.find(head);
    if(subsystem != subsystems.end()) {
      return subsystem->second->getConnections(tail);
    }
  }

  return &connections[0];
}

oms_status_enu_t oms3::System::addConnection(const oms3::ComRef &crefA, const oms3::ComRef &crefB)
{
  oms3::ComRef tailA(crefA);
  oms3::ComRef headA = tailA.pop_front();

  oms3::ComRef tailB(crefB);
  oms3::ComRef headB = tailB.pop_front();

  //If both A and B references the same subsystem, recurse into that subsystem
  if(headA == headB) {
    auto subsystem = subsystems.find(headA);
    if(subsystem != subsystems.end()) {
      return subsystem->second->addConnection(tailA,tailB);
    }
  }

  for(auto &connection : connections) {
    if(connection &&
       (connection->getSignalA() == crefA ||
       connection->getSignalB() == crefA))
      return logError("Connector is already connected: "+std::string(crefA));
    if(connection &&
       (connection->getSignalA() == crefB ||
       connection->getSignalB() == crefB))
      return logError("Connector is already connected: "+std::string(crefB));
  }

  //Attempt to create connection inside current system
  oms3::Connector *conA = 0;
  oms3::Connector *conB = 0;

  //Look in own connectors
  if(!conA)
    conA = this->getConnector(crefA);
  if(!conB)
    conB = this->getConnector(crefA);

  //Look in subsystem connectors
  auto subsystemA = subsystems.find(headA);
  if(!conA && subsystemA != subsystems.end())
    conA = subsystemA->second->getConnector(tailA);
  auto subsystemB = subsystems.find(headB);
  if(!conB && subsystemB != subsystems.end())
    conB = subsystemB->second->getConnector(tailB);

  //Look in component connectors
  auto componentA = components.find(headA);
  if(!conA && componentA != components.end())
    conA = componentA->second->getConnector(tailA);
  auto componentB = components.find(headB);
  if(!conB && componentB != components.end())
    conB = componentB->second->getConnector(tailB);

  //todo: Look in bus connectors

  if(conA && conB) {
    if(conA->getType() != conB->getType())
      return logError("Type mismatch in connection: "+std::string(crefA)+" -> "+std::string(crefB));
    if((conA->getCausality() == oms_causality_output && conB->getCausality() != oms_causality_input) ||
       (conB->getCausality() == oms_causality_output && conA->getCausality() != oms_causality_input))
      return logError("Causality mismatch in connection: "+std::string(crefA)+" -> "+std::string(crefB));

    connections.back() = new oms3::Connection(crefA,crefB);
    connections.push_back(NULL);
    return oms_status_ok;
  }

  return logError("Connector(s) not found in system");
}

oms_status_enu_t oms3::System::addBus(const oms3::ComRef &cref)
{
  oms3::ComRef tail(cref);
  oms3::ComRef head = tail.pop_front();
  auto subsystem = subsystems.find(head);
  if(subsystem != subsystems.end()) {
    return subsystem->second->addBus(tail);
  }
  if(!cref.isValidIdent()) {
    return logError("Not a valid ident: "+std::string(cref));
  }
  oms3::BusConnector* bus = new oms3::BusConnector(cref);
  busconnectors.push_back(bus);
  return oms_status_ok;
}

oms_status_enu_t oms3::System::addConnectorToBus(const oms3::ComRef &busCref, const oms3::ComRef &connectorCref)
{
  oms3::ComRef busTail(busCref);
  oms3::ComRef busHead = busTail.pop_front();
  oms3::ComRef connectorTail(connectorCref);
  oms3::ComRef connectorHead = connectorTail.pop_front();
  //If both bus and connector references the same subsystem, recurse into that subsystem
  if(busHead == connectorHead) {
    auto subsystem = subsystems.find(busHead);
    if(subsystem != subsystems.end()) {
      return subsystem->second->addConnectorToBus(busTail,connectorTail);
    }
  }
  for(auto& bus : busconnectors) {
    if(bus->getName() == busCref) {
      bus->addConnector(connectorCref);
    }
  }
  return oms_status_ok;
}

oms_status_enu_t oms3::System::setConnectorGeometry(const oms3::ComRef &cref, const oms2::ssd::ConnectorGeometry *geometry)
{
  oms3::ComRef tail(cref);
  oms3::ComRef head = tail.pop_front();
  auto subsystem = subsystems.find(head);
  if(subsystem != subsystems.end())
    return subsystem->second->setConnectorGeometry(tail,geometry);

  auto component = components.find(head);
  if(component != components.end()) {
    oms3::Connector *connector = component->second->getConnector(tail);
    if(connector) {
      connector->setGeometry(geometry);
      return oms_status_ok;
    }
    else {
      return logError("Connector "+std::string(tail)+" not found in component "+std::string(head));
    }
  }

  oms3::Connector* connector = this->getConnector(cref);
  if(connector) {
    connector->setGeometry(geometry);
    return oms_status_ok;
  }
  return logError("Connector "+std::string(cref)+" not found in system "+std::string(getName()));
}

oms_status_enu_t oms3::System::setConnectionGeometry(const oms3::ComRef &crefA, const oms3::ComRef &crefB, const oms2::ssd::ConnectionGeometry *geometry)
{
  oms3::ComRef tailA(crefA);
  oms3::ComRef headA = tailA.pop_front();

  oms3::ComRef tailB(crefB);
  oms3::ComRef headB = tailB.pop_front();

  //If both A and B references the same subsystem, recurse into that subsystem
  if(headA == headB) {
    auto subsystem = subsystems.find(headA);
    if(subsystem != subsystems.end()) {
      return subsystem->second->setConnectionGeometry(tailA,tailB,geometry);
    }
  }

  for(auto &connection : connections) {
    if((connection && connection->getSignalA() == crefA && connection->getSignalB() == crefB) ||
       (connection && connection->getSignalA() == crefB && connection->getSignalB() == crefA)) {
      connection->setGeometry(geometry);
      return oms_status_ok;
    }
  }

  return logError("Connector(s) not found in system");
}
