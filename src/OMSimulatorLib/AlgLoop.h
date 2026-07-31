/*
 * This file is part of OpenModelica.
 *
 * Copyright (c) 1998-2026, Open Source Modelica Consortium (OSMC),
 * c/o Linköpings universitet, Department of Computer and Information Science,
 * SE-58183 Linköping, Sweden.
 *
 * All rights reserved.
 *
 * THIS PROGRAM IS PROVIDED UNDER THE TERMS OF AGPL VERSION 3 LICENSE OR
 * THIS OSMC PUBLIC LICENSE (OSMC-PL) VERSION 1.8.
 * ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS PROGRAM CONSTITUTES
 * RECIPIENT'S ACCEPTANCE OF THE OSMC PUBLIC LICENSE OR THE GNU AGPL
 * VERSION 3, ACCORDING TO RECIPIENTS CHOICE.
 *
 * The OpenModelica software and the OSMC (Open Source Modelica Consortium)
 * Public License (OSMC-PL) are obtained from OSMC, either from the above
 * address, from the URLs:
 * http://www.openmodelica.org or
 * https://github.com/OpenModelica/ or
 * http://www.ida.liu.se/projects/OpenModelica,
 * and in the OpenModelica distribution.
 *
 * GNU AGPL version 3 is obtained from:
 * https://www.gnu.org/licenses/licenses.html#GPL
 *
 * This program is distributed WITHOUT ANY WARRANTY; without
 * even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE, EXCEPT AS EXPRESSLY SET FORTH
 * IN THE BY RECIPIENT SELECTED SUBSIDIARY LICENSE CONDITIONS OF OSMC-PL.
 *
 * See the full OSMC Public License conditions for more details.
 *
 */

#ifndef _OMS_ALGLOOP_H_
#define _OMS_ALGLOOP_H_

#include <memory>
#include <string>
#include <vector>
#include "OMSimulator/Types.h"
#include "DirectedGraph.h"

#include <sundials/sundials_context.h>  /* SUNContext */
#include <sundials/sundials_logger.h>   /* SUNLogger */
#include <kinsol/kinsol.h>
#include <nvector/nvector_serial.h>
#include <sunlinsol/sunlinsol_dense.h>  /* Default dense linear solver */

namespace oms
{
  class System;
  class DirectedGraph;

  typedef struct KINSOL_USER_DATA {
    System*         syst;
    DirectedGraph*  graph;
    const int       algLoopNumber;
    unsigned int    iteration;
  }KINSOL_USER_DATA;

  class KinsolSolver
  {
  public:
    ~KinsolSolver();
    static KinsolSolver* NewKinsolSolver(const int algLoopNum, const unsigned int size, double relativeTolerance, const bool useDirectionalDerivative);
    oms_status_enu_t kinsolSolve(System& syst, DirectedGraph& graph);

  private:
    /* All members start out empty, so that the destructor can be run on an
     * object NewKinsolSolver() gave up on half way through. */

    /* tolerances */
    double fnormtol = 0.0;  /* function tolerance */

    /* work arrays */
    N_Vector initialGuess = nullptr;
    N_Vector uScale = nullptr;        /* Scaling vector for u */
    N_Vector fScale = nullptr;        /* Scaling vector for f(u) */
    N_Vector fTmp = nullptr;          /* Vector used for tmp computations */

    /* kinsol internal data */
    SUNContext sunctx = nullptr;      /* SUNDIALS simulation context */
    void* kinsolMemory = nullptr;
    void* user_data = nullptr;
    int size = 0;

    /* linear solver data */
    SUNLinearSolver linSol = nullptr; /* Linear solver object used by KINSOL */
    N_Vector y = nullptr;             /* Template for cloning vectors needed inside linear solver */
    SUNMatrix J = nullptr;            /* (Non-)Sparse matrix template for cloning matrices needed within linear solver */

    /* member function */
    static int nlsKinsolJac(N_Vector u, N_Vector fu, SUNMatrix J, void *user_data, N_Vector tmp1, N_Vector tmp2);
    static int nlsKinsolResiduals(N_Vector u, N_Vector fval, void *user_data);
    static void sundialsErrorHandlerFunction(int line, const char *func, const char *file, const char *msg,
                                             SUNErrCode err_code, void *err_user_data, SUNContext sunctx);
  };

  class AlgLoop
  {
  public:
    AlgLoop(oms_alg_solver_enu_t method, double relativeTolerance, scc_t SCC, const int systNumber, const bool useDirectionalDerivative);

    scc_t getSCC() {return SCC;}
    oms_status_enu_t solveAlgLoop(System& syst, DirectedGraph& graph);
    std::string getAlgSolverName();
    std::string dumpLoopVars(DirectedGraph& graph);

  private:
    oms_alg_solver_enu_t algSolverMethod;
    oms_status_enu_t fixPointIteration(System& syst, DirectedGraph& graph);

    /* Owns the solver: an AlgLoop is created as a temporary and moved into
     * System::algLoops, so the copy constructor has to stay deleted. */
    std::unique_ptr<KinsolSolver> kinsolData;

    /* Loop data */
    const scc_t SCC;            ///< Strong connected components
    const int systNumber;
    double relativeTolerance;
  };
}

#endif
