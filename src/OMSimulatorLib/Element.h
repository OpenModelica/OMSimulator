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

#ifndef _OMS2_ELEMENT_H_
#define _OMS2_ELEMENT_H_

#include "ComRef.h"
#include "SignalRef.h"
#include "Connector.h"
#include "BusConnector.h"
#include "TLMBusConnector.h"
#include "Types.h"
#include "ssd/ElementGeometry.h"

#include <string>
#include <vector>

namespace oms3
{
  /**
   * \brief Element
   */
  class Element : protected oms3_element_t
  {
  public:
    Element(oms3_element_enu_t type, const ComRef& name);
    ~Element();

    const oms3_element_enu_t getType() const {return type;}
    const oms3::ComRef getName() const {return oms3::ComRef(std::string(name));}
    oms3::Connector** getConnectors() const {return reinterpret_cast<oms3::Connector**>(connectors);}
    const oms3::ssd::ElementGeometry* getGeometry() const {return reinterpret_cast<oms3::ssd::ElementGeometry*>(geometry);}

    void setName(const ComRef& name);
    void setGeometry(const oms3::ssd::ElementGeometry* newGeometry);
    void setConnectors(oms3::Connector** newConnectors);
    void setBusConnectors(oms3::BusConnector **newBusConnectors);
    void setTLMBusConnectors(oms3::TLMBusConnector **newTLMBusConnectors);
    void setSubElements(oms3_element_t** subelements);

  private:
    // methods to copy the object
    Element(const Element& rhs);            ///< not implemented
    Element& operator=(const Element& rhs); ///< not implemented
  };
}

namespace oms2
{
  /**
   * \brief Element
   */
  class Element : protected oms_element_t
  {
  public:
    Element(oms_element_type_enu_t type, const ComRef& name);
    ~Element();

    const oms_element_type_enu_t getType() const {return type;}
    const oms2::ComRef getName() const {return oms2::ComRef(std::string(name));}
    oms2::Connector** getConnectors() const {return reinterpret_cast<oms2::Connector**>(connectors);}
    const oms2::ssd::ElementGeometry* getGeometry() const {return reinterpret_cast<oms2::ssd::ElementGeometry*>(geometry);}

    void setName(const ComRef& name);
    void setGeometry(const oms2::ssd::ElementGeometry* newGeometry);
    void setConnectors(const std::vector<oms2::Connector> newConnectors);

    void describe();

  private:
    // methods to copy the object
    Element(const Element& rhs);            ///< not implemented
    Element& operator=(const Element& rhs); ///< not implemented
  };
}

#endif
