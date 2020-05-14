#ifndef TEST_FMU_1_H
#define TEST_FMU_1_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef OPENCPS_CALLBACKS
typedef void (*fmi2GetTLMEffort_ft)(void* env, unsigned int vr, size_t dimensions, const double time,
                                                const double velocity[], double force[]);
typedef void (*fmi2SetTLMFlow_ft)(void* env, unsigned int vr, size_t dimensions, const double time,
                                               const double velocity[]);
#endif

void    setReal(int idx, double value);
void    setRealDerivative(int idx, double value);
void    setInteger(int idx, int value);

double  getReal(int idx);
int     getInteger(int idx);


int initializeSolvers();
int takeStep(double t1);
void printDiagnostics();

#ifdef OPENCPS_CALLBACKS
void initialize(void *env, fmi2GetTLMEffort_ft, fmi2SetTLMFlow_ft);
/*
void setInterpolatedRealCallback(int vr, fmi2GetTLMEffort_ft cb);
void setTimeStampedRealCallback(int vr, fmi2SetTLMFlow_ft cb);
*/
#endif

#ifdef __cplusplus
}
#endif



#endif // TEST_FMU_1_H

