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

#ifndef _OMS_SYSTEM_H_
#define _OMS_SYSTEM_H_

#include "ComRef.h"
#include "DirectedGraph.h"
#include "Types.h"
#include "Element.h"
#include "Connection.h"
#include "ssd/ConnectorGeometry.h"
#include "BusConnector.h"
#include "TLMBusConnector.h"
#include "ExternalModel.h"

#include <pugixml.hpp>
#include <map>

namespace oms3
{
  class Component;
  class Model;
  class Variable;

  class System
  {
  public:
    virtual ~System();

    static System* NewSystem(const ComRef& cref, oms_system_enu_t type, Model* parentModel, System* parentSystem);
    System* getSystem(const ComRef& cref);
    Component* getComponent(const ComRef& cref);
    const ComRef& getName() const {return cref;}
    ComRef getFullName();
    Element* getElement() {return &element;}
    oms_status_enu_t list(const ComRef& cref, char** contents);
    oms_system_enu_t getType() const {return type;}
    oms_status_enu_t addSubSystem(const ComRef& cref, oms_system_enu_t type);
    oms_status_enu_t addSubModel(const ComRef& cref, const std::string& fmuPath);
    bool validCref(const ComRef& cref);
    oms_status_enu_t exportToSSD(pugi::xml_node& node) const;
    oms_status_enu_t importFromSSD(const pugi::xml_node& node);
    virtual oms_status_enu_t exportToSSD_SimulationInformation(pugi::xml_node& node) const = 0;
    virtual oms_status_enu_t importFromSSD_SimulationInformation(const pugi::xml_node& node) = 0;
    void setGeometry(const ssd::ElementGeometry& geometry) {element.setGeometry(&geometry);}
    oms_status_enu_t addConnector(const ComRef& cref, oms_causality_enu_t causality, oms_signal_type_enu_t type);
    Connector* getConnector(const ComRef& cref);
    BusConnector* getBusConnector(const ComRef& cref);
    TLMBusConnector *getTLMBusConnector(const ComRef& cref);
    Connection* getConnection(const ComRef& crefA, const ComRef& crefB);
    Connection** getConnections(const ComRef &cref);
    oms_status_enu_t addConnection(const ComRef& crefA, const ComRef& crefB);
    oms_status_enu_t updateConnection(const ComRef& crefA, const ComRef& crefB, const oms3_connection_t* connection);
    oms_status_enu_t addTLMConnection(const ComRef& crefA, const ComRef& crefB, double delay, double alpha, double linearimpedance, double angularimpedance);
    oms_status_enu_t setConnectorGeometry(const ComRef& cref, const oms2::ssd::ConnectorGeometry* geometry);
    oms_status_enu_t setConnectionGeometry(const ComRef &crefA, const ComRef &crefB, const oms2::ssd::ConnectionGeometry* geometry);
    oms_status_enu_t addBus(const ComRef& cref);
    oms_status_enu_t addTLMBus(const ComRef& cref, const std::string domain, const int dimensions, const oms_tlm_interpolation_t interpolation);
    oms_status_enu_t addConnectorToBus(const ComRef& busCref, const ComRef& connectorCref);
    oms_status_enu_t deleteConnectorFromBus(const ComRef& busCref, const ComRef& connectorCref);
    oms_status_enu_t addConnectorToTLMBus(const ComRef& busCref, const ComRef& connectorCref, const std::string type);
    oms_status_enu_t setBusGeometry(const ComRef& cref, const oms2::ssd::ConnectorGeometry* geometry);
    oms_status_enu_t setTLMBusGeometry(const ComRef& cref, const oms2::ssd::ConnectorGeometry* geometry);
    oms_status_enu_t addExternalModel(const ComRef &cref, std::string path, std::string startscript);
    Model* getModel();
    bool copyResources();
    oms_status_enu_t getAllResources(std::vector<std::string>& resources);
    const std::map<ComRef, System*>& getSubSystems() {return subsystems;}
    const std::map<ComRef, Component*>& getComponents() {return components;}
    double getTolerance() {return 1e-4;}
    oms_status_enu_t updateDependencyGraphs();
    const DirectedGraph& getInitialUnknownsGraph() {return initialUnknownsGraph;}
    const DirectedGraph& getOutputsGraph() {return outputsGraph;}
    oms_status_enu_t exportDependencyGraphs(const std::string& pathInitialization, const std::string& pathSimulation);

    virtual oms_status_enu_t instantiate() = 0;
    virtual oms_status_enu_t initialize() = 0;
    virtual oms_status_enu_t terminate() = 0;

  protected:
    System(const ComRef& cref, oms_system_enu_t type, Model* parentModel, System* parentSystem);

    // stop the compiler generating methods copying the object
    System(System const& copy);            ///< not implemented
    System& operator=(System const& copy); ///< not implemented

  private:
    ComRef cref;
    oms_system_enu_t type;
    Model* parentModel;
    System* parentSystem;
    std::map<ComRef, System*> subsystems;
    std::map<ComRef, Component*> components;

    Element element;
    std::vector<Connector*> connectors;   ///< last element is always NULL
    std::vector<oms3_element_t*> subelements;   ///< last element is always NULL; don't free it
    std::vector<BusConnector*> busconnectors;
    std::vector<TLMBusConnector*> tlmbusconnectors;
    std::vector<Connection*> connections; ///< last element is always NULL

    DirectedGraph initialUnknownsGraph;
    DirectedGraph outputsGraph;
  };
}

#endif
