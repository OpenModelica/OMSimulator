/* Main Simulation File */
#include "cs_adder1_model.h"


/* dummy VARINFO and FILEINFO */
const FILE_INFO dummyFILE_INFO = omc_dummyFileInfo;
const VAR_INFO dummyVAR_INFO = omc_dummyVarInfo;
#if defined(__cplusplus)
extern "C" {
#endif

int cs_adder1_input_function(DATA *data, threadData_t *threadData)
{
  TRACE_PUSH

  data->localData[0]->realVars[0] /* x1 variable */ = data->simulationInfo->inputVars[0];
  data->localData[0]->realVars[1] /* x2 variable */ = data->simulationInfo->inputVars[1];
  
  TRACE_POP
  return 0;
}

int cs_adder1_input_function_init(DATA *data, threadData_t *threadData)
{
  TRACE_PUSH

  data->simulationInfo->inputVars[0] = data->modelData->realVarsData[0].attribute.start;
  data->simulationInfo->inputVars[1] = data->modelData->realVarsData[1].attribute.start;
  
  TRACE_POP
  return 0;
}

int cs_adder1_input_function_updateStartValues(DATA *data, threadData_t *threadData)
{
  TRACE_PUSH

  data->modelData->realVarsData[0].attribute.start = data->simulationInfo->inputVars[0];
  data->modelData->realVarsData[1].attribute.start = data->simulationInfo->inputVars[1];
  
  TRACE_POP
  return 0;
}

int cs_adder1_inputNames(DATA *data, char ** names){
  TRACE_PUSH

  names[0] = (char *) data->modelData->realVarsData[0].info.name;
  names[1] = (char *) data->modelData->realVarsData[1].info.name;
  
  TRACE_POP
  return 0;
}

int cs_adder1_output_function(DATA *data, threadData_t *threadData)
{
  TRACE_PUSH

  data->simulationInfo->outputVars[0] = data->localData[0]->realVars[2] /* y variable */;
  
  TRACE_POP
  return 0;
}


/*
 equation index: 2
 type: SIMPLE_ASSIGN
 y = x1 + x2
 */
void cs_adder1_eqFunction_2(DATA *data, threadData_t *threadData)
{
  TRACE_PUSH
  const int equationIndexes[2] = {1,2};
  data->localData[0]->realVars[2] /* y variable */ = data->localData[0]->realVars[0] /* x1 variable */ + data->localData[0]->realVars[1] /* x2 variable */;
  TRACE_POP
}


int cs_adder1_functionDAE(DATA *data, threadData_t *threadData)
{
  TRACE_PUSH
  int equationIndexes[1] = {0};
  
  data->simulationInfo->needToIterate = 0;
  data->simulationInfo->discreteCall = 1;
  cs_adder1_functionLocalKnownVars(data, threadData);
  cs_adder1_eqFunction_2(data, threadData);
  data->simulationInfo->discreteCall = 0;
  
  TRACE_POP
  return 0;
}


int cs_adder1_functionLocalKnownVars(DATA *data, threadData_t *threadData)
{
  TRACE_PUSH

  
  TRACE_POP
  return 0;
}


int cs_adder1_functionODE(DATA *data, threadData_t *threadData)
{
  TRACE_PUSH

  
  data->simulationInfo->callStatistics.functionODE++;
  
  cs_adder1_functionLocalKnownVars(data, threadData);
  /* no ODE systems */

  
  TRACE_POP
  return 0;
}

#ifdef FMU_EXPERIMENTAL
#endif
/* forward the main in the simulation runtime */
extern int _main_SimulationRuntime(int argc, char**argv, DATA *data, threadData_t *threadData);

#include "cs_adder1_12jac.h"
#include "cs_adder1_13opt.h"

struct OpenModelicaGeneratedFunctionCallbacks cs_adder1_callback = {
   NULL,
   NULL,
   NULL,
   cs_adder1_callExternalObjectDestructors,
   NULL,
   NULL,
   NULL,
   #if !defined(OMC_NO_STATESELECTION)
   cs_adder1_initializeStateSets,
   #else
   NULL,
   #endif
   cs_adder1_initializeDAEmodeData,
   cs_adder1_functionODE,
   cs_adder1_functionAlgebraics,
   cs_adder1_functionDAE,
   cs_adder1_functionLocalKnownVars,
   cs_adder1_input_function,
   cs_adder1_input_function_init,
   cs_adder1_input_function_updateStartValues,
   cs_adder1_output_function,
   cs_adder1_function_storeDelayed,
   cs_adder1_updateBoundVariableAttributes,
   cs_adder1_functionInitialEquations,
   cs_adder1_functionRemovedInitialEquations,
   cs_adder1_updateBoundParameters,
   cs_adder1_checkForAsserts,
   cs_adder1_function_ZeroCrossingsEquations,
   cs_adder1_function_ZeroCrossings,
   cs_adder1_function_updateRelations,
   cs_adder1_checkForDiscreteChanges,
   cs_adder1_zeroCrossingDescription,
   cs_adder1_relationDescription,
   cs_adder1_function_initSample,
   cs_adder1_INDEX_JAC_A,
   cs_adder1_INDEX_JAC_B,
   cs_adder1_INDEX_JAC_C,
   cs_adder1_INDEX_JAC_D,
   cs_adder1_initialAnalyticJacobianA,
   cs_adder1_initialAnalyticJacobianB,
   cs_adder1_initialAnalyticJacobianC,
   cs_adder1_initialAnalyticJacobianD,
   cs_adder1_functionJacA_column,
   cs_adder1_functionJacB_column,
   cs_adder1_functionJacC_column,
   cs_adder1_functionJacD_column,
   cs_adder1_linear_model_frame,
   cs_adder1_linear_model_datarecovery_frame,
   cs_adder1_mayer,
   cs_adder1_lagrange,
   cs_adder1_pickUpBoundsForInputsInOptimization,
   cs_adder1_setInputData,
   cs_adder1_getTimeGrid,
   cs_adder1_symbolicInlineSystem,
   cs_adder1_function_initSynchronous,
   cs_adder1_function_updateSynchronous,
   cs_adder1_function_equationsSynchronous,
   cs_adder1_read_input_fmu
   #ifdef FMU_EXPERIMENTAL
   ,cs_adder1_functionODE_Partial
   ,cs_adder1_functionFMIJacobian
   #endif
   ,cs_adder1_inputNames


};

void cs_adder1_setupDataStruc(DATA *data, threadData_t *threadData)
{
  assertStreamPrint(threadData,0!=data, "Error while initialize Data");
  data->callback = &cs_adder1_callback;
  data->modelData->modelName = "adder1";
  data->modelData->modelFilePrefix = "cs_adder1";
  data->modelData->resultFileName = NULL;
  data->modelData->modelDir = "";
  data->modelData->modelGUID = "{7202d81a-71e0-4d70-98ad-a8a8c83ea908}";
  data->modelData->initXMLData = NULL;
  data->modelData->modelDataXml.infoXMLData =
  #if defined(OMC_MINIMAL_METADATA)
    NULL;
  #else
  #include "cs_adder1_info.c"
  #endif
  ;
  
  data->modelData->nStates = 0;
  data->modelData->nVariablesReal = 3;
  data->modelData->nDiscreteReal = 0;
  data->modelData->nVariablesInteger = 0;
  data->modelData->nVariablesBoolean = 0;
  data->modelData->nVariablesString = 0;
  data->modelData->nParametersReal = 0;
  data->modelData->nParametersInteger = 0;
  data->modelData->nParametersBoolean = 0;
  data->modelData->nParametersString = 0;
  data->modelData->nInputVars = 2;
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
  data->modelData->modelDataXml.fileName = "cs_adder1_info.json";
  data->modelData->modelDataXml.modelInfoXmlLength = 0;
  data->modelData->modelDataXml.nFunctions = 0;
  data->modelData->modelDataXml.nProfileBlocks = 0;
  data->modelData->modelDataXml.nEquations = 3;
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

