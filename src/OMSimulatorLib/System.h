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

#ifndef _OMS_SYSTEM_H_
#define _OMS_SYSTEM_H_

#include "ComRef.h"
#include "Types.h"
#include "Element.h"
#include "Connection.h"
#include "ssd/ConnectorGeometry.h"

#include <pugixml.hpp>
#include <map>

namespace oms3
{
  class Component;
  class Model;

  class System
  {
  public:
    virtual ~System();

    static System* NewSystem(const oms3::ComRef& cref, oms_system_enu_t type, Model* parentModel, System* parentSystem);
    System* getSystem(const oms3::ComRef& cref);
    Component* getComponent(const oms3::ComRef& cref);
    const ComRef& getName() const {return cref;}
    ComRef getFullName();
    oms3::Element* getElement() {return &element;}
    oms_status_enu_t list(const ComRef& cref, char** contents);
    oms_system_enu_t getType() const {return type;}
    oms_status_enu_t addSubSystem(const oms3::ComRef& cref, oms_system_enu_t type);
    bool validCref(const oms3::ComRef& cref);
    oms_status_enu_t exportToSSD(pugi::xml_node& node) const;
    virtual oms_status_enu_t exportToSSD_SimulationInformation(pugi::xml_node& node) const = 0;
    void setGeometry(const oms3::ssd::ElementGeometry& geometry) {element.setGeometry(&geometry);}
    oms_status_enu_t addConnector(const oms3::ComRef& cref, oms_causality_enu_t causality, oms_signal_type_enu_t type);
    oms3::Connector *getConnector(const oms3::ComRef& cref);
    oms_status_enu_t addConnection(const oms3::ComRef& crefA, const oms3::ComRef& crefB);
    oms_status_enu_t setConnectorGeometry(const oms3::ComRef& cref, const oms2::ssd::ConnectorGeometry* geometry);
    oms_status_enu_t setConnectionGeometry(const oms3::ComRef &crefA, const oms3::ComRef &crefB, const oms2::ssd::ConnectionGeometry* geometry);

  protected:
    System(const ComRef& cref, oms_system_enu_t type, Model* parentModel, System* parentSystem);

    // stop the compiler generating methods copying the object
    System(System const& copy);            ///< not implemented
    System& operator=(System const& copy); ///< not implemented

  private:
    ComRef cref;
    oms_system_enu_t type;
    Model* parentModel;
    System* parentSystem;
    std::map<ComRef, System*> subsystems;
    std::map<ComRef, Component*> components;

    oms3::Element element;
    std::vector<oms3::Connector*> connectors;   ///< last element is always NULL
    std::vector<oms3_element_t*> subelements;   ///< last element is always NULL; don't free it
    std::vector<oms3::Connection*> connections; ///< last element is always NULL
  };
}

#endif
