/* Events: Sample, Zero Crossings, Relations, Discrete Changes */
#include "EventTest_model.h"
#if defined(__cplusplus)
extern "C" {
#endif

/* Initializes the raw time events of the simulation using the now
   calcualted parameters. */
void EventTest_function_initSample(DATA *data, threadData_t *threadData)
{
  long i=0;
}

const char *EventTest_zeroCrossingDescription(int i, int **out_EquationIndexes)
{
  static const char *res[] = {"height < 0.0"};
  static const int occurEqs0[] = {1,4};
  static const int *occurEqs[] = {occurEqs0};
  *out_EquationIndexes = (int*) occurEqs[i];
  return res[i];
}

/* forwarded equations */
extern void EventTest_eqFunction_5(DATA* data, threadData_t *threadData);

int EventTest_function_ZeroCrossingsEquations(DATA *data, threadData_t *threadData)
{
  TRACE_PUSH

  data->simulationInfo->callStatistics.functionZeroCrossingsEquations++;

  EventTest_eqFunction_5(data, threadData);
  
  TRACE_POP
  return 0;
}

int EventTest_function_ZeroCrossings(DATA *data, threadData_t *threadData, double *gout)
{
  TRACE_PUSH
  const int *equationIndexes = NULL;

  modelica_boolean tmp0;
  modelica_real tmp1;
  modelica_real tmp2;

#if !defined(OMC_MINIMAL_RUNTIME)
  if (measure_time_flag) rt_tick(SIM_TIMER_ZC);
#endif
  data->simulationInfo->callStatistics.functionZeroCrossings++;

  tmp1 = 1.0;
  tmp2 = 0.0;
  tmp0 = LessZC((data->localData[0]->realVars[data->simulationInfo->realVarsIndex[0]] /* height STATE(1) */), 0.0, tmp1, tmp2, data->simulationInfo->storedRelations[0]);
  gout[0] = (tmp0) ? 1 : -1;

#if !defined(OMC_MINIMAL_RUNTIME)
  if (measure_time_flag) rt_accumulate(SIM_TIMER_ZC);
#endif

  TRACE_POP
  return 0;
}

const char *EventTest_relationDescription(int i)
{
  const char *res[] = {"height < 0.0"};
  return res[i];
}

int EventTest_function_updateRelations(DATA *data, threadData_t *threadData, int evalforZeroCross)
{
  TRACE_PUSH
  const int *equationIndexes = NULL;

  modelica_boolean tmp3;
  modelica_real tmp4;
  modelica_real tmp5;
  
  if(evalforZeroCross) {
    tmp4 = 1.0;
    tmp5 = 0.0;
    tmp3 = LessZC((data->localData[0]->realVars[data->simulationInfo->realVarsIndex[0]] /* height STATE(1) */), 0.0, tmp4, tmp5, data->simulationInfo->storedRelations[0]);
    data->simulationInfo->relations[0] = tmp3;
  } else {
    data->simulationInfo->relations[0] = ((data->localData[0]->realVars[data->simulationInfo->realVarsIndex[0]] /* height STATE(1) */) < 0.0);
  }
  
  TRACE_POP
  return 0;
}

#if defined(__cplusplus)
}
#endif

