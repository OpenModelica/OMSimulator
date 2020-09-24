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

#include <kinsol/kinsol.h>
#include <nvector/nvector_serial.h>
#include <sunlinsol/sunlinsol_dense.h>       /* Default dense linear solver */

namespace oms
{
  class System;
  class DirectedGraph;

  class KinsolSolver
  {
    public:
    KinsolSolver* NewKinsolSolver();

    private:
    /* ### configuration  ### */
    int linearSolverMethod;     /* specifies the method to solve the underlying linear problem */
    int nonLinearSystemNumber;
    int kinsolStrategy;
    int retries;
    int solved;                 /* if the system is once solved reuse linear matrix information */
    int nominalJac;

    /* ### tolerances ### */
    double fnormtol;        /* function tolerance */
    double scsteptol;       /* step tolerance */
    double maxstepfactor;   /* maximum newton step factor mxnewtstep = maxstepfactor * norm2(xScaling) */

    /* ### work arrays ### */
    N_Vector initialGuess;
    N_Vector xScale;
    N_Vector fScale;
    N_Vector fRes;
    N_Vector fTmp;

    int iflag;
    long countResCalls;        /* case of sparse function not avaiable */

    /* ### kinsol internal data */
    void* kinsolMemory;
    void* userData;

    /* settings */
    int size;
    int nnz;

    /* member function */
    static int nlsKinsolResiduals(N_Vector uu, N_Vector fval, void *userData);
    void nlsKinsolErrorPrint(int error_code, const char *module, const char *function, char *msg, void *userData);
    void nlsKinsolInfoPrint(const char *module, const char *function, char *msg, void *userData);
    int nlsDenseJac(long int N, N_Vector x, N_Vector fx, SUNMatrix Jac, void *userData, N_Vector tmp1, N_Vector tmp2);
    void nlsKinsolJacSumDense(SUNMatrix mat);
  };

  class AlgLoop
  {
    public:
    AlgLoop();
    AlgLoop(oms_alg_solver_enu_t method, std::vector< std::pair<int, int> > SCC);
    
    oms_status_enu_t solveAlgLoop(System& syst, DirectedGraph& graph);
    std::string getAlgSolverName();
    std::string dumpLoopVars(DirectedGraph& graph);

    private:
    oms_alg_solver_enu_t algSolverMethod;
    oms_status_enu_t fixPointIteration(System& syst, DirectedGraph& graph);

    KinsolSolver kinsolData;

    /* Loop data */
    const std::vector< std::pair<int, int> > SCC; ///< Strong connected components

  };
}


#endif // _OMS_ALGLOOP_H_
