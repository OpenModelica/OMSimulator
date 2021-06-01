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

#ifndef _OMS_COMPONENT_FMU_ME_H_
#define _OMS_COMPONENT_FMU_ME_H_

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

namespace oms
{
  class ComponentFMUME : public Component
  {
  public:
    ~ComponentFMUME();

    static Component* NewComponent(const oms::ComRef& cref, System* parentSystem, const std::string& fmuPath);
    static Component* NewComponent(const pugi::xml_node& node, System* parentSystem,  const std::string& sspVersion, const Snapshot& snapshot);
    const FMUInfo* getFMUInfo() const {return &(this->fmuInfo);}

    oms_status_enu_t exportToSSD(pugi::xml_node& node, Snapshot& snapshot) const;
    oms_status_enu_t exportToSSV(pugi::xml_node& ssvNode);
    oms_status_enu_t exportToSSVTemplate(pugi::xml_node& ssvNode);
    oms_status_enu_t exportToSSMTemplate(pugi::xml_node& ssmNode);
    oms_status_enu_t instantiate();
    oms_status_enu_t initialize();
    oms_status_enu_t terminate();
    oms_status_enu_t reset();

    oms_status_enu_t initializeDependencyGraph_initialUnknowns();
    oms_status_enu_t initializeDependencyGraph_outputs();

    Variable* getVariable(const ComRef& cref);

    oms_status_enu_t getBoolean(const ComRef& cref, bool& value);
    oms_status_enu_t getBoolean(const fmi2_value_reference_t& vr, bool& value);
    oms_status_enu_t getInteger(const ComRef& cref, int& value);
    oms_status_enu_t getInteger(const fmi2_value_reference_t& vr, int& value);
    oms_status_enu_t getReal(const ComRef& cref, double& value);
    oms_status_enu_t getReal(const fmi2_value_reference_t& vr, double& value);
    oms_status_enu_t setBoolean(const ComRef& cref, bool value);
    oms_status_enu_t setInteger(const ComRef& cref, int value);
    oms_status_enu_t setReal(const ComRef& cref, double value);

    oms_status_enu_t deleteStartValue(const ComRef& cref);

    oms_status_enu_t registerSignalsForResultFile(ResultWriter& resultFile);
    oms_status_enu_t updateSignals(ResultWriter& resultWriter);
    oms_status_enu_t addSignalsToResults(const char* regex);
    oms_status_enu_t removeSignalsFromResults(const char* regex);

    oms_status_enu_t doEventIteration();

    size_t getNumberOfContinuousStates() const {return nContinuousStates;}
    size_t getNumberOfEventIndicators() const {return nEventIndicators;}
    oms_status_enu_t getContinuousStates(double* states);
    oms_status_enu_t setContinuousStates(double* states);
    oms_status_enu_t getDerivatives(double* derivatives);
    oms_status_enu_t getNominalsOfContinuousStates(double* nominals);
    oms_status_enu_t getEventindicators(double* eventindicators);

    fmi2_import_t* getFMU() {return fmu;}
    fmi2_event_info_t* getEventInfo() {return &eventInfo;}

    bool getCanGetAndSetState() {return getFMUInfo()->getCanGetAndSetFMUstate();}

    oms_status_enu_t setFaultInjection(const ComRef& signal, oms_fault_type_enu_t faultType, double faultValue);

    void getFilteredSignals(std::vector<Connector>& filteredSignals) const;

    oms_status_enu_t addResources(std::string& filename);
    oms_status_enu_t setResourcesHelper1(Values value);
    oms_status_enu_t setResourcesHelper2(Values value);

  protected:
    ComponentFMUME(const ComRef& cref, System* parentSystem, const std::string& fmuPath);

    // stop the compiler generating methods copying the object
    ComponentFMUME(ComponentFMUME const& copy);            ///< not implemented
    ComponentFMUME& operator=(ComponentFMUME const& copy); ///< not implemented

    oms_status_enu_t renameValues(const ComRef& oldCref, const ComRef& newCref);

  private:
    jm_callbacks callbacks;
    fmi2_callback_functions_t callbackFunctions;
    fmi_import_context_t* context = NULL;
    fmi2_import_t* fmu = NULL;

    fmi2_event_info_t eventInfo;
    size_t nContinuousStates;
    size_t nEventIndicators;

    FMUInfo fmuInfo;

    std::vector<Variable> allVariables;
    std::vector<Variable> inputs;
    std::vector<Variable> outputs;
    std::vector<Variable> parameters;
    std::vector<Variable> calculatedParameters;
    std::vector<bool> exportVariables;

    Values values; ///< start values defined before instantiating the FMU and external inputs defined after initialization

    std::unordered_map<unsigned int /*result file var ID*/, unsigned int /*allVariables ID*/> resultFileMapping;

    std::map<fmi2_value_reference_t, oms_fault_type_t> fib;  ///< fault injection blocks
    oms::ComRef getValidCref(ComRef cref);
  };
}

#endif
