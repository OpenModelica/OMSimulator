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

#include "Types.h"
#include "Pkg_oms2.h"
#include "CompositeModel.h"
#include "ComRef.h"
#include "Connection.h"
#include "DirectedGraph.h"
#include "FMISubModel.h"
#include "SignalRef.h"
#include "Variable.h"
#include "TLMInterface.h"

#include <map>
#include <vector>

#include <pugixml.hpp>

class ResultWriter;

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

    FMISubModel* getSubModel(const oms2::ComRef& cref);
    oms2::Connection** getConnections() {return &connections[0];}

    oms_status_enu_t renameSubModel(const oms2::ComRef& identOld, const oms2::ComRef& identNew);

    oms2::Element** getElements();

    oms_status_enu_t exportCompositeStructure(const std::string& filename);
    oms_status_enu_t exportDependencyGraphs(const std::string& initialization, const std::string& simulation);

    oms_status_enu_t initialize(double startTime, double tolerance);
    oms_status_enu_t reset();
    oms_status_enu_t terminate();
    oms_status_enu_t doSteps(ResultWriter& resultWriter, const int numberOfSteps, double communicationInterval);
    oms_status_enu_t stepUntil(ResultWriter& resultWriter, double stopTime, double communicationInterval, MasterAlgorithm masterAlgorithm, bool realtime_sync);
    oms_status_enu_t simulateTLM(ResultWriter *resultWriter, double stopTime, double communicationInterval, std::string address);
    void simulate_asynchronous(ResultWriter& resultWriter, double stopTime, double communicationInterval, void (*cb)(const char* ident, double time, oms_status_enu_t status));

    oms_status_enu_t setReal(const oms2::SignalRef& sr, double value);
    oms_status_enu_t setReals(const std::vector<oms2::SignalRef> &sr, std::vector<double> values);
    oms_status_enu_t getReal(const oms2::SignalRef& sr, double& value);
    oms_status_enu_t getReals(const std::vector<oms2::SignalRef> &sr, std::vector<double> &values);

    oms_status_enu_t setRealInputDerivatives(const oms2::SignalRef& sr, int order, double value);

    oms_status_enu_t addTLMInterface(TLMInterface *ifc);
    oms_status_enu_t setTLMInitialValues(std::string ifc, std::vector<double> value);

    double getCurrentTime() {return time;}


  private:
    oms_status_enu_t loadElementGeometry(const pugi::xml_node& node);
    oms_status_enu_t loadConnections(const pugi::xml_node& node);
    oms_status_enu_t loadSubModel(const pugi::xml_node& node);
    oms2::Variable* getVariable(const oms2::SignalRef& signal);
    oms_causality_enu_t getSignalCausality(const oms2::SignalRef& signal);
    oms_status_enu_t updateInputs(oms2::DirectedGraph& graph);
    oms_status_enu_t solveAlgLoop(oms2::DirectedGraph& graph, const std::vector< std::pair<int, int> >& SCC);

    oms_status_enu_t registerSignalsForResultFile(ResultWriter& resultWriter);
    oms_status_enu_t emit(ResultWriter& resultWriter);

    oms_status_enu_t stepUntilStandard(ResultWriter& resultWriter, double stopTime, double communicationInterval, bool realtime_sync);
    oms_status_enu_t stepUntilPCTPL(ResultWriter& resultWriter, double stopTime, double communicationInterval, bool realtime_sync);

    oms_status_enu_t initializeSockets(double stopTime, double &communicationInterval, std::string server);
    void readFromSockets();
    void writeToSockets();
    void finalizeSockets();

  protected:
    void deleteComponents();
    void updateComponents();

  private:
    FMICompositeModel(const oms2::ComRef& name);
    ~FMICompositeModel();

    // stop the compiler generating methods copying the object
    FMICompositeModel(FMICompositeModel const& copy);            ///< not implemented
    FMICompositeModel& operator=(FMICompositeModel const& copy); ///< not implemented

  private:
    std::map<oms2::ComRef, oms2::FMISubModel*> subModels;
    std::vector<oms2::Connection*> connections; ///< last element is always NULL
    oms2::Element** components;
    std::vector<TLMInterface*> tlmInterfaces;
    TLMPlugin *plugin;

    DirectedGraph initialUnknownsGraph;
    DirectedGraph outputsGraph;

    double time;
    double tolerance;
    double communicationInterval;

    std::vector<SignalRef> tlmSigRefs;
    std::map<std::string, std::vector<double> > tlmInitialValues;
  };
}

#endif
