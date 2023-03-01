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

#include "Variable.h"

#include "Logging.h"
#include "Util.h"

oms::Variable::Variable(fmiHandle* fmi4c, int index_)
  : der_index(0), state_index(0), is_state(false), is_der(false), is_continuous_time_state(false), is_continuous_time_der(false), index(index_)
{
  // extract the attributes
  fmi2VariableHandle *var = fmi2_getVariableByIndex(fmi4c, index_);
  cref = fmi2_getVariableName(var);
  description = fmi2_getVariableDescription(var) ? fmi2_getVariableDescription(var) : "";
  trim(description);
  vr = fmi2_getVariableValueReference(var);
  causality = fmi2_getVariableCausality(var);
  variability = fmi2_getVariableVariability(var);
  // TODO implement the initial attribute table in fmi4c according to FMI specification
  initialProperty = fmi2_getVariableInitial(var);

  switch (fmi2_getVariableDataType(var))
  {
    case fmi2DataTypeReal:
      type = oms_signal_type_real;
      break;
    case fmi2DataTypeInteger:
      type = oms_signal_type_integer;
      break;
    case fmi2DataTypeBoolean:
      type = oms_signal_type_boolean;
      break;
    case fmi2DataTypeString:
      type = oms_signal_type_string;
      break;
    case fmi2DataTypeEnumeration:
      type = oms_signal_type_enum;
      break;
    default:
      logError("Unknown fmi base type");
      type = oms_signal_type_real;
      break;
  }

  // mark derivatives
  if (oms_signal_type_real == type)
  {
    int derivative_index = fmi2_getVariableDerivativeIndex(var);
    if (derivative_index != 0)
    {
      is_der = true;
      state_index = derivative_index;
      if (variability == fmi2VariabilityContinuous)
      {
        is_continuous_time_der = true;
      }
    }
  }
}

oms::Variable::~Variable()
{
}

oms_causality_enu_t oms::Variable::getCausality() const
{
  switch (causality)
  {
  case fmi2CausalityInput:
    return oms_causality_input;

  case fmi2CausalityOutput:
    return oms_causality_output;

  case fmi2CausalityParameter:
    return oms_causality_parameter;

  case fmi2CausalityCalculatedParameter:
    return oms_causality_calculatedParameter;

  default:
    return oms_causality_undefined;
  }
}

std::string oms::Variable::getCausalityString() const
{
  switch (causality)
  {
  case fmi2CausalityInput:
    return "input";

  case fmi2CausalityOutput:
    return "output";

  case fmi2CausalityParameter:
    return "parameter";

  case fmi2CausalityCalculatedParameter:
    return "calculatedParameter";

  case fmi2CausalityIndependent:
    return "independent";

  case fmi2CausalityLocal:
    return "local";

  default:
    return "undefined";
  }
}

bool oms::operator==(const oms::Variable& v1, const oms::Variable& v2)
{
  return v1.cref == v2.cref && v1.vr == v2.vr;
}

bool oms::operator!=(const oms::Variable& v1, const oms::Variable& v2)
{
  return !(v1 == v2);
}
