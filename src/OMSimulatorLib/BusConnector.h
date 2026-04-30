/*
 * This file is part of OpenModelica.
 *
 * Copyright (c) 1998-2026, Open Source Modelica Consortium (OSMC),
 * c/o Linköpings universitet, Department of Computer and Information Science,
 * SE-58183 Linköping, Sweden.
 *
 * All rights reserved.
 *
 * THIS PROGRAM IS PROVIDED UNDER THE TERMS OF AGPL VERSION 3 LICENSE OR
 * THIS OSMC PUBLIC LICENSE (OSMC-PL) VERSION 1.8.
 * ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS PROGRAM CONSTITUTES
 * RECIPIENT'S ACCEPTANCE OF THE OSMC PUBLIC LICENSE OR THE GNU AGPL
 * VERSION 3, ACCORDING TO RECIPIENTS CHOICE.
 *
 * The OpenModelica software and the OSMC (Open Source Modelica Consortium)
 * Public License (OSMC-PL) are obtained from OSMC, either from the above
 * address, from the URLs:
 * http://www.openmodelica.org or
 * https://github.com/OpenModelica/ or
 * http://www.ida.liu.se/projects/OpenModelica,
 * and in the OpenModelica distribution.
 *
 * GNU AGPL version 3 is obtained from:
 * https://www.gnu.org/licenses/licenses.html#GPL
 *
 * This program is distributed WITHOUT ANY WARRANTY; without
 * even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE, EXCEPT AS EXPRESSLY SET FORTH
 * IN THE BY RECIPIENT SELECTED SUBSIDIARY LICENSE CONDITIONS OF OSMC-PL.
 *
 * See the full OSMC Public License conditions for more details.
 *
 */

#ifndef BUSCONNECTOR_H
#define BUSCONNECTOR_H


#include "ComRef.h"
#include "OMSimulator/Types.h"
#include "Connector.h"
#include "ssd/ConnectorGeometry.h"

#include <string>
#include <vector>

#include <pugixml.hpp>

namespace oms
{
  /**
   * \brief BusConnector
   */
  class BusConnector : protected oms_busconnector_t
  {
  public:
    BusConnector(const oms::ComRef& name);
    ~BusConnector();

    oms_status_enu_t exportToSSD(pugi::xml_node& root) const;

    // methods to copy the object
    BusConnector(const BusConnector& rhs);
    BusConnector& operator=(const BusConnector& rhs);

    void setName(const oms::ComRef& name);
    void setGeometry(const oms::ssd::ConnectorGeometry* newGeometry);

    const oms::ComRef getName() const {return oms::ComRef(name);}
    const oms::ssd::ConnectorGeometry* getGeometry() const {return reinterpret_cast<oms::ssd::ConnectorGeometry*>(geometry);}

    oms_status_enu_t addConnector(const oms::ComRef& cref);
    oms_status_enu_t deleteConnector(const oms::ComRef& cref);
    std::vector<oms::ComRef> getConnectors() const {return conrefs;}

    void updateConnectors();

  private:
    std::vector<oms::ComRef> conrefs;
  };
}

#endif // BUSCONNECTOR_H
