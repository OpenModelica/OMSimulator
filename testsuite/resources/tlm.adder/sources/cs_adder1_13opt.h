#if defined(__cplusplus)
  extern "C" {
#endif
  int cs_adder1_mayer(DATA* data, modelica_real** res, short*);
  int cs_adder1_lagrange(DATA* data, modelica_real** res, short *, short *);
  int cs_adder1_pickUpBoundsForInputsInOptimization(DATA* data, modelica_real* min, modelica_real* max, modelica_real*nominal, modelica_boolean *useNominal, char ** name, modelica_real * start, modelica_real * startTimeOpt);
  int cs_adder1_setInputData(DATA *data, const modelica_boolean file);
  int cs_adder1_getTimeGrid(DATA *data, modelica_integer * nsi, modelica_real**t);
#if defined(__cplusplus)
}
#endif