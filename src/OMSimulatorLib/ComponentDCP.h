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

#ifndef _OMS_COMPONENT_DCP_H_
#define _OMS_COMPONENT_DCP_H_



#include "Component.h"
#include "ComRef.h"
#include "ResultWriter.h"
#include "Snapshot.h"
#include "Values.h"
#include "Variable.h"

#include <fmi4c.h>
#include <map>
#include <pugixml.hpp>
#include <string>
#include <unordered_map>
#include <vector>
#include <cmath>
#include <dcp/driver/ethernet/udp/UdpDriver.hpp>    //Must be included first because of include conflict with Asio and windows.h
#include <dcp/logic/DcpManagerMaster.hpp>

namespace oms
{
class System;

class ComponentDCP : public Component
{
public:
    ~ComponentDCP();

    static Component* NewComponent(const ComRef& cref, System* parentSystem, const std::string& dcpPath, std::string replaceComponent = "");
    static Component* NewComponent(const pugi::xml_node& node, System* parentSystem, const std::string& sspVersion, const Snapshot& snapshot, std::string variantName);
    const FMUInfo* getFMUInfo() const {return &(this->fmuInfo);}

    oms_status_enu_t addSignalsToResults(const char* regex);
    oms_status_enu_t exportToSSD(pugi::xml_node& node, Snapshot& snapshot, std::string variantName) const;
    oms_status_enu_t initialize();
    oms_status_enu_t instantiate();
    oms_status_enu_t stepUntil(double stopTime) { return logError_NotImplemented; }
    oms_status_enu_t registerSignalsForResultFile(ResultWriter& resultFile);
    oms_status_enu_t removeSignalsFromResults(const char* regex);
    oms_status_enu_t reset();
    oms_status_enu_t terminate();
    oms_status_enu_t updateSignals(ResultWriter& resultWriter);
    Variable* getVariable(const ComRef& cref);
    void getFilteredSignals(std::vector<Connector>& filteredSignals) const;
    oms_status_enu_t initializeDependencyGraph_outputs();

protected:
    ComponentDCP(const ComRef& cref, System* parentSystem, const std::string& fmuPath);

    // stop the compiler generating methods copying the object
    ComponentDCP(ComponentDCP const& copy);            ///< not implemented
    ComponentDCP& operator=(ComponentDCP const& copy); ///< not implemented

private:
    FMUInfo fmuInfo;
    SlaveDescription_t *desc;

    std::vector<Variable> allVariables;
    std::vector<unsigned int> calculatedParameters;
    std::vector<unsigned int> derivatives;
    std::vector<unsigned int> inputs;
    std::vector<unsigned int> outputs;
    std::vector<unsigned int> parameters;
    std::vector<bool> exportVariables;
    Values values; ///< start values defined before instantiating the FMU and external inputs defined after initialization

    double time;

    void dcp_initialize();
    void dcp_configuration();
    void dcp_configure();
    void dcp_run(DcpState currentState);
    void dcp_doStep();
    void dcp_stop();
    void dcp_deregister();
    void dcp_sendOutputs(DcpState currentState, uint8_t sender);
    void dcp_receiveAck(uint8_t sender, uint16_t);
    void dcp_receiveNAck(uint8_t sender, uint16_t pduSeqId, DcpError errorCode);
    void dcp_dataReceived(uint16_t dataId, size_t length, uint8_t payload[]);
    void dcp_receiveStateChangedNotification(uint8_t sender, DcpState state);

    UdpDriver *driver;
    DcpManagerMaster *manager;
    uint8_t maxInitRuns = 0;
    uint8_t intializationRuns = 1;
    std::map<dcpId_t, uint64_t> receivedAcks;

    oms::ComRef getValidCref(ComRef cref);
};
}

#endif
