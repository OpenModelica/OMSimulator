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

#ifndef _OMS2_FMI_COMPOSITE_MODEL_H_
#define _OMS2_FMI_COMPOSITE_MODEL_H_

#include "Clock.h"
#include "CompositeModel.h"
#include "ComRef.h"
#include "Connection.h"
#include "DirectedGraph.h"
#include "FMISubModel.h"
#include "Pkg_oms2.h"
#include "SignalRef.h"
#include "Solver.h"
#include "TLMInterface.h"
#include "Types.h"
#include "Variable.h"

#include <atomic>
#include <map>
#include <vector>

#include <pugixml.hpp>

namespace oms3
{
  class ResultWriter;
}

namespace oms2
{
  class FMICompositeModel : public CompositeModel
  {
  public:
    static FMICompositeModel* NewModel(const oms2::ComRef& name);
    static FMICompositeModel* LoadModel(const pugi::xml_node& node);
    oms_status_enu_t save(pugi::xml_node& node);

    oms_element_type_enu_t getType() {return oms_component_fmi;}
    oms_status_enu_t addFMU(const std::string& filename, const oms2::ComRef& cref);
    oms_status_enu_t addTable(const std::string& filename, const oms2::ComRef& cref);
    oms_status_enu_t deleteSubModel(const oms2::ComRef& cref);

    oms_status_enu_t setRealParameter(const oms2::SignalRef& sr, double value);
    oms_status_enu_t setIntegerParameter(const oms2::SignalRef& sr, int value);
    oms_status_enu_t setBooleanParameter(const oms2::SignalRef& sr, int value);

    oms2::Connection* getConnection(const oms2::SignalRef& conA, const oms2::SignalRef& conB);
    oms_status_enu_t addConnection(const oms2::SignalRef& conA, const oms2::SignalRef& conB);
    oms_status_enu_t deleteConnection(const oms2::SignalRef& conA, const oms2::SignalRef& conB);

    FMISubModel* getSubModel(const oms2::ComRef& cref, bool showWarning=true);
    Solver* getSolver(const oms2::ComRef& cref, bool showWarning=true);
    oms2::Connection** getConnections() {return &connections[0];}

    void setName(const oms2::ComRef& name);
    oms_status_enu_t renameSubModel(const oms2::ComRef& identOld, const oms2::ComRef& identNew);

    oms2::Element** getElements();

    oms_status_enu_t exportCompositeStructure(const std::string& filename);
    oms_status_enu_t exportDependencyGraphs(const std::string& initialization, const std::string& simulation);

    oms_status_enu_t initialize(double startTime, double tolerance);
    oms_status_enu_t reset(bool terminate);
    oms_status_enu_t doSteps(oms3::ResultWriter& resultWriter, const int numberOfSteps, double communicationInterval, double loggingInterval);
    oms_status_enu_t stepUntil(oms3::ResultWriter& resultWriter, double stopTime, double communicationInterval, double loggingInterval, MasterAlgorithm masterAlgorithm, bool realtime_sync);
#if !defined(NO_TLM)
    oms_status_enu_t initializeTLM(double startTime, double tolerance, std::string address);
    oms_status_enu_t simulateTLM(double stopTime, double loggingInterval);
#endif
    void simulate_asynchronous(oms3::ResultWriter& resultWriter, double stopTime, double communicationInterval, double loggingInterval, void (*cb)(const char* ident, double time, oms_status_enu_t status));

    oms_status_enu_t setInteger(const oms2::SignalRef& sr, int value);
    oms_status_enu_t getInteger(const oms2::SignalRef& sr, int& value);

    oms_status_enu_t setBoolean(const oms2::SignalRef& sr, bool value);
    oms_status_enu_t getBoolean(const oms2::SignalRef& sr, bool& value);

    oms_status_enu_t setReal(const oms2::SignalRef& sr, double value);
    oms_status_enu_t setReals(const std::vector<oms2::SignalRef> &sr, std::vector<double> values);
    oms_status_enu_t getReal(const oms2::SignalRef& sr, double& value);
    oms_status_enu_t getReals(const std::vector<oms2::SignalRef> &sr, std::vector<double> &values);

    oms_status_enu_t setRealInputDerivatives(const oms2::SignalRef& sr, int order, double value);

#if !defined(NO_TLM)
    oms_status_enu_t addTLMInterface(TLMInterface *ifc);
    std::vector<TLMInterface*> getTLMInterfaces() const { return tlmInterfaces; }
    oms_status_enu_t setTLMInitialValues(std::string ifc, std::vector<double> value);
#endif

    double getCurrentTime() {return time;}

    oms_status_enu_t addSignalsToResults(const std::string& regex);
    oms_status_enu_t removeSignalsFromResults(const std::string& regex);

    oms_status_enu_t describe();

    oms_status_enu_t addSolver(const oms2::ComRef& solverCref, const std::string& solver);
    oms_status_enu_t setSolverTolerance(const oms2::ComRef& cref, double tolerance);
    oms_status_enu_t connectSolver(const oms2::ComRef& fmuCref, const oms2::ComRef& solverCref);
    oms_status_enu_t unconnectSolver(const oms2::ComRef& fmuCref, const oms2::ComRef& solverCref);

  private:
    oms_status_enu_t loadElementGeometry(const pugi::xml_node& node);
    oms_status_enu_t loadConnections(const pugi::xml_node& node);
    oms_status_enu_t loadSubModel(const pugi::xml_node& node);
    oms2::Variable* getVariable(const oms2::SignalRef& signal);
    oms_causality_enu_t getSignalCausality(const oms2::SignalRef& signal);
    oms_status_enu_t updateInputs(oms2::DirectedGraph& graph);
    oms_status_enu_t solveAlgLoop(oms2::DirectedGraph& graph, const std::vector< std::pair<int, int> >& SCC);

    oms_status_enu_t registerSignalsForResultFile(oms3::ResultWriter& resultWriter);
    oms_status_enu_t emit(oms3::ResultWriter& resultWriter);

    oms_status_enu_t stepUntilStandard(oms3::ResultWriter& resultWriter, double stopTime, double communicationInterval, double loggingInterval, bool realtime_sync);
#if !defined(__arm__)
    oms_status_enu_t stepUntilPCTPL(oms3::ResultWriter& resultWriter, double stopTime, double communicationInterval, double loggingInterval, bool realtime_sync);
#endif // if !defined(__arm__)
    oms_status_enu_t stepUntilASSC(oms3::ResultWriter& resultWriter, double stopTime, double communicationInterval, double loggingInterval, bool realtime_sync);
    oms_status_enu_t updateDependencyGraphs();

#if !defined(NO_TLM)
    oms_status_enu_t updateInitialTLMValues();
    void finalizeTLMSockets();
  public:
    oms_status_enu_t setupTLMSockets(double startTime, std::string server);
    void readFromTLMSockets(double time, std::string fmu = "");
    void writeToTLMSockets(double time, std::string fmu = "");
    bool isTLMConnected() { return tlmConnected; }
    bool isTLMInitialized() { return tlmInitialized; }
#endif
  protected:
    void deleteComponents();
    void updateComponents();
    bool validAndUnusedCref(const oms2::ComRef& cref, bool showWaring);

  private:
    FMICompositeModel(const oms2::ComRef& name);
    ~FMICompositeModel();

    // stop the compiler generating methods copying the object
    FMICompositeModel(FMICompositeModel const& copy);            ///< not implemented
    FMICompositeModel& operator=(FMICompositeModel const& copy); ///< not implemented

  private:
    std::map<oms2::ComRef, oms2::FMISubModel*> subModels;
    std::map<oms2::ComRef, oms2::Solver*> solvers;
    std::vector<oms2::Connection*> connections; ///< last element is always NULL
    oms2::Element** components;
#if !defined(NO_TLM)
    std::vector<TLMInterface*> tlmInterfaces;
    TLMPlugin *plugin=0;
#endif
    DirectedGraph initialUnknownsGraph;
    DirectedGraph outputsGraph;

    double time;
    double tolerance;
    double communicationInterval;
    double loggingInterval;
    double tLastEmit;
    Clock clock;
    unsigned int clock_id;

#if !defined(NO_TLM)
    std::vector<SignalRef> tlmSigRefs;
    std::map<std::string, std::vector<double> > tlmInitialValues;
    bool tlmConnected = false;
    bool tlmInitialized = false;
#endif
  };
}

#endif
