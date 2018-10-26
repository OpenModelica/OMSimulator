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
    oms_status_enu_t stepUntil(double stopTime);

    oms_status_enu_t connectToSockets(const oms3::ComRef cref, std::string server);
    void disconnectFromSockets(const oms3::ComRef cref);
    oms_status_enu_t setInitialValues(ComRef cref, std::vector<double> values);
    oms_status_enu_t updateInitialValues(const oms3::ComRef cref);

    void writeToSockets(oms3::SystemWC *system, double time, Component *component);
    void readFromSockets(SystemWC *system, double time, Component *component);

  protected:
    SystemTLM(const ComRef& cref, Model* parentModel, System* parentSystem);

    // stop the compiler generating methods copying the object
    SystemTLM(SystemTLM const& copy);            ///< not implemented
    SystemTLM& operator=(SystemTLM const& copy); ///< not implemented

  private:
    void* model;
    std::string address = "";
    int managerPort=0;
    int monitorPort=0;

    std::vector<ComRef> connectedsubsystems;
    std::map<System*, TLMPlugin*> plugins;
    std::vector<ComRef> initializedsubsystems;
    std::map<ComRef, std::vector<double> > initialValues;

    // simulation information
  };
}

#endif
