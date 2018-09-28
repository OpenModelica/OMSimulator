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
    if(connection)
      delete connection;

  for (const auto& component : components)
    delete component.second;

  for (const auto& subsystem : subsystems)
    delete subsystem.second;

  for (const auto& busconnector : busconnectors)
    if(busconnector)
      delete busconnector;

  for (const auto tlmbusconnector : tlmbusconnectors)
    if(tlmbusconnector)
      delete tlmbusconnector;
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

  std::vector<oms3::Connection*> busconnections;
  pugi::xml_node connections_node = node.append_child(oms2::ssd::ssd_connections);
  for (const auto& connection : connections)
    if (connection && connection->getType() == oms3_connection_single)
      connection->exportToSSD(connections_node);
    else if(connection)
      busconnections.push_back(connection);

  if(busconnectors[0] || tlmbusconnectors[0] || !busconnections.empty()) {
    pugi::xml_node annotations_node = node.append_child(oms2::ssd::ssd_annotations);
    pugi::xml_node annotation_node = annotations_node.append_child(oms2::ssd::ssd_annotation);
    annotation_node.append_attribute("type") = "org.openmodelica";
    for (const auto& busconnector : busconnectors)
      if(busconnector)
        busconnector->exportToSSD(annotation_node);
    for (const auto& tlmbusconnector : tlmbusconnectors)
      if(tlmbusconnector)
        tlmbusconnector->exportToSSD(annotation_node);
    if(!busconnections.empty()) {
      pugi::xml_node busconnections_node = annotation_node.append_child("OMSimulator:BusConnections");
      for (const auto& busconnection : busconnections) {
        busconnection->exportToSSD(busconnections_node);
      }
    }
  }

  return oms_status_ok;
}

oms_status_enu_t oms3::System::importFromSSD(const pugi::xml_node& node)
{
  for(pugi::xml_node_iterator it = node.begin(); it != node.end(); ++it)
  {
    std::string name = it->name();
    if(name == oms2::ssd::ssd_simulation_information)
    {
      if (oms_status_ok != importFromSSD_SimulationInformation(*it))
        return logError("Failed to import " + std::string(oms2::ssd::ssd_simulation_information));
    }
    else if(name == oms2::ssd::ssd_element_geometry)
    {
      oms3::ssd::ElementGeometry geometry;
      double x1 = (*it).attribute("x1").as_double();
      double y1 = (*it).attribute("y1").as_double();
      double x2 = (*it).attribute("x2").as_double();
      double y2 = (*it).attribute("y2").as_double();
      geometry.setSizePosition(x1, y1, x2, y2);

      double rotation = (*it).attribute("rotation").as_double();
      geometry.setRotation(rotation);

      std::string iconSource = (*it).attribute("iconSource").as_string();
      geometry.setIconSource(iconSource);

      double iconRotation = (*it).attribute("iconRotation").as_double();
      geometry.setIconRotation(iconRotation);

      bool iconFlip = (*it).attribute("iconFlip").as_bool();
      geometry.setIconFlip(iconFlip);

      bool iconFixedAspectRatio = (*it).attribute("iconFixedAspectRatio").as_bool();
      geometry.setIconFixedAspectRatio(iconFixedAspectRatio);

      setGeometry(geometry);
    }
    else if(name == oms2::ssd::ssd_connections)
    {
      for(pugi::xml_node_iterator itConnectors = (*it).begin(); itConnectors != (*it).end(); ++itConnectors)
      {
        ComRef startElement = ComRef(itConnectors->attribute("startElement").as_string());
        ComRef startConnector = ComRef(itConnectors->attribute("startConnector").as_string());
        ComRef endElement = ComRef(itConnectors->attribute("endElement").as_string());
        ComRef endConnector = ComRef(itConnectors->attribute("endConnector").as_string());
        ComRef crefA = startElement;
        if (!startConnector.isEmpty())
          crefA = crefA + startConnector;
        ComRef crefB = endElement;
        if (!endConnector.isEmpty())
          crefB = crefB + endConnector;
        if (oms_status_ok != addConnection(crefA, crefB))
          return logError("Failed to import " + std::string(oms2::ssd::ssd_connector));
      }
    }
    else if(name == oms2::ssd::ssd_connectors)
    {
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
        else
          return logError("Failed to import " + std::string(oms2::ssd::ssd_connector) + ":causality");
        oms_signal_type_enu_t type = oms_signal_type_real;
        if (typeString == "Real")
          type = oms_signal_type_real;
        else if (typeString == "Integer")
          type = oms_signal_type_integer;
        else if (typeString == "Boolean")
          type = oms_signal_type_boolean;
        else
          return logError("Failed to import " + std::string(oms2::ssd::ssd_connector) + ":type");
        if (oms_status_ok != addConnector(cref, causality, type))
          return logError("Failed to import " + std::string(oms2::ssd::ssd_connector));
        else
        {
          // Load connector geometry
          pugi::xml_node connectorGeometryNode = itConnectors->child(oms2::ssd::ssd_connector_geometry);
          if (connectorGeometryNode)
          {
            oms3::Connector* connector = getConnector(cref);
            if (connector)
            {
              oms2::ssd::ConnectorGeometry geometry(0.0, 0.0);
              geometry.setPosition(connectorGeometryNode.attribute("x").as_double(), connectorGeometryNode.attribute("y").as_double());
              connector->setGeometry(&geometry);
            }
          }
        }
      }
    }
    else if(name == oms2::ssd::ssd_elements)
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
        else
          return logError("wrong xml schema detected: " + name);
      }
    }
    else if(name == oms2::ssd::ssd_annotations)
    {
      pugi::xml_node annotation_node = it->child(oms2::ssd::ssd_annotation);
      if(annotation_node && std::string(annotation_node.attribute("type").as_string()) == "org.openmodelica") {
        for(pugi::xml_node_iterator itAnnotations = annotation_node.begin(); itAnnotations != annotation_node.end(); ++itAnnotations)
        {
          name = itAnnotations->name();
          if (std::string(name) == "OMSimulator:Bus")
          {
            //Load bus connector
            std::string busname = itAnnotations->attribute("name").as_string();
            if(std::string(itAnnotations->attribute("type").as_string()) == "tlm") {
              std::string domain = itAnnotations->attribute("domain").as_string();
              int dimensions = itAnnotations->attribute("dimensions").as_int();
              std::string interpolationStr = itAnnotations->attribute("interpolation").as_string();
              oms_tlm_interpolation_t interpolation;
              if(interpolationStr == "none")
                interpolation = oms_tlm_no_interpolation;
              else if(interpolationStr == "coarsegrained")
                interpolation = oms_tlm_coarse_grained;
              else if(interpolationStr == "finegrained")
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
            pugi::xml_node signals_node = itAnnotations->child("Signals");
            if(signals_node) {
              for(pugi::xml_node_iterator itSignals = signals_node.begin(); itSignals != signals_node.end(); ++itSignals) {
                name = itSignals->name();
                if(name == "Signal") {
                  std::string signalname = itSignals->attribute("name").as_string();
                  if(std::string(itAnnotations->attribute("type").as_string()) == "tlm") {
                    std::string signaltype = itSignals->attribute("type").as_string();
                    addConnectorToTLMBus(busname, signalname, signaltype);
                  }
                  else {
                    addConnectorToBus(busname, signalname);
                  }
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
          else if(std::string(name) == "OMSimulator:BusConnections")
          {

            //Load bus connections

            for(pugi::xml_node_iterator itTLMConnection = itAnnotations->begin(); itTLMConnection != itAnnotations->end(); ++itTLMConnection) {

              //Load TLM bus connection

              oms3::ComRef element1(itTLMConnection->attribute("startElement").as_string());
              oms3::ComRef connector1(itTLMConnection->attribute("startConnector").as_string());
              oms3::ComRef element2(itTLMConnection->attribute("endElement").as_string());
              oms3::ComRef connector2(itTLMConnection->attribute("endConnector").as_string());
              double delay = itTLMConnection->attribute("delay").as_double();
              double alpha = itTLMConnection->attribute("alpha").as_double();
              double impedance = itTLMConnection->attribute("impedance").as_double();
              double impedancerot = itTLMConnection->attribute("impedancerot").as_double();

              addTLMConnection(element1+connector1, element2+connector2, delay, alpha, impedance, impedancerot);
            }
          }
        }
      }
    }
    else
      return logError("wrong xml schema detected: " + name);
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
  oms3::ComRef tail(cref);
  oms3::ComRef head = tail.pop_front();
  auto subsystem = subsystems.find(head);
  if (subsystem != subsystems.end()) {
    return subsystem->second->getConnector(tail);
  }

  if (!cref.isValidIdent()) {
    logError("Not a valid ident: "+std::string(cref));
    return NULL;
  }

  for(auto &connector : connectors) {
    if(connector && connector->getName() == cref)
      return connector;
  }
  return NULL;
}

oms3::BusConnector *oms3::System::getBusConnector(const oms3::ComRef &cref)
{
  oms3::ComRef tail(cref);
  oms3::ComRef head = tail.pop_front();
  auto subsystem = subsystems.find(head);
  if (subsystem != subsystems.end()) {
    return subsystem->second->getBusConnector(tail);
  }
  if (!cref.isValidIdent()) {
    logError("Not a valid ident: "+std::string(cref));
    return NULL;
  }

  for(auto &busconnector : busconnectors) {
    if(busconnector && busconnector->getName() == cref)
      return busconnector;
  }
  return NULL;
}

oms3::TLMBusConnector *oms3::System::getTLMBusConnector(const oms3::ComRef &cref)
{
  for(auto &tlmbusconnector : tlmbusconnectors) {
    if(tlmbusconnector && tlmbusconnector->getName() == cref)
      return tlmbusconnector;
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

  //First check if it is a bus connection
  BusConnector *busA=0, *busB=0;
  auto subsystemA = subsystems.find(headA);
  if(subsystemA != subsystems.end())
    busA = subsystemA->second->getBusConnector(tailA);
  auto subsystemB = subsystems.find(headB);
  if(subsystemB != subsystems.end())
    busB = subsystemB->second->getBusConnector(tailB);
  if(busA && busB) {
    //Verify that all connectors in each bus are connected to a connector in the other bus
    std::vector<oms3::ComRef> connectorsA = busA->getConnectors();
    std::vector<oms3::ComRef> connectorsB = busB->getConnectors();
    if(connectorsA.size() != connectorsB.size())
      return logError("Can only connect buses with same number of connectors");
    for(auto &conA : connectorsA) {
      bool connectedToB = false;
      for(auto &conB : connectorsB) {
        for(auto &connection : connections) {
          if(connection && connection->getSignalA() == headA+conA && connection->getSignalB() == headB+conB)
            connectedToB = true;
        }
      }
      if(!connectedToB)
        return logError("All connectors in each bus must be connected to a connector in the other bus before creating bus connection.");
    }

    //Create bus connection
    connections.back() = new oms3::Connection(crefA,crefB,oms3_connection_bus);
    connections.push_back(NULL);
    return oms_status_ok;
  }

  //Not a bus connection, attempt normal connection
  oms3::Connector *conA = 0;
  oms3::Connector *conB = 0;

  //Look in own connectors
  if(!conA)
    conA = this->getConnector(crefA);
  if(!conB)
    conB = this->getConnector(crefB);

  //Look in subsystem connectors
  if(!conA && subsystemA != subsystems.end())
    conA = subsystemA->second->getConnector(tailA);
  if(!conB && subsystemB != subsystems.end())
    conB = subsystemB->second->getConnector(tailB);

  //Look in component connectors
  auto componentA = components.find(headA);
  if(!conA && componentA != components.end())
    conA = componentA->second->getConnector(tailA);
  auto componentB = components.find(headB);
  if(!conB && componentB != components.end())
    conB = componentB->second->getConnector(tailB);

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

oms_status_enu_t oms3::System::addTLMConnection(const oms3::ComRef &crefA, const oms3::ComRef &crefB, double delay, double alpha, double impedance, double impedancerot)
{
  oms3::ComRef tailA(crefA);
  oms3::ComRef headA = tailA.pop_front();

  oms3::ComRef tailB(crefB);
  oms3::ComRef headB = tailB.pop_front();

  //If both A and B references the same subsystem, recurse into that subsystem
  if(headA == headB) {
    auto subsystem = subsystems.find(headA);
    if(subsystem != subsystems.end()) {
      return subsystem->second->addTLMConnection(tailA,tailB,delay,alpha,impedance,impedancerot);
    }
  }

  TLMBusConnector *busA=0, *busB=0;
  auto subsystemA = subsystems.find(headA);
  if(subsystemA != subsystems.end())
    busA = subsystemA->second->getTLMBusConnector(tailA);
  auto subsystemB = subsystems.find(headB);
  if(subsystemB != subsystems.end())
    busB = subsystemB->second->getTLMBusConnector(tailB);
  if(busA && busB) {
    //Create bus connection
    connections.back() = new oms3::Connection(crefA,crefB,oms3_connection_tlm);
    connections.back()->setTLMParameters(delay,alpha,impedance,impedancerot);
    connections.push_back(NULL);
    return oms_status_ok;
  }

  return logError("TLM bus connector(s) not found in system");
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
  busconnectors.back() = bus;
  busconnectors.push_back(NULL);
  element.setBusConnectors(&busconnectors[0]);
  return oms_status_ok;
}

oms_status_enu_t oms3::System::addTLMBus(const oms3::ComRef &cref, const std::string domain, const int dimensions, const oms_tlm_interpolation_t interpolation)
{
  oms3::ComRef tail(cref);
  oms3::ComRef head = tail.pop_front();
  auto subsystem = subsystems.find(head);
  if(subsystem != subsystems.end()) {
    return subsystem->second->addTLMBus(tail, domain, dimensions, interpolation);
  }
  if(!cref.isValidIdent()) {
    return logError("Not a valid ident: "+std::string(cref));
  }
  oms3::TLMBusConnector* bus = new oms3::TLMBusConnector(cref, domain, dimensions, interpolation);
  tlmbusconnectors.back() = bus;
  tlmbusconnectors.push_back(NULL);
  element.setTLMBusConnectors(&tlmbusconnectors[0]);
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

  if(!busTail.isEmpty() && !connectorTail.isEmpty() && busHead != connectorHead)
    return logError("Connector and bus must belong to the same system");

  for(auto& bus : busconnectors) {
    if(bus && bus->getName() == busCref) {
      bus->addConnector(connectorCref);
    }
  }
  return oms_status_ok;
}

oms_status_enu_t oms3::System::addConnectorToTLMBus(const oms3::ComRef &busCref, const oms3::ComRef &connectorCref, const std::string type)
{
  oms3::ComRef busTail(busCref);
  oms3::ComRef busHead = busTail.pop_front();
  oms3::ComRef connectorTail(connectorCref);
  oms3::ComRef connectorHead = connectorTail.pop_front();
  //If both bus and connector references the same subsystem, recurse into that subsystem
  if(busHead == connectorHead) {
    auto subsystem = subsystems.find(busHead);
    if(subsystem != subsystems.end()) {
      return subsystem->second->addConnectorToTLMBus(busTail,connectorTail,type);
    }
  }

  //Check that connector exists in system
  bool found = false;
  for(auto& connector : connectors)
    if(connector && connector->getName() == connectorCref)
      found = true;
  if(!found)
    return logError("Connector not found in system: "+std::string(connectorCref));

  for(auto& bus : tlmbusconnectors) {
    if(bus && bus->getName() == busCref) {
      oms_status_enu_t status = bus->addConnector(connectorCref,type);
      if(oms_status_ok != status)
        return status;
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

oms_status_enu_t oms3::System::setBusGeometry(const oms3::ComRef &cref, const oms2::ssd::ConnectorGeometry *geometry)
{
  oms3::ComRef tail(cref);
  oms3::ComRef head = tail.pop_front();
  auto subsystem = subsystems.find(head);
  if (subsystem != subsystems.end())
    return subsystem->second->setBusGeometry(tail,geometry);

  oms3::BusConnector* busConnector = this->getBusConnector(cref);
  if (busConnector) {
    busConnector->setGeometry(geometry);
    return oms_status_ok;
  }
  return logError("Bus "+std::string(cref)+" not found in system "+std::string(getName()));
}

oms_status_enu_t oms3::System::setTLMBusGeometry(const oms3::ComRef &cref, const oms2::ssd::ConnectorGeometry *geometry)
{
  oms3::ComRef tail(cref);
  oms3::ComRef head = tail.pop_front();
  auto subsystem = subsystems.find(head);
  if (subsystem != subsystems.end())
    return subsystem->second->setTLMBusGeometry(tail,geometry);

  oms3::TLMBusConnector* tlmBusConnector = this->getTLMBusConnector(cref);
  if (tlmBusConnector) {
    tlmBusConnector->setGeometry(geometry);
    return oms_status_ok;
  }
  return logError("TLM Bus "+std::string(cref)+" not found in system "+std::string(getName()));
}
