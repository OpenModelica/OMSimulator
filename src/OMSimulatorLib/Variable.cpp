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
#include <iostream>
#include "dcp/xml/DcpSlaveDescriptionElements.hpp"

oms::Variable::Variable(fmiHandle* fmi4c, int index_, oms_component_enu_t componentType)
  : der_index(0), state_index(0), is_state(false), is_der(false), is_continuous_time_state(false), is_continuous_time_der(false), index(index_), fmi2(false), fmi3(false), componentType(componentType)
{

  // Check the component type
  switch (componentType)
  {
    case oms_component_fmu: // FMI 2.0
      fmi2 = true;
      configureFMI2Variable(fmi4c, index);
      break;

    case oms_component_fmu3: // FMI 3.0
      fmi3 = true;
      configureFMI3Variable(fmi4c, index);
      break;

    default: // Unsupported type
      logError("Unsupported component type for Variable constructor");
  }
}

oms::Variable::Variable(SlaveDescription_t *desc, int index)
{
    configureDCPVariable(desc, index);
}


void oms::Variable::configureFMI2Variable(fmiHandle* fmi4c, int index_)
{
  // extract the attributes
  fmi2VariableHandle *var = fmi2_getVariableByIndex(fmi4c, index_+1);
  cref = fmi2_getVariableName(var);
  description = fmi2_getVariableDescription(var) ? fmi2_getVariableDescription(var) : "";
  trim(description);
  fmi2Vr = fmi2_getVariableValueReference(var);
  fmi2Causality_ = fmi2_getVariableCausality(var);
  fmi2Variability_ = fmi2_getVariableVariability(var);
  // TODO implement the initial attribute table in fmi4c according to FMI specification
  fmi2InitialProperty = fmi2_getVariableInitial(var);

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
      logWarning("Unknown fmi base type: " + fmi2_getVariableDataType(var));
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
      if (fmi2Variability_ == fmi2VariabilityContinuous)
      {
        is_continuous_time_der = true;
      }
    }
  }
}

void oms::Variable::configureFMI3Variable(fmiHandle* fmi4c, int index_)
{
  // extract the attributes
  fmi3VariableHandle *var = fmi3_getVariableByIndex(fmi4c, index_+1);
  cref = fmi3_getVariableName(var);
  description = fmi3_getVariableDescription(var) ? fmi3_getVariableDescription(var) : "";
  trim(description);
  fmi3Vr = fmi3_getVariableValueReference(var);
  fmi3Causality_ = fmi3_getVariableCausality(var);
  fmi3Variability_ = fmi3_getVariableVariability(var);
  // TODO implement the initial attribute table in fmi4c according to FMI specification
  fmi3InitialProperty = fmi3_getVariableInitial(var);

  switch (fmi3_getVariableDataType(var))
  {
    case fmi3DataTypeFloat64:
      type = oms_signal_type_real;
      numericType = oms_signal_numeric_type_FLOAT64;
      break;
    case fmi3DataTypeFloat32:
      type = oms_signal_type_real;
      numericType = oms_signal_numeric_type_FLOAT32;
      break;
    case fmi3DataTypeInt64:
      type = oms_signal_type_integer;
      numericType = oms_signal_numeric_type_INT64;
      break;
    case fmi3DataTypeInt32:
      type = oms_signal_type_integer;
      numericType  = oms_signal_numeric_type_INT32;
      break;
    case fmi3DataTypeInt16:
      type = oms_signal_type_integer;
      numericType  = oms_signal_numeric_type_INT16;
      break;
    case fmi3DataTypeInt8:
      type = oms_signal_type_integer;
      numericType  = oms_signal_numeric_type_INT8;
      break;
    case fmi3DataTypeUInt64:
      type = oms_signal_type_integer;
      numericType  = oms_signal_numeric_type_UINT64;
      break;
    case fmi3DataTypeUInt32:
      type = oms_signal_type_integer;
      numericType  = oms_signal_numeric_type_UINT32;
      break;
    case fmi3DataTypeUInt16:
      type = oms_signal_type_integer;
      numericType  = oms_signal_numeric_type_UINT16;
      break;
    case fmi3DataTypeUInt8:
      type = oms_signal_type_integer;
      numericType  = oms_signal_numeric_type_UINT8;
      break;
    case fmi3DataTypeBoolean:
      type = oms_signal_type_boolean;
      break;
    case fmi3DataTypeString:
      type = oms_signal_type_string;
      break;
    case fmi3DataTypeEnumeration:
      type = oms_signal_type_enum;
      numericType = oms_signal_numeric_type_INT64;
      break;
    default:
      logWarning("Unknown FMI3 base type for var : " + std::string(cref));
      type = oms_signal_type_real;
      break;
  }

  // mark derivatives
  if (oms_signal_type_real == type)
  {
    int derivative_index = fmi3_getVariableDerivativeIndex(var);
    if (derivative_index != 0)
    {
      is_der = true;
      state_index = derivative_index;
      if (fmi3Variability_ == fmi3VariabilityContinuous)
      {
        is_continuous_time_der = true;
      }
    }
  }
}

void oms::Variable::configureDCPVariable(SlaveDescription_t *desc, int index)
{
    this->index = index;

    Variable_t *var = &desc->Variables[index];

    cref = var->name;
    if(var->description != nullptr) {
        description = var->description->data();
        trim(description);
    }
    else {
        description = "";
    }

    dcpVr = var->valueReference;

    if(var->Input != nullptr) {
        dcpCausality = dcpCausalityInput;
        if(var->Input->Float64 != nullptr) {
            type = oms_signal_type_real;
            numericType = oms_signal_numeric_type_FLOAT64;
        }
        else if(var->Input->Float32 != nullptr) {
            type = oms_signal_type_real;
            numericType = oms_signal_numeric_type_FLOAT32;
        }
        else if(var->Input->Int64!= nullptr) {
            type = oms_signal_type_integer;
            numericType = oms_signal_numeric_type_INT64;
        }
        else if(var->Input->Int32!= nullptr) {
            type = oms_signal_type_integer;
            numericType  = oms_signal_numeric_type_INT32;
        }
        else if(var->Input->Int16!= nullptr) {
            type = oms_signal_type_integer;
            numericType  = oms_signal_numeric_type_INT16;
        }
        else if(var->Input->Int8!= nullptr) {
            type = oms_signal_type_integer;
            numericType  = oms_signal_numeric_type_INT8;
        }
        else if(var->Input->Uint64!= nullptr) {
            type = oms_signal_type_integer;
            numericType  = oms_signal_numeric_type_UINT64;
        }
        else if(var->Input->Uint32!= nullptr) {
            type = oms_signal_type_integer;
            numericType  = oms_signal_numeric_type_UINT32;
        }
        else if(var->Input->Uint16!= nullptr) {
            type = oms_signal_type_integer;
            numericType  = oms_signal_numeric_type_UINT16;
        }
        else if(var->Input->Uint8!= nullptr) {
            type = oms_signal_type_integer;
            numericType  = oms_signal_numeric_type_UINT8;
        }
        else if(var->Input->String!= nullptr) {
            type = oms_signal_type_string;
        }
        else if(var->Input->Binary!= nullptr) {
            // TODO: Support binary variables
        }
    }
    else if(var->Output != nullptr) {
        dcpCausality = dcpCausalityOutput;
        if(var->Output->Float64 != nullptr) {
            type = oms_signal_type_real;
            numericType = oms_signal_numeric_type_FLOAT64;
        }
        else if(var->Output->Float32 != nullptr) {
            type = oms_signal_type_real;
            numericType = oms_signal_numeric_type_FLOAT32;
        }
        else if(var->Output->Int64!= nullptr) {
            type = oms_signal_type_integer;
            numericType = oms_signal_numeric_type_INT64;
        }
        else if(var->Output->Int32!= nullptr) {
            type = oms_signal_type_integer;
            numericType  = oms_signal_numeric_type_INT32;
        }
        else if(var->Output->Int16!= nullptr) {
            type = oms_signal_type_integer;
            numericType  = oms_signal_numeric_type_INT16;
        }
        else if(var->Output->Int8!= nullptr) {
            type = oms_signal_type_integer;
            numericType  = oms_signal_numeric_type_INT8;
        }
        else if(var->Output->Uint64!= nullptr) {
            type = oms_signal_type_integer;
            numericType  = oms_signal_numeric_type_UINT64;
        }
        else if(var->Output->Uint32!= nullptr) {
            type = oms_signal_type_integer;
            numericType  = oms_signal_numeric_type_UINT32;
        }
        else if(var->Output->Uint16!= nullptr) {
            type = oms_signal_type_integer;
            numericType  = oms_signal_numeric_type_UINT16;
        }
        else if(var->Output->Uint8!= nullptr) {
            type = oms_signal_type_integer;
            numericType  = oms_signal_numeric_type_UINT8;
        }
        else if(var->Output->String!= nullptr) {
            type = oms_signal_type_string;
        }
        else if(var->Output->Binary!= nullptr) {
            // TODO: Support binary variables
        }
    }
    else if(var->Parameter != nullptr) {
        dcpCausality = dcpCausalityParameter;
        if(var->Parameter->Float64 != nullptr) {
            type = oms_signal_type_real;
            numericType = oms_signal_numeric_type_FLOAT64;
        }
        else if(var->Parameter->Float32 != nullptr) {
            type = oms_signal_type_real;
            numericType = oms_signal_numeric_type_FLOAT32;
        }
        else if(var->Parameter->Int64!= nullptr) {
            type = oms_signal_type_integer;
            numericType = oms_signal_numeric_type_INT64;
        }
        else if(var->Parameter->Int32!= nullptr) {
            type = oms_signal_type_integer;
            numericType  = oms_signal_numeric_type_INT32;
        }
        else if(var->Parameter->Int16!= nullptr) {
            type = oms_signal_type_integer;
            numericType  = oms_signal_numeric_type_INT16;
        }
        else if(var->Parameter->Int8!= nullptr) {
            type = oms_signal_type_integer;
            numericType  = oms_signal_numeric_type_INT8;
        }
        else if(var->Parameter->Uint64!= nullptr) {
            type = oms_signal_type_integer;
            numericType  = oms_signal_numeric_type_UINT64;
        }
        else if(var->Parameter->Uint32!= nullptr) {
            type = oms_signal_type_integer;
            numericType  = oms_signal_numeric_type_UINT32;
        }
        else if(var->Parameter->Uint16!= nullptr) {
            type = oms_signal_type_integer;
            numericType  = oms_signal_numeric_type_UINT16;
        }
        else if(var->Parameter->Uint8!= nullptr) {
            type = oms_signal_type_integer;
            numericType  = oms_signal_numeric_type_UINT8;
        }
        else if(var->Parameter->String!= nullptr) {
            type = oms_signal_type_string;
        }
        else if(var->Parameter->Binary!= nullptr) {
            // TODO: Support binary variables
        }
    }
    else if(var->StructuralParameter != nullptr) {
        dcpCausality = dcpCausalityStructuralParameter;
        if(var->StructuralParameter->Uint64!= nullptr) {
            dcpDataType = dcpUInt64;
        }
        else if(var->StructuralParameter->Uint32!= nullptr) {
            dcpDataType = dcpUInt32;
        }
        else if(var->StructuralParameter->Uint16!= nullptr) {
            dcpDataType = dcpUInt16;
        }
        else if(var->StructuralParameter->Uint8!= nullptr) {
            dcpDataType = dcpUInt8;
        }
    }
    dcpVariability = var->variability;
}

oms::Variable::~Variable()
{
}

oms_causality_enu_t oms::Variable::getCausality() const
{
  if (isFmi2())
  {
    // FMI2
    switch (fmi2Causality_)
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
  else
  {
    // FMI3
    switch (fmi3Causality_)
    {
    case fmi3CausalityInput:
      return oms_causality_input;

    case fmi3CausalityOutput:
      return oms_causality_output;

    case fmi3CausalityParameter:
      return oms_causality_parameter;

    case fmi3CausalityCalculatedParameter:
      return oms_causality_calculatedParameter;

    default:
      return oms_causality_undefined;
    }
  }
}

std::string oms::Variable::getCausalityString() const
{
  if (isFmi2())
  {
    // FMI2
    switch (fmi2Causality_)
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
  else
  {
    // FMI3
    switch (fmi3Causality_)
    {
    case fmi3CausalityInput:
      return "input";
    case fmi3CausalityOutput:
      return "output";
    case fmi3CausalityParameter:
      return "parameter";
    case fmi3CausalityCalculatedParameter:
      return "calculatedParameter";
    case fmi3CausalityIndependent:
      return "independent";
    case fmi3CausalityLocal:
      return "local";
    default:
      return "undefined";
    }
  }
}

bool oms::operator==(const oms::Variable& v1, const oms::Variable& v2)
{
  return v1.cref == v2.cref &&
        ((v1.isFmi2() && v2.isFmi2() && v1.fmi2Vr == v2.fmi2Vr) ||
            (v1.isFmi3() && v2.isFmi3() && v1.fmi3Vr == v2.fmi3Vr));
}

bool oms::operator!=(const oms::Variable& v1, const oms::Variable& v2)
{
  return !(v1 == v2);
}
