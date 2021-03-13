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

#ifndef _OMS_CONNECTOR_H_
#define _OMS_CONNECTOR_H_

#include "ComRef.h"
#include "ssd/ConnectorGeometry.h"
#include "Types.h"

#include <string>

#include <pugixml.hpp>

namespace oms
{
  /**
   * \brief Connector
   */
  class Connector : protected oms_connector_t
  {
  public:
    Connector(oms_causality_enu_t causality, oms_signal_type_enu_t type, const oms::ComRef& name);
    Connector(oms_causality_enu_t causality, oms_signal_type_enu_t type, const oms::ComRef& name, double height);
    ~Connector();

    // you have to free the memory yourself
    static Connector* NewConnector(const pugi::xml_node& node, const std::string& sspVersion);

    oms_status_enu_t exportToSSD(pugi::xml_node& root) const;
    static std::string getTypeString(const pugi::xml_node& node, const std::string& sspVersion);
    std::string getTypeString() const;
    std::string getCausalityString() const;

    // methods to copy the object
    Connector(const Connector& rhs);
    Connector& operator=(const Connector& rhs);

    operator std::string() const {return std::string(name);}

    void setName(const oms::ComRef& name);
    void setGeometry(const oms::ssd::ConnectorGeometry* newGeometry);

    const oms_causality_enu_t getCausality() const {return causality;}
    const oms_signal_type_enu_t getType() const {return type;}
    const oms::ComRef getName() const {return oms::ComRef(name);}
    const oms::ssd::ConnectorGeometry* getGeometry() const {return reinterpret_cast<oms::ssd::ConnectorGeometry*>(geometry);}
    Connector addPrefix(const ComRef& prefix) const;

    bool isInput() const { return oms_causality_input == causality; }
    bool isOutput() const { return oms_causality_output == causality; }
    bool isParameter() const { return oms_causality_parameter == causality; }
    bool isCalculatedParameter() const { return oms_causality_calculatedParameter == causality; }

    bool isTypeReal() const { return oms_signal_type_real == type; }
    bool isTypeInteger() const { return oms_signal_type_integer == type || oms_signal_type_enum == type; }
    bool isTypeBoolean() const { return oms_signal_type_boolean == type; }

  private:
    friend bool operator==(const Connector& v1, const Connector& v2);
    friend bool operator!=(const Connector& v1, const Connector& v2);
  };

  bool operator==(const Connector& v1, const Connector& v2);
  bool operator!=(const Connector& v1, const Connector& v2);
}

#endif
