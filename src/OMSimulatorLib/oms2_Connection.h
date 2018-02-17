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

#include "oms2_ComRef.h"
#include "oms2_SignalRef.h"

#include <string>

namespace oms2
{
  /**
   * \brief Connection
   */
  class Connection
  {
  public:
    Connection(const ComRef& cref, const std::string& varA, const std::string& varB);
    Connection(const SignalRef& conA, const SignalRef& conB);
    ~Connection();

    static Connection FromStrings(const ComRef& cref, const std::string& conA, const std::string& conB);
    static Connection FromStrings(const std::string& conA, const std::string& conB);

    // methods to copy the object
    Connection(const Connection& rhs);
    Connection& operator=(const Connection& rhs);

  private:
    oms2::SignalRef conA;
    oms2::SignalRef conB;

    friend bool operator==(const Connection& lhs, const Connection& rhs);
    friend bool operator!=(const Connection& lhs, const Connection& rhs);
  };

  inline bool operator==(const Connection& lhs, const Connection& rhs) {return (lhs.conA == rhs.conA && lhs.conB == rhs.conB) || (lhs.conA == rhs.conB && lhs.conB == rhs.conA);}
  inline bool operator!=(const Connection& lhs, const Connection& rhs) {return !(lhs == rhs);}
}

#endif
