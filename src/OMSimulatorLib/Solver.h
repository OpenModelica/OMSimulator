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
 * even the implied warranty of  MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE, EXCEPT AS EXPRESSLY SET FORTH
 * IN THE BY RECIPIENT SELECTED SUBSIDIARY LICENSE CONDITIONS OF OSMC-PL.
 *
 * See the full OSMC Public License conditions for more details.
 *
 */

#ifndef _OMS2_SOLVER_H_
#define _OMS2_SOLVER_H_

#include "ComRef.h"
#include "DirectedGraph.h"
#include "FMISubModel.h"
#include "FMUInfo.h"
#include "Logging.h"
#include "Option.h"
#include "Variable.h"

#include <map>
#include <string>
#include <unordered_map>
#include <vector>

#include <fmilib.h>
#include <pugixml.hpp>

#include "cvode/cvode.h"             /* prototypes for CVODE fcts., consts. */
#include "nvector/nvector_serial.h"  /* serial N_Vector types, fcts., macros */

namespace oms2
{
  class FMUWrapper;
  int cvode_rhs(realtype t, N_Vector y, N_Vector ydot, void *user_data);

  class Solver
  {
  public:
    Solver(const oms2::ComRef& name, oms_solver_enu_t solverMethod);
    ~Solver();

    const oms2::ComRef& getName() const {return name;}
    oms_status_enu_t setTolerance(double tolerance) {this->relativeTolerance = tolerance; return oms_status_ok;}
    oms_status_enu_t setTime(double time) {this->time = time; return oms_status_ok;}
    oms_status_enu_t addFMU(oms2::FMUWrapper* fmu);
    oms_status_enu_t removeFMU(const oms2::ComRef& fmu);
    oms_solver_enu_t getMethod() const {return solverMethod;}
    oms_status_enu_t initializeFMU(FMUWrapper* fmu);
    oms_status_enu_t initializeSolver(double startTime);
    oms_status_enu_t freeSolver();
    oms_status_enu_t doStep(double stopTime);

    double getTime() const {return time;}
    double getTolerance() const {return relativeTolerance;}

  private:
    void doEventIteration(int i);

  private:
    struct SolverDataInternal_t
    {
      // empty
    };

    struct SolverDataEuler_t
    {
    };

    struct SolverDataCVODE_t
    {
      void *mem;
      N_Vector y;
      N_Vector abstol;
    };

    union SolverData_t
    {
      SolverDataInternal_t internal;
      SolverDataEuler_t euler;
      SolverDataCVODE_t cvode;
    };

    friend int oms2::cvode_rhs(realtype t, N_Vector y, N_Vector ydot, void *user_data);
    friend class FMUWrapper;

  private:
    oms2::ComRef name;
    std::vector<oms2::FMUWrapper*> fmus;
    std::vector<double> fmuTime;
    std::vector<size_t> nStates;
    std::vector<size_t> nEventIndicators;
    std::vector<fmi2_event_info_t> eventInfo;
    std::vector<fmi2_boolean_t> callEventUpdate;
    std::vector<fmi2_boolean_t> terminateSimulation;

    std::vector<double*> states;
    std::vector<double*> states_der;
    std::vector<double*> states_nominal;
    std::vector<double*> event_indicators;
    std::vector<double*> event_indicators_prev;

    double time;
    oms_solver_enu_t solverMethod;
    SolverData_t solverData;
    double relativeTolerance = 1e-4;
  };
}

#endif
