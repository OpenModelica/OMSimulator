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

void oms::FMUInfo::update(oms_component_enu_t componentType, fmiHandle* fmu)
{
  // Check the component type
  switch (componentType)
  {
    case oms_component_fmu: // FMI 2.0
      updateFMI2Info(fmu);
      break;
    case oms_component_fmu3: // FMI 3.0
      updateFMI3Info(fmu);
      break;
    case oms_component_dcp: // DCP
      updateDCPInfo();
      break;
    default: // Unsupported type
      logError("Unsupported component type for Variable constructor");
  }

}

void oms::FMUInfo::updateFMI2Info(fmiHandle* fmu)
{
  if (fmi2_getSupportsCoSimulation(fmu))
    this->fmiKind = oms_fmi_kind_cs;
  else if (fmi2_getSupportsModelExchange(fmu))
    this->fmiKind = oms_fmi_kind_me;
  else if (fmi2_getSupportsModelExchange(fmu) && fmi2_getSupportsCoSimulation(fmu))
    this->fmiKind = oms_fmi_kind_me_and_cs;

  this->author = allocateAndCopyString(fmi2_getAuthor(fmu));
  this->copyright = allocateAndCopyString(fmi2_getCopyright(fmu));
  this->description = allocateAndCopyString(fmi2_getModelDescription(fmu));
  this->fmiVersion = allocateAndCopyString("2.0");
  this->generationDateAndTime = allocateAndCopyString(fmi2_getGenerationDateAndTime(fmu));
  this->generationTool = allocateAndCopyString(fmi2_getGenerationTool(fmu));
  this->guid = allocateAndCopyString(fmi2_getGuid(fmu));

  this->license = allocateAndCopyString(fmi2_getLicense(fmu));
  this->modelName = allocateAndCopyString(fmi2_getModelName(fmu));
  //this->version = allocateAndCopyString(fmi2_getVersion(fmu));

  if (oms_fmi_kind_cs == fmiKind || oms_fmi_kind_me_and_cs == fmiKind)
  {
    this->canBeInstantiatedOnlyOncePerProcess = fmi2cs_getCanBeInstantiatedOnlyOncePerProcess(fmu) > 0;
    this->canGetAndSetFMUstate = fmi2cs_getCanGetAndSetFMUState(fmu) > 0;
    this->canNotUseMemoryManagementFunctions = fmi2cs_getCanNotUseMemoryManagementFunctions(fmu) > 0;
    this->canSerializeFMUstate = fmi2cs_getCanSerializeFMUState(fmu) > 0;
    this->completedIntegratorStepNotNeeded = false;
    this->needsExecutionTool = fmi2cs_getNeedsExecutionTool(fmu) > 0;
    this->providesDirectionalDerivative = fmi2cs_getProvidesDirectionalDerivative(fmu) > 0;
    this->canInterpolateInputs = fmi2cs_getCanInterpolateInputs(fmu) > 0;
    this->maxOutputDerivativeOrder = fmi2cs_getMaxOutputDerivativeOrder(fmu);
  }

  if (oms_fmi_kind_me == fmiKind || oms_fmi_kind_me_and_cs == fmiKind)
  {
    this->canBeInstantiatedOnlyOncePerProcess = fmi2me_getCanBeInstantiatedOnlyOncePerProcess(fmu) > 0;
    this->canGetAndSetFMUstate = fmi2me_getCanGetAndSetFMUState(fmu) > 0;
    this->canNotUseMemoryManagementFunctions = fmi2me_getCanNotUseMemoryManagementFunctions(fmu) > 0;
    this->canSerializeFMUstate = fmi2me_getCanSerializeFMUState(fmu) > 0;
    this->completedIntegratorStepNotNeeded = fmi2me_getCompletedIntegratorStepNotNeeded(fmu) > 0;
    this->needsExecutionTool = fmi2me_getNeedsExecutionTool(fmu) > 0;
    this->providesDirectionalDerivative = fmi2me_getProvidesDirectionalDerivative(fmu) > 0;
  }
}

void oms::FMUInfo::updateFMI3Info(fmiHandle* fmu)
{
  if (fmi3_supportsCoSimulation(fmu))
    this->fmiKind = oms_fmi_kind_cs;
  else if (fmi3_supportsModelExchange(fmu))
    this->fmiKind = oms_fmi_kind_me;
  else if (fmi3_supportsModelExchange(fmu) && fmi3_supportsCoSimulation(fmu))
    this->fmiKind = oms_fmi_kind_me_and_cs;

  this->author = allocateAndCopyString(fmi3_author(fmu));
  this->copyright = allocateAndCopyString(fmi3_copyright(fmu));
  this->description = allocateAndCopyString(fmi3_description(fmu));
  this->fmiVersion = allocateAndCopyString("3.0");
  this->generationDateAndTime = allocateAndCopyString(fmi3_generationDateAndTime(fmu));
  this->generationTool = allocateAndCopyString(fmi3_generationTool(fmu));
  this->guid = allocateAndCopyString(fmi3_instantiationToken(fmu));

  this->license = allocateAndCopyString(fmi3_license(fmu));
  this->modelName = allocateAndCopyString(fmi3_modelName(fmu));

  if (oms_fmi_kind_cs == fmiKind || oms_fmi_kind_me_and_cs == fmiKind)
  {

    this->canBeInstantiatedOnlyOncePerProcess = fmi3cs_getCanBeInstantiatedOnlyOncePerProcess(fmu) > 0;
    this->canGetAndSetFMUstate = fmi3cs_getCanGetAndSetFMUState(fmu) > 0;
    this->canSerializeFMUstate = fmi3cs_getCanSerializeFMUState(fmu) > 0;
    this->completedIntegratorStepNotNeeded = false;
    this->needsExecutionTool = fmi3cs_getNeedsExecutionTool(fmu) > 0;
    this->providesDirectionalDerivative = fmi3cs_getProvidesDirectionalDerivative(fmu) > 0;
    this->maxOutputDerivativeOrder = fmi3cs_getMaxOutputDerivativeOrder(fmu);
    // TODO handle the following FMI3 CS attributes
    // providesIntermediateUpdate
    // mightReturnEarlyFromDoStep
    // providesEvaluateDiscreteStates
    // recommendedIntermediateInputSmoothness
    // canHandleVariableCommunicationStepSize
    // canReturnEarlyAfterIntermediateUpdate
    // fixedInternalStepSize
    // hasEventMode
  }

  if (oms_fmi_kind_me == fmiKind || oms_fmi_kind_me_and_cs == fmiKind)
  {
    this->canBeInstantiatedOnlyOncePerProcess = fmi3me_getCanBeInstantiatedOnlyOncePerProcess(fmu) > 0;
    this->canGetAndSetFMUstate = fmi3me_getCanGetAndSetFMUState(fmu) > 0;
    this->canSerializeFMUstate = fmi3me_getCanSerializeFMUState(fmu) > 0;
    this->completedIntegratorStepNotNeeded = fmi3me_getNeedsCompletedIntegratorStep(fmu) > 0;
    this->needsExecutionTool = fmi3me_getNeedsExecutionTool(fmu) > 0;
    this->providesDirectionalDerivative = fmi3me_getProvidesDirectionalDerivative(fmu) > 0;
    // TODO handle the following FMI3 ME attributes
    //providesAdjointDerivatives
    //providesPerElementDependencies
    //providesEvaluateDiscreteStates
  }
}

void oms::FMUInfo::updateDCPInfo()
{
    //Not sure if we need anything for DCP?
}
