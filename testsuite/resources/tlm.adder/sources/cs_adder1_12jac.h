/* Jacobians */
static const REAL_ATTRIBUTE dummyREAL_ATTRIBUTE = omc_dummyRealAttribute;
/* Jacobian Variables */
#if defined(__cplusplus)
extern "C" {
#endif
  #define cs_adder1_INDEX_JAC_A 3
  int cs_adder1_functionJacA_column(void* data, threadData_t *threadData);
  int cs_adder1_initialAnalyticJacobianA(void* data, threadData_t *threadData);
#if defined(__cplusplus)
}
#endif
/* A */

#if defined(__cplusplus)
extern "C" {
#endif
  #define cs_adder1_INDEX_JAC_B 2
  int cs_adder1_functionJacB_column(void* data, threadData_t *threadData);
  int cs_adder1_initialAnalyticJacobianB(void* data, threadData_t *threadData);
#if defined(__cplusplus)
}
#endif
/* B */

#if defined(__cplusplus)
extern "C" {
#endif
  #define cs_adder1_INDEX_JAC_C 1
  int cs_adder1_functionJacC_column(void* data, threadData_t *threadData);
  int cs_adder1_initialAnalyticJacobianC(void* data, threadData_t *threadData);
#if defined(__cplusplus)
}
#endif
/* C */

#if defined(__cplusplus)
extern "C" {
#endif
  #define cs_adder1_INDEX_JAC_D 0
  int cs_adder1_functionJacD_column(void* data, threadData_t *threadData);
  int cs_adder1_initialAnalyticJacobianD(void* data, threadData_t *threadData);
#if defined(__cplusplus)
}
#endif
/* D */
#define $Px1SeedD data->simulationInfo->analyticJacobians[0].seedVars[0]
#define $Px2SeedD data->simulationInfo->analyticJacobians[0].seedVars[1]


