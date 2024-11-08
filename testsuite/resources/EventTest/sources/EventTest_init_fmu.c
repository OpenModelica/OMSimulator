#include "simulation_data.h"

OMC_DISABLE_OPT

void EventTest_read_simulation_info(SIMULATION_INFO* simulationInfo)
{
  simulationInfo->startTime = 0.0;
  simulationInfo->stopTime = 1.0;
  simulationInfo->stepSize = 0.002;
  simulationInfo->tolerance = 1e-6;
  simulationInfo->solverMethod = "dassl";
  simulationInfo->outputFormat = "mat";
  simulationInfo->variableFilter = ".*";
  simulationInfo->OPENMODELICAHOME = "C:/OPENMODELICAGIT/OpenModelica/build";
}

void EventTest_read_input_fmu(MODEL_DATA* modelData)
{
  modelData->realVarsData[0].info.id = 1000;
  modelData->realVarsData[0].info.name = "height";
  modelData->realVarsData[0].info.comment = "";
  modelData->realVarsData[0].info.info.filename = "<interactive>";
  modelData->realVarsData[0].info.info.lineStart = 4;
  modelData->realVarsData[0].info.info.colStart = 3;
  modelData->realVarsData[0].info.info.lineEnd = 4;
  modelData->realVarsData[0].info.info.colEnd = 46;
  modelData->realVarsData[0].info.info.readonly = 0;
  modelData->realVarsData[0].attribute.unit = "";
  modelData->realVarsData[0].attribute.displayUnit = "";
  modelData->realVarsData[0].attribute.min = -DBL_MAX;
  modelData->realVarsData[0].attribute.max = DBL_MAX;
  modelData->realVarsData[0].attribute.fixed = 1;
  modelData->realVarsData[0].attribute.useNominal = 0;
  modelData->realVarsData[0].attribute.nominal = 1.0;
  modelData->realVarsData[0].attribute.start = 0.0;
  modelData->realVarsData[1].info.id = 1001;
  modelData->realVarsData[1].info.name = "der(height)";
  modelData->realVarsData[1].info.comment = "";
  modelData->realVarsData[1].info.info.filename = "<interactive>";
  modelData->realVarsData[1].info.info.lineStart = 4;
  modelData->realVarsData[1].info.info.colStart = 3;
  modelData->realVarsData[1].info.info.lineEnd = 4;
  modelData->realVarsData[1].info.info.colEnd = 46;
  modelData->realVarsData[1].info.info.readonly = 0;
  modelData->realVarsData[1].attribute.unit = "";
  modelData->realVarsData[1].attribute.displayUnit = "";
  modelData->realVarsData[1].attribute.min = -DBL_MAX;
  modelData->realVarsData[1].attribute.max = DBL_MAX;
  modelData->realVarsData[1].attribute.fixed = 0;
  modelData->realVarsData[1].attribute.useNominal = 0;
  modelData->realVarsData[1].attribute.nominal = 1.0;
  modelData->realVarsData[1].attribute.start = 0.0;
  modelData->realParameterData[0].info.id = 1002;
  modelData->realParameterData[0].info.name = "start_height";
  modelData->realParameterData[0].info.comment = "";
  modelData->realParameterData[0].info.info.filename = "<interactive>";
  modelData->realParameterData[0].info.info.lineStart = 3;
  modelData->realParameterData[0].info.info.colStart = 3;
  modelData->realParameterData[0].info.info.lineEnd = 3;
  modelData->realParameterData[0].info.info.colEnd = 36;
  modelData->realParameterData[0].info.info.readonly = 0;
  modelData->realParameterData[0].attribute.unit = "";
  modelData->realParameterData[0].attribute.displayUnit = "";
  modelData->realParameterData[0].attribute.min = -DBL_MAX;
  modelData->realParameterData[0].attribute.max = DBL_MAX;
  modelData->realParameterData[0].attribute.fixed = 1;
  modelData->realParameterData[0].attribute.useNominal = 0;
  modelData->realParameterData[0].attribute.nominal = 1.0;
  modelData->realParameterData[0].attribute.start = 2.0;
  modelData->booleanVarsData[0].info.id = 1003;
  modelData->booleanVarsData[0].info.name = "$whenCondition1";
  modelData->booleanVarsData[0].info.comment = "height < 0.0";
  modelData->booleanVarsData[0].info.info.filename = "<interactive>";
  modelData->booleanVarsData[0].info.info.lineStart = 8;
  modelData->booleanVarsData[0].info.info.colStart = 5;
  modelData->booleanVarsData[0].info.info.lineEnd = 8;
  modelData->booleanVarsData[0].info.info.colEnd = 33;
  modelData->booleanVarsData[0].info.info.readonly = 0;
  modelData->booleanVarsData[0].attribute.fixed = 1;
  modelData->booleanVarsData[0].attribute.start = 0;
}