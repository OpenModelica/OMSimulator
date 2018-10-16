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
#include "Logging.h"
#include "Option.h"
#include "Variable.h"

#include <map>
#include <string>
#include <unordered_map>
#include <vector>

#include <fmilib.h>
#include <pugixml.hpp>

#include "cvode/cvode.h"             /* prototypes for CVODE fcts., consts. */
#include "nvector/nvector_serial.h"  /* serial N_Vector types, fcts., macros */

namespace oms2
{
  class Solver;

  class FMUWrapper : public FMISubModel
  {
  public:
    static FMUWrapper* newSubModel(const ComRef& cref, const std::string& filename, const ComRef &parent);

    oms_status_enu_t enterInitialization(const double time);
    oms_status_enu_t exitInitialization();
    oms_status_enu_t reset(bool terminate);
    oms_status_enu_t doStep(double stopTime) {return logError("don't call FMUWrapper::doStep");}

    oms_status_enu_t exportToSSD(pugi::xml_node& root) const;

    oms_element_type_enu_t getType() const { return oms_component_fmu_old; }

    void readFromTLMSockets(double time);
    void writeToTLMSockets(double time);

    oms_status_enu_t setRealParameter(const std::string& var, double value);
    oms_status_enu_t getRealParameter(const std::string& var, double& value);
    oms_status_enu_t getReal(const std::string& var, double& value);
    oms_status_enu_t setIntegerParameter(const std::string& var, int value);
    oms_status_enu_t getIntegerParameter(const std::string& var, int& value);
    oms_status_enu_t setBooleanParameter(const std::string& var, bool value);
    oms_status_enu_t getBooleanParameter(const std::string& var, bool& value);
    const std::string getFMUPath() const {return fmuInfo.getPath();}
    const std::string getPath() const {return getFMUPath();}
    const oms3::FMUInfo* getFMUInfo() const {return &(this->fmuInfo);}
    const std::map<std::string, oms3::Option<double>>& getRealParameters() const {return realParameters;}
    const std::map<std::string, oms3::Option<int>>& getIntegerParameters() const {return integerParameters;}
    const std::map<std::string, oms3::Option<bool>>& getBooleanParameters() const {return booleanParameters;}

    oms_status_enu_t setReal(const oms2::SignalRef& sr, double value);
    oms_status_enu_t getReal(const oms2::SignalRef& sr, double& value);
    oms_status_enu_t setInteger(const oms2::SignalRef& sr, int value);
    oms_status_enu_t getInteger(const oms2::SignalRef& sr, int& value);
    oms_status_enu_t setBoolean(const oms2::SignalRef& sr, bool value);
    oms_status_enu_t getBoolean(const oms2::SignalRef& sr, bool& value);
    oms_status_enu_t setRealInputDerivatives(const oms2::SignalRef& sr, int order, double value);

    oms_status_enu_t registerSignalsForResultFile(ResultWriter& resultWriter);
    oms_status_enu_t emit(ResultWriter& resultWriter);

    void addSignalsToResults(const std::string& regex);
    void removeSignalsFromResults(const std::string& regex);

    void fetchAllVars();

    fmi2_import_t* getFMU() {return fmu;}
    oms2::Solver* getSolver() {return solver;}
    void setSolver(Solver* solver) {this->solver = solver;}

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
    FMUWrapper(const ComRef& cref, const std::string& filename, const ComRef& parent);
    ~FMUWrapper();

    ComRef parent;

    oms3::FMUInfo fmuInfo;
    std::vector<oms2::Variable> inputs;
    std::vector<oms2::Variable> outputs;
    std::vector<oms2::Variable> parameters;
    std::vector<oms2::Variable> allVariables;
    std::vector<bool> exportVariables;

    std::map<std::string, oms3::Option<double>> realParameters;
    std::map<std::string, oms3::Option<int>> integerParameters;
    std::map<std::string, oms3::Option<bool>> booleanParameters;

    std::string tempDir;

    std::unordered_map<unsigned int /*result file var ID*/, unsigned int /*allVariables ID*/> resultFileMapping;

    jm_callbacks callbacks;
    fmi2_callback_functions_t callbackFunctions;
    fmi_import_context_t* context = NULL;
    fmi2_import_t* fmu = NULL;
    bool initialized = false;
    oms2::Solver* solver = NULL;    ///< Never allocate or free memory for this pointer.
  };
}

#endif
