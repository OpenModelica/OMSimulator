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
#include <math.h>

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
static double reltol = 1e-6;    //Relative tolerance
static double tstart = 0;       //Start time
static double hmax = 5e-4;      //Max step-size
static double hcur = hmax;
static double tsolver;          //Solver inernal time
static double tcur;             //Current time
static size_t n_states = 2;     //Number of states
static void *mem;               //Solver memory pointer
static int flag;                //Solver return flag

static N_Vector y, yp, abstol;

static double L = 10;
static double M = 1;
static double B = 1000;
static double J = 0.1;

static double r_x, v_x, r_y, v_y;
static double F_x = 0;
static double F_y = 0;
static double T_phi = 0;

//These variables are used for interpolating forces
static double c_x = 0;    //Wave variable
static double Zc_x = 1;   //Impedance
static double dC_x = 0;   //Derivative of force, used for interpolation
static double c_y = 0;
static double Zc_y = 1;
static double dC_y = 0;
static double c_phi = 0;
static double Zc_phi = 1;
static double dC_phi = 0;

static std::vector<double> c_data_x;
static std::vector<double> t_data_x;
static std::vector<double> c_data_y;
static std::vector<double> t_data_y;
static std::vector<double> c_data_phi;
static std::vector<double> t_data_phi;

static int interpolationMethod = 0;


void setRealDerivative(int idx, double value) {
    switch(idx) {
    case 13:
        dC_x = value;
        break;
    case 15:
        dC_y = value;
        break;
    case 17:
        dC_phi = value;
        break;
    }
}

void setReal(int idx, double value) {
    switch(idx) {
    case 1:
        r_x = value;
        break;
    case 2:
        v_x = value;
        break;
    case 3:
        F_x = value;
        break;
    case 4:
        r_y = value;
        break;
    case 5:
        v_y = value;
        break;
    case 6:
        F_y = value;
        break;
    case 7:
        Ith(y,2) = value;
        break;
    case 8:
        Ith(y,1) = value;
        break;
    case 9:
        T_phi = value;
        break;
    case 10:
        L = value;
        break;
    case 11:
        M = value;
        break;
    case 12:
        B = value;
        break;
    case 13:
        c_x = value;
        break;
    case 14:
        Zc_x = value;
        break;
    case 15:
        c_y = value;
        break;
    case 16:
        Zc_y = value;
        break;
    case 17:
        c_phi = value;
        break;
    case 18:
        Zc_phi = value;
        break;
    case 19:
        c_data_x.push_back(value);
        if(c_data_x.size() > 1000) {
            c_data_x.erase(c_data_x.begin(), c_data_x.begin()+500);
        }
        break;
    case 20:
        t_data_x.push_back(value);
        if(t_data_x.size() > 1000) {
            t_data_x.erase(t_data_x.begin(), t_data_x.begin()+500);
        }
        break;
    case 21:
        c_data_y.push_back(value);
        if(c_data_y.size() > 1000) {
            c_data_y.erase(c_data_y.begin(), c_data_y.begin()+500);
        }
        break;
    case 22:
        t_data_y.push_back(value);
        if(t_data_y.size() > 1000) {
            t_data_y.erase(t_data_y.begin(), t_data_y.begin()+500);
        }
        break;
    case 23:
        c_data_phi.push_back(value);
        if(c_data_phi.size() > 1000) {
            c_data_phi.erase(c_data_phi.begin(), c_data_phi.begin()+500);
        }
        break;
    case 24:
        t_data_phi.push_back(value);
        if(t_data_phi.size() > 1000) {
            t_data_phi.erase(t_data_phi.begin(), t_data_phi.begin()+500);
        }
        break;
    }
}


void setInteger(int idx, int value) {
    switch(idx) {
    case 0:
        interpolationMethod = value;
        break;
    }
}


int getInteger(int idx) {
    switch(idx) {
    case 0:
        return interpolationMethod;
    default:
        return 0;
    }
}


double getReal(int idx) {
    switch(idx) {
    case 1:
        return -L*sin(Ith(y,2));
    case 2:
        return -L*cos(Ith(y,2))*Ith(y,1);
    case 3:
        return F_x;
    case 4:
        return L*cos(Ith(y,2));
    case 5:
        return -L*sin(Ith(y,2))*Ith(y,1);
    case 6:
        return F_y;
    case 7:
        return Ith(y,2);
    case 8:
        return Ith(y,1);
    case 9:
        return T_phi;
    case 10:
        return L;
    case 11:
        return M;
    case 12:
        return B;
    case 13:
        return c_x;
    case 14:
        return Zc_x;
    case 15:
      return c_y;
    case 16:
      return Zc_y;
    case 17:
      return c_phi;
    case 18:
      return Zc_phi;
    case 19:
        if(c_data_x.size() > 0) {
            return c_data_x[c_data_x.size()-1];
        }
        else {
            return 0;
        }
    case 20:
        if(t_data_x.size() > 0) {
            return t_data_x[t_data_x.size()-1];
        }
        else {
            return 0;
        }
    case 21:
        if(c_data_y.size() > 0) {
            return c_data_y[c_data_y.size()-1];
        }
        else {
            return 0;
        }
    case 22:
        if(t_data_y.size() > 0) {
            return t_data_y[t_data_y.size()-1];
        }
        else {
            return 0;
        }
    case 23:
        if(c_data_phi.size() > 0) {
            return c_data_phi[c_data_phi.size()-1];
        }
        else {
            return 0;
        }
    case 24:
        if(t_data_phi.size() > 0) {
            return t_data_phi[t_data_phi.size()-1];
        }
        else {
            return 0;
        }
    default:
        return 0;
    }
}


double getForceX(double time) {
    v_x = -L*cos(Ith(y,2))*Ith(y,1);

    switch(interpolationMethod) {
    case 0:
        (void)time;
        return F_x;   //Linear extrapolation
    case 1:
    {
        //Interpolate wave variable using derivative
        double C_int = c_x+(time-tcur)*dC_x;
        double F_int = C_int - v_x*Zc_x;
        return -F_int;
    }
    case 2:
        double C_int,F_int;

        if(t_data_x[0] < time && t_data_x.size() > 1) {
            size_t i2 = t_data_x.size()-1;
            for(; i2>0; --i2) {
                if(t_data_x[i2-1] < time) {
                    break;
                }
            }
            size_t i1 = 0;
            for(; i1<t_data_x.size()-1; ++i1) {
                if(t_data_x[i1+1] >= time) {
                    break;
                }
            }
            if(i2 == i1 || t_data_x[i2] == t_data_x[i1]) {
                C_int = c_data_x[i1];
            }
            else {
                C_int = c_data_x[i1] + (c_data_x[i2]-c_data_x[i1])*(time-t_data_x[i1])/(t_data_x[i2]-t_data_x[i1]);
            }
            F_int = C_int - v_x*Zc_x;
        }
        else {
            F_int = -F_x;
        }

        return -F_int;
    }
}


double getForceY(double time) {
    v_y = -L*sin(Ith(y,2))*Ith(y,1);

    switch(interpolationMethod) {
    case 0:
        (void)time;
        return F_y;   //Linear extrapolation
    case 1:
    {
        //Interpolate wave variable using derivative
        double C_int = c_y+(time-tcur)*dC_y;
        double F_int = C_int - v_y*Zc_y;
        return -F_int;
    }
    case 2:
        double C_int,F_int;

        if(t_data_y[0] < time && t_data_y.size() > 1) {
            size_t i2 = t_data_y.size()-1;
            for(; i2>0; --i2) {
                if(t_data_y[i2-1] < time) {
                    break;
                }
            }
            size_t i1 = 0;
            for(; i1<t_data_y.size()-1; ++i1) {
                if(t_data_y[i1+1] >= time) {
                    break;
                }
            }
            if(i2 == i1 || t_data_y[i2] == t_data_y[i1]) {
                C_int = c_data_y[i1];
            }
            else {
                C_int = c_data_y[i1] + (c_data_y[i2]-c_data_y[i1])*(time-t_data_y[i1])/(t_data_y[i2]-t_data_y[i1]);
            }
            F_int = C_int - v_y*Zc_y;
        }
        else {
            F_int = -F_y;
        }

        return -F_int;
    }
}



double getTorque(double time) {
    switch(interpolationMethod) {
    case 0:
        (void)time;
        return T_phi;   //Linear extrapolation
    case 1:
    {
        //Interpolate wave variable using derivative
        double C_int = c_phi+(time-tcur)*dC_phi;
        double T_int = C_int - Ith(y,1)*Zc_phi;
        return -T_int;
    }
    case 2:
        double C_int,T_int;

        if(t_data_phi[0] < time && t_data_phi.size() > 1) {
            size_t i2 = t_data_phi.size()-1;
            for(; i2>0; --i2) {
                if(t_data_phi[i2-1] < time) {
                    break;
                }
            }
            size_t i1 = 0;
            for(; i1<t_data_phi.size()-1; ++i1) {
                if(t_data_phi[i1+1] >= time) {
                    break;
                }
            }
            if(i2 == i1 || t_data_phi[i2] == t_data_phi[i1]) {
                C_int = c_data_phi[i1];
            }
            else {
                C_int = c_data_phi[i1] + (c_data_phi[i2]-c_data_phi[i1])*(time-t_data_phi[i1])/(t_data_phi[i2]-t_data_phi[i1]);
            }
            T_int = C_int - Ith(y,1)*Zc_phi;
        }
        else {
            T_int = -T_phi;
        }

        return -T_int;
    }
}


static int rhs(realtype t, N_Vector yy, N_Vector yp, void *user_data)
{
    (void)user_data;

    //Joint forces
    double Fx = -getForceX(t);
    double Fy = -getForceY(t);
    double T = -getTorque(t);
    J = M*L*L/4.0;

    //Update derivatives
    Ith(yp,1) = L/2.0*M*9.81/J*cos(Ith(yy,2)) - L*Fx/J*cos(Ith(yy,2)) - L*Fy/J*sin(Ith(yy,2)) - B/J*Ith(yy,1) + T/J;
    Ith(yp,2) = Ith(yy,1);

    return(0);
}


static int rhs_ida(realtype t, N_Vector yy, N_Vector yp, N_Vector rr, void *user_data)
{
  (void)user_data;

  //Joint forces
  double Fx = -getForceX(t);
  double Fy = -getForceY(t);
  double T = -getTorque(t);
  J = M*L*L/4.0;

  //Update residuals
  Ith(rr,1) = Ith(yp,1) - L*M/9.81/2.0*cos(Ith(yy,2)) + L*Fx/J*cos(Ith(yy,2)) + L*Fy/J*sin(Ith(yy,2)) + B/J*Ith(y,1) - T/J;
  Ith(rr,2) = Ith(yp,2) - Ith(yy,1);

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
        }
    }
    else if(solver == IDA) {
        while(tsolver < t1){
            flag = IDASolve(mem, t1, &tsolver, y, yp, IDA_NORMAL);
            if (check_flag(&flag, "IDASolve", 1)) {
                cout << "IDA solver failed!";
                exit(1);
            }
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

