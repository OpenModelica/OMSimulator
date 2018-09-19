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

#ifndef _OMS2_CONNECTOR_H_
#define _OMS2_CONNECTOR_H_

#include "ComRef.h"
#include "SignalRef.h"
#include "Types.h"
#include "ssd/ConnectorGeometry.h"

#include <string>

#include <pugixml.hpp>

namespace oms3
{
  /**
   * \brief Connector
   */
  class Connector : protected oms_connector_t
  {
  public:
    /**
     * This constructor creates a oms2::Connector without geometry information.
     */
    Connector(oms_causality_enu_t causality, oms_signal_type_enu_t type, const oms3::ComRef& name);
    /**
     * This constructor is used if the optional SSD element giving the geometry
     * information of the connector is initialized as well.
     */
    Connector(oms_causality_enu_t causality, oms_signal_type_enu_t type, const oms3::ComRef& name, double height);
    ~Connector();

    oms_status_enu_t exportToSSD(pugi::xml_node& root) const;

    // methods to copy the object
    Connector(const Connector& rhs);
    Connector& operator=(const Connector& rhs);

    void setName(const oms3::ComRef& name);
    void setGeometry(const oms2::ssd::ConnectorGeometry* newGeometry);

    const oms_causality_enu_t getCausality() const {return causality;}
    const oms_signal_type_enu_t getType() const {return type;}
    const oms3::ComRef getName() const {return oms3::ComRef(std::string(name));}
    const oms2::ssd::ConnectorGeometry* getGeometry() const {return reinterpret_cast<oms2::ssd::ConnectorGeometry*>(geometry);}
  };
}

namespace oms2
{
/**
   * \brief Connector
   */
class Connector : protected oms_connector_t
{
public:
  /**
     * This constructor creates a oms2::Connector without geometry information.
     */
    Connector(oms_causality_enu_t causality, oms_signal_type_enu_t type, const oms2::SignalRef& name);
    /**
     * This constructor is used if the optional SSD element giving the geometry
     * information of the connector is initialized as well.
     */
    Connector(oms_causality_enu_t causality, oms_signal_type_enu_t type, const oms2::SignalRef& name, double height);
    ~Connector();

    oms_status_enu_t exportToSSD(pugi::xml_node& root) const;

    // methods to copy the object
    Connector(const Connector& rhs);
    Connector& operator=(const Connector& rhs);

    void setName(const oms2::SignalRef& name);
    void setGeometry(const oms2::ssd::ConnectorGeometry* newGeometry);

    const oms_causality_enu_t getCausality() const {return causality;}
    const oms_signal_type_enu_t getType() const {return type;}
    const oms2::SignalRef getName() const {return oms2::SignalRef(std::string(name));}
    const oms2::ssd::ConnectorGeometry* getGeometry() const {return reinterpret_cast<oms2::ssd::ConnectorGeometry*>(geometry);}
  };
}

#endif
