/* Main Simulation File */
#include "cs_source1_model.h"


/* dummy VARINFO and FILEINFO */
const FILE_INFO dummyFILE_INFO = omc_dummyFileInfo;
const VAR_INFO dummyVAR_INFO = omc_dummyVarInfo;
#if defined(__cplusplus)
extern "C" {
#endif

int cs_source1_input_function(DATA *data, threadData_t *threadData)
{
  TRACE_PUSH

  
  TRACE_POP
  return 0;
}

int cs_source1_input_function_init(DATA *data, threadData_t *threadData)
{
  TRACE_PUSH

  
  TRACE_POP
  return 0;
}

int cs_source1_input_function_updateStartValues(DATA *data, threadData_t *threadData)
{
  TRACE_PUSH

  
  TRACE_POP
  return 0;
}

int cs_source1_inputNames(DATA *data, char ** names){
  TRACE_PUSH

  
  TRACE_POP
  return 0;
}

int cs_source1_output_function(DATA *data, threadData_t *threadData)
{
  TRACE_PUSH

  data->simulationInfo->outputVars[0] = data->localData[0]->realVars[1] /* y variable */;
  
  TRACE_POP
  return 0;
}


/*
 equation index: 2
 type: SIMPLE_ASSIGN
 $cse1 = sin(omega * time)
 */
void cs_source1_eqFunction_2(DATA *data, threadData_t *threadData)
{
  TRACE_PUSH
  const int equationIndexes[2] = {1,2};
  data->localData[0]->realVars[0] /* $cse1 variable */ = sin((data->simulationInfo->realParameter[1]) * (data->localData[0]->timeValue));
  TRACE_POP
}
/*
 equation index: 3
 type: SIMPLE_ASSIGN
 y = A * $cse1
 */
void cs_source1_eqFunction_3(DATA *data, threadData_t *threadData)
{
  TRACE_PUSH
  const int equationIndexes[2] = {1,3};
  data->localData[0]->realVars[1] /* y variable */ = (data->simulationInfo->realParameter[0]) * (data->localData[0]->realVars[0] /* $cse1 variable */);
  TRACE_POP
}


int cs_source1_functionDAE(DATA *data, threadData_t *threadData)
{
  TRACE_PUSH
  int equationIndexes[1] = {0};
  
  data->simulationInfo->needToIterate = 0;
  data->simulationInfo->discreteCall = 1;
  cs_source1_functionLocalKnownVars(data, threadData);
  cs_source1_eqFunction_2(data, threadData);

  cs_source1_eqFunction_3(data, threadData);
  data->simulationInfo->discreteCall = 0;
  
  TRACE_POP
  return 0;
}


int cs_source1_functionLocalKnownVars(DATA *data, threadData_t *threadData)
{
  TRACE_PUSH

  
  TRACE_POP
  return 0;
}


int cs_source1_functionODE(DATA *data, threadData_t *threadData)
{
  TRACE_PUSH

  
  data->simulationInfo->callStatistics.functionODE++;
  
  cs_source1_functionLocalKnownVars(data, threadData);
  /* no ODE systems */

  
  TRACE_POP
  return 0;
}

#ifdef FMU_EXPERIMENTAL
#endif
/* forward the main in the simulation runtime */
extern int _main_SimulationRuntime(int argc, char**argv, DATA *data, threadData_t *threadData);

#include "cs_source1_12jac.h"
#include "cs_source1_13opt.h"

struct OpenModelicaGeneratedFunctionCallbacks cs_source1_callback = {
   NULL,
   NULL,
   NULL,
   cs_source1_callExternalObjectDestructors,
   NULL,
   NULL,
   NULL,
   #if !defined(OMC_NO_STATESELECTION)
   cs_source1_initializeStateSets,
   #else
   NULL,
   #endif
   cs_source1_initializeDAEmodeData,
   cs_source1_functionODE,
   cs_source1_functionAlgebraics,
   cs_source1_functionDAE,
   cs_source1_functionLocalKnownVars,
   cs_source1_input_function,
   cs_source1_input_function_init,
   cs_source1_input_function_updateStartValues,
   cs_source1_output_function,
   cs_source1_function_storeDelayed,
   cs_source1_updateBoundVariableAttributes,
   cs_source1_functionInitialEquations,
   cs_source1_functionRemovedInitialEquations,
   cs_source1_updateBoundParameters,
   cs_source1_checkForAsserts,
   cs_source1_function_ZeroCrossingsEquations,
   cs_source1_function_ZeroCrossings,
   cs_source1_function_updateRelations,
   cs_source1_checkForDiscreteChanges,
   cs_source1_zeroCrossingDescription,
   cs_source1_relationDescription,
   cs_source1_function_initSample,
   cs_source1_INDEX_JAC_A,
   cs_source1_INDEX_JAC_B,
   cs_source1_INDEX_JAC_C,
   cs_source1_INDEX_JAC_D,
   cs_source1_initialAnalyticJacobianA,
   cs_source1_initialAnalyticJacobianB,
   cs_source1_initialAnalyticJacobianC,
   cs_source1_initialAnalyticJacobianD,
   cs_source1_functionJacA_column,
   cs_source1_functionJacB_column,
   cs_source1_functionJacC_column,
   cs_source1_functionJacD_column,
   cs_source1_linear_model_frame,
   cs_source1_linear_model_datarecovery_frame,
   cs_source1_mayer,
   cs_source1_lagrange,
   cs_source1_pickUpBoundsForInputsInOptimization,
   cs_source1_setInputData,
   cs_source1_getTimeGrid,
   cs_source1_symbolicInlineSystem,
   cs_source1_function_initSynchronous,
   cs_source1_function_updateSynchronous,
   cs_source1_function_equationsSynchronous,
   cs_source1_read_input_fmu
   #ifdef FMU_EXPERIMENTAL
   ,cs_source1_functionODE_Partial
   ,cs_source1_functionFMIJacobian
   #endif
   ,cs_source1_inputNames


};

void cs_source1_setupDataStruc(DATA *data, threadData_t *threadData)
{
  assertStreamPrint(threadData,0!=data, "Error while initialize Data");
  data->callback = &cs_source1_callback;
  data->modelData->modelName = "source1";
  data->modelData->modelFilePrefix = "cs_source1";
  data->modelData->resultFileName = NULL;
  data->modelData->modelDir = "";
  data->modelData->modelGUID = "{12fd500b-81c8-46f3-8e51-3c5397a9d39a}";
  data->modelData->initXMLData = NULL;
  data->modelData->modelDataXml.infoXMLData =
  #if defined(OMC_MINIMAL_METADATA)
    NULL;
  #else
  #include "cs_source1_info.c"
  #endif
  ;
  
  data->modelData->nStates = 0;
  data->modelData->nVariablesReal = 2;
  data->modelData->nDiscreteReal = 0;
  data->modelData->nVariablesInteger = 0;
  data->modelData->nVariablesBoolean = 0;
  data->modelData->nVariablesString = 0;
  data->modelData->nParametersReal = 2;
  data->modelData->nParametersInteger = 0;
  data->modelData->nParametersBoolean = 0;
  data->modelData->nParametersString = 0;
  data->modelData->nInputVars = 0;
  data->modelData->nOutputVars = 1;
  
  data->modelData->nAliasReal = 0;
  data->modelData->nAliasInteger = 0;
  data->modelData->nAliasBoolean = 0;
  data->modelData->nAliasString = 0;
  
  data->modelData->nZeroCrossings = 0;
  data->modelData->nSamples = 0;
  data->modelData->nRelations = 0;
  data->modelData->nMathEvents = 0;
  data->modelData->nExtObjs = 0;
  data->modelData->modelDataXml.fileName = "cs_source1_info.json";
  data->modelData->modelDataXml.modelInfoXmlLength = 0;
  data->modelData->modelDataXml.nFunctions = 0;
  data->modelData->modelDataXml.nProfileBlocks = 0;
  data->modelData->modelDataXml.nEquations = 4;
  data->modelData->nMixedSystems = 0;
  data->modelData->nLinearSystems = 0;
  data->modelData->nNonLinearSystems = 0;
  data->modelData->nStateSets = 0;
  data->modelData->nJacobians = 4;
  data->modelData->nOptimizeConstraints = 0;
  data->modelData->nOptimizeFinalConstraints = 0;
  
  data->modelData->nDelayExpressions = 0;
  
  data->modelData->nClocks = 0;
  data->modelData->nSubClocks = 0;
  
  data->modelData->nSensitivityVars = 0;
  data->modelData->nSensitivityParamVars = 0;
}

#ifdef __cplusplus
}
#endif

static int rml_execution_failed()
{
  fflush(NULL);
  fprintf(stderr, "Execution failed!\n");
  fflush(NULL);
  return 1;
}

