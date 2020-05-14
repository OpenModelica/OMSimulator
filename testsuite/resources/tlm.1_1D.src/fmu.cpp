// C++ includes
#include <iostream>
#include <string>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <vector>
#include <fstream>
#include <map>

#define Ith(v,i)    NV_Ith_S(v,i-1)       /* Ith numbers components 1..NEQ */
#define IJth(A,i,j) DENSE_ELEM(A,i-1,j-1) /* IJth numbers rows,cols 1..NEQ */

#include "fmu.h"

#include "cvode/cvode.h"             /* prototypes for CVODE fcts., consts. */
#include "nvector/nvector_serial.h"  /* serial N_Vector types, fcts., macros */
#include "cvode/cvode_dense.h"       /* prototype for CVDense */
#include "sundials/sundials_dense.h" /* definitions DlsMat DENSE_ELEM */
#include "sundials/sundials_types.h" /* definition of type realtype */

#include "ida/ida.h"
#include "ida/ida_dense.h"
#include "nvector/nvector_serial.h"
#include "sundials/sundials_math.h"
#include "sundials/sundials_types.h"

using namespace std;

enum solver_t { ExplicitEuler, RungeKutta, CVODE, IDA };


static solver_t solver = IDA;
static double reltol = 1e-4;    //Relative tolerance
static double tstart = 0;       //Start time
static double hmax = 1.5e-4;      //Max step-size
static double hcur = hmax;
static double tsolver;          //Solver inernal time
static double tcur;             //Current time
static size_t n_states = 4;     //Number of states
static void *mem;               //Solver memory pointer
static int flag;                //Solver return flag

static N_Vector y, yp, abstol;

static double M1 = 0.005;
static double B1 = 0.2;
static double K1 = 90;
static double M2 = 0.005;
static double B2 = 0.2;
static double K2 = 140;
static double F = 0;    //Initial force = zero

//These variables are used for interpolating forces
static double C = 0;    //Wave variable
static double Zc = 1;   //Impedance
static double dC = 0;   //Derivative of force, used for interpolation

static std::vector<double> cData(10,0);
static std::vector<double> tData(10,0);

static int interpolationMethod = 2;

#ifdef OPENCPS_CALLBACKS

static void* environment;
fmi2GetTLMEffort_ft getTLMEffort;
fmi2SetTLMFlow_ft setTLMFlow;
/*
void setInterpolatedRealCallback(int   vr, fmi2GetTLMEffort_ft cb) {
    std::printf("Setting interpolation callback.\n");
    if(cb) {
        std::printf("Success!\n");
        getTLMEffort.insert(std::pair<int,fmi2GetTLMEffort_ft>(vr,cb));
    }
    else {
        std::printf("Failure!\n");
    }
}

void setInterpolatedRealCallback(interpolatedRealCallbackT cb) {
  //std::printf("Setting interpolation callback.\n");
  interpolatedRealCallback = cb;
  if(interpolatedRealCallback) {
    //std::printf("Success!\n");
  }
  else {
    //std::printf("Failure!\n");
  }
}
*/
#endif

void setRealDerivative(int idx, double value) {
    switch(idx) {
    case 10:
        dC = value;
        break;
    }
}

void setReal(int idx, double value) {
    switch(idx) {
    case 0:
        Ith(y,1) = value;
        break;
    case 1:
        Ith(y,2) = value;
        break;
    case 2:
        F = value;
        break;
    case 4:
        M1 = value;
        break;
    case 5:
        B1 = value;
        break;
    case 6:
        K1 = value;
        break;
    case 7:
        M2 = value;
        break;
    case 8:
        B2 = value;
        break;
    case 9:
        K2 = value;
        break;
    case 10:
        C = value;
        break;
    case 11:
        Zc = value;
        break;
    case 12:
        cData[0] = value;
        std::cout << "cData[0] = " << cData[0] << "\n";
        break;
    case 13:
        cData[1] = value;
        std::cout << "cData[1] = " << cData[1] << "\n";
        break;
    case 14:
        cData[2] = value;
        std::cout << "cData[2] = " << cData[2] << "\n";
        break;
    case 15:
        cData[3] = value;
        std::cout << "cData[3] = " << cData[3] << "\n";
        break;
    case 16:
        cData[4] = value;
        std::cout << "cData[4] = " << cData[4] << "\n";
        break;
    case 17:
        cData[5] = value;
        std::cout << "cData[5] = " << cData[5] << "\n";
        break;
    case 18:
        cData[6] = value;
        std::cout << "cData[6] = " << cData[6] << "\n";
        break;
    case 19:
        cData[7] = value;
        std::cout << "cData[7] = " << cData[7] << "\n";
        break;
    case 20:
        cData[8] = value;
        std::cout << "cData[8] = " << cData[8] << "\n";
        break;
    case 21:
        cData[9] = value;
        std::cout << "cData[9] = " << cData[9] << "\n";
        break;
    case 22:
        tData[0] = value;
        std::cout << "tData[0] = " << tData[0] << "\n";
        break;
    case 23:
        tData[1] = value;
        std::cout << "tData[1] = " << tData[1] << "\n";
        break;
    case 24:
        tData[2] = value;
        std::cout << "tData[2] = " << tData[2] << "\n";
        break;
    case 25:
        tData[3] = value;
        std::cout << "tData[3] = " << tData[3] << "\n";
        break;
    case 26:
        tData[4] = value;
        std::cout << "tData[4] = " << tData[4] << "\n";
        break;
    case 27:
        tData[5] = value;
        std::cout << "tData[5] = " << tData[5] << "\n";
        break;
    case 28:
        tData[6] = value;
        std::cout << "tData[6] = " << tData[6] << "\n";
        break;
    case 29:
        tData[7] = value;
        std::cout << "tData[7] = " << tData[7] << "\n";
        break;
    case 30:
        tData[8] = value;
        std::cout << "tData[8] = " << tData[8] << "\n";
        break;
    case 31:
        tData[9] = value;
        std::cout << "tData[9] = " << tData[9] << "\n";
        break;
    }
}


void setInteger(int idx, int value) {
    std::cout << "FMU: Setting integer " << idx << " to " << value << "\n";
    switch(idx) {
    case 3:
        interpolationMethod = value;
        break;
    }
}


int getInteger(int idx) {
    switch(idx) {
    case 3:
        return interpolationMethod;
    default:
        return 0;
    }
}


double getReal(int idx) {
    switch(idx) {
    case 0:
        return Ith(y,1);
    case 1:
        return Ith(y,2);
    case 2:
        return F;
    case 4:
        return M1;
    case 5:
        return B1;
    case 6:
        return K1;
    case 7:
        return M2;
    case 8:
        return B2;
    case 9:
        return K2;
    case 10:
        return C;
    case 11:
        return Zc;
    case 12:
        if(cData.size() > 0) {
            return cData[cData.size()-1];
        }
        else {
            return 0;
        }
    case 13:
        if(tData.size() > 0) {
            return tData[tData.size()-1];
        }
        else {
            return 0;
        }
    default:
        return 0;
    }
}


double getForceX(double time, double speed) {
    switch(interpolationMethod) {
    case 0:
        (void)time;
#ifdef OPENCPS_CALLBACKS
      if(interpolatedRealCallback)
        return interpolatedRealCallback(2, time, speed);
      else
#endif
        return F;   //Linear extrapolation
    case 1:
    {
        //Interpolate wave variable using derivative
        double C_int = C+(time-tcur)*dC;
        double F_int = C_int - Ith(y,1)*Zc;
        return -F_int;
    }
    case 2:
        double C_int,F_int;
        C_int = cData[0];

        size_t i2 = tData.size()-1;
        for(; i2>0; --i2) {
            if(tData[i2-1] < time) {
                break;
            }
        }
        size_t i1 = 0;
        for(; i1<tData.size()-1; ++i1) {
            if(tData[i1+1] >= time) {
                break;
            }
        }
        if(i2 == i1 || tData[i2] == tData[i1]) {
            C_int = cData[i1];
        }
        else {
            C_int = cData[i1] + (cData[i2]-cData[i1])*(time-tData[i1])/(tData[i2]-tData[i1]);

        }
        F_int = C_int - Ith(y,1)*Zc;

        return -F_int;
    }
}


static int rhs(realtype t, N_Vector yy, N_Vector yp, void *user_data)
{
    (void)user_data;
    Ith(yp,1) = -B1/M1*Ith(yy,1) - K1/M1*Ith(yy,2) + K1/M1*Ith(yy,4) - getForceX(t,Ith(yy,1))/M1;
    Ith(yp,2) = Ith(yy,1);
    Ith(yp,3) = -B2/M2*Ith(yy,3) + K1/M2*Ith(yy,2) - (K1+K2)/M2*Ith(yy,4);
    Ith(yp,4) = Ith(yy,3);
    return(0);
}



static int rhs_ida(realtype t, N_Vector yy, N_Vector yp, N_Vector rr, void *user_data)
{
    (void)user_data;
    Ith(rr,1) = Ith(yp,1) + B1/M1*Ith(yy,1) + K1/M1*Ith(yy,2) - K1/M1*Ith(yy,4) + getForceX(t,Ith(yy,1))/M1;
    Ith(rr,2) = Ith(yp,2) - Ith(yy,1);
    Ith(rr,3) = Ith(yp,3) + B2/M2*Ith(yy,3) - K1/M2*Ith(yy,2) + (K1+K2)/M2*Ith(yy,4);
    Ith(rr,4) = Ith(yp,4) - Ith(yy,3);
    return(0);
}


static int check_flag(void *flagvalue, const char *funcname, int opt)
{
    int *errflag;

    /* Check if SUNDIALS function returned NULL pointer - no memory allocated */
    if (opt == 0 && flagvalue == NULL) {
        fprintf(stderr, "\nSUNDIALS_ERROR: %s() failed - returned NULL pointer\n\n",
                funcname);
        return(1); }

    /* Check if flag < 0 */
    else if (opt == 1) {
        errflag = (int *) flagvalue;
        if (*errflag < 0) {
            fprintf(stderr, "\nSUNDIALS_ERROR: %s() failed with flag = %d\n\n",
                    funcname, *errflag);
            return(1); }}

    /* Check if function returned NULL pointer - no memory allocated */
    else if (opt == 2 && flagvalue == NULL) {
        fprintf(stderr, "\nMEMORY_ERROR: %s() failed - returned NULL pointer\n\n",
                funcname);
        return(1); }

    return(0);
}






int initializeSolvers() {
    mem = NULL;
    y = yp = abstol = NULL;

    /* Create serial vector of length NEQ for I.C. and abstol */
    y = N_VNew_Serial(n_states);
    if (check_flag((void *)y, "N_VNew_Serial", 0)) return(1);
    yp = N_VNew_Serial(n_states);
    if (check_flag((void *)yp, "N_VNew_Serial", 0)) return(1);
    abstol = N_VNew_Serial(n_states);
    if (check_flag((void *)abstol, "N_VNew_Serial", 0)) return(1);


    /* Initialize y */
    for(size_t i=0; i<n_states; ++i) {
        Ith(y,i+1) = 0;     //Start at zero position and velocity
    }

    /* Initialize yp */
    for(size_t i=0; i<n_states; ++i) {
        Ith(yp,i+1) = 0;    //Start at steady-state
    }

    /* Set the vector absolute tolerance */
    for(size_t i=0; i<n_states; ++i) {
        Ith(abstol,i+1) = 0.01*reltol;  //Same absolute tolerance for all states for now
    }

    if(solver == CVODE) {
        /* Call CVodeCreate to create the solver memory and specify the
     * Backward Differentiation Formula and the use of a Newton iteration */
        mem = CVodeCreate(CV_BDF, CV_NEWTON);
        if (check_flag((void *)mem, "CVodeCreate", 0)) return(1);

        /* Call CVodeInit to initialize the integrator memory and specify the
     * user's right hand side function in y'=f(t,y), the inital time T0, and
     * the initial dependent variable vector y. */
        flag = CVodeInit(mem, rhs, tstart, y);
        if (check_flag(&flag, "CVodeInit", 1)) return(1);

        /* Call CVodeSVtolerances to specify the scalar relative tolerance
     * and vector absolute tolerances */
        flag = CVodeSVtolerances(mem, reltol, abstol);
        if (check_flag(&flag, "CVodeSVtolerances", 1)) return(1);

        /* Call CVDense to specify the CVDENSE dense linear solver */
        flag = CVDense(mem, n_states);
        if (check_flag(&flag, "CVDense", 1)) return(1);

        flag = CVodeSetMaxStep(mem, hmax);
        if (check_flag(&flag, "CVodeSetMaxStep", 1)) return(1);

        CVodeSetMaxNumSteps(mem, 1000000);

        /* Set the Jacobian routine to Jac (user-supplied) */
        //flag = CVDlsSetDenseJacFn(cvode_mem, Jac);        //TODO: Supply with jacobian somehow
        //if (check_flag(&flag, "CVDlsSetDenseJacFn", 1)) return(1);
    }
    else if(solver == IDA) {
        /* Call IDACreate to create the solver memory and specify the
       * Backward Differentiation Formula and the use of a Newton iteration */
        mem = IDACreate();
        if (check_flag((void *)mem, "IDACreate", 0)) return(1);

        /* Call IDAInit and IDAInit to initialize IDA memory */
        flag = IDAInit(mem, rhs_ida, tstart, y, yp);
        if (check_flag(&flag, "IDAInit", 1)) return(1);

        /* Call IDASVtolerances to specify the scalar relative tolerance
     * and vector absolute tolerances */
        flag = IDASVtolerances(mem, reltol, abstol);
        if (check_flag(&flag, "IDASVtolerances", 1)) return(1);

        /* Call IDADense to specify the CVDENSE dense linear solver */
        flag = IDADense(mem, n_states);
        if (check_flag(&flag, "IDADense", 1)) return(1);

        flag = IDASetMaxStep(mem, hmax);
        if (check_flag(&flag, "IDASetMaxStep", 1)) return(1);

        IDASetMaxNumSteps(mem,1000000);

        /* Set the Jacobian routine to Jac (user-supplied) */
        //flag = CVDlsSetDenseJacFn(cvode_mem, Jac);        //TODO: Supply with jacobian somehow
        //if (check_flag(&flag, "CVDlsSetDenseJacFn", 1)) return(1);
    }

    tsolver = tstart;
    tcur = tstart;

    return 0;
}

#ifdef OPENCPS_CALLBACKS
void initialize(void* env, fmi2GetTLMEffort_ft effort_f, fmi2SetTLMFlow_ft flow_f) {
    environment = env;
    getTLMEffort = effort_f;
    setTLMFlow = flow_f;
}
#endif



int takeStep(double t1) {
    int flag;

    //Integrate using specified solver
    if(solver == CVODE) {
        while(tsolver < t1){
            flag = CVode(mem, t1, y, &tsolver, CV_NORMAL);
            if (check_flag(&flag, "CVode", 1)) {
                cout << "CVODE solver failed!";
                exit(1);
            }
#ifdef OPENCPS_CALLBACKS
            if(setTLMFlow) {
                double flow = Ith(y,1);
                setTLMFlow(environment,0,1,tsolver,&flow);
            }
#endif
        }
    }
    else if(solver == IDA) {
        while(tsolver < t1){
            flag = IDASolve(mem, t1, &tsolver, y, yp, IDA_NORMAL);
            if (check_flag(&flag, "IDASolve", 1)) {
                cout << "IDA solver failed!";
                exit(1);
            }
#ifdef OPENCPS_CALLBACKS
            if(setTLMFlow) {
                double flow = Ith(y,1);
                setTLMFlow(environment,0,1,tsolver,&flow);
            }
#endif
        }
    }
    else if(solver == ExplicitEuler) {
        for (size_t k = 0; k < n_states; k++) {
            Ith(y,k+1) = Ith(y,k+1) + hcur*Ith(yp,k+1);
        }
    }
    else if(solver == RungeKutta) {
        N_Vector k1,k2,k3,k4;
        k1 = N_VNew_Serial(n_states);
        if (check_flag((void *)y, "N_VNew_Serial", 0)) return(1);
        k2 = N_VNew_Serial(n_states);
        if (check_flag((void *)y, "N_VNew_Serial", 0)) return(1);
        k3 = N_VNew_Serial(n_states);
        if (check_flag((void *)y, "N_VNew_Serial", 0)) return(1);
        k4 = N_VNew_Serial(n_states);
        if (check_flag((void *)y, "N_VNew_Serial", 0)) return(1);

        k1 = yp;
        for(size_t k=0; k<n_states; ++k) {
            Ith(y,k+1) = Ith(y,k+1) + hcur/2.0*Ith(k1,k+1);
        }
        rhs(tcur+hcur/2.0,y,yp,0);
        k2 = yp;
        for(size_t k=0; k<n_states; ++k) {
            Ith(y,k+1) = Ith(y,k+1) + hcur/2.0*Ith(k2,k+1);
        }
        rhs(tcur+hcur/2.0,y,yp,0);
        k3 = yp;
        for(size_t k=0; k<n_states; ++k) {
            Ith(y,k+1) = Ith(y,k+1) + hcur*Ith(k3,k+1);
        }
        rhs(tcur+hcur/2.0,y,yp,0);
        k4 = yp;

        for(size_t k=0; k<n_states; ++k) {
            Ith(y,k+1) = Ith(y,k+1)+hcur*(Ith(k1,k+1)+2*Ith(k2,k+1)+2*Ith(k3,k+1)+Ith(k4,k+1))/6.0;
        }
    }

    tcur = t1;

    return 0;
}





void printDiagnostics()
{
  if(solver != CVODE && solver != IDA) {
    return;
  }

  std::ofstream outfile;
  outfile.open("solver_diagnostics.csv", std::ios_base::app);
  outfile << interpolationMethod;
  if(solver == CVODE) {
    long nEvals,nSteps;
    CVodeGetNumRhsEvals(mem,&nEvals);
    CVodeGetNumSteps(mem,&nSteps);
    outfile << "," << nEvals << "," << nSteps << "\n";
  }
  else if(solver == IDA) {
    long nEvals,nSteps;
    IDAGetNumResEvals(mem,&nEvals);
    IDAGetNumSteps(mem,&nSteps);
    outfile << "," << nEvals << "," << nSteps << "\n";
  }

  outfile.close();
}

