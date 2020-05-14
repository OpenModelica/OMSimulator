/* Events: Sample, Zero Crossings, Relations, Discrete Changes */
#include "cs_adder1_model.h"
#if defined(__cplusplus)
extern "C" {
#endif

/* Initializes the raw time events of the simulation using the now
   calcualted parameters. */
void cs_adder1_function_initSample(DATA *data, threadData_t *threadData)
{
  long i=0;
}

const char *cs_adder1_zeroCrossingDescription(int i, int **out_EquationIndexes)
{
  *out_EquationIndexes = NULL;
  return "empty";
}

/* forwarded equations */

int cs_adder1_function_ZeroCrossingsEquations(DATA *data, threadData_t *threadData)
{
  TRACE_PUSH

  data->simulationInfo->callStatistics.functionZeroCrossingsEquations++;

  
  TRACE_POP
  return 0;
}

int cs_adder1_function_ZeroCrossings(DATA *data, threadData_t *threadData, double *gout)
{
  TRACE_PUSH
  
  data->simulationInfo->callStatistics.functionZeroCrossings++;
  
  
  TRACE_POP
  return 0;
}

const char *cs_adder1_relationDescription(int i)
{
  return "empty";
}

int cs_adder1_function_updateRelations(DATA *data, threadData_t *threadData, int evalforZeroCross)
{
  TRACE_PUSH
  
  if(evalforZeroCross) {
  } else {
  }
  
  TRACE_POP
  return 0;
}

int cs_adder1_checkForDiscreteChanges(DATA *data, threadData_t *threadData)
{
  TRACE_PUSH
  int needToIterate = 0;

  infoStreamPrint(LOG_EVENTS_V, 1, "check for discrete changes at time=%.12g", data->localData[0]->timeValue);
  if (ACTIVE_STREAM(LOG_EVENTS_V)) messageClose(LOG_EVENTS_V);
  
  TRACE_POP
  return needToIterate;
}

#if defined(__cplusplus)
}
#endif

