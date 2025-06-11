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

#ifndef _OMS_COMPONENT_TABLE_H_
#define _OMS_COMPONENT_TABLE_H_

#include "Component.h"
#include "ComRef.h"
#include "ResultReader.h"
#include "ResultWriter.h"
#include "SignalDerivative.h"
#include "Snapshot.h"
#include <map>
#include <pugixml.hpp>
#include <string>
#include <unordered_map>
#include <vector>

namespace oms
{
  class ComponentTable : public Component
  {
  public:
    ~ComponentTable();

    static Component* NewComponent(const oms::ComRef& cref, System* parentSystem, const std::string& path);
    static Component* NewComponent(const pugi::xml_node& node, System* parentSystem, const std::string& sspVersion, const Snapshot& snapshot);

    oms_status_enu_t exportToSSD(pugi::xml_node& node, Snapshot& snapshot, std::string variantName) const;
    oms_status_enu_t exportToSSVTemplate(pugi::xml_node& ssvNode) {return oms_status_ok;}
    oms_status_enu_t exportToSSMTemplate(pugi::xml_node& ssmNode) {return oms_status_ok;}
    oms_status_enu_t instantiate();
    oms_status_enu_t initialize() {return oms_status_ok;}
    oms_status_enu_t terminate() {return oms_status_ok;}
    oms_status_enu_t reset();

    oms_status_enu_t stepUntil(double stopTime) {time = stopTime; return oms_status_ok;}
    oms_status_enu_t setTime(double time) {this->time = time; return oms_status_ok;}

    Variable* getVariable(const ComRef& cref) {logError_NotImplemented; return NULL;}
    oms_status_enu_t getReal(const ComRef& cref, double& value);
    oms_status_enu_t getRealOutputDerivative(const ComRef& cref, SignalDerivative& value);
    oms_status_enu_t getInteger(const ComRef& cref, int& value);
    oms_status_enu_t getBoolean(const ComRef& cref, bool& value);

    oms_status_enu_t registerSignalsForResultFile(ResultWriter& resultFile);
    oms_status_enu_t updateSignals(ResultWriter& resultWriter);
    oms_status_enu_t addSignalsToResults(const char* regex);
    oms_status_enu_t removeSignalsFromResults(const char* regex);

    bool getCanGetAndSetState() {return true;}
    oms_status_enu_t saveState();
    oms_status_enu_t freeState();
    oms_status_enu_t restoreState();

    void getFilteredSignals(std::vector<Connector>& filteredSignals) const;

  protected:
    ComponentTable(const ComRef& cref, System* parentSystem, const std::string& path);

    // stop the compiler generating methods copying the object
    ComponentTable(ComponentTable const& copy);            ///< not implemented
    ComponentTable& operator=(ComponentTable const& copy); ///< not implemented

  private:
    ResultReader* resultReader;
    std::unordered_map<ComRef, ResultReader::Series*> series;
    std::unordered_map<ComRef, bool> exportSeries;
    std::unordered_map<unsigned int /*result file var ID*/, unsigned int /*allVariables ID*/> resultFileMapping;
    double time;
    double storedTime;
    size_t lastIndex = 0;
  };
}

#endif
