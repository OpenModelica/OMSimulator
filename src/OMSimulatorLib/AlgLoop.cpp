/*
 * This file is part of OpenModelica.
 *
 * Copyright (c) 1998-CurrentYear, Open Source Modelica Consortium (OSMC),
 * c/o Linköpings universitet, Department of Computer and Information Science,
 * SE-58183 Linköping, Sweden.
 *
 * All rights reserved.
 *
 * THIS PROGRAM IS PROVIDED UNDER THE TERMS OF GPL VERSION 3 LICENSE OR
 * THIS OSMC PUBLIC LICENSE (OSMC-PL) VERSION 1.2.
 * ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS PROGRAM CONSTITUTES
 * RECIPIENT'S ACCEPTANCE OF THE OSMC PUBLIC LICENSE OR THE GPL VERSION 3,
 * ACCORDING TO RECIPIENTS CHOICE.
 *
 * The OpenModelica software and the Open Source Modelica
 * Consortium (OSMC) Public License (OSMC-PL) are obtained
 * from OSMC, either from the above address,
 * from the URLs: http://www.ida.liu.se/projects/OpenModelica or
 * http://www.openmodelica.org, and in the OpenModelica distribution.
 * GNU version 3 is obtained from: http://www.gnu.org/copyleft/gpl.html.
 *
 * This program is distributed WITHOUT ANY WARRANTY; without
 * even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE, EXCEPT AS EXPRESSLY SET FORTH
 * IN THE BY RECIPIENT SELECTED SUBSIDIARY LICENSE CONDITIONS OF OSMC-PL.
 *
 * See the full OSMC Public License conditions for more details.
 *
 */

#include "AlgLoop.h"

#include "Component.h"
#include "DirectedGraph.h"
#include "Flags.h"
#include "System.h"

/**
 * @brief Check flag returned by KINSOL function and log error
 *
 * @param flag            Flag with some error value retured by KINSOL function
 * @param functionName    Name of KINSOL function that returned the flag
 * @return true           The flag was no error code
 * @return false          The flag was an error code
 */
inline bool checkFlag(int flag, std::string functionName)
{
  if (flag < 0)
  {
    logError("SUNDIALS_ERROR: " + functionName + "() failed with flag = " + std::to_string(flag));
    return false;
  }
  return true;
}

/**
 * @brief Error handler function given to KINSOL.
 *
 * @param errorCode   Error code from KINSOL
 * @param module      Name of the module reporting the error.
 * @param function    Name of the function in which the error occurred.
 * @param msg         Error Message.
 * @param userData    Pointer to user data. Unused.
 */
void oms::KinsolSolver::sundialsErrorHandlerFunction(int errorCode, const char *module,
                                                     const char *function, char *msg,
                                                     void *userData)
{
  KINSOL_USER_DATA* kinsolUserData;
  std::string systNum = "unknown";
  std::string mod = module;
  std::string func = function;

  if (userData != NULL) {
    kinsolUserData = (KINSOL_USER_DATA *)userData;
    systNum = std::to_string(kinsolUserData->algLoopNumber);
  }

  logError("SUNDIALS_ERROR: [system] " + systNum + " [module] " + mod + " | [function] " + func
          +" | [error_code] " + std::to_string(errorCode));
  logError(msg);
}

/**
 * @brief Info handler function given to KINSOL.
 *
 * Will only print information when debug loging is active.
 *
 * @param module      Name of the module reporting the information.
 * @param function    Name of the function reporting the information.
 * @param msg         Message.
 * @param userData   Pointer to user data. Unused.
 */
void oms::KinsolSolver::sundialsInfoHandlerFunction(const char *module, const char *function,
                                                    char *msg, void *userData)
{
  KINSOL_USER_DATA* kinsolUserData;
  std::string systNum = "unknown";
  std::string mod = module;
  std::string func = function;

  if (userData != NULL) {
    kinsolUserData = (KINSOL_USER_DATA *)userData;
    systNum = std::to_string(kinsolUserData->algLoopNumber);
  }

  logDebug("SUNDIALS_INFO: [system] " + systNum + " [module] " + mod + " | [function] " + func);
  logDebug(msg);
}

/**
 * @brief Residual function for KINSOL
 *
 * @param uu          Input value
 * @param fval        Contains residual at output
 * @param userData    Pointer to user data to access System, AlgLoop, Directed Graph and CSS as well as getReal and setReal
 * @return int        Return 0 on success, 1 if an recoverable error occured, -1 if a fatal error occured.
 */
int oms::KinsolSolver::nlsKinsolResiduals(N_Vector uu, N_Vector fval, void *userData)
{
  double *uu_data = NV_DATA_S(uu);
  double *fval_data = NV_DATA_S(fval);

  KINSOL_USER_DATA* kinsoluserData = (KINSOL_USER_DATA*) userData;
  System* syst = kinsoluserData->syst;
  AlgLoop* algLoop = syst->getAlgLoop(kinsoluserData->algLoopNumber);
  DirectedGraph* graph = kinsoluserData->graph;
  const oms_ssc_t SCC = algLoop->getSCC();

  const int size = SCC.size();
  double maxRes;
  oms_status_enu_t status;

  // Set values from uu
  for (int i=0; i<size; ++i)
  {
    int output = SCC[i].second;
    status = syst->setReal(graph->getNodes()[output].getName(), uu_data[i]);
    if (status == oms_status_discard || status == oms_status_error || status == oms_status_warning)
    {
      return 1 /* recoverable error */;
    }
    else if (status == oms_status_fatal)
    {
      return -1 /* not recoverable error */;
    }
  }

  // Get updated values and calulate residual
  for (int i=0; i<size; ++i)
  {
    int output = SCC[i].first;
    status = syst->getReal(graph->getNodes()[output].getName(), fval_data[i]);
    if (status == oms_status_discard || status == oms_status_error || status == oms_status_warning)
    {
      return 1 /* recoverable error */;
    }
    else if (status == oms_status_fatal)
    {
      return -1 /* not recoverable error */;
    }
    fval_data[i] = fval_data[i] - uu_data[i];
  }

  return 0 /* success */;
}

/**
 * @brief Destroy the oms::KinsolSolver::KinsolSolver object
 *
 */
oms::KinsolSolver::~KinsolSolver()
{
  KINFree(&(this->kinsolMemory));
  N_VDestroy_Serial(this->initialGuess);   /* TODO: Or is N_VDestroy_Serial correct? It won't free internal data */
  N_VDestroy_Serial(this->uScale);
  N_VDestroy_Serial(this->fScale);
  N_VDestroy_Serial(this->fTmp);

  /* Free linear solver data */
  SUNLinSolFree(this->linSol);
  SUNMatDestroy(this->J);
  N_VDestroy_Serial(this->y);

  delete((KINSOL_USER_DATA*)(this->userData));
}

/**
 * @brief Create new oms::KinsolSolver::KinsolSolver object
 *
 * @param algLoopNum            Number of algebraic loop
 * @param size                  Dimension of algebraic loop
 * @param absoluteTolerance     Tolerance used for solving the loop
 * @return oms::KinsolSolver*   Retruns pointer to KinsolSolver object
 */
oms::KinsolSolver* oms::KinsolSolver::NewKinsolSolver(const int algLoopNum, const unsigned int size, double absoluteTolerance)
{
  int flag;
  int printLevel;
  KinsolSolver* kinsolSolver = new KinsolSolver();

  logDebug("Create new KinsolSolver object for algebraic loop number " + std::to_string(algLoopNum));

  kinsolSolver->size = size;

  /* Allocate memory */
  kinsolSolver->initialGuess = N_VNew_Serial(kinsolSolver->size);
  kinsolSolver->uScale = N_VNew_Serial(kinsolSolver->size);
  kinsolSolver->fScale = N_VNew_Serial(kinsolSolver->size);
  kinsolSolver->fTmp = N_VNew_Serial(kinsolSolver->size);
  kinsolSolver->y = N_VNew_Serial(kinsolSolver->size);
  kinsolSolver->kinsolMemory = NULL;

  /* Create KINSOL memory block */
  kinsolSolver->kinsolMemory = KINCreate();
  if (kinsolSolver->kinsolMemory == NULL)
  {
    logError("SUNDIALS_ERROR: KINCreate() failed");
    return NULL;
  }

  /* Set user data given to KINSOL */
  kinsolSolver->userData = new KINSOL_USER_DATA{/*.syst=*/NULL, /*.graph=*/NULL, /*.algLoopNumber=*/algLoopNum};
  flag = KINSetUserData(kinsolSolver->kinsolMemory, kinsolSolver->userData);
  if (!checkFlag(flag, "KINSetUserData")) return NULL;

  /* Set error handler and print level */
  if (Log::DebugEnabled()) {
    logDebug("SUNDIALS KINSOL: Set print level to maximum.");
    printLevel = 3;
  } else {
    printLevel = 0;
  }
  flag = KINSetPrintLevel(kinsolSolver->kinsolMemory, printLevel);
  if (!checkFlag(flag, "KINSetPrintLevel")) return NULL;

  flag = KINSetErrHandlerFn(kinsolSolver->kinsolMemory, sundialsErrorHandlerFunction, kinsolSolver->userData);
  if (!checkFlag(flag, "KINSetErrHandlerFn")) return NULL;

  flag = KINSetInfoHandlerFn(kinsolSolver->kinsolMemory, sundialsInfoHandlerFunction, kinsolSolver->userData);
  if (!checkFlag(flag, "KINSetInfoHandlerFn")) return NULL;

  /* Initialize KINSOL object */
  flag = KINInit(kinsolSolver->kinsolMemory, nlsKinsolResiduals, kinsolSolver->initialGuess);
  if (!checkFlag(flag, "KINInit")) return NULL;

  /* Create matrix object */
  kinsolSolver->J = SUNDenseMatrix(kinsolSolver->size, kinsolSolver->size);

  /* Create linear solver object */
  kinsolSolver->linSol = SUNLinSol_Dense(kinsolSolver->y, kinsolSolver->J);

  /* Set linear solver */
  flag = KINSetLinearSolver(kinsolSolver->kinsolMemory, kinsolSolver->linSol, kinsolSolver->J);
  if (!checkFlag(flag, "KINSetLinearSolver")) return NULL;

  /* Set Jacobian for linear solver */
  flag = KINSetJacFn(kinsolSolver->kinsolMemory, NULL); /* Use KINSOLs internal difference quotient function for Jacobian approximation */
  if (!checkFlag(flag, "KINSetJacFn")) return NULL;

  /* Set function-norm stopping tolerance */
  kinsolSolver->fnormtol = absoluteTolerance;
  flag = KINSetFuncNormTol(kinsolSolver->kinsolMemory, kinsolSolver->fnormtol);
  if (!checkFlag(flag, "KINSetFuncNormTol")) return NULL;

  /* Set scaled-step stopping tolerance */
  flag = KINSetScaledStepTol(kinsolSolver->kinsolMemory, 0.0);
  if (!checkFlag(flag, "KINSetScaledStepTol")) return NULL;

  /* Set max. number of nonlinear iterations */
  flag = KINSetNumMaxIters(kinsolSolver->kinsolMemory, 100 * kinsolSolver->size);
  if (!checkFlag(flag, "KINSetNumMaxIters")) return NULL;

  flag = KINSetNoInitSetup(kinsolSolver->kinsolMemory, SUNFALSE);
  if (!checkFlag(flag, "KINSetNoInitSetup")) return NULL;

  /* Set default scaling */
  double *uScaleData = NV_DATA_S(kinsolSolver->uScale);
  double *fScaleData = NV_DATA_S(kinsolSolver->fScale);
  for (int i=0; i < kinsolSolver->size; i++)
  {
    uScaleData[i] = 1.0;
    fScaleData[i] = 1.0;
  }

  return kinsolSolver;
}

/**
 * @brief Solve algebraic system with KINSOL
 *
 * @param syst                Reference to System object
 * @param graph               Reference to graph opbject
 * @return oms_status_enu_t   Return `oms_status_ok` on success,
 *                            `oms_status_warning` if solving was computed, but solution is not within tolerance
 *                            and `oms_status_error` if an error occured.
 */
oms_status_enu_t oms::KinsolSolver::kinsolSolve(System& syst, DirectedGraph& graph)
{
  /* Update user data */
  KINSOL_USER_DATA* kinsolUserData = (KINSOL_USER_DATA*) userData;
  kinsolUserData->syst = &syst;
  kinsolUserData->graph = &graph;
  AlgLoop* algLoop = syst.getAlgLoop(kinsolUserData->algLoopNumber);
  const oms_ssc_t SCC = algLoop->getSCC();

  int flag;
  double fNormValue;

  logDebug("Solving system " + std::to_string(kinsolUserData->algLoopNumber));

  if (SCC.size() != size)
  {
    logError("The size of the loop changed! This shouldn't be possible...");
    throw("Serious problem encountered. Open a ticket!");
  }

  /* Set initial guess */
  double *initialGuess_data = NV_DATA_S(initialGuess);
  for (int i=0; i < size; i++)
  {
    int output = SCC[i].first;
    if (oms_status_ok != syst.getReal(graph.getNodes()[output].getName(), initialGuess_data[i]))
    {
      return oms_status_error;
    }
  }

  /* u and f scaling */
  // TODO: Add scaling that is not only constant ones

  /* Solve algebraic loop with KINSol() */
  flag = KINSol(kinsolMemory,   /* KINSol memory block */
                initialGuess,   /* initial guess on input; solution vector */
                KIN_NONE,       /* global strategy choice: Basic newton iteration */
                uScale,         /* scaling vector, for the variable uu */
                fScale);        /* scaling vector for function values fval */
  if (flag < 0)
  {
    logError("SUNDIALS_ERROR: KINSol() failed with flag = " + std::to_string(flag));
    return oms_status_error;
  }
  else
  {
    logDebug("SUNDIALS_INFO: KINSol() succeded with flag = " + std::to_string(flag));
  }

  /* Check solution */
  flag = nlsKinsolResiduals(initialGuess, fTmp, userData);
  fNormValue = N_VWL2Norm(fTmp, fTmp);
  if ( fNormValue > fnormtol )
  {
    logWarning("Solution of algebraic loop " + std::to_string(((KINSOL_USER_DATA *)userData)->algLoopNumber) + "not within precission given by fnormtol: " + std::to_string(fnormtol));
    logDebug("2-norm of residual of solution: " + std::to_string(fNormValue));
    return oms_status_warning;
  }

  logDebug("Solved system " + std::to_string(kinsolUserData->algLoopNumber) + " successfully");
  return oms_status_ok;
}

/**
 * @brief Construct a new oms::AlgLoop::AlgLoop object
 *
 * @param method  Specifies used solver for the loop.
 *                Can be `oms_alg_solver_fixedpoint` for fixed-point-iteration
 *                or `oms_alg_solver_kinsol` for SUNDIALS KINSOL
 * @param absTol  Tolerance used for the algebraic solver.
 * @param scc     Strong Connected Componten, a vector of connected
 * @param number
 */
oms::AlgLoop::AlgLoop(oms_alg_solver_enu_t method, double absTol, oms_ssc_t scc, const int number): absoluteTolerance(absTol), SCC(scc), systNumber(number)
{
  switch (method)
  {
    case oms_alg_solver_fixedpoint:
    case oms_alg_solver_kinsol:
      algSolverMethod = method;
      break;
    default:
      logError("Unknown alg. loop solver method");
      throw;
  }

  if (method == oms_alg_solver_kinsol)
  {
    kinsolData = KinsolSolver::NewKinsolSolver(systNumber, SCC.size(), absoluteTolerance);
    if (kinsolData==NULL)
    {
      logError("NewKinsolSolver() failed. Aborting!");
      throw("AlgLoop() failed!");
    }
  }
}

/**
 * @brief Solve algebraic loop
 *
 * Using solver method saved during AlgLoop creation.
 * Can use fixed-point-iteration and KINSOL.
 *
 * @param syst                Reference to System
 * @param graph               Reference to directed graph
 * @return oms_status_enu_t   Returns `oms_status_ok` on success
 */
oms_status_enu_t oms::AlgLoop::solveAlgLoop(System& syst, DirectedGraph& graph)
{
  logDebug("Solving algebraic loop formed by connections\n" + dumpLoopVars(graph));
  logDebug("Using solver " + getAlgSolverName());

  switch (algSolverMethod)
  {
  case oms_alg_solver_fixedpoint:
    return fixPointIteration(syst, graph);
  case oms_alg_solver_kinsol:
    return kinsolData->kinsolSolve(syst, graph);
  default:
    logError("Invalid algebraic solver method!");
    return oms_status_error;
  }
}

/**
 * @brief Fixed-point-iteration to solve algebraic loop.
 *
 * @param syst
 * @param graph
 * @return oms_status_enu_t
 */
oms_status_enu_t oms::AlgLoop::fixPointIteration(System& syst, DirectedGraph& graph)
{
  const int size = SCC.size();
  const int maxIterations = Flags::MaxLoopIteration();
  int it=0;
  double maxRes;
  double *res = new double[size]();

  do
  {
    it++;
    // get old values
    for (int i=0; i<size; ++i)
    {
      int output = SCC[i].first;
      if (oms_status_ok != syst.getReal(graph.getNodes()[output].getName(), res[i]))
      {
        delete[] res;
        return oms_status_error;
      }
    }

    // update inputs
    for (int i=0; i<size; ++i)
    {
      int input = SCC[i].second;
      if (oms_status_ok != syst.setReal(graph.getNodes()[input].getName(), res[i]))
      {
        delete[] res;
        return oms_status_error;
      }
    }

    // calculate residuals
    maxRes = 0.0;
    double value;
    for (int i=0; i<size; ++i)
    {
      int output = SCC[i].first;
      if (oms_status_ok != syst.getReal(graph.getNodes()[output].getName(), value))
      {
        delete[] res;
        return oms_status_error;
      }
      res[i] -= value;

      if (fabs(res[i]) > maxRes)
        maxRes = fabs(res[i]);
    }
  } while(maxRes > absoluteTolerance && it < maxIterations);

  delete[] res;

  if (it >= maxIterations)
  {
    return logError("max. number of iterations (" + std::to_string(maxIterations) + ") exceeded at time = " + std::to_string(syst.getTime()));
  }
  logDebug("CompositeModel::solveAlgLoop: maxRes: " + std::to_string(maxRes) + ", iterations: " + std::to_string(it) + " at time = " + std::to_string(syst.getTime()));
  return oms_status_ok;
}

/**
 * @brief Return solver method
 *
 * @return std::string  Name of solver method
 */
std::string oms::AlgLoop::getAlgSolverName(void)
{
  switch (algSolverMethod)
  {
  case oms_alg_solver_none:
    return "None";
  case oms_alg_solver_fixedpoint:
    return "Fixed-Point-Iteration";
  case oms_alg_solver_kinsol:
    return "KINSOL";
  default:
    logError("This should not be reachable!");
    return "Can't figure out what solver method is used!";
  }
}

/**
 * @brief Dump variables of algebraic loop.
 *
 * @param graph
 * @return std::string
 */
std::string oms::AlgLoop::dumpLoopVars(DirectedGraph& graph)
{
  const int size = SCC.size();
  std::string varNames = "\t";
  int output;

  for (int i=0; i<size-1; ++i)
  {
    output = SCC[i].first;
    varNames.append(graph.getNodes()[output].getName().c_str());
    varNames.append(" -> ");
    output = SCC[i].second;
    varNames.append(graph.getNodes()[output].getName().c_str());
    varNames.append("\n\t");
  }
  output = SCC[size-1].first;
  varNames.append(graph.getNodes()[output].getName().c_str());
  varNames.append(" -> ");
  output = SCC[size-1].second;
  varNames.append(graph.getNodes()[output].getName().c_str());

  return varNames;
}
