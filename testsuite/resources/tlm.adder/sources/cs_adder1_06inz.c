/* Initialization */
#include "cs_adder1_model.h"
#include "cs_adder1_11mix.h"
#include "cs_adder1_12jac.h"
#if defined(__cplusplus)
extern "C" {
#endif

void cs_adder1_functionInitialEquations_0(DATA *data, threadData_t *threadData);


/*
 equation index: 1
 type: SIMPLE_ASSIGN
 y = x1 + x2
 */
void cs_adder1_eqFunction_1(DATA *data, threadData_t *threadData)
{
  TRACE_PUSH
  const int equationIndexes[2] = {1,1};
  data->localData[0]->realVars[2] /* y variable */ = data->localData[0]->realVars[0] /* x1 variable */ + data->localData[0]->realVars[1] /* x2 variable */;
  TRACE_POP
}
void cs_adder1_functionInitialEquations_0(DATA *data, threadData_t *threadData)
{
  TRACE_PUSH
  cs_adder1_eqFunction_1(data, threadData);
  TRACE_POP
}


int cs_adder1_functionInitialEquations(DATA *data, threadData_t *threadData)
{
  TRACE_PUSH

  data->simulationInfo->discreteCall = 1;
  cs_adder1_functionInitialEquations_0(data, threadData);
  data->simulationInfo->discreteCall = 0;
  
  TRACE_POP
  return 0;
}
int cs_adder1_functionRemovedInitialEquations(DATA *data, threadData_t *threadData)
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

