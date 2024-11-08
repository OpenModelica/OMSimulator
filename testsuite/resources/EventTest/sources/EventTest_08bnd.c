/* update bound parameters and variable attributes (start, nominal, min, max) */
#include "EventTest_model.h"
#if defined(__cplusplus)
extern "C" {
#endif


/*
equation index: 7
type: SIMPLE_ASSIGN
$START.height = start_height
*/
static void EventTest_eqFunction_7(DATA *data, threadData_t *threadData)
{
  TRACE_PUSH
  const int equationIndexes[2] = {1,7};
  (data->modelData->realVarsData[0] /* height STATE(1) */).attribute .start = (data->simulationInfo->realParameter[0] /* start_height PARAM */);
    (data->localData[0]->realVars[data->simulationInfo->realVarsIndex[0]] /* height STATE(1) */) = (data->modelData->realVarsData[0] /* height STATE(1) */).attribute .start;
    infoStreamPrint(LOG_INIT_V, 0, "updated start value: %s(start=%g)", data->modelData->realVarsData[0].info /* height */.name, (modelica_real) (data->localData[0]->realVars[data->simulationInfo->realVarsIndex[0]] /* height STATE(1) */));
  TRACE_POP
}
OMC_DISABLE_OPT
int EventTest_updateBoundVariableAttributes(DATA *data, threadData_t *threadData)
{
  TRACE_PUSH
  /* min ******************************************************** */
  infoStreamPrint(LOG_INIT, 1, "updating min-values");
  if (ACTIVE_STREAM(LOG_INIT)) messageClose(LOG_INIT);
  
  /* max ******************************************************** */
  infoStreamPrint(LOG_INIT, 1, "updating max-values");
  if (ACTIVE_STREAM(LOG_INIT)) messageClose(LOG_INIT);
  
  /* nominal **************************************************** */
  infoStreamPrint(LOG_INIT, 1, "updating nominal-values");
  if (ACTIVE_STREAM(LOG_INIT)) messageClose(LOG_INIT);
  
  /* start ****************************************************** */
  infoStreamPrint(LOG_INIT, 1, "updating primary start-values");
  EventTest_eqFunction_7(data, threadData);
  if (ACTIVE_STREAM(LOG_INIT)) messageClose(LOG_INIT);
  
  TRACE_POP
  return 0;
}

OMC_DISABLE_OPT
int EventTest_updateBoundParameters(DATA *data, threadData_t *threadData)
{
  TRACE_PUSH
  TRACE_POP
  return 0;
}

#if defined(__cplusplus)
}
#endif

