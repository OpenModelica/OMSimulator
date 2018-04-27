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

#ifndef _OMS2_FMU_WRAPPER_H_
#define _OMS2_FMU_WRAPPER_H_

#include "ComRef.h"
#include "DirectedGraph.h"
#include "FMISubModel.h"
#include "FMUInfo.h"
#include "Option.h"
#include "Variable.h"

#include <map>
#include <string>
#include <unordered_map>
#include <vector>

#include <fmilib.h>
#include <pugixml.hpp>

namespace oms2
{
  class FMUWrapper : public FMISubModel
  {
  public:
    static FMUWrapper* newSubModel(const ComRef& cref, const std::string& filename);

    oms_status_enu_t enterInitialization(const double time);
    oms_status_enu_t exitInitialization();
    void do_event_iteration();
    oms_status_enu_t reset();
    oms_status_enu_t terminate();
    oms_status_enu_t doStep(double stopTime);

    oms_status_enu_t exportToSSD(pugi::xml_node& root) const;

    oms_element_type_enu_t getType() const { return oms_component_fmu; }

    oms_status_enu_t setRealParameter(const std::string& var, double value);
    oms_status_enu_t getRealParameter(const std::string& var, double& value);
    oms_status_enu_t getReal(const std::string& var, double& value);
    oms_status_enu_t setIntegerParameter(const std::string& var, int value);
    oms_status_enu_t getIntegerParameter(const std::string& var, int& value);
    oms_status_enu_t setBooleanParameter(const std::string& var, bool value);
    oms_status_enu_t getBooleanParameter(const std::string& var, bool& value);
    const std::string getFMUPath() const {return fmuInfo.getPath();}
    const oms2::FMUInfo* getFMUInfo() const {return &(this->fmuInfo);}
    const std::map<std::string, oms2::Option<double>>& getRealParameters() const {return realParameters;}
    const std::map<std::string, oms2::Option<int>>& getIntegerParameters() const {return integerParameters;}
    const std::map<std::string, oms2::Option<bool>>& getBooleanParameters() const {return booleanParameters;}

    oms_status_enu_t setReal(const oms2::SignalRef& sr, double value);
    oms_status_enu_t getReal(const oms2::SignalRef& sr, double& value);
    oms_status_enu_t setInteger(const oms2::SignalRef& sr, int value);
    oms_status_enu_t getInteger(const oms2::SignalRef& sr, int& value);
    oms_status_enu_t setBoolean(const oms2::SignalRef& sr, bool booleanValue);
    oms_status_enu_t getBoolean(const oms2::SignalRef& sr, bool& booleanValue);
    oms_status_enu_t setRealInputDerivatives(const oms2::SignalRef& sr, int order, double value);

    oms_status_enu_t registerSignalsForResultFile(ResultWriter& resultWriter);
    oms_status_enu_t emit(ResultWriter& resultWriter);

  private:
    oms_status_enu_t initializeDependencyGraph_initialUnknowns();
    oms_status_enu_t initializeDependencyGraph_outputs();

    oms_status_enu_t setReal(const oms2::Variable& var, double realValue);
    oms_status_enu_t getReal(const oms2::Variable& var, double& realValue);
    oms_status_enu_t setRealInputDerivatives(const oms2::Variable& var, int order, double realValue);
    oms_status_enu_t setInteger(const oms2::Variable& var, int integerValue);
    oms_status_enu_t getInteger(const oms2::Variable& var, int& integerValue);
    oms_status_enu_t setBoolean(const oms2::Variable& var, bool booleanValue);
    oms_status_enu_t getBoolean(const oms2::Variable& var, bool& booleanValue);

    oms2::Variable* getVariable(const std::string& var);

  private:
    FMUWrapper(const ComRef& cref, const std::string& filename);
    ~FMUWrapper();

    oms2::FMUInfo fmuInfo;
    std::vector<oms2::Variable> inputs;
    std::vector<oms2::Variable> outputs;
    std::vector<oms2::Variable> parameters;
    std::vector<oms2::Variable> allVariables;

    std::map<std::string, oms2::Option<double>> realParameters;
    std::map<std::string, oms2::Option<int>> integerParameters;
    std::map<std::string, oms2::Option<bool>> booleanParameters;

    std::string tempDir;

    std::unordered_map<unsigned int /*result file var ID*/, unsigned int /*allVariables ID*/> resultFileMapping;

    // ME & CS
    jm_callbacks callbacks;
    fmi2_callback_functions_t callBackFunctions;
    fmi_import_context_t* context;
    fmi2_import_t* fmu;
    fmi2_event_info_t eventInfo;
    double time;
    double relativeTolerance;

    // ME
    fmi2_boolean_t callEventUpdate;
    fmi2_boolean_t terminateSimulation;
    size_t n_states;
    size_t n_event_indicators;
    double* states;
    double* states_der;
    double* states_nominal;
    double* event_indicators;
    double* event_indicators_prev;
  };
}

#endif
