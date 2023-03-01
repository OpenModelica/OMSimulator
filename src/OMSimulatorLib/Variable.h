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

#ifndef _OMS_VARIABLE_H_
#define _OMS_VARIABLE_H_

#include "ComRef.h"
#include "Connector.h"
#include "Types.h"

#include <fmi4c.h>
#include <string>
#include <vector>

namespace oms
{
  class Variable
  {
  public:
    Variable(fmiHandle * fmi4c, int index);
    ~Variable();

    void markAsState(size_t der_index) { is_state = true; this->der_index = der_index; }
    void markAsContinuousTimeState(size_t der_index) { is_continuous_time_state = true; this->der_index = der_index; }
    void markAsContinuousTimeDer() { is_continuous_time_der = true; }

    unsigned int getStateIndex() const { return state_index; }

    // causality attribute
    bool isParameter() const { return fmi2CausalityParameter == causality; }
    bool isCalculatedParameter() const { return fmi2CausalityCalculatedParameter == causality; }
    bool isInput() const { return fmi2CausalityInput == causality; }
    bool isOutput() const { return fmi2CausalityOutput == causality; }
    bool isLocal() const { return fmi2CausalityLocal == causality; }
    bool isIndependent() const { return fmi2CausalityIndependent == causality; }

    bool isState() const { return is_state; }
    bool isDer() const { return is_der; }
    bool isContinuousTimeState() const { return is_continuous_time_state; }
    bool isContinuousTimeDer() const { return is_continuous_time_der; }

    // initial attribute
    bool isExact() const { return fmi2InitialExact == initialProperty; }
    bool isApprox() const { return fmi2InitialApprox == initialProperty; }
    bool isCalculated() const { return fmi2InitialCalculated == initialProperty; }

    bool isInitialUnknown() const {
      return (isOutput() && (isApprox() || isCalculated()))
        || (isCalculatedParameter())
        || (isContinuousTimeState() && (isApprox() || isCalculated()))
        || (isContinuousTimeDer() && (isApprox() || isCalculated()));
    }

    const ComRef& getCref() const { return cref; }
    operator std::string() const { return std::string(cref); }

    fmi2ValueReference getValueReference() const { return vr; }
    oms_signal_type_enu_t getType() const { return type; }
    const std::string& getDescription() const { return description; }

    bool isTypeBoolean() const { return oms_signal_type_boolean == type; }
    bool isTypeInteger() const { return oms_signal_type_integer == type || oms_signal_type_enum == type; }
    bool isTypeReal() const { return oms_signal_type_real == type; }
    bool isTypeString() const { return oms_signal_type_string == type; }

    std::string getCausalityString() const;
    oms_causality_enu_t getCausality() const;

    unsigned int getIndex() const { return index; }
    oms::Connector makeConnector(const oms::ComRef& owner) const { return oms::Connector(getCausality(), type, cref, owner); }

  private:
    ComRef cref;
    std::string description;
    fmi2ValueReference vr;
    fmi2Causality causality;
    fmi2Variability variability;
    fmi2Initial initialProperty;

    bool is_state;
    bool is_der;
    bool is_continuous_time_state;
    bool is_continuous_time_der;
    oms_signal_type_enu_t type;
    unsigned int index; ///< index origin = 0
    size_t state_index; ///< index origin = 0
    size_t der_index; ///< index origin = 0

    friend bool operator==(const oms::Variable& v1, const oms::Variable& v2);
    friend bool operator!=(const oms::Variable& v1, const oms::Variable& v2);
  };

  bool operator==(const oms::Variable& v1, const oms::Variable& v2);
  bool operator!=(const oms::Variable& v1, const oms::Variable& v2);
}

#endif
