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

#include <cstring>

oms::FMUInfo::FMUInfo(const std::string& path, oms_fmi_kind_enu_t fmuKind)
{
  this->author = NULL;
  this->copyright = NULL;
  this->description = NULL;
  this->fmiKind = fmuKind;
  this->fmiVersion = NULL;
  this->generationDateAndTime = NULL;
  this->generationTool = NULL;
  this->guid = NULL;
  this->license = NULL;
  this->modelName = NULL;
  this->path = new char[path.size()+1]; strcpy(this->path, path.c_str());
  this->version = NULL;
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

oms_status_enu_t oms::FMUInfo::update(fmi_version_enu_t version, fmi2_import_t* fmu)
{
  std::string value;

  value = std::string(fmi2_import_get_author(fmu));
  this->author = new char[value.size()+1]; strcpy(this->author, value.c_str());

  value = std::string(fmi2_import_get_copyright(fmu));
  this->copyright = new char[value.size()+1]; strcpy(this->copyright, value.c_str());

  value = std::string(fmi2_import_get_description(fmu));
  this->description = new char[value.size()+1]; strcpy(this->description, value.c_str());

  value = std::string(fmi_version_to_string(version));
  this->fmiVersion = new char[value.size()+1]; strcpy(this->fmiVersion, value.c_str());

  value = std::string(fmi2_import_get_generation_date_and_time(fmu));
  this->generationDateAndTime = new char[value.size()+1]; strcpy(this->generationDateAndTime, value.c_str());

  value = std::string(fmi2_import_get_generation_tool(fmu));
  this->generationTool = new char[value.size()+1]; strcpy(this->generationTool, value.c_str());

  value = std::string(fmi2_import_get_GUID(fmu));
  this->guid = new char[value.size()+1]; strcpy(this->guid, value.c_str());

  value = std::string(fmi2_import_get_license(fmu));
  this->license = new char[value.size()+1]; strcpy(this->license, value.c_str());

  value = std::string(fmi2_import_get_model_name(fmu));
  this->modelName = new char[value.size()+1]; strcpy(this->modelName, value.c_str());

  value = std::string(fmi2_import_get_model_version(fmu));
  this->version = new char[value.size()+1]; strcpy(this->version, value.c_str());

  if (oms_fmi_kind_cs == fmiKind)
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
  else
  {
    this->canBeInstantiatedOnlyOncePerProcess = fmi2_import_get_capability(fmu, fmi2_me_canBeInstantiatedOnlyOncePerProcess) > 0;
    this->canGetAndSetFMUstate = fmi2_import_get_capability(fmu, fmi2_me_canGetAndSetFMUstate) > 0;
    this->canNotUseMemoryManagementFunctions = fmi2_import_get_capability(fmu, fmi2_me_canNotUseMemoryManagementFunctions) > 0;
    this->canSerializeFMUstate = fmi2_import_get_capability(fmu, fmi2_me_canSerializeFMUstate) > 0;
    this->completedIntegratorStepNotNeeded = fmi2_import_get_capability(fmu, fmi2_me_completedIntegratorStepNotNeeded) > 0;
    this->needsExecutionTool = fmi2_import_get_capability(fmu, fmi2_me_needsExecutionTool) > 0;
    this->providesDirectionalDerivative = fmi2_import_get_capability(fmu, fmi2_me_providesDirectionalDerivatives) > 0;
  }

  return oms_status_ok;
}
