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

#ifndef _OMS_CONNECTION_H_
#define _OMS_CONNECTION_H_

#include "ComRef.h"
#include "Connector.h"
#include "ssd/ConnectionGeometry.h"
#include "OMSimulator/Types.h"

#include <string>

namespace oms
{
  /// **
  // * \brief Connection
  // * /
  class Connection : protected oms_connection_t
  {
  public:
    Connection(const oms::ComRef& conA, const oms::ComRef& conB, bool suppressUnitConversion=false, oms_connection_type_enu_t type=oms_connection_single);
    ~Connection();

    // methods to copy the object
    Connection(const Connection& rhs);
    Connection& operator=(const Connection& rhs);

    oms_status_enu_t exportToSSD(pugi::xml_node& root) const;

    const oms::ComRef getSignalA() const {return oms::ComRef(conA);}
    const oms::ComRef getSignalB() const {return oms::ComRef(conB);}

    const oms::ssd::ConnectionGeometry* getGeometry() const {return reinterpret_cast<oms::ssd::ConnectionGeometry*>(geometry);}
    void setGeometry(const oms::ssd::ConnectionGeometry* newGeometry, bool inverse=false);

    oms_connection_type_enu_t getType() const {return type;}

    bool isEqual(const oms::Connection& connection) const;
    bool isEqual(const oms::ComRef& signalA, const oms::ComRef& signalB) const; ///< "A->B" and "B->A" are treated the same
    bool isStrictEqual(const oms::ComRef& signalA, const oms::ComRef& signalB) const; ///< "A->B" is not strict equal "B->A"
    bool containsSignal(const oms::ComRef& signal) const;
    bool containsSignalB(const oms::ComRef& signal) const;

    bool getSuppressUnitConversion() {return suppressUnitConversion;}
    /**
    * \brief Checks a connection based on SSP-1.0 connection table
    */
    static bool isValid(const ComRef& crefA, const ComRef& crefB, const Connector& conA, const Connector& conB);
    static bool isValidExportConnectorName(const Connector& conA, const Connector& conB);

    static bool isValidUnits(const ComRef& crefA, const ComRef& crefB, const Connector& conA, const Connector& conB);
    static void getSIUnits(const Connector& connector, std::map<std::string, std::string>& baseUnits);
    oms_status_enu_t rename(const oms::ComRef& cref, const oms::ComRef& newCref);
  private:
    friend bool operator==(const Connection& lhs, const Connection& rhs);
    friend bool operator!=(const Connection& lhs, const Connection& rhs);
  };

  inline bool operator==(const Connection& lhs, const Connection& rhs) {return lhs.isEqual(rhs);}
  inline bool operator!=(const Connection& lhs, const Connection& rhs) {return !(lhs == rhs);}
}

#endif
