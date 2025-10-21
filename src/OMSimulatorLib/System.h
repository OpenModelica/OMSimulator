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

#ifndef _OMS_SYSTEM_H_
#define _OMS_SYSTEM_H_

#include "AlgLoop.h"
#include "BusConnector.h"
#include "Clock.h"
#include "ComRef.h"
#include "Connection.h"
#include "DirectedGraph.h"
#include "Element.h"
#include "ExternalModel.h"
#include "ResultWriter.h"
#include "Snapshot.h"
#include "ssd/ConnectorGeometry.h"
#include "ssd/SystemGeometry.h"
#if !defined(NO_TLM)
#include "TLMBusConnector.h"
#endif
#include "OMSimulator/Types.h"
#include "Values.h"

#include <map>
#include <pugixml.hpp>
#include <unordered_map>

#include <ctpl_stl.h>

namespace oms
{
  class AlgLoop;
  class Component;
  class Model;
  class Variable;

  class System
  {
  public: // methods
    virtual ~System();

    virtual oms_status_enu_t exportToSSD_SimulationInformation(pugi::xml_node& node) const = 0;
    virtual oms_status_enu_t importFromSSD_SimulationInformation(const pugi::xml_node& node, const std::string& sspVersion) = 0;
    virtual oms_status_enu_t registerSignalsForResultFile(ResultWriter& resultFile);
    virtual oms_status_enu_t updateSignals(ResultWriter& resultFile);
    virtual oms_status_enu_t setSolver(oms_solver_enu_t solver) {return oms_status_error;}
    virtual oms_status_enu_t instantiate() = 0;
    virtual oms_status_enu_t initialize() = 0;
    virtual oms_status_enu_t terminate() = 0;
    virtual oms_status_enu_t reset() = 0;
    virtual oms_status_enu_t doStep() = 0;
    virtual oms_status_enu_t stepUntil(double stopTime) = 0;

    static System* NewSystem(const ComRef& cref, oms_system_enu_t type, Model* parentModel, System* parentSystem);

    const ComRef& getCref() const {return cref;}
    ComRef getFullCref() const;
    System* getSystem(const ComRef& cref);
    Component* getComponent(const ComRef& cref);
    Variable* getVariable(const ComRef& cref);
    Element* getElement() {return &element;}
    oms_status_enu_t listUnconnectedConnectors(char** contents) const;
    oms_system_enu_t getType() const {return type;}
    oms_status_enu_t addSubSystem(const ComRef& cref, oms_system_enu_t type);
    oms_status_enu_t addSubModel(const ComRef& cref, const std::string& fmuPath);
    oms_status_enu_t replaceSubModel(const ComRef& cref, const std::string& fmuPath, bool dryRun, int& warningCount);
    bool isValidScalarVariable(Component* referenceComponent, Component* replacingComponent, Connection* connection, const ComRef& crefA, const ComRef& crefB, const ComRef& signalName, const std::string& path, std::vector<std::string>& warningList);
    bool validCref(const ComRef& cref);
    oms_status_enu_t exportToSSD(pugi::xml_node& node, Snapshot& snapshot, std::string variantName) const;
    oms_status_enu_t exportToSSV(Snapshot& snapshot) const;
    oms_status_enu_t importFromSnapshot(const pugi::xml_node& node, const std::string& sspVersion, const Snapshot& snapshot, std::string variantName);
    void setGeometry(const ssd::ElementGeometry& geometry) {element.setGeometry(&geometry);}
    oms_status_enu_t addConnector(const ComRef& cref, oms_causality_enu_t causality, oms_signal_type_enu_t type);
    Connector* getConnector(const ComRef& cref);
    std::string getConnectorOwner(const ComRef& cref) const;
    Connector** getConnectors() {return &connectors[0];}
    BusConnector* getBusConnector(const ComRef& cref);
    oms_status_enu_t addTLMConnection(const ComRef& crefA, const ComRef& crefB, double delay, double alpha, double linearimpedance, double angularimpedance);
    oms_status_enu_t setTLMConnectionParameters(const ComRef &crefA, const ComRef &crefB, const oms_tlm_connection_parameters_t* parameters);
    oms_status_enu_t addTLMBus(const ComRef& cref, oms_tlm_domain_t domain, const int dimensions, const oms_tlm_interpolation_t interpolation);
    oms_status_enu_t addConnectorToTLMBus(const ComRef& busCref, const ComRef& connectorCref, const std::string type);
    oms_status_enu_t deleteConnectorFromTLMBus(const ComRef& busCref, const ComRef& connectorCref);
    oms_status_enu_t setTLMBusGeometry(const ComRef& cref, const oms::ssd::ConnectorGeometry* geometry);
#if !defined(NO_TLM)
    TLMBusConnector *getTLMBusConnector(const ComRef& cref);
    TLMBusConnector **getTLMBusConnectors() {return &tlmbusconnectors[0];}
#endif
    Connection* getConnection(const ComRef& crefA, const ComRef& crefB);
    Connection** getConnections(const ComRef &cref);
    oms_status_enu_t addConnection(const ComRef& crefA, const ComRef& crefB, bool suppressUnitConversion = false);
    oms_status_enu_t deleteConnection(const ComRef& crefA, const ComRef& crefB);
    oms_status_enu_t setConnectorGeometry(const ComRef& cref, const oms::ssd::ConnectorGeometry* geometry);
    oms_status_enu_t setConnectionGeometry(const ComRef &crefA, const ComRef &crefB, const oms::ssd::ConnectionGeometry* geometry);
    oms_status_enu_t addBus(const ComRef& cref);
    oms_status_enu_t newResources(const ComRef& cref, const std::string& ssvFilename, const std::string& ssmFilename = "", bool externalresources = false);
    oms_status_enu_t addConnectorToBus(const ComRef& busCref, const ComRef& connectorCref);
    oms_status_enu_t deleteConnectorFromBus(const ComRef& busCref, const ComRef& connectorCref);
    oms_status_enu_t setBusGeometry(const ComRef& cref, const oms::ssd::ConnectorGeometry* geometry);
    oms_status_enu_t addExternalModel(const ComRef &cref, std::string path, std::string startscript);
    oms_status_enu_t delete_(const ComRef& cref);
    oms_status_enu_t deleteAllConectionsTo(const ComRef& cref);
    oms_status_enu_t deleteReferencesInSSD(const ComRef& cref, const std::string& filename);
    oms_status_enu_t deleteResourcesInSSP(const std::string& filename);

    bool isConnected(const ComRef& cref) const;
    Model& getModel();
    System* getParentSystem() const {return parentSystem;}
    bool copyResources();
    void getAllResources(std::vector<std::string>& resources) const;
    std::map<ComRef, System*>& getSubSystems() {return subsystems;}
    std::map<ComRef, Component*>& getComponents() {return components;}
    std::vector<Connection*>& getConnections() {return connections;}
    oms_status_enu_t updateDependencyGraphs();
    const DirectedGraph& getInitialUnknownsGraph() {return initializationGraph;}
    const DirectedGraph& getOutputsGraph() {return eventGraph;}
    oms_status_enu_t exportDependencyGraphs(const std::string& pathInitialization, const std::string& pathEvent, const std::string& pathSimulation);
    oms_status_enu_t setFaultInjection(const ComRef& signal, oms_fault_type_enu_t faultType, double faultValue);

    double getTime() const {return time;}

    oms_status_enu_t getBoolean(const ComRef& cref, bool& value);
    oms_status_enu_t getInteger(const ComRef& cref, int& value);
    oms_status_enu_t getReal(const ComRef& cref, double& value);
    oms_status_enu_t getString(const ComRef& cref, std::string& value);
    oms_status_enu_t setBoolean(const ComRef& cref, bool value);
    oms_status_enu_t setInteger(const ComRef& cref, int value);
    oms_status_enu_t setReal(const ComRef& cref, double value);
    oms_status_enu_t setString(const ComRef& cref, const std::string& value);
    oms_status_enu_t setUnit(const ComRef& cref, const std::string& value);

    oms_status_enu_t getReals(const std::vector<ComRef> &crefs, std::vector<double> &values);
    oms_status_enu_t setReals(const std::vector<ComRef> &crefs, std::vector<double> values);
    oms_status_enu_t setRealInputDerivatives(const ComRef &cref, int order, double value);

    oms_status_enu_t getDirectionalDerivative(const ComRef& unknownCref, const ComRef& knownCref, double& value);

    oms_status_enu_t getState(const ComRef& cref);
    oms_status_enu_t setState(const ComRef& cref);
    oms_status_enu_t freeState(const ComRef& cref);

    oms_status_enu_t rename(const ComRef& newCref); ///< rename the system itself
    oms_status_enu_t rename(const ComRef& cref, const ComRef& newCref); ///< rename any component within the system
    oms_status_enu_t renameConnections(const ComRef& cref, const ComRef& newCref);
    oms_status_enu_t renameConnectors();

    bool isTopLevelSystem() const {return (parentSystem == NULL);}

    oms_status_enu_t addSignalsToResults(const char* regex);
    oms_status_enu_t removeSignalsFromResults(const char* regex);

    void getTolerance(double* absoluteTolerance, double* relativeTolerance) const {if (absoluteTolerance) *absoluteTolerance=this->absoluteTolerance; if (relativeTolerance) *relativeTolerance=this->relativeTolerance;}
    void getStepSize(double* initialStepSize, double* minimumStepSize, double* maximumStepSize) const {if (initialStepSize) *initialStepSize=this->initialStepSize; if (minimumStepSize) *minimumStepSize=this->minimumStepSize; if (maximumStepSize) *maximumStepSize=this->maximumStepSize;}
    oms_status_enu_t setTolerance(double absoluteTolerance, double relativeTolerance) {this->absoluteTolerance=absoluteTolerance; this->relativeTolerance=relativeTolerance; return oms_status_ok;}
    oms_status_enu_t setFixedStepSize(double stepSize) {this->minimumStepSize=this->maximumStepSize=this->initialStepSize=stepSize; return oms_status_ok;}
    oms_status_enu_t setVariableStepSize(double initialStepSize, double minimumStepSize, double maximumStepSize) {this->minimumStepSize=minimumStepSize; this->maximumStepSize=maximumStepSize; this->initialStepSize=initialStepSize; return oms_status_ok;}
    double getMaximumStepSize() {return maximumStepSize;}
    oms_solver_enu_t getSolver() {return solverMethod;}

    AlgLoop* getAlgLoop(const int systemNumber);
    oms_status_enu_t addAlgLoop(scc_t SCC, const int algLoopNum, DirectedGraph& graph, bool supportsDirectionalDerivatives);
    oms_status_enu_t updateAlgebraicLoops(const std::vector< scc_t >& sortedConnections, DirectedGraph& graph);
    oms_status_enu_t solveAlgLoop(DirectedGraph& graph, int loopNumber, double tolerance = 0.0);

    bool useThreadPool();
    ctpl::thread_pool& getThreadPool();

    std::string getUniqueID() const;

    void forceLoopsToBeUpdated() {loopsNeedUpdate = true;}

    void getFilteredSignals(std::vector<Connector>& filteredSignals) const;

    bool supportsDirectionalDerivatives(int i, DirectedGraph& graph);   ///< returns true if all FMUs support providesDirectionalDerivative

    Values& getValues() { return values; }
    std::map<std::string, filesystem::path> fmuGuid;

  protected: // methods
    System(const ComRef& cref, oms_system_enu_t type, Model* parentModel, System* parentSystem, oms_solver_enu_t solverMethod);

    // stop the compiler generating methods copying the object
    System(System const& copy);            ///< not implemented
    System& operator=(System const& copy); ///< not implemented

  private: // methods
    oms_status_enu_t importFromSSD_ConnectionGeometry(const pugi::xml_node& node, const ComRef& crefA, const ComRef& crefB);
    oms_status_enu_t importTLMBus(const pugi::xml_node& node, Component* component);
    oms_status_enu_t importBusConnectorSignals(const pugi::xml_node& node);
    oms_status_enu_t importBusConnectorGeometry(const pugi::xml_node& node);

  protected: // attributes
    double time;

    DirectedGraph initializationGraph;  ///< dependency graph, with all connections, solved at initialization
    DirectedGraph eventGraph;  ///< filtered dependency graph, without parameters, solved at event mode
    DirectedGraph simulationGraph;  ///< filtered dependency graph, with connections of type Real, solved at continuous time mode;

    Clock clock;
    unsigned int clock_id;

    oms_solver_enu_t solverMethod = oms_solver_none;

    double absoluteTolerance = 1e-4;
    double relativeTolerance = 1e-4;
    double minimumStepSize;
    double maximumStepSize;
    double initialStepSize;

    std::unordered_map<unsigned int /*result file var ID*/, unsigned int /*allVariables ID*/> resultFileMapping;
    std::unordered_map<ComRef, bool> exportConnectors;

  private: // attributes
    ComRef cref;
    oms_system_enu_t type;
    Model* parentModel;
    System* parentSystem;
    std::map<ComRef, System*> subsystems;
    std::map<ComRef, Component*> components;

    Values values;  ///< system level connectors, parameters and their start values defined before instantiating the FMU and external inputs defined after initialization

    Element element;
    ssd::SystemGeometry* system_geometry = NULL;

    std::vector<Connector*> connectors;  ///< last element is always NULL
    std::vector<oms_element_t*> subelements;  ///< last element is always NULL; don't free it
    std::vector<BusConnector*> busconnectors;
#if !defined(NO_TLM)
    std::vector<TLMBusConnector*> tlmbusconnectors;
#endif
    std::vector<Connection*> connections;  ///< last element is always NULL

    bool loopsNeedUpdate = true;
    std::vector<AlgLoop> algLoops;  ///< vector of algebraic loop objects
  };
}

#endif
