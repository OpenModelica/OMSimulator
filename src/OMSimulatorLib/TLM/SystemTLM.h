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

#ifndef _OMS_SYSTEM_TLM_H_
#define _OMS_SYSTEM_TLM_H_

#include "ComRef.h"
#include "System.h"
#include "Types.h"
#include "../../OMTLMSimulator/common/Plugin/PluginImplementer.h"

namespace oms3
{
  class Model;
  class SystemWC;

  class SystemTLM : public System
  {
  public:
    ~SystemTLM();

    static System* NewSystem(const oms3::ComRef& cref, Model* parentModel, System* parentSystem);
    oms_status_enu_t exportToSSD_SimulationInformation(pugi::xml_node& node) const;
    oms_status_enu_t importFromSSD_SimulationInformation(const pugi::xml_node& node);
    oms_status_enu_t setSocketData(const std::string& address, int managerPort, int monitorPort);
    oms_status_enu_t setPositionAndOrientation(const ComRef &cref, std::vector<double> x, std::vector<double> A);

    oms_status_enu_t instantiate();
    oms_status_enu_t initialize();
    oms_status_enu_t terminate();
    oms_status_enu_t reset();
    oms_status_enu_t stepUntil(double stopTime, void (*cb)(const char* ident, double time, oms_status_enu_t status));

    oms_status_enu_t connectToSockets(const oms3::ComRef cref, std::string server);
    void disconnectFromSockets(const oms3::ComRef cref);
    oms_status_enu_t updateInitialValues(const oms3::ComRef cref);

    oms_status_enu_t initializeSubSystem(ComRef cref);
    oms_status_enu_t simulateSubSystem(ComRef cref, double stopTime);
    void writeToSockets(oms3::SystemWC *system, double time, Component *component);
    void readFromSockets(SystemWC *system, double time, Component *component);
    void sendValueToLogger(int varId, double time, double value);
    int registerLogVariable();
    void registerLogVariables(System* system, ResultWriter& resultFile);
    void sendValuesToLogger(System* system, double time);
    oms_status_enu_t registerSignalsForResultFile(ResultWriter& resultFile);
    oms_status_enu_t updateSignals(ResultWriter& resultFile);

  protected:
    SystemTLM(const ComRef& cref, Model* parentModel, System* parentSystem);

    // stop the compiler generating methods copying the object
    SystemTLM(SystemTLM const& copy);            ///< not implemented
    SystemTLM& operator=(SystemTLM const& copy); ///< not implemented

  private:
    double interpolate(double t1, double t2, double x1, double x2, double t)
    {
      if(t2 == t1)
        return x2;
      return x1 + (x2-x1)/(t2-t1)*(t-t1);
    }

    void* model;
    std::string address = "";
    int desiredManagerPort=0;
    int desiredMonitorPort=0;
    int actualManagerPort=0;
    int actualMonitorPort=0;

    std::vector<ComRef> connectedsubsystems;
    std::map<System*, TLMPlugin*> plugins;
    std::vector<ComRef> initializedsubsystems;
    std::mutex pluginsMutex;
    std::mutex setConnectedMutex;
    std::mutex setInitializedMutex;
    std::map<System*, std::mutex> socketMutexes;
    std::mutex logMutex;

    int numLogVars = 0;
    std::map<int, std::vector<std::pair<double,double> > > logBuffer;
    double nextLogTime;
    double logTime;
    double logStep = 1e-2;
    std::map<TLMBusConnector*, int> busLogIds;
    std::map<Connector*, int> connectorLogIds;

    // simulation information
  };
}

#endif
