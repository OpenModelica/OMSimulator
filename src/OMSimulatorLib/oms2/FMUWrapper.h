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

#ifndef _OMS2_FMU_WRAPPER_H_
#define _OMS2_FMU_WRAPPER_H_

#include "ComRef.h"
#include "FMISubModel.h"
#include "Variable.h"
#include "Option.h"

#include <map>
#include <string>
#include <vector>

#include <fmilib.h>

namespace oms2
{
  class FMUWrapper : public FMISubModel
  {
  public:
    static FMUWrapper* newSubModel(const ComRef& cref, const std::string& filename);

    oms_component_type_enu_t getType() const { return oms_component_fmu; }

    oms_status_enu_t setRealParameter(const std::string& var, double value);
    oms_status_enu_t getRealParameter(const std::string& var, double& value);
    const std::string& getFMUPath() const {return filename;}
    const std::map<std::string, oms2::Option<double>>& getRealParameters() const {return realParameters;}

  private:
    oms_status_enu_t setReal(const oms2::Variable& var, double realValue);
    oms_status_enu_t getReal(const oms2::Variable& var, double& realValue);

    oms2::Variable* getVar(const std::string& var);

  private:
    FMUWrapper(const ComRef& cref, const std::string& filename);
    ~FMUWrapper();

    std::string filename;
    std::vector<oms2::Variable> inputs;
    std::vector<oms2::Variable> outputs;
    std::vector<oms2::Variable> parameters;
    std::vector<oms2::Variable> allVariables;

    std::map<std::string, oms2::Option<double>> realParameters;

    std::string tempDir;
    jm_callbacks callbacks;
    fmi2_fmu_kind_enu_t fmuKind;
    fmi2_callback_functions_t callBackFunctions;
    fmi_import_context_t* context;
    fmi2_import_t* fmu;
    fmi2_event_info_t eventInfo;
  };
}

#endif
