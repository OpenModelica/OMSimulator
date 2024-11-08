/* Initialization */
#include "EventTest_model.h"
#include "EventTest_11mix.h"
#include "EventTest_12jac.h"
#if defined(__cplusplus)
extern "C" {
#endif

void EventTest_functionInitialEquations_0(DATA *data, threadData_t *threadData);
extern void EventTest_eqFunction_5(DATA *data, threadData_t *threadData);


/*
equation index: 2
type: SIMPLE_ASSIGN
height = start_height
*/
void EventTest_eqFunction_2(DATA *data, threadData_t *threadData)
{
  TRACE_PUSH
  const int equationIndexes[2] = {1,2};
  (data->localData[0]->realVars[data->simulationInfo->realVarsIndex[0]] /* height STATE(1) */) = (data->simulationInfo->realParameter[0] /* start_height PARAM */);
  TRACE_POP
}
extern void EventTest_eqFunction_4(DATA *data, threadData_t *threadData);

OMC_DISABLE_OPT
void EventTest_functionInitialEquations_0(DATA *data, threadData_t *threadData)
{
  TRACE_PUSH
  EventTest_eqFunction_5(data, threadData);
  EventTest_eqFunction_2(data, threadData);
  EventTest_eqFunction_4(data, threadData);
  TRACE_POP
}

int EventTest_functionInitialEquations(DATA *data, threadData_t *threadData)
{
  TRACE_PUSH

  data->simulationInfo->discreteCall = 1;
  EventTest_functionInitialEquations_0(data, threadData);
  data->simulationInfo->discreteCall = 0;
  
  TRACE_POP
  return 0;
}

/* No EventTest_functionInitialEquations_lambda0 function */

int EventTest_functionRemovedInitialEquations(DATA *data, threadData_t *threadData)
{
  TRACE_PUSH
  const int *equationIndexes = NULL;
  double res = 0.0;

  
  TRACE_POP
  return 0;
}


#if defined(__cplusplus)
}
#endif

