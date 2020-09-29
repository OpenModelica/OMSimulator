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

#ifndef _OMS_ALGLOOP_H_
#define _OMS_ALGLOOP_H_

#include <string>
#include <vector>
#include "Types.h"
#include "DirectedGraph.h"

#include <kinsol/kinsol.h>
#include <nvector/nvector_serial.h>
#include <sunlinsol/sunlinsol_dense.h>       /* Default dense linear solver */

namespace oms
{
  class System;
  class DirectedGraph;

  typedef struct KINSOL_USER_DATA {
    System*         syst;
    DirectedGraph*  graph;
    const int       algLoopNumber;
  }KINSOL_USER_DATA;

  class KinsolSolver
  {
    public:
    ~KinsolSolver();
    static KinsolSolver* NewKinsolSolver(const int algLoopNum, const unsigned int size, double absoluteTolerance);
    oms_status_enu_t kinsolSolve(System& syst, DirectedGraph& graph);

    private:
    /* tolerances */
    double fnormtol;        /* function tolerance */

    /* work arrays */
    N_Vector initialGuess;
    N_Vector uScale;        /* Scaling vector for u */
    N_Vector fScale;        /* Scaling vector for f(u) */
    N_Vector fTmp;          /* Vector used for tmp computations */

    /* kinsol internal data */
    void* kinsolMemory;
    void* userData;
    int size;

    /* linear solver data */
    SUNLinearSolver linSol; /* Linear solver object used by KINSOL */
    N_Vector y;             /* Template for cloning vectors needed inside linear solver */
    SUNMatrix J;            /* (Non-)Sparse matrix template for cloning matrices needed within linear solver */

    /* member function */
    static int nlsKinsolResiduals(N_Vector uu, N_Vector fval, void *userData);
    static void sundialsErrorHandlerFunction(int error_code, const char *module, const char *function, char *msg, void *userData);
    static void sundialsInfoHandlerFunction(const char *module, const char *function, char *msg, void *userData);
  };

  class AlgLoop
  {
    public:
    AlgLoop(oms_alg_solver_enu_t method, double absTol, oms_ssc_t SCC, const int systNumber);

    oms_ssc_t getSCC() {return SCC;}
    oms_status_enu_t solveAlgLoop(System& syst, DirectedGraph& graph);
    std::string getAlgSolverName();
    std::string dumpLoopVars(DirectedGraph& graph);

    private:
    oms_alg_solver_enu_t algSolverMethod;
    oms_status_enu_t fixPointIteration(System& syst, DirectedGraph& graph);

    KinsolSolver* kinsolData;

    /* Loop data */
    const oms_ssc_t SCC;            ///< Strong connected components
    const int systNumber;
    double absoluteTolerance;
  };
}

#endif // _OMS_ALGLOOP_H_
