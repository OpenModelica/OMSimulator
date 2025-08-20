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
#include "Connector.h"
#include "DirectedGraph.h"
#include "Element.h"
#include "FMUInfo.h"
#include "Logging.h"
#include "ResultWriter.h"
#include "SignalDerivative.h"
#include "Snapshot.h"
#include "OMSimulator/Types.h"
#include "Values.h"

#include <fmi4c.h>
#include <pugixml.hpp>

namespace oms
{
  class System;
  class Model;
  class Variable;

  void fmi2logger(fmi2ComponentEnvironment env, fmi2String instanceName, fmi2Status status, fmi2String category, fmi2String message, ...);
  void fmi3logger(fmi3InstanceEnvironment env, fmi3Status status, fmi3String category, fmi3String message);
  class Component
  {
  public:
    virtual ~Component();

    virtual oms_status_enu_t addSignalsToResults(const char* regex) = 0;
    virtual oms_status_enu_t exportToSSD(pugi::xml_node& node, Snapshot& snapshot, std::string variantName) const = 0;
    virtual oms_status_enu_t exportToSSV(pugi::xml_node& ssvNode) { return logError_NotImplemented; }
    virtual void getFilteredUnitDefinitionsToSSD(std::map<std::string, std::map<std::string, std::string>>& unitDefinitions) { return ; }
    virtual void getFilteredEnumerationDefinitionsToSSD(std::map<std::string, std::map<std::string, std::string>>& enumerationDefinitions) { return ; }

    virtual oms_status_enu_t initialize() = 0;
    virtual oms_status_enu_t instantiate() = 0;
    virtual oms_status_enu_t registerSignalsForResultFile(ResultWriter& resultFile) = 0;
    virtual oms_status_enu_t removeSignalsFromResults(const char* regex) = 0;
    virtual oms_status_enu_t reset() = 0;
    virtual oms_status_enu_t terminate() = 0;
    virtual oms_status_enu_t updateSignals(ResultWriter& resultWriter) = 0;
    virtual Variable* getVariable(const ComRef& cref) = 0;

    virtual bool getCanGetAndSetState() { return false; }
    virtual const FMUInfo* getFMUInfo() const { return nullptr; }
    virtual oms_status_enu_t deleteStartValue(const ComRef& cref) { return oms_status_ok; }
    virtual std::vector<Values> getValuesResources() { return{}; }
    virtual Values& getValues() {return values;}
    virtual oms_status_enu_t setValuesResources(Values& values) { return oms_status_ok; }
    virtual oms_status_enu_t updateOrDeleteStartValueInReplacedComponent(std::vector<std::string> &warningList) { return oms_status_ok; }
    virtual oms_status_enu_t exportToSSMTemplate(pugi::xml_node& ssmNode) { return logError_NotImplemented; }
    virtual oms_status_enu_t exportToSSVTemplate(pugi::xml_node& ssvNode, Snapshot& snapshot) { return logError_NotImplemented; }
    virtual oms_status_enu_t freeState() { return logError_NotImplemented; }
    virtual oms_status_enu_t getBoolean(const ComRef& cref, bool& value) { return logError_NotImplemented; }
    virtual oms_status_enu_t getInteger(const ComRef& cref, int& value) { return logError_NotImplemented; }
    virtual oms_status_enu_t getReal(const ComRef& cref, double& value) { return logError_NotImplemented; }
    virtual oms_status_enu_t getString(const ComRef& cref, std::string& value) { return logError_NotImplemented; }
    virtual oms_status_enu_t getRealOutputDerivative(const ComRef& cref, SignalDerivative& der) { return logError_NotImplemented; }
    virtual oms_status_enu_t getDirectionalDerivative(const ComRef& unknownCref, const ComRef& knownCref, double& value) { return logError_NotImplemented; }
    virtual oms_status_enu_t restoreState() { return logError_NotImplemented; }
    virtual oms_status_enu_t saveState() { return logError_NotImplemented; }
    virtual oms_status_enu_t setBoolean(const ComRef& cref, bool value) { return logError_NotImplemented; }
    virtual oms_status_enu_t setInteger(const ComRef& cref, int value) { return logError_NotImplemented; }
    virtual oms_status_enu_t setReal(const ComRef& cref, double value) { return logError_NotImplemented; }
    virtual oms_status_enu_t setString(const ComRef& cref, const std::string& value) { return logError_NotImplemented; }
    virtual oms_status_enu_t setUnit(const ComRef& cref, const std::string& value) { return logError_NotImplemented; }
    virtual oms_status_enu_t setRealInputDerivative(const ComRef& cref, const SignalDerivative& der) { return logError_NotImplemented; }
    virtual oms_status_enu_t stepUntil(double stopTime) { return logError_NotImplemented; }
    virtual oms_status_enu_t setTime(double time) { return logError_NotImplemented; }
    virtual oms_status_enu_t setExportName(const std::string & exportName) { return oms_status_ok; }
    virtual oms_status_enu_t newResources(const std::string& ssvFileName, const std::string& ssmFileName, bool externalResources) { return logError_NotImplemented; }
    virtual oms_status_enu_t addResources(std::string& filename) { return logError_NotImplemented; }
    virtual oms_status_enu_t deleteReferencesInSSD(const std::string& filename) {return logError_NotImplemented;}
    virtual oms_status_enu_t deleteResourcesInSSP(const std::string& filename) {return logError_NotImplemented;}

    const ComRef& getCref() const { return cref; }
    ComRef getFullCref() const;
    Element* getElement() { return &element; }
    Connector* getConnector(const ComRef& cref);
    Connector** getConnectors() { return &connectors[0]; }
    oms_status_enu_t deleteConnector(const ComRef& cref);
    void getAllResources(std::vector<std::string>& resources) const { resources.push_back(path); }
    const std::string& getPath() const { return path; }
    const std::string& getTempDir() const { return tempDir; }
    void setPath(const std::string& newPath) { this->path = newPath; }
    void setTempDir(const std::string& tempDir) { this->tempDir = tempDir; }
    oms_component_enu_t getType() const { return type; }
    System* getParentSystem() const { return parentSystem; }
    Model& getModel() const;
    void setGeometry(const ssd::ElementGeometry& geometry) { element.setGeometry(&geometry); }

    const DirectedGraph& getInitialUnknownsGraph() { return initialUnknownsGraph; }
    const DirectedGraph& getOutputsGraph() { return outputsGraph; }

    oms_status_enu_t rename(const ComRef& newCref); ///< rename submodules, e.g., fmu:s

    virtual void getFilteredSignals(std::vector<Connector>& filteredSignals) const = 0;

  protected:
    Component(const ComRef& cref, oms_component_enu_t type, System* parentSystem, const std::string& path);

    // stop the compiler generating methods copying the object
    Component(Component const&);            ///< not implemented
    Component& operator=(Component const&); ///< not implemented

    virtual oms_status_enu_t renameValues(const ComRef& oldCref, const ComRef& newCref) { return oms_status_ok; }

  protected:
    DirectedGraph initialUnknownsGraph;
    DirectedGraph outputsGraph;
    Element element;
    std::vector<Connector*> connectors;

    Clock clock;
    unsigned int clock_id;

  private:
    System* parentSystem;
    ComRef cref;
    oms_component_enu_t type;
    std::string path;  ///< resource file (fmu, mat)
    std::string tempDir;  ///< unzipped fmu
    Values values;
  };
}

#endif
