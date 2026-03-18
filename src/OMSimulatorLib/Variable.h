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
#include "OMSimulator/Types.h"
#include "dcp/model/DcpTypes.hpp"
#include "dcp/xml/DcpSlaveDescriptionElements.hpp"

#include <fmi4c.h>
#include <string>
#include <vector>

// TODO: Maybe move this to separate file (dcp)
typedef enum {dcpCausalityInput, dcpCausalityOutput, dcpCausalityParameter, dcpCausalityLocal, dcpCausalityStructuralParameter } dcpCausality_t;
typedef enum {dcpFloat64, dcpFloat32, dcpInt64, dcpInt32, dcpInt16, dcpInt8, dcpUInt64, dcpUInt32, dcpUInt16, dcpUInt8, dcpString, dcpBinary} dcpDataType_t;

class SlaveDescription_t;

namespace oms
{
  class Variable
  {
  public:
    Variable(fmiHandle * fmi4c, int index, oms_component_enu_t componentType);
    Variable(SlaveDescription_t *desc, int index);   //For DCP components, component type is implicit
    ~Variable();

    void markAsState(size_t der_index) { is_state = true; this->der_index = der_index; }
    void markAsContinuousTimeState(size_t der_index) { is_continuous_time_state = true; this->der_index = der_index; }
    void markAsContinuousTimeDer() { is_continuous_time_der = true; }

    unsigned int getStateIndex() const { return state_index; }

    bool isFmi2() const {return fmi2;}
    bool isFmi3() const {return fmi3;}

    // causality attribute
    bool isParameter() const { return isFmi2() ? (fmi2CausalityParameter == fmi2Causality_) : (fmi3CausalityParameter == fmi3Causality_);}
    bool isCalculatedParameter() const { return isFmi2() ? (fmi2CausalityCalculatedParameter == fmi2Causality_) : (fmi3CausalityCalculatedParameter ==fmi3Causality_);}
    bool isInput() const { return isFmi2() ? (fmi2CausalityInput == fmi2Causality_) : (fmi3CausalityInput == fmi3Causality_);}
    bool isOutput() const { return isFmi2() ? (fmi2CausalityOutput == fmi2Causality_) : (fmi3CausalityOutput == fmi3Causality_);}
    bool isLocal() const { return isFmi2() ? (fmi2CausalityLocal == fmi2Causality_) : (fmi3CausalityLocal == fmi3Causality_);}
    bool isIndependent() const { return isFmi2() ? (fmi2CausalityIndependent == fmi2Causality_) : (fmi3CausalityIndependent == fmi3Causality_);}

    bool isState() const { return is_state; }
    bool isDer() const { return is_der; }
    bool isContinuousTimeState() const { return is_continuous_time_state; }
    bool isContinuousTimeDer() const { return is_continuous_time_der; }

    // initial attribute
    bool isExact() const { return isFmi2() ? (fmi2InitialExact == fmi2InitialProperty) : (fmi3InitialExact == fmi3InitialProperty);}
    bool isApprox() const { return isFmi2() ? (fmi2InitialApprox == fmi2InitialProperty) : (fmi3InitialApprox == fmi3InitialProperty);}
    bool isCalculated() const { return isFmi2() ? (fmi2InitialCalculated == fmi2InitialProperty) : (fmi3InitialCalculated == fmi3InitialProperty); }

    bool isInitialUnknown() const {
      return (isOutput() && (isApprox() || isCalculated()))
        || (isCalculatedParameter())
        || (isContinuousTimeState() && (isApprox() || isCalculated()))
        || (isContinuousTimeDer() && (isApprox() || isCalculated()));
    }

    const ComRef& getCref() const { return cref; }
    operator std::string() const { return std::string(cref); }

    fmi2ValueReference getValueReference() const { return fmi2Vr; }
    fmi3ValueReference getValueReferenceFMI3() const { return fmi3Vr; }
    valueReference_t getValueReferenceDCP() const { return dcpVr; }
    oms_signal_type_enu_t getType() const { return type; }
    oms_signal_numeric_type_enu_t getNumericType() const {return numericType;}
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

    void configureFMI2Variable(fmiHandle *fmi4c, int index);
    void configureFMI3Variable(fmiHandle *fmi4c, int index);
    void configureDCPVariable(SlaveDescription_t *desc, int index);

    ComRef cref;
    std::string description;
    oms_component_enu_t componentType;

    // FMI 2.0 specific members
    fmi2ValueReference fmi2Vr;
    fmi2Causality fmi2Causality_;
    fmi2Variability fmi2Variability_;
    fmi2Initial fmi2InitialProperty;

    // FMI 3.0 specific members
    fmi3ValueReference fmi3Vr;
    fmi3Causality fmi3Causality_;
    fmi3Variability fmi3Variability_;
    fmi3Initial fmi3InitialProperty;

    // DCP specific members
    valueReference_t dcpVr;
    dcpCausality_t dcpCausality;
    Variability dcpVariability;
    dcpDataType_t dcpDataType;

    bool is_state;
    bool is_der;
    bool is_continuous_time_state;
    bool is_continuous_time_der;
    oms_signal_type_enu_t type;
    oms_signal_numeric_type_enu_t numericType;
    unsigned int index; ///< index origin = 0
    size_t state_index; ///< index origin = 0
    size_t der_index; ///< index origin = 0
    bool fmi2;
    bool fmi3;
    friend bool operator==(const oms::Variable& v1, const oms::Variable& v2);
    friend bool operator!=(const oms::Variable& v1, const oms::Variable& v2);
  };

  bool operator==(const oms::Variable& v1, const oms::Variable& v2);
  bool operator!=(const oms::Variable& v1, const oms::Variable& v2);
}

#endif
