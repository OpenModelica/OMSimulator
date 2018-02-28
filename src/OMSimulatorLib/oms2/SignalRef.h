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

#ifndef _OMS_SIGNAL_REF_H_
#define _OMS_SIGNAL_REF_H_

#include "ComRef.h"

#include <string>

namespace oms2
{
  /**
   * \brief SignalRef - signal reference
   */
  class SignalRef
  {
  public:
    SignalRef(const std::string& signal); ///< format: comp1.comp2.comp3:var
    SignalRef(const ComRef& cref, const std::string& var);
    ~SignalRef();

    // methods to copy the signal reference
    SignalRef(SignalRef const& copy);
    SignalRef& operator=(SignalRef const& copy);
    bool operator<(const SignalRef& rhs);

    std::string toString() const {return cref.toString() + ":" + var;}

    const oms2::ComRef& getCref() const {return cref;}
    const std::string& getVar() const {return var;}

    bool isEqual(const char* str) const {return toString().compare(str) == 0;}

  private:
    oms2::ComRef cref;
    std::string var;
  };

  inline bool operator==(const SignalRef& lhs, const SignalRef& rhs) {return lhs.toString() == rhs.toString();}
  inline bool operator!=(const SignalRef& lhs, const SignalRef& rhs) {return !(lhs == rhs);}
}

#endif
