/* Synchronous systems */
#include "cs_source1_model.h"
#if defined(__cplusplus)
extern "C" {
#endif

/* Initializes the clocks of model. */
void cs_source1_function_initSynchronous(DATA *data, threadData_t *threadData)
{
  TRACE_PUSH
  long i=0, j=0;
  TRACE_POP
}

/* Update the base clock. */
void cs_source1_function_updateSynchronous(DATA *data, threadData_t *threadData, long i)
{
  TRACE_PUSH
  modelica_boolean ret;
  switch (i) {
    default:
      throwStreamPrint(NULL, "Internal Error: unknown base partition %ld", i);
      break;
  }
  TRACE_POP
}



/*Clocked systems equations */
int cs_source1_function_equationsSynchronous(DATA *data, threadData_t *threadData, long i)
{
  TRACE_PUSH
  int ret;

  switch (i) {
    default:
      throwStreamPrint(NULL, "Internal Error: unknown sub partition %ld", i);
      ret = 1;
      break;
  }

  TRACE_POP
  return ret;
}

/* pre(%v%) = %v% */
void cs_source1_function_savePreSynchronous(DATA *data, threadData_t *threadData)
{
  TRACE_PUSH

  
  TRACE_POP
}

#if defined(__cplusplus)
}
#endif

