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


oms::Variable::Variable(fmi2_import_variable_t* var)
  : der_index(0), state_index(0), is_state(false), is_der(false), cref(fmi2_import_get_variable_name(var)), index(fmi2_import_get_variable_original_order(var))
{
  // extract the attributes
  description = fmi2_import_get_variable_description(var) ? fmi2_import_get_variable_description(var) : "";
  trim(description);
  vr = fmi2_import_get_variable_vr(var);
  causality = fmi2_import_get_causality(var);
  variability = fmi2_import_get_variability(var);
  initialProperty = fmi2_import_get_initial(var);

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
