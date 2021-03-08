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

#include "FMUInfo.h"

#include "Logging.h"
#include "OMSString.h"


oms::FMUInfo::FMUInfo(const std::string& path)
{
  this->author = nullptr;
  this->copyright = nullptr;
  this->description = nullptr;
  this->fmiKind = oms_fmi_kind_unknown;
  this->fmiVersion = nullptr;
  this->generationDateAndTime = nullptr;
  this->generationTool = nullptr;
  this->guid = nullptr;
  this->license = nullptr;
  this->modelName = nullptr;
  this->path = allocateAndCopyString(path);
  this->version = nullptr;
  this->canBeInstantiatedOnlyOncePerProcess = false;
  this->canGetAndSetFMUstate = false;
  this->canNotUseMemoryManagementFunctions = false;
  this->canSerializeFMUstate = false;
  this->completedIntegratorStepNotNeeded = false;
  this->needsExecutionTool = false;
  this->providesDirectionalDerivative = false;
  this->canInterpolateInputs = false;
  this->maxOutputDerivativeOrder = 0;
}

oms::FMUInfo::~FMUInfo()
{
  if (this->author) delete[] this->author;
  if (this->copyright) delete[] this->copyright;
  if (this->description) delete[] this->description;
  if (this->fmiVersion) delete[] this->fmiVersion;
  if (this->generationDateAndTime) delete[] this->generationDateAndTime;
  if (this->generationTool) delete[] this->generationTool;
  if (this->guid) delete[] this->guid;
  if (this->license) delete[] this->license;
  if (this->modelName) delete[] this->modelName;
  if (this->path) delete[] this->path;
  if (this->version) delete[] this->version;
}

void oms::FMUInfo::update(fmi_version_enu_t version, fmi2_import_t* fmu)
{
  fmi2_fmu_kind_enu_t fmuKind = fmi2_import_get_fmu_kind(fmu);
  if (fmi2_fmu_kind_me == fmuKind)
    this->fmiKind = oms_fmi_kind_me;
  else if (fmi2_fmu_kind_cs == fmuKind)
    this->fmiKind = oms_fmi_kind_cs;
  else if (fmi2_fmu_kind_me_and_cs == fmuKind)
    this->fmiKind = oms_fmi_kind_me_and_cs;

  this->author = allocateAndCopyString(fmi2_import_get_author(fmu));
  this->copyright = allocateAndCopyString(fmi2_import_get_copyright(fmu));
  this->description = allocateAndCopyString(fmi2_import_get_description(fmu));
  this->fmiVersion = allocateAndCopyString(fmi_version_to_string(version));
  this->generationDateAndTime = allocateAndCopyString(fmi2_import_get_generation_date_and_time(fmu));
  this->generationTool = allocateAndCopyString(fmi2_import_get_generation_tool(fmu));
  this->guid = allocateAndCopyString(fmi2_import_get_GUID(fmu));
  this->license = allocateAndCopyString(fmi2_import_get_license(fmu));
  this->modelName = allocateAndCopyString(fmi2_import_get_model_name(fmu));
  this->version = allocateAndCopyString(fmi2_import_get_model_version(fmu));

  if (oms_fmi_kind_cs == fmiKind || oms_fmi_kind_me_and_cs == fmiKind)
  {
    this->canBeInstantiatedOnlyOncePerProcess = fmi2_import_get_capability(fmu, fmi2_cs_canBeInstantiatedOnlyOncePerProcess) > 0;
    this->canGetAndSetFMUstate = fmi2_import_get_capability(fmu, fmi2_cs_canGetAndSetFMUstate) > 0;
    this->canNotUseMemoryManagementFunctions = fmi2_import_get_capability(fmu, fmi2_cs_canNotUseMemoryManagementFunctions) > 0;
    this->canSerializeFMUstate = fmi2_import_get_capability(fmu, fmi2_cs_canSerializeFMUstate) > 0;
    this->completedIntegratorStepNotNeeded = false;
    this->needsExecutionTool = fmi2_import_get_capability(fmu, fmi2_cs_needsExecutionTool) > 0;
    this->providesDirectionalDerivative = fmi2_import_get_capability(fmu, fmi2_cs_providesDirectionalDerivatives) > 0;
    this->canInterpolateInputs = fmi2_import_get_capability(fmu, fmi2_cs_canInterpolateInputs) > 0;
    this->maxOutputDerivativeOrder = fmi2_import_get_capability(fmu, fmi2_cs_maxOutputDerivativeOrder);
  }

  if (oms_fmi_kind_me == fmiKind || oms_fmi_kind_me_and_cs == fmiKind)
  {
    this->canBeInstantiatedOnlyOncePerProcess = fmi2_import_get_capability(fmu, fmi2_me_canBeInstantiatedOnlyOncePerProcess) > 0;
    this->canGetAndSetFMUstate = fmi2_import_get_capability(fmu, fmi2_me_canGetAndSetFMUstate) > 0;
    this->canNotUseMemoryManagementFunctions = fmi2_import_get_capability(fmu, fmi2_me_canNotUseMemoryManagementFunctions) > 0;
    this->canSerializeFMUstate = fmi2_import_get_capability(fmu, fmi2_me_canSerializeFMUstate) > 0;
    this->completedIntegratorStepNotNeeded = fmi2_import_get_capability(fmu, fmi2_me_completedIntegratorStepNotNeeded) > 0;
    this->needsExecutionTool = fmi2_import_get_capability(fmu, fmi2_me_needsExecutionTool) > 0;
    this->providesDirectionalDerivative = fmi2_import_get_capability(fmu, fmi2_me_providesDirectionalDerivatives) > 0;
  }
}
