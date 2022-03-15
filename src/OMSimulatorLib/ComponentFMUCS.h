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

#ifndef _OMS_COMPONENT_FMU_CS_H_
#define _OMS_COMPONENT_FMU_CS_H_

#include "Component.h"
#include "ComRef.h"
#include "ResultWriter.h"
#include "Snapshot.h"
#include "Values.h"
#include "Variable.h"

#include <fmilib.h>
#include <map>
#include <pugixml.hpp>
#include <string>
#include <unordered_map>
#include <vector>
#include <cmath>

namespace oms
{
  class System;

  class ComponentFMUCS : public Component
  {
  public:
    ~ComponentFMUCS();

    static Component* NewComponent(const ComRef& cref, System* parentSystem, const std::string& fmuPath);
    static Component* NewComponent(const pugi::xml_node& node, System* parentSystem, const std::string& sspVersion, const Snapshot& snapshot);
    const FMUInfo* getFMUInfo() const {return &(this->fmuInfo);}

    oms_status_enu_t exportToSSD(pugi::xml_node& node, Snapshot& snapshot) const;
    oms_status_enu_t exportToSSV(pugi::xml_node& ssvNode);
    oms_status_enu_t exportToSSVTemplate(pugi::xml_node& ssvNode);
    oms_status_enu_t exportToSSMTemplate(pugi::xml_node& ssmNode);
    oms_status_enu_t instantiate();
    oms_status_enu_t initialize();
    oms_status_enu_t terminate();
    oms_status_enu_t reset();

    oms_status_enu_t stepUntil(double stopTime);

    oms_status_enu_t initializeDependencyGraph_initialUnknowns();
    oms_status_enu_t initializeDependencyGraph_outputs();

    Variable* getVariable(const ComRef& cref);

    oms_status_enu_t getBoolean(const ComRef& cref, bool& value);
    oms_status_enu_t getBoolean(const fmi2_value_reference_t& vr, bool& value);
    oms_status_enu_t getInteger(const ComRef& cref, int& value);
    oms_status_enu_t getInteger(const fmi2_value_reference_t& vr, int& value);
    oms_status_enu_t getReal(const ComRef& cref, double& value);
    oms_status_enu_t getReal(const fmi2_value_reference_t& vr, double& value);
    oms_status_enu_t getString(const ComRef& cref, std::string& value);
    oms_status_enu_t getString(const fmi2_value_reference_t& vr, std::string& value);
    oms_status_enu_t setBoolean(const ComRef& cref, bool value);
    oms_status_enu_t setInteger(const ComRef& cref, int value);
    oms_status_enu_t setReal(const ComRef& cref, double value);
    oms_status_enu_t setString(const ComRef& cref, const std::string& value);
    oms_status_enu_t setUnit(const ComRef& cref, const std::string& value);

    oms_status_enu_t getDirectionalDerivative(const ComRef& unknownCref, const ComRef& knownCref, double& value);
    oms_status_enu_t getDirectionalDerivativeHeper(const int unknownIndex, const int knownindex, const std::vector<int>& dependencyList, double& value);

    oms_status_enu_t deleteStartValue(const ComRef& cref);

    oms_status_enu_t setFmuTime(double time) {this->time = time; return oms_status_ok;}
    fmi2_import_t* getFMU() {return fmu;}
    std::vector<Variable> getAllVariables() {return allVariables;}

    oms_status_enu_t getRealOutputDerivative(const ComRef& cref, SignalDerivative& der);
    oms_status_enu_t setRealInputDerivative(const ComRef& cref, const SignalDerivative& der);

    oms_status_enu_t registerSignalsForResultFile(ResultWriter& resultFile);
    oms_status_enu_t updateSignals(ResultWriter& resultWriter);
    oms_status_enu_t addSignalsToResults(const char* regex);
    oms_status_enu_t removeSignalsFromResults(const char* regex);

    bool getCanGetAndSetState() {return getFMUInfo()->getCanGetAndSetFMUstate();}
    oms_status_enu_t saveState();
    oms_status_enu_t freeState();
    oms_status_enu_t restoreState();

    oms_status_enu_t setFaultInjection(const ComRef& signal, oms_fault_type_enu_t faultType, double faultValue);

    void getFilteredSignals(std::vector<Connector>& filteredSignals) const;

    oms_status_enu_t newResources(const std::string& ssvFilename, const std::string& ssmFilename, bool externalResources);
    oms_status_enu_t setResourcesHelper1(Values value);
    oms_status_enu_t setResourcesHelper2(Values value);

    oms_status_enu_t deleteReferencesInSSD(const std::string& filename);
    oms_status_enu_t deleteResourcesInSSP(const std::string& filename);

  protected:
    ComponentFMUCS(const ComRef& cref, System* parentSystem, const std::string& fmuPath);

    // stop the compiler generating methods copying the object
    ComponentFMUCS(ComponentFMUCS const& copy);            ///< not implemented
    ComponentFMUCS& operator=(ComponentFMUCS const& copy); ///< not implemented

    oms_status_enu_t renameValues(const ComRef& oldCref, const ComRef& newCref);

    void dumpInitialUnknowns();

  private:
    jm_callbacks callbacks;
    fmi2_callback_functions_t callbackFunctions;
    fmi_import_context_t* context = NULL;
    fmi2_import_t* fmu = NULL;

    FMUInfo fmuInfo;

    std::vector<Variable> allVariables;
    std::vector<unsigned int> calculatedParameters;
    std::vector<unsigned int> derivatives;
    std::vector<unsigned int> inputs;
    std::vector<unsigned int> outputs;
    std::vector<unsigned int> parameters;
    std::vector<bool> exportVariables;

    Values values; ///< start values defined before instantiating the FMU and external inputs defined after initialization

    std::unordered_map<unsigned int /*result file var ID*/, unsigned int /*allVariables ID*/> resultFileMapping;

    double time;

    fmi2_FMU_state_t fmuState = NULL;
    double fmuStateTime;

    std::map<fmi2_value_reference_t, oms_fault_type_t> fib;  ///< fault injection blocks
    oms::ComRef getValidCref(ComRef cref);
  };
}

#endif
