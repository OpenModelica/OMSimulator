/* Algebraic */
#include "cs_source1_model.h"

#ifdef __cplusplus
extern "C" {
#endif


/* forwarded equations */
extern void cs_source1_eqFunction_2(DATA* data, threadData_t *threadData);
extern void cs_source1_eqFunction_3(DATA* data, threadData_t *threadData);

static void functionAlg_system0(DATA *data, threadData_t *threadData)
{
  cs_source1_eqFunction_2(data, threadData);

  cs_source1_eqFunction_3(data, threadData);
}
/* for continuous time variables */
int cs_source1_functionAlgebraics(DATA *data, threadData_t *threadData)
{
  TRACE_PUSH
  
  data->simulationInfo->callStatistics.functionAlgebraics++;
  
  functionAlg_system0(data, threadData);

  cs_source1_function_savePreSynchronous(data, threadData);
  
  TRACE_POP
  return 0;
}

#ifdef __cplusplus
}
#endif
