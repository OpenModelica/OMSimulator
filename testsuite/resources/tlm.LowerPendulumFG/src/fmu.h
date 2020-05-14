#ifndef TEST_FMU_1_H
#define TEST_FMU_1_H

#ifdef __cplusplus
extern "C" {
#endif

void    setReal(int idx, double value);
void    setRealDerivative(int idx, double value);
void    setInteger(int idx, int value);

double  getReal(int idx);
int     getInteger(int idx);

int initializeSolvers();
int takeStep(double t1);
void printDiagnostics();

#ifdef __cplusplus
}
#endif



#endif // TEST_FMU_1_H

