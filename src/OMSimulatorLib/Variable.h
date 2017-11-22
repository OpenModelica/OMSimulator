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

#ifndef _OMS_VARIABLE_H_
#define _OMS_VARIABLE_H_

#include <fmilib.h>
#include <string>
#include <vector>

class FMUWrapper;

class Variable
{
public:
  Variable(fmi2_import_variable_t *var, FMUWrapper* fmuInstance);
  ~Variable();

  void markAsState() {is_state = true;}

  // causality attribute
  bool isParameter() const {return fmi2_causality_enu_parameter == causality;}
  bool isCalculatedParameter() const {return fmi2_causality_enu_calculated_parameter == causality;}
  bool isInput() const {return fmi2_causality_enu_input == causality;}
  bool isOutput() const {return fmi2_causality_enu_output == causality;}
  bool isLocal() const {return fmi2_causality_enu_local == causality;}
  bool isState() const {return is_state;}
  bool isIndependent() const {return fmi2_causality_enu_independent == causality;}

  // initial attribute
  bool isExact() const {return fmi2_initial_enu_exact == initialProperty;}
  bool isApprox() const {return fmi2_initial_enu_approx == initialProperty;}
  bool isCalculated() const {return fmi2_initial_enu_calculated == initialProperty;}

  bool isInitialUnknown() const {return (isOutput() && (isApprox() || isCalculated()))
                              || (isCalculatedParameter())
                              || (isState() && (isApprox() || isCalculated()));}

  const std::string& getName() const {return name;}
  const std::string& getFMUInstanceName() const {return fmuInstanceName;}
  FMUWrapper* getFMUInstance() const;
  fmi2_value_reference_t getValueReference() const {return vr;}
  fmi2_base_type_enu_t getBaseType() const {return baseType;}
  const std::string& getDescription() const {return description;}

  bool isTypeReal() const {return fmi2_base_type_real == baseType;}
  bool isTypeInteger() const {return fmi2_base_type_int == baseType;}
  bool isTypeBoolean() const {return fmi2_base_type_bool == baseType;}

  std::string getCausalityString() {return std::string(fmi2_causality_to_string(causality));}

protected:
  std::string name;
  std::string description;
  std::string fmuInstanceName;
  FMUWrapper* fmuInstance;
  fmi2_value_reference_t vr;
  fmi2_causality_enu_t causality;
  fmi2_initial_enu_t initialProperty;
  bool is_state;
  fmi2_base_type_enu_t baseType;

  friend bool operator==(const Variable& v1, const Variable& v2);
  friend bool operator!=(const Variable& v1, const Variable& v2);
};

bool operator==(const Variable& v1, const Variable& v2);
bool operator!=(const Variable& v1, const Variable& v2);

#endif
