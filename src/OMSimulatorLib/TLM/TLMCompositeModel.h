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

#ifndef _OMS2_TLM_COMPOSITE_MODEL_H_
#define _OMS2_TLM_COMPOSITE_MODEL_H_

#include "Types.h"
#include "CompositeModel.h"

#include <list>
#include <map>
#include <atomic>

#include <pugixml.hpp>

class ResultWriter;

namespace oms2
{
  class ComRef;
  class ExternalModel;
  class FMICompositeModel;
  class SignalRef;
  class TLMConnection;
  class TLMInterface;

  class TLMCompositeModel : public CompositeModel
  {
  public:
    static TLMCompositeModel* NewModel(const ComRef& name);
    static TLMCompositeModel* LoadModel(const pugi::xml_node& node);

    oms_element_type_enu_t getType() {return oms_component_tlm;}
    oms_status_enu_t addFMIModel(FMICompositeModel *model);

    oms_status_enu_t addInterface(oms2::TLMInterface* ifc);
    oms_status_enu_t addInterface(std::string name, int dimensions, oms_causality_enu_t causality, std::string domain, oms_tlm_interpolation_t interpolation, const ComRef &cref, std::vector<SignalRef> &sigrefs);

    oms_status_enu_t setPositionAndOrientation(const SignalRef& ifc, std::vector<double> x, std::vector<double> A);

    oms_status_enu_t addExternalModel(ExternalModel *externalModel);
    oms_status_enu_t addExternalModel(std::string modelFile, std::string startScript, const ComRef& cref);

    oms_status_enu_t addConnection(const oms2::TLMConnection& connection);
    oms_status_enu_t addConnection(const SignalRef& signalA, const SignalRef& signalB, double delay, double alpha, double Zf, double Zfr);

    bool exists(const ComRef& subref);
    FMICompositeModel* getFMIModel(const oms2::ComRef& cref);


    oms_status_enu_t setSocketData(const std::string &address,
                                   int managerPort,
                                   int monitorPort);

    oms_status_enu_t setTLMInitialValues(const SignalRef &ifc, std::vector<double> value);

    void setLoggingLevel(int level);

    oms_status_enu_t describe();

    oms_status_enu_t initialize(double startTime, double tolerance);
    oms_status_enu_t reset(bool terminate);
    oms_status_enu_t simulate(ResultWriter &resultWriter, double stopTime, double communicationInterval, double loggingLevel, oms2::MasterAlgorithm masterAlgorithm);
    void simulate_asynchronous(ResultWriter& resultWriter, double stopTime, double communicationInterval, double loggingLevel, void (*cb)(const char* ident, double time, oms_status_enu_t status));
    oms_status_enu_t doSteps(ResultWriter& resultWriter, const int numberOfSteps, double communicationInterval, double loggingLevel);
    oms_status_enu_t stepUntil(ResultWriter &resultWriter, double stopTime, double communicationInterval, double loggingLevel, oms2::MasterAlgorithm masterAlgorithm, bool realtime_sync);

    oms_status_enu_t registerSignalsForResultFile(ResultWriter& resultWriter) {return oms_status_ok;}
    oms_status_enu_t emit(ResultWriter& resultWriter) {return oms_status_ok;}

  private:
    TLMCompositeModel(const ComRef& name);
    ~TLMCompositeModel();

    // stop the compiler generating methods copying the object
    TLMCompositeModel(TLMCompositeModel const& copy);            ///< not implemented
    TLMCompositeModel& operator=(TLMCompositeModel const& copy); ///< not implemented

  private:
    ComRef name;
    void* model;
    double startTime;
    std::map<ComRef, FMICompositeModel*> fmiModels;
    std::map<ComRef, ExternalModel*> externalModels;
    std::list<TLMConnection> connections;
    std::list<TLMInterface*> interfaces;
    std::string address;
    int managerPort;
  };
}

#endif
