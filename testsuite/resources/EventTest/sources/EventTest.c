/* Main Simulation File */

#if defined(__cplusplus)
extern "C" {
#endif

#include "EventTest_model.h"
#include "simulation/solver/events.h"



/* dummy VARINFO and FILEINFO */
const VAR_INFO dummyVAR_INFO = omc_dummyVarInfo;

int EventTest_input_function(DATA *data, threadData_t *threadData)
{
  TRACE_PUSH

  
  TRACE_POP
  return 0;
}

int EventTest_input_function_init(DATA *data, threadData_t *threadData)
{
  TRACE_PUSH

  
  TRACE_POP
  return 0;
}

int EventTest_input_function_updateStartValues(DATA *data, threadData_t *threadData)
{
  TRACE_PUSH

  
  TRACE_POP
  return 0;
}

int EventTest_inputNames(DATA *data, char ** names){
  TRACE_PUSH

  
  TRACE_POP
  return 0;
}

int EventTest_data_function(DATA *data, threadData_t *threadData)
{
  TRACE_PUSH

  TRACE_POP
  return 0;
}

int EventTest_dataReconciliationInputNames(DATA *data, char ** names){
  TRACE_PUSH

  
  TRACE_POP
  return 0;
}

int EventTest_dataReconciliationUnmeasuredVariables(DATA *data, char ** names)
{
  TRACE_PUSH

  
  TRACE_POP
  return 0;
}

int EventTest_output_function(DATA *data, threadData_t *threadData)
{
  TRACE_PUSH

  
  TRACE_POP
  return 0;
}

int EventTest_setc_function(DATA *data, threadData_t *threadData)
{
  TRACE_PUSH

  
  TRACE_POP
  return 0;
}

int EventTest_setb_function(DATA *data, threadData_t *threadData)
{
  TRACE_PUSH

  
  TRACE_POP
  return 0;
}


/*
equation index: 4
type: SIMPLE_ASSIGN
$whenCondition1 = height < 0.0
*/
void EventTest_eqFunction_4(DATA *data, threadData_t *threadData)
{
  TRACE_PUSH
  const int equationIndexes[2] = {1,4};
  modelica_boolean tmp0;
  modelica_real tmp1;
  modelica_real tmp2;
  tmp1 = 1.0;
  tmp2 = 0.0;
  relationhysteresis(data, &tmp0, (data->localData[0]->realVars[data->simulationInfo->realVarsIndex[0]] /* height STATE(1) */), 0.0, tmp1, tmp2, 0, Less, LessZC);
  (data->localData[0]->booleanVars[data->simulationInfo->booleanVarsIndex[0]] /* $whenCondition1 DISCRETE */) = tmp0;
  TRACE_POP
}
/*
equation index: 5
type: SIMPLE_ASSIGN
$DER.height = -1.0
*/
void EventTest_eqFunction_5(DATA *data, threadData_t *threadData)
{
  TRACE_PUSH
  const int equationIndexes[2] = {1,5};
  (data->localData[0]->realVars[data->simulationInfo->realVarsIndex[1]] /* der(height) STATE_DER */) = -1.0;
  TRACE_POP
}
/*
equation index: 6
type: WHEN

when {$whenCondition1} then
  reinit(height,  start_height);
end when;
*/
void EventTest_eqFunction_6(DATA *data, threadData_t *threadData)
{
  TRACE_PUSH
  const int equationIndexes[2] = {1,6};
  if(((data->localData[0]->booleanVars[data->simulationInfo->booleanVarsIndex[0]] /* $whenCondition1 DISCRETE */) && !(data->simulationInfo->booleanVarsPre[0] /* $whenCondition1 DISCRETE */) /* edge */))
  {
    (data->localData[0]->realVars[data->simulationInfo->realVarsIndex[0]] /* height STATE(1) */) = (data->simulationInfo->realParameter[0] /* start_height PARAM */);
    infoStreamPrint(LOG_EVENTS, 0, "reinit height = %g", (data->localData[0]->realVars[data->simulationInfo->realVarsIndex[0]] /* height STATE(1) */));
    data->simulationInfo->needToIterate = 1;
  }
  TRACE_POP
}

OMC_DISABLE_OPT
int EventTest_functionDAE(DATA *data, threadData_t *threadData)
{
  TRACE_PUSH
  int equationIndexes[1] = {0};
#if !defined(OMC_MINIMAL_RUNTIME)
  if (measure_time_flag) rt_tick(SIM_TIMER_DAE);
#endif

  data->simulationInfo->needToIterate = 0;
  data->simulationInfo->discreteCall = 1;
  EventTest_functionLocalKnownVars(data, threadData);
  EventTest_eqFunction_4(data, threadData);

  EventTest_eqFunction_5(data, threadData);

  EventTest_eqFunction_6(data, threadData);
  data->simulationInfo->discreteCall = 0;
  
#if !defined(OMC_MINIMAL_RUNTIME)
  if (measure_time_flag) rt_accumulate(SIM_TIMER_DAE);
#endif
  TRACE_POP
  return 0;
}


int EventTest_functionLocalKnownVars(DATA *data, threadData_t *threadData)
{
  TRACE_PUSH

  
  TRACE_POP
  return 0;
}

/* forwarded equations */
extern void EventTest_eqFunction_5(DATA* data, threadData_t *threadData);

static void functionODE_system0(DATA *data, threadData_t *threadData)
{
  int id;

  static void (*const eqFunctions[1])(DATA*, threadData_t*) = {
    EventTest_eqFunction_5
  };
  
  static const int eqIndices[1] = {
    5
  };
  
  for (id = 0; id < 1; id++) {
    eqFunctions[id](data, threadData);
    threadData->lastEquationSolved = eqIndices[id];
  }
}

int EventTest_functionODE(DATA *data, threadData_t *threadData)
{
  TRACE_PUSH
#if !defined(OMC_MINIMAL_RUNTIME)
  if (measure_time_flag) rt_tick(SIM_TIMER_FUNCTION_ODE);
#endif

  
  data->simulationInfo->callStatistics.functionODE++;
  
  EventTest_functionLocalKnownVars(data, threadData);
  functionODE_system0(data, threadData);

#if !defined(OMC_MINIMAL_RUNTIME)
  if (measure_time_flag) rt_accumulate(SIM_TIMER_FUNCTION_ODE);
#endif

  TRACE_POP
  return 0;
}

/* forward the main in the simulation runtime */
extern int _main_SimulationRuntime(int argc, char**argv, DATA *data, threadData_t *threadData);

#include "EventTest_12jac.h"
#include "EventTest_13opt.h"

struct OpenModelicaGeneratedFunctionCallbacks EventTest_callback = {
   NULL,    /* performSimulation */
   NULL,    /* performQSSSimulation */
   NULL,    /* updateContinuousSystem */
   EventTest_callExternalObjectDestructors,    /* callExternalObjectDestructors */
   NULL,    /* initialNonLinearSystem */
   NULL,    /* initialLinearSystem */
   NULL,    /* initialMixedSystem */
   #if !defined(OMC_NO_STATESELECTION)
   EventTest_initializeStateSets,
   #else
   NULL,
   #endif    /* initializeStateSets */
   EventTest_initializeDAEmodeData,
   EventTest_functionODE,
   EventTest_functionAlgebraics,
   EventTest_functionDAE,
   EventTest_functionLocalKnownVars,
   EventTest_input_function,
   EventTest_input_function_init,
   EventTest_input_function_updateStartValues,
   EventTest_data_function,
   EventTest_output_function,
   EventTest_setc_function,
   EventTest_setb_function,
   EventTest_function_storeDelayed,
   EventTest_function_storeSpatialDistribution,
   EventTest_function_initSpatialDistribution,
   EventTest_updateBoundVariableAttributes,
   EventTest_functionInitialEquations,
   1, /* useHomotopy - 0: local homotopy (equidistant lambda), 1: global homotopy (equidistant lambda), 2: new global homotopy approach (adaptive lambda), 3: new local homotopy approach (adaptive lambda)*/
   NULL,
   EventTest_functionRemovedInitialEquations,
   EventTest_updateBoundParameters,
   EventTest_checkForAsserts,
   EventTest_function_ZeroCrossingsEquations,
   EventTest_function_ZeroCrossings,
   EventTest_function_updateRelations,
   EventTest_zeroCrossingDescription,
   EventTest_relationDescription,
   EventTest_function_initSample,
   EventTest_INDEX_JAC_A,
   EventTest_INDEX_JAC_B,
   EventTest_INDEX_JAC_C,
   EventTest_INDEX_JAC_D,
   EventTest_INDEX_JAC_F,
   EventTest_INDEX_JAC_H,
   EventTest_initialAnalyticJacobianA,
   EventTest_initialAnalyticJacobianB,
   EventTest_initialAnalyticJacobianC,
   EventTest_initialAnalyticJacobianD,
   EventTest_initialAnalyticJacobianF,
   EventTest_initialAnalyticJacobianH,
   EventTest_functionJacA_column,
   EventTest_functionJacB_column,
   EventTest_functionJacC_column,
   EventTest_functionJacD_column,
   EventTest_functionJacF_column,
   EventTest_functionJacH_column,
   EventTest_linear_model_frame,
   EventTest_linear_model_datarecovery_frame,
   EventTest_mayer,
   EventTest_lagrange,
   EventTest_pickUpBoundsForInputsInOptimization,
   EventTest_setInputData,
   EventTest_getTimeGrid,
   EventTest_symbolicInlineSystem,
   EventTest_function_initSynchronous,
   EventTest_function_updateSynchronous,
   EventTest_function_equationsSynchronous,
   EventTest_inputNames,
   EventTest_dataReconciliationInputNames,
   EventTest_dataReconciliationUnmeasuredVariables,
   EventTest_read_simulation_info,
   EventTest_read_input_fmu,
   NULL,
   NULL,
   -1,
   NULL,
   NULL,
   -1

};

#define _OMC_LIT_RESOURCE_0_name_data "EventTest"
#define _OMC_LIT_RESOURCE_0_dir_data "."
static const MMC_DEFSTRINGLIT(_OMC_LIT_RESOURCE_0_name,9,_OMC_LIT_RESOURCE_0_name_data);
static const MMC_DEFSTRINGLIT(_OMC_LIT_RESOURCE_0_dir,1,_OMC_LIT_RESOURCE_0_dir_data);

static const MMC_DEFSTRUCTLIT(_OMC_LIT_RESOURCES,2,MMC_ARRAY_TAG) {MMC_REFSTRINGLIT(_OMC_LIT_RESOURCE_0_name), MMC_REFSTRINGLIT(_OMC_LIT_RESOURCE_0_dir)}};
void EventTest_setupDataStruc(DATA *data, threadData_t *threadData)
{
  assertStreamPrint(threadData,0!=data, "Error while initialize Data");
  threadData->localRoots[LOCAL_ROOT_SIMULATION_DATA] = data;
  data->callback = &EventTest_callback;
  OpenModelica_updateUriMapping(threadData, MMC_REFSTRUCTLIT(_OMC_LIT_RESOURCES));
  data->modelData->modelName = "EventTest";
  data->modelData->modelFilePrefix = "EventTest";
  data->modelData->resultFileName = NULL;
  data->modelData->modelDir = "";
  data->modelData->modelGUID = "{2e209521-b55c-45ae-980d-418d0e87300b}";
  data->modelData->initXMLData = NULL;
  data->modelData->modelDataXml.infoXMLData = NULL;
  GC_asprintf(&data->modelData->modelDataXml.fileName, "%s/EventTest_info.json", data->modelData->resourcesDir);
  data->modelData->runTestsuite = 0;
  data->modelData->nStates = 1;
  data->modelData->nVariablesRealArray = 2;
  data->modelData->nDiscreteReal = 0;
  data->modelData->nVariablesIntegerArray = 0;
  data->modelData->nVariablesBooleanArray = 1;
  data->modelData->nVariablesStringArray = 0;
  data->modelData->nParametersReal = 1;
  data->modelData->nParametersInteger = 0;
  data->modelData->nParametersBoolean = 0;
  data->modelData->nParametersString = 0;
  data->modelData->nInputVars = 0;
  data->modelData->nOutputVars = 0;
  data->modelData->nAliasReal = 0;
  data->modelData->nAliasInteger = 0;
  data->modelData->nAliasBoolean = 0;
  data->modelData->nAliasString = 0;
  data->modelData->nZeroCrossings = 1;
  data->modelData->nSamples = 0;
  data->modelData->nRelations = 1;
  data->modelData->nMathEvents = 0;
  data->modelData->nExtObjs = 0;
  data->modelData->modelDataXml.modelInfoXmlLength = 0;
  data->modelData->modelDataXml.nFunctions = 0;
  data->modelData->modelDataXml.nProfileBlocks = 0;
  data->modelData->modelDataXml.nEquations = 8;
  data->modelData->nMixedSystems = 0;
  data->modelData->nLinearSystems = 0;
  data->modelData->nNonLinearSystems = 0;
  data->modelData->nStateSets = 0;
  data->modelData->nJacobians = 6;
  data->modelData->nOptimizeConstraints = 0;
  data->modelData->nOptimizeFinalConstraints = 0;
  data->modelData->nDelayExpressions = 0;
  data->modelData->nBaseClocks = 0;
  data->modelData->nSpatialDistributions = 0;
  data->modelData->nSensitivityVars = 0;
  data->modelData->nSensitivityParamVars = 0;
  data->modelData->nSetcVars = 0;
  data->modelData->ndataReconVars = 0;
  data->modelData->nSetbVars = 0;
  data->modelData->nRelatedBoundaryConditions = 0;
  data->modelData->linearizationDumpLanguage = OMC_LINEARIZE_DUMP_LANGUAGE_MODELICA;
}

static int rml_execution_failed()
{
  fflush(NULL);
  fprintf(stderr, "Execution failed!\n");
  fflush(NULL);
  return 1;
}

