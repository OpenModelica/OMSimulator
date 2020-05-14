/* Initialization */
#include "cs_source1_model.h"
#include "cs_source1_11mix.h"
#include "cs_source1_12jac.h"
#if defined(__cplusplus)
extern "C" {
#endif

void cs_source1_functionInitialEquations_0(DATA *data, threadData_t *threadData);


/*
 equation index: 1
 type: SIMPLE_ASSIGN
 y = A * sin(omega * time)
 */
void cs_source1_eqFunction_1(DATA *data, threadData_t *threadData)
{
  TRACE_PUSH
  const int equationIndexes[2] = {1,1};
  data->localData[0]->realVars[1] /* y variable */ = (data->simulationInfo->realParameter[0]) * (sin((data->simulationInfo->realParameter[1]) * (data->localData[0]->timeValue)));
  TRACE_POP
}
void cs_source1_functionInitialEquations_0(DATA *data, threadData_t *threadData)
{
  TRACE_PUSH
  cs_source1_eqFunction_1(data, threadData);
  TRACE_POP
}


int cs_source1_functionInitialEquations(DATA *data, threadData_t *threadData)
{
  TRACE_PUSH

  data->simulationInfo->discreteCall = 1;
  cs_source1_functionInitialEquations_0(data, threadData);
  data->simulationInfo->discreteCall = 0;
  
  TRACE_POP
  return 0;
}
int cs_source1_functionRemovedInitialEquations(DATA *data, threadData_t *threadData)
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

