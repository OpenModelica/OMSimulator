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

#ifndef _OMS2_CONNECTION_H_
#define _OMS2_CONNECTION_H_

#include "ComRef.h"
#include "SignalRef.h"
#include "Types.h"
#include "ssd/ConnectionGeometry.h"

#include <string>

namespace oms3
{
  /// **
  // * \brief Connection
  // * /
  class Connection : protected oms3_connection_t
  {
  public:
    Connection(const oms3::ComRef& conA, const oms3::ComRef& conB, oms3_connection_type_enu_t type=oms3_connection_single);
    ~Connection();

    // methods to copy the object
    Connection(const Connection& rhs);
    Connection& operator=(const Connection& rhs);

    oms_status_enu_t exportToSSD(pugi::xml_node& root) const;

    const oms3::ComRef getSignalA() const {return oms3::ComRef(conA);}
    const oms3::ComRef getSignalB() const {return oms3::ComRef(conB);}

    const oms2::ssd::ConnectionGeometry* getGeometry() const {return reinterpret_cast<oms2::ssd::ConnectionGeometry*>(geometry);}
    void setGeometry(const oms2::ssd::ConnectionGeometry* newGeometry);

    void setTLMParameters(double delay, double alpha, double impedance, double impedancerot);

    oms3_connection_type_enu_t getType() const {return type;}

    bool isEqual(const oms3::Connection& connection) const;
    bool isEqual(const oms3::ComRef& signalA, const oms3::ComRef& signalB) const;

  private:
    friend bool operator==(const Connection& lhs, const Connection& rhs);
    friend bool operator!=(const Connection& lhs, const Connection& rhs);
  };

  inline bool operator==(const Connection& lhs, const Connection& rhs) {return lhs.isEqual(rhs);}
  inline bool operator!=(const Connection& lhs, const Connection& rhs) {return !(lhs == rhs);}
}


namespace oms2
{
  /**
   * \brief Connection
   */
  class Connection : protected oms_connection_t
  {
  public:
    Connection(const oms2::ComRef& parent, const oms2::SignalRef& conA, const oms2::SignalRef& conB);
    ~Connection();

    // methods to copy the object
    Connection(const Connection& rhs);
    Connection& operator=(const Connection& rhs);

    const oms2::ComRef getParent() const {return oms2::ComRef(parent);}
    void setParent(const oms2::ComRef& parent);
    const oms2::SignalRef getSignalA() const {return oms2::SignalRef(conA);}
    const oms2::SignalRef getSignalB() const {return oms2::SignalRef(conB);}

    const oms2::ssd::ConnectionGeometry* getGeometry() const {return reinterpret_cast<oms2::ssd::ConnectionGeometry*>(geometry);}
    void setGeometry(const oms2::ssd::ConnectionGeometry* newGeometry);

    bool isEqual(const oms2::Connection& connection) const;
    bool isEqual(const oms2::ComRef& parent, const oms2::SignalRef& signalA, const oms2::SignalRef& signalB) const;

    void describe();

  private:
    friend bool operator==(const Connection& lhs, const Connection& rhs);
    friend bool operator!=(const Connection& lhs, const Connection& rhs);
  };

  inline bool operator==(const Connection& lhs, const Connection& rhs) {return lhs.isEqual(rhs);}
  inline bool operator!=(const Connection& lhs, const Connection& rhs) {return !(lhs == rhs);}
}

#endif
