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

#include <JM/jm_portability.h>
#include <iostream>

oms::Variable::Variable(fmi2_import_variable_t* var)
  : der_index(0), state_index(0), is_state(false), is_der(false), is_continuous_time_state(false), is_continuous_time_der(false), cref(fmi2_import_get_variable_name(var)), index(fmi2_import_get_variable_original_order(var))
{
  // extract the attributes
  description = fmi2_import_get_variable_description(var) ? fmi2_import_get_variable_description(var) : "";
  trim(description);
  vr = fmi2_import_get_variable_vr(var);
  causality = fmi2_import_get_causality(var);
  variability = fmi2_import_get_variability(var);
  initialProperty = fmi2_import_get_initial(var);
  //std::cout << "\nVariable.cpp: " << initialProperty;
  switch (fmi2_import_get_variable_base_type(var))
  {
    case fmi2_base_type_real:
      type = oms_signal_type_real;
      break;
    case fmi2_base_type_int:
      type = oms_signal_type_integer;
      break;
    case fmi2_base_type_bool:
      type = oms_signal_type_boolean;
      break;
    case fmi2_base_type_str:
      type = oms_signal_type_string;
      break;
    case fmi2_base_type_enum:
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
    fmi2_import_real_variable_t* varReal = fmi2_import_get_variable_as_real(var);
    fmi2_import_variable_t* varState = (fmi2_import_variable_t*)fmi2_import_get_real_variable_derivative_of(varReal);
    if (varState)
    {
      is_der = true;
      state_index = fmi2_import_get_variable_original_order(varState);
    }
  }
}

oms::Variable::Variable(fmiHandle* fmi4c, int index_)
  : der_index(0), state_index(0), is_state(false), is_der(false), is_continuous_time_state(false), is_continuous_time_der(false), index(index_)
{
  //std::cout << "\n inside Variable structure";

  // extract the attributes
  fmi2VariableHandle *var = fmi2_getVariableByIndex(fmi4c, index_);

  cref = fmi2_getVariableName(var);
  //index = index_;

  description = fmi2_getVariableDescription(var) ? fmi2_getVariableDescription(var) : "";
  trim(description);
  vr = fmi2_getVariableValueReference(var);
  causality_ = fmi2_getVariableCausality(var);
  variability_ = fmi2_getVariableVariability(var);
  initialProperty_ = fmi2_getVariableInitial(var);
  //std::cout << "\nVariablefmi4c.cpp:  " << cref.c_str() << "=>" << initialProperty_;
  // if (!initialProperty_)
  //   std::cout << "\n initial not present";

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

  switch (causality_)
  {
  case fmi2CausalityInput:
    causality = fmi2_causality_enu_input;
    break;
  case fmi2CausalityOutput:
    causality =  fmi2_causality_enu_output;
    break;
  case fmi2CausalityParameter:
    causality =  fmi2_causality_enu_parameter;
    break;
  case fmi2CausalityCalculatedParameter:
    causality =  fmi2_causality_enu_calculated_parameter;
    break;
  default:
    causality = fmi2_causality_enu_unknown;
  }

  switch (initialProperty_)
  {
  case fmi2InitialExact:
    initialProperty = fmi2_initial_enu_exact;
    break;
  case fmi2InitialApprox:
    initialProperty = fmi2_initial_enu_approx;
    break;
  case fmi2InitialCalculated:
    initialProperty = fmi2_initial_enu_calculated;
    break;
  default:
    initialProperty = fmi2_initial_enu_unknown;
    break;
  }

  switch (variability_)
  {
  case fmi2VariabilityContinuous:
    variability = fmi2_variability_enu_continuous;
    break;
  case fmi2VariabilityConstant:
    variability = fmi2_variability_enu_constant;
    break;
  case fmi2VariabilityDiscrete:
    variability = fmi2_variability_enu_discrete;
    break;
  case fmi2VariabilityFixed:
    variability = fmi2_variability_enu_fixed;
    break;
  case fmi2VariabilityTunable:
    variability = fmi2_variability_enu_tunable;
    break;
  default:
    variability = fmi2_variability_enu_unknown;
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
      if (variability_ == fmi2VariabilityContinuous)
      {
        is_continuous_time_der = true;
      }
    }
  }
  //std::cout << "\n check variable name :  " << cref.c_str() << "==>" << vr << "==>" << "==>" << is_der << "==>" << state_index;


}

oms::Variable::~Variable()
{
}

oms_causality_enu_t oms::Variable::getCausality() const
{
  switch (causality)
  {
  case fmi2_causality_enu_input:
    return oms_causality_input;

  case fmi2_causality_enu_output:
    return oms_causality_output;

  case fmi2_causality_enu_parameter:
    return oms_causality_parameter;

  default:
    return oms_causality_undefined;
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
