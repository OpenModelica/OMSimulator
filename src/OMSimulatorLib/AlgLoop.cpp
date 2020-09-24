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
 * @brief Error handler function given to KINSOL.
 *
 * @param errorCode   Error code from KINSOL
 * @param module      Name of the module reporting the error.
 * @param function    Name of the function in which the error occurred.
 * @param msg         Error Message.
 * @param userData    Pointer to user data. Unused.
 */
void sundialsErrorHandlerFunction(int errorCode, const char *module,
                                  const char *function, char *msg,
                                  void *userData) {

  std::string mod = module;
  std::string func = function;
  logError("SUNDIALS_ERROR: [module] " + mod + " | [function] " + func
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
 * @param user_data   Pointer to user data. Unused.
 */
void sundialsInfoHandlerFunction(const char *module, const char *function,
                               char *msg, void *user_data) {

  std::string mod = module;
  std::string func = function;
  logDebug("SUNDIALS_INFO: [module] " + mod + " | [function] " + func);
  logDebug(msg);
}

int oms::KinsolSolver::nlsKinsolResiduals(N_Vector uu, N_Vector fval, void *userData)
{
  double *uu_data = NV_DATA_S(uu);
  double *fval_data = NV_DATA_S(fval);

  //SystemSC* system = (SystemSC*)user_data;
  //System* syst;
  //DirectedGraph* graph;
  //const std::vector< std::pair<int, int> >* SCC;
  //oms_status_enu_t status;

  int someErrorCode = 0;

  /* Update statistics */


  // Try block

  // get old values
  //for (int i=0; i<size; ++i)
  //{
  //  int output = SCC[i].first;
  //  if (oms_status_ok != syst.getReal(graph.getNodes()[output].getName(), res[i]))
  //  {
  //    delete[] res;
  //    return oms_status_error;
  //  }
  //}
//
  //// update inputs
  //for (int i=0; i<size; ++i)
  //{
  //  int input = SCC[i].second;
  //  if (oms_status_ok != syst.setReal(graph.getNodes()[input].getName(), res[i]))
  //  {
  //    delete[] res;
  //    return oms_status_error;
  //  }
  //}
//
  //// calculate residuals
  //maxRes = 0.0;
  //double value;
  //for (int i=0; i<size; ++i)
  //{
  //  int output = SCC[i].first;
  //  if (oms_status_ok != syst.getReal(graph.getNodes()[output].getName(), value))
  //  {
  //    delete[] res;
  //    return oms_status_error;
  //  }
  //  res[i] -= value;
//
  //  if (fabs(res[i]) > maxRes)
  //    maxRes = fabs(res[i]);
  //}
  


  // Catch error

  return someErrorCode;
}



oms::KinsolSolver* oms::KinsolSolver::NewKinsolSolver(void)
{
  int flag;
  int printLevel;
  KinsolSolver* kinsolSolver = new KinsolSolver();

  /* Create KINSOL memory block */
  kinsolSolver->kinsolMemory = KINCreate();
  if (kinsolSolver->kinsolMemory == NULL) logError("SUNDIALS_ERROR: KINCreate() failed");

  /* Set error handler and print level */
  if (Log::DebugEnabled()) {
    logDebug("SUNDIALS KINSOL: Set print level to maximum.");
    printLevel = 3;
  } else {
    printLevel = 0;
  }
  flag = KINSetPrintLevel(kinsolSolver->kinsolMemory, printLevel);
  if (flag < 0) logError("SUNDIALS_ERROR: KINSetPrintLevel() failed with flag = " + std::to_string(flag));

  // TODO: Set some system number
  //kinsolData->userData.sysNumber = -1;
  flag = KINSetErrHandlerFn(kinsolSolver->kinsolMemory, sundialsErrorHandlerFunction, NULL);
  if (flag < 0) logError("SUNDIALS_ERROR: KINSetErrHandlerFn() failed with flag = " + std::to_string(flag));

  flag = KINSetInfoHandlerFn(kinsolSolver->kinsolMemory, sundialsInfoHandlerFunction, NULL);
  if (flag < 0) logError("SUNDIALS_ERROR: KINSetInfoHandlerFn() failed with flag = " + std::to_string(flag));

  /* Initialize KINSOL object */
  flag = KINInit(kinsolSolver->kinsolMemory, nlsKinsolResiduals, kinsolSolver->initialGuess);
  if (flag < 0) logError("SUNDIALS_ERROR: KINInit() failed with flag = " + std::to_string(flag));

  return kinsolSolver;
}


// Default construcutor, using fixed-point-iteration
oms::AlgLoop::AlgLoop(void): algSolverMethod(oms_alg_solver_fixedpoint)
{
}

oms::AlgLoop::AlgLoop(oms_alg_solver_enu_t method, std::vector< std::pair<int, int> > scc): algSolverMethod(method), SCC(scc)
{
}

oms_status_enu_t oms::AlgLoop::solveAlgLoop(System& syst, DirectedGraph& graph)
{
  logDebug("Solving algebraic loop formed by connections\n" + dumpLoopVars(graph));
  logDebug("Using solver " + getAlgSolverName());

  switch (algSolverMethod)
  {
  case oms_alg_solver_fixedpoint:
    return fixPointIteration(syst, graph);
  case oms_alg_solver_kinsol:
    logError("Algebraic solver method Kinsol not implemented yet!");
    return oms_status_error;
  default:
    logError("Invalid algebraic solver method!");
    return oms_status_error;
  }
}


oms_status_enu_t oms::AlgLoop::fixPointIteration(System& syst, DirectedGraph& graph)
{
  const int size = SCC.size();
  const int maxIterations = Flags::MaxLoopIteration();
  int it=0;
  double maxRes;
  double absoluteTolerance, relativeTolerance;
  double *res = new double[size]();

  syst.getTolerance(&absoluteTolerance, &relativeTolerance);
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


std::string oms::AlgLoop::getAlgSolverName(void)
{
  switch (algSolverMethod)
  {
  case oms_alg_solver_none:
    return "None";
  case oms_alg_solver_fixedpoint:
    return "Fixed-Point-Iteration";
  case oms_alg_solver_kinsol:
    return "Kinsol";
  default:
    logError("This should not be reachable!");
    return "Can't figure out what solver method is used!";
  }
}

std::string oms::AlgLoop::dumpLoopVars(DirectedGraph& graph)
{
  const int size = SCC.size();
  std::string varNames = "\t";
  int output;

  for (int i=0; i<size-1; ++i)
  {
    output = SCC[i].first;
    varNames.append(graph.getNodes()[output].getName().c_str());
    varNames.append(" <-> ");
    output = SCC[i].second;
    varNames.append(graph.getNodes()[output].getName().c_str());
    varNames.append("\n\t");
  }
  output = SCC[size-1].first;
  varNames.append(graph.getNodes()[output].getName().c_str());
  varNames.append(" <-> ");
  output = SCC[size-1].second;
  varNames.append(graph.getNodes()[output].getName().c_str());

  return varNames;
}
