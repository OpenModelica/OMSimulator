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

#include <sstream>

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
    logError("SUNDIALS_ERROR: " + functionName + " failed with flag = " + std::to_string(flag));
    return false;
  }
  logDebug("SUNDIALS_INFO: " + functionName + " failed with flag = " + std::to_string(flag));
  return true;
}

/**
 * @brief Error handler function given to KINSOL.
 *
 * @param errorCode   Error code from KINSOL
 * @param module      Name of the module reporting the error.
 * @param function    Name of the function in which the error occurred.
 * @param msg         Error Message.
 * @param user_data   Pointer to user data. Unused.
 */
void oms::KinsolSolver::sundialsErrorHandlerFunction(int errorCode, const char *module,
                                                     const char *function, char *msg,
                                                     void *user_data)
{
  KINSOL_USER_DATA* kinsolUserData;
  std::string systNum = "unknown";
  std::string mod = module;
  std::string func = function;

  if (user_data != NULL)
  {
    kinsolUserData = (KINSOL_USER_DATA *)user_data;
    systNum = std::to_string(kinsolUserData->algLoopNumber);
  }

  logError("SUNDIALS_ERROR: [system] " + systNum + " [module] " + mod + " | [function] " + func
           + " | [error_code] " + std::to_string(errorCode) + "\n" + std::string(msg));
}

/**
 * @brief Info handler function given to KINSOL.
 *
 * Will only print information when debug loging is active.
 *
 * @param module      Name of the module reporting the information.
 * @param function    Name of the function reporting the information.
 * @param msg         Message.
 * @param user_data   Pointer to user data. Unused.
 */
void oms::KinsolSolver::sundialsInfoHandlerFunction(const char *module, const char *function,
                                                    char *msg, void *user_data)
{
  KINSOL_USER_DATA* kinsolUserData;
  std::string systNum = "unknown";
  std::string mod = module;
  std::string func = function;

  if (user_data != NULL) {
    kinsolUserData = (KINSOL_USER_DATA *)user_data;
    systNum = std::to_string(kinsolUserData->algLoopNumber);
  }

  logDebug("SUNDIALS_INFO: [system] " + systNum + " [module] " + mod + " | [function] " + func + "\n" + std::string(msg));
}

/**
 * @brief Jacobian function for KINSOL
 *
 * @param u     is the current (unscaled) iterate
 * @param fu    is the current value of the vector F(u)
 * @param J     is the output approximate Jacobian matrix, J = ∂F/∂u,
 *              of type SUNMatrix
 * @param data  is a pointer to user data, the same as the user data
 *              parameter passed to KINSetUserData
 * @param tmp1  pointer to memory allocated for variables of type
 *              N_Vector which can be used by the KINJacFn function as
 *              temporary storage or work space
 * @param tmp2  pointer to memory allocated for variables of type
 *              N_Vector which can be used by the KINJacFn function as
 *              temporary storage or work space
 * @return int  A function of type KINLsJacFn should return 0 if
 *              successful or a non-zero value otherwise
 */
int oms::KinsolSolver::nlsKinsolJac(N_Vector u, N_Vector fu, SUNMatrix J, void *user_data, N_Vector tmp1, N_Vector tmp2)
{
  KINSOL_USER_DATA* kinsoluserData = (KINSOL_USER_DATA*) user_data;
  System* syst = kinsoluserData->syst;
  AlgLoop* algLoop = syst->getAlgLoop(kinsoluserData->algLoopNumber);
  DirectedGraph* graph = kinsoluserData->graph;
  const scc_t SCC = algLoop->getSCC();
  const int size = SCC.connections.size();

  oms_status_enu_t status;
  double *u_data = NV_DATA_S(u);

  for(int j=0; j<size; j++)
  {
    const int output = SCC.connections[j].first;
    const oms::ComRef& crefOutput = graph->getNodes()[output].getName();
    const int input = SCC.connections[j].second;
    const oms::ComRef& crefInput = graph->getNodes()[input].getName();
    // res[j] = output - input
    // std::cout << "res[" << j << "] = " << std::string(crefOutput) << " - " << std::string(crefInput) << std::endl;

    for(int i=0; i<size; i++)
    {
      double der = 0.0;
      const int input_col = SCC.connections[i].second;
      oms::ComRef crefInput_col = graph->getNodes()[input_col].getName();
      oms::ComRef front = crefInput_col.pop_front();

      if (front == crefOutput.front())
        if (oms_status_ok != syst->getDirectionalDerivative(crefOutput, crefInput_col, der))
          return logError("not recoverable error");
      if (input_col == input)
        der -= 1.0;

      SM_ELEMENT_D(J, j, i) = der;
      // std::cout << "J[" << j << ", " << i << "] = " << der << std::endl;
    }
  }

  return 0;
}

/**
 * @brief Residual function for KINSOL
 *
 * This function computes F(u) (or G(u) for fixed-point iteration and
 * Anderson acceleration) for a given value of the vector u.
 *
 * @param u          is the current value of the variable vector, u
 * @param fval       is the output vector F(u)
 * @param user_data  a is a pointer to user data, the pointer user
 *                   data passed to KINSetUserData
 * @return int       A KINSysFn function should return 0 if
 *                   successful, a positive value if a recoverable
 *                   error occurred (in which case kinsol will attempt
 *                   to correct), or a negative value if it failed
 *                   unrecoverably (in which case the solution process
 *                   is halted and KIN SYSFUNC FAIL is returned).
 */
int oms::KinsolSolver::nlsKinsolResiduals(N_Vector u, N_Vector fval, void *user_data)
{
  double *u_data = NV_DATA_S(u);
  double *fval_data = NV_DATA_S(fval);

  KINSOL_USER_DATA* kinsoluserData = (KINSOL_USER_DATA*) user_data;
  System* syst = kinsoluserData->syst;
  AlgLoop* algLoop = syst->getAlgLoop(kinsoluserData->algLoopNumber);
  DirectedGraph* graph = kinsoluserData->graph;
  const scc_t SCC = algLoop->getSCC();
  kinsoluserData->iteration++;

  const int size = SCC.connections.size();
  double maxRes;
  oms_status_enu_t status;
  std::stringstream ss;

  if (Flags::DumpAlgLoops())
  {
    ss << "iteration " << std::to_string(kinsoluserData->iteration) << std::endl;
    ss << "inputs:" << std::endl;
  }

  // Set values from u
  for (int i=0; i<size; ++i)
  {
    int input = SCC.connections[i].second;
    status = syst->setReal(graph->getNodes()[input].getName(), u_data[i]);
    if (Flags::DumpAlgLoops())
      ss << "  " << graph->getNodes()[input].getName().c_str() << ": " << u_data[i] << std::endl;
    if (status == oms_status_discard || status == oms_status_error || status == oms_status_warning)
    {
      logInfo("iteration " + std::to_string(kinsoluserData->iteration) + ": recoverable error (1)");
      return 1 /* recoverable error */;
    }
    else if (status == oms_status_fatal)
    {
      logInfo("iteration " + std::to_string(kinsoluserData->iteration) + ": not recoverable error (1)");
      return -1 /* not recoverable error */;
    }
  }

  if (Flags::DumpAlgLoops())
    ss << "outputs:" << std::endl;

  // Get updated values and calulate residual
  for (int i=0; i<size; ++i)
  {
    int output = SCC.connections[i].first;
    status = syst->getReal(graph->getNodes()[output].getName(), fval_data[i]);
    if (Flags::DumpAlgLoops())
      ss << "  " << graph->getNodes()[output].getName().c_str() << ": " << fval_data[i] << std::endl;
    if (status == oms_status_discard || status == oms_status_error || status == oms_status_warning)
    {
      logInfo("iteration " + std::to_string(kinsoluserData->iteration) + ": recoverable error (2)");
      return 1 /* recoverable error */;
    }
    else if (status == oms_status_fatal)
    {
      logInfo("iteration " + std::to_string(kinsoluserData->iteration) + ": not recoverable error (2)");
      return -1 /* not recoverable error */;
    }
    fval_data[i] = fval_data[i] - u_data[i];
  }

  if (Flags::DumpAlgLoops())
  {
    ss << "residuals:" << std::endl;
    for (int i=0; i<size; ++i)
      ss << "  res[" << i << "]: " << fval_data[i] << std::endl;
    logInfo(ss.str());
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

  delete((KINSOL_USER_DATA*)(this->user_data));
}

/**
 * @brief Create new oms::KinsolSolver::KinsolSolver object
 *
 * @param algLoopNum            Number of algebraic loop
 * @param size                  Dimension of algebraic loop
 * @param absoluteTolerance     Tolerance used for solving the loop
 * @return oms::KinsolSolver*   Retruns pointer to KinsolSolver object
 */
oms::KinsolSolver* oms::KinsolSolver::NewKinsolSolver(const int algLoopNum, const unsigned int size, double absoluteTolerance, const bool useDirectionalDerivative)
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
  kinsolSolver->user_data = new KINSOL_USER_DATA{/*.syst=*/NULL, /*.graph=*/NULL, /*.algLoopNumber=*/algLoopNum, /*.iteration=*/0};
  flag = KINSetUserData(kinsolSolver->kinsolMemory, kinsolSolver->user_data);
  if (!checkFlag(flag, "KINSetUserData")) return NULL;

  /* Set error handler and print level */
  if (Log::DebugEnabled())
  {
    logDebug("SUNDIALS KINSOL: Set print level to maximum.");
    printLevel = 3;
  }
  else
  {
    printLevel = 0;
  }
  flag = KINSetPrintLevel(kinsolSolver->kinsolMemory, printLevel);
  if (!checkFlag(flag, "KINSetPrintLevel")) return NULL;

  flag = KINSetErrHandlerFn(kinsolSolver->kinsolMemory, sundialsErrorHandlerFunction, kinsolSolver->user_data);
  if (!checkFlag(flag, "KINSetErrHandlerFn")) return NULL;

  flag = KINSetInfoHandlerFn(kinsolSolver->kinsolMemory, sundialsInfoHandlerFunction, kinsolSolver->user_data);
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
  if (useDirectionalDerivative && Flags::DirectionalDerivatives())
    flag = KINSetJacFn(kinsolSolver->kinsolMemory, nlsKinsolJac); /* Use symbolic Jacobian */
  else
    flag = KINSetJacFn(kinsolSolver->kinsolMemory, NULL); /* Use KINSOLs internal difference quotient function for Jacobian approximation */
  if (!checkFlag(flag, "KINSetJacFn")) return NULL;

  /* Set function-norm stopping tolerance */
  kinsolSolver->fnormtol = 0.01 * absoluteTolerance;
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
oms_status_enu_t oms::KinsolSolver::kinsolSolve(System& syst, DirectedGraph& graph, double tolerance /*= 0.0*/)
{
  /* Update user data */
  KINSOL_USER_DATA* kinsolUserData = (KINSOL_USER_DATA*) user_data;
  kinsolUserData->syst = &syst;
  kinsolUserData->graph = &graph;
  kinsolUserData->iteration = 0;
  AlgLoop* algLoop = syst.getAlgLoop(kinsolUserData->algLoopNumber);
  const scc_t SCC = algLoop->getSCC();

  int flag;
  double fNormValue;

  double tol = tolerance != 0.0 ? tolerance : fnormtol;

  if (SCC.connections.size() != size)
  {
    logError("The size of the loop changed! This shouldn't be possible...");
    throw("Serious problem encountered. Open a ticket!");
  }

  /* Set initial guess */
  double *initialGuess_data = NV_DATA_S(initialGuess);
  for (int i=0; i < size; i++)
  {
    int output = SCC.connections[i].first;
    if (oms_status_ok != syst.getReal(graph.getNodes()[output].getName(), initialGuess_data[i]))
    {
      return oms_status_error;
    }
  }

  /* u and f scaling */
  // TODO: Add scaling that is not only constant ones

  KINSetFuncNormTol(kinsolMemory, tol);

  /* Solve algebraic loop with KINSol() */
  flag = KINSol(kinsolMemory,   /* KINSol memory block */
                initialGuess,   /* initial guess on input; solution vector */
                KIN_NONE,       /* global strategy choice: Basic newton iteration */
                uScale,         /* scaling vector, for the variable u */
                fScale);        /* scaling vector for function values fval */
  if (!checkFlag(flag, "KINSol")) return oms_status_error;

  /* Check solution */
  KINGetFuncNorm(kinsolMemory, &fNormValue);
  if ( fNormValue > tol )
  {
    logWarning("Solution of algebraic loop " + std::to_string(((KINSOL_USER_DATA *)user_data)->algLoopNumber) + "not within precission given by fnormtol: " + std::to_string(fnormtol));
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
oms::AlgLoop::AlgLoop(oms_alg_solver_enu_t method, double absTol, scc_t scc, const int number, const bool useDirectionalDerivative): absoluteTolerance(absTol), SCC(scc), systNumber(number)
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
    kinsolData = KinsolSolver::NewKinsolSolver(systNumber, SCC.connections.size(), absoluteTolerance, useDirectionalDerivative);
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
oms_status_enu_t oms::AlgLoop::solveAlgLoop(System& syst, DirectedGraph& graph, double tolerance)
{
  logDebug("Solving algebraic loop formed by connections\n" + dumpLoopVars(graph));
  logDebug("Using solver " + getAlgSolverName());

  switch (algSolverMethod)
  {
  case oms_alg_solver_fixedpoint:
    return fixPointIteration(syst, graph, tolerance);
  case oms_alg_solver_kinsol:
    return kinsolData->kinsolSolve(syst, graph, tolerance);
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
oms_status_enu_t oms::AlgLoop::fixPointIteration(System& syst, DirectedGraph& graph, double tolerance)
{
  const int size = SCC.connections.size();
  const int maxIterations = Flags::MaxLoopIteration();
  int it=0;
  double maxRes;
  double *res = new double[size]();

  if (tolerance == 0.0 || tolerance > absoluteTolerance)
    tolerance = absoluteTolerance;

  do
  {
    std::stringstream ss;
    it++;
    // get old values
    for (int i=0; i<size; ++i)
    {
      int output = SCC.connections[i].first;
      if (graph.getNodes()[output].getType() == oms_signal_type_real)
      {
        if (oms_status_ok != syst.getReal(graph.getNodes()[output].getName(), res[i]))
        {
          delete[] res;
          return oms_status_error;
        }
      }
      // check for boolean types in discrete connections
      else if (graph.getNodes()[output].getType() == oms_signal_type_boolean)
      {
        bool value;
        if (oms_status_ok != syst.getBoolean(graph.getNodes()[output].getName(), value))
        {
          delete[] res;
          return oms_status_error;
        }
        res[i] = value ? 1.0 : 0.0;
      }
    }


    // update inputs
    for (int i=0; i<size; ++i)
    {
      int input = SCC.connections[i].second;
      if (graph.getNodes()[input].getType() == oms_signal_type_real)
      {
        if (oms_status_ok != syst.setReal(graph.getNodes()[input].getName(), res[i]))
        {
          delete[] res;
          return oms_status_error;
        }
      }
      // check for boolean types in discrete connections
      else if (graph.getNodes()[input].getType() == oms_signal_type_boolean)
      {
        if (oms_status_ok != syst.setBoolean(graph.getNodes()[input].getName(), res[i] > 0.1))
        {
          delete[] res;
          return oms_status_error;
        }
      }
    }

    if (Flags::DumpAlgLoops())
    {
      ss << "iteration " << std::to_string(it) << std::endl;
      ss << "inputs:" << std::endl;
      for (int i=0; i<size; ++i)
      {
        const int input = SCC.connections[i].second;
        ss << "  " << graph.getNodes()[input].getName().c_str() << ": " << res[i] << std::endl;
      }
      ss << "outputs:" << std::endl;
    }

    // calculate residuals
    maxRes = 0.0;
    double value;
    for (int i=0; i<size; ++i)
    {
      int output = SCC.connections[i].first;
      if (graph.getNodes()[output].getType() == oms_signal_type_real)
      {
        if (oms_status_ok != syst.getReal(graph.getNodes()[output].getName(), value))
        {
          delete[] res;
          return oms_status_error;
        }
        res[i] -= value;
      }
      // check for boolean types in discrete connections
      else if (graph.getNodes()[output].getType() == oms_signal_type_boolean)
      {
        bool _value;
        if (oms_status_ok != syst.getBoolean(graph.getNodes()[output].getName(), _value))
        {
          delete[] res;
          return oms_status_error;
        }
        value = _value ? 1.0 : 0.0;
        res[i] -= value;
      }

      if (Flags::DumpAlgLoops())
        ss << "  " << graph.getNodes()[output].getName().c_str() << ": " << value << std::endl;

      if (fabs(res[i]) > maxRes)
        maxRes = fabs(res[i]);
    }

    if (Flags::DumpAlgLoops())
    {
      ss << "residuals:" << std::endl;
      for (int i=0; i<size; ++i)
        ss << "  res[" << i << "]: " << res[i] << std::endl;
      logInfo(ss.str());
    }

  } while(maxRes > tolerance && it < maxIterations);

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
  const int size = SCC.connections.size();
  std::string varNames = "";
  int output;

  for (int i=0; i<size-1; ++i)
  {
    varNames.append("  ");
    output = SCC.connections[i].first;
    varNames.append(graph.getNodes()[output].getName().c_str());
    varNames.append(" -> ");
    output = SCC.connections[i].second;
    varNames.append(graph.getNodes()[output].getName().c_str());
    varNames.append("\n");
  }
  varNames.append("  ");
  output = SCC.connections[size-1].first;
  varNames.append(graph.getNodes()[output].getName().c_str());
  varNames.append(" -> ");
  output = SCC.connections[size-1].second;
  varNames.append(graph.getNodes()[output].getName().c_str());

  return varNames;
}
