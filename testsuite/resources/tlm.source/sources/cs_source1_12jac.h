/* Jacobians */
static const REAL_ATTRIBUTE dummyREAL_ATTRIBUTE = omc_dummyRealAttribute;
/* Jacobian Variables */
#if defined(__cplusplus)
extern "C" {
#endif
  #define cs_source1_INDEX_JAC_A 3
  int cs_source1_functionJacA_column(void* data, threadData_t *threadData);
  int cs_source1_initialAnalyticJacobianA(void* data, threadData_t *threadData);
#if defined(__cplusplus)
}
#endif
/* A */

#if defined(__cplusplus)
extern "C" {
#endif
  #define cs_source1_INDEX_JAC_B 2
  int cs_source1_functionJacB_column(void* data, threadData_t *threadData);
  int cs_source1_initialAnalyticJacobianB(void* data, threadData_t *threadData);
#if defined(__cplusplus)
}
#endif
/* B */

#if defined(__cplusplus)
extern "C" {
#endif
  #define cs_source1_INDEX_JAC_C 1
  int cs_source1_functionJacC_column(void* data, threadData_t *threadData);
  int cs_source1_initialAnalyticJacobianC(void* data, threadData_t *threadData);
#if defined(__cplusplus)
}
#endif
/* C */

#if defined(__cplusplus)
extern "C" {
#endif
  #define cs_source1_INDEX_JAC_D 0
  int cs_source1_functionJacD_column(void* data, threadData_t *threadData);
  int cs_source1_initialAnalyticJacobianD(void* data, threadData_t *threadData);
#if defined(__cplusplus)
}
#endif
/* D */


