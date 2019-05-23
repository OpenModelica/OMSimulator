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

#ifndef _OMS_COMPONENT_H_
#define _OMS_COMPONENT_H_

#include "Clock.h"
#include "ComRef.h"
#include "DirectedGraph.h"
#include "Element.h"
#include "FMUInfo.h"
#include "Logging.h"
#include "ResultWriter.h"
#include "SignalDerivative.h"
#include "Types.h"
#include <fmilib.h>
#include <pugixml.hpp>

namespace oms
{
  class System;
  class Model;
  class Variable;

  void fmiLogger(jm_callbacks* c, jm_string module, jm_log_level_enu_t log_level, jm_string message);
  void fmi2logger(fmi2_component_environment_t env, fmi2_string_t instanceName, fmi2_status_t status, fmi2_string_t category, fmi2_string_t message, ...);

  class Component
  {
  public:
    virtual ~Component();

    const ComRef& getCref() const {return cref;}
    ComRef getFullCref() const;
    Element* getElement() {return &element;}
    Connector* getConnector(const ComRef& cref);
    Connector** getConnectors() {return &connectors[0];}
    oms_status_enu_t deleteConnector(const ComRef& cref);
    oms_status_enu_t deleteResources();
    oms_status_enu_t getAllResources(std::vector<std::string>& resources) const {resources.push_back(path); return oms_status_ok;}
    const std::string& getPath() const {return path;}
    oms_component_enu_t getType() const {return type;}
    virtual const FMUInfo* getFMUInfo() const {return NULL;}
    void fetchAllVars(bool enableOption) {fetchAllVars_ = enableOption;}
    System* getParentSystem() const {return parentSystem;}
    Model* getModel() const;
    void setGeometry(const ssd::ElementGeometry& geometry) {element.setGeometry(&geometry);}

    oms_status_enu_t addTLMBus(const oms::ComRef& cref, oms_tlm_domain_t domain, const int dimensions, const oms_tlm_interpolation_t interpolation);
#if !defined(NO_TLM)
    oms::TLMBusConnector *getTLMBusConnector(const oms::ComRef &cref);
    TLMBusConnector **getTLMBusConnectors() {return &tlmbusconnectors[0];}
#endif
    oms_status_enu_t addConnectorToTLMBus(const ComRef& busCref, const ComRef& connectorCref, const std::string type);
    oms_status_enu_t deleteConnectorFromTLMBus(const ComRef& busCref, const ComRef& connectorCref);

    virtual oms_status_enu_t exportToSSD(pugi::xml_node& node) const = 0;
    virtual oms_status_enu_t instantiate() = 0;
    virtual oms_status_enu_t initialize() = 0;
    virtual oms_status_enu_t terminate() = 0;
    virtual oms_status_enu_t reset() = 0;
    virtual oms_status_enu_t stepUntil(double stopTime) {return oms_status_ok;}

    const DirectedGraph& getInitialUnknownsGraph() {return initialUnknownsGraph;}
    const DirectedGraph& getOutputsGraph() {return outputsGraph;}

    virtual oms_status_enu_t getBoolean(const ComRef& cref, bool& value) {return logError_NotImplemented;}
    virtual oms_status_enu_t getInteger(const ComRef& cref, int& value) {return logError_NotImplemented;}
    virtual oms_status_enu_t getReal(const ComRef& cref, double& value) {return logError_NotImplemented;}
    virtual oms_status_enu_t setBoolean(const ComRef& cref, bool value) {return logError_NotImplemented;}
    virtual oms_status_enu_t setInteger(const ComRef& cref, int value) {return logError_NotImplemented;}
    virtual oms_status_enu_t setReal(const ComRef& cref, double value) {return logError_NotImplemented;}

    virtual oms_status_enu_t getRealOutputDerivative(const ComRef& cref, SignalDerivative& der) {return logError_NotImplemented;}
    virtual oms_status_enu_t setRealInputDerivative(const ComRef& cref, const SignalDerivative& der) {return logError_NotImplemented;}

    virtual Variable* getVariable(const ComRef& cref) {logError_NotImplemented; return NULL;}

    virtual oms_status_enu_t registerSignalsForResultFile(ResultWriter& resultFile) = 0;
    virtual oms_status_enu_t updateSignals(ResultWriter& resultWriter) = 0;
    virtual oms_status_enu_t addSignalsToResults(const char* regex) = 0;
    virtual oms_status_enu_t removeSignalsFromResults(const char* regex) = 0;

    virtual bool getCanGetAndSetState() {return false;}
    virtual oms_status_enu_t saveState() {return logError_NotImplemented;}
    virtual oms_status_enu_t freeState() {return logError_NotImplemented;}
    virtual oms_status_enu_t restoreState() {return logError_NotImplemented;}

  protected:
    Component(const ComRef& cref, oms_component_enu_t type, System* parentSystem, const std::string& path);

    // stop the compiler generating methods copying the object
    Component(Component const&);            ///< not implemented
    Component& operator=(Component const&); ///< not implemented

    DirectedGraph initialUnknownsGraph;
    DirectedGraph outputsGraph;
    Element element;
    std::vector<Connector*> connectors;
#if !defined(NO_TLM)
    std::vector<TLMBusConnector*> tlmbusconnectors;
#endif

    Clock clock;
    unsigned int clock_id;
    bool fetchAllVars_ = false;

  private:
    System* parentSystem;
    ComRef cref;
    oms_component_enu_t type;
    std::string path;
  };
}

#endif
