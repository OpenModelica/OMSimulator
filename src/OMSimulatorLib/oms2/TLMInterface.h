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

#ifndef _OMS2_TLM_INTERFACE_H_
#define _OMS2_TLM_INTERFACE_H_

#include "ComRef.h"
#include "SignalRef.h"
#include "../Types.h"

namespace oms2
{
  class TLMInterface
  {
  public:
    TLMInterface(const oms2::ComRef& cref, const std::string name, oms_causality_enu_t causality, const std::string domain, const int dimensions);
    ~TLMInterface() {}

    oms2::SignalRef getSignal() { return sig; }
    oms2::ComRef getSubModelName() { return cref; }
    std::string getName() { return name; }
    oms_causality_enu_t getCausality() { return causality; }
    std::string getDomain() { return domain; }
    int getDimensions() { return dimensions; }

    friend bool operator==(const TLMInterface& lhs, const TLMInterface& rhs);
    friend bool operator!=(const TLMInterface& lhs, const TLMInterface& rhs);

  private:
    SignalRef sig;
    oms2::ComRef cref;
    std::string name;
    oms_causality_enu_t causality;
    std::string domain;
    int dimensions;
  };

  inline bool operator==(const TLMInterface& lhs, const TLMInterface& rhs) {return (lhs.cref.c_str() == rhs.cref.c_str() && lhs.name == rhs.name);}
  inline bool operator!=(const TLMInterface& lhs, const TLMInterface& rhs) {return !(lhs == rhs);}
}

#endif
