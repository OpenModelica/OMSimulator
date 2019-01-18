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

#ifndef _OMS_SYSTEM_SC_H_
#define _OMS_SYSTEM_SC_H_

#include "ComRef.h"
#include "System.h"
#include "Types.h"

#include "cvode/cvode.h"             /* prototypes for CVODE fcts., consts. */
#include "nvector/nvector_serial.h"  /* serial N_Vector types, fcts., macros */

namespace oms
{
  class Model;
  class ComponentFMUME;
  int cvode_rhs(realtype t, N_Vector y, N_Vector ydot, void* user_data);

  class SystemSC : public System
  {
  public:
    ~SystemSC();

    static System* NewSystem(const oms::ComRef& cref, Model* parentModel, System* parentSystem);
    oms_status_enu_t exportToSSD_SimulationInformation(pugi::xml_node& node) const;
    oms_status_enu_t importFromSSD_SimulationInformation(const pugi::xml_node& node);

    oms_status_enu_t instantiate();
    oms_status_enu_t initialize();
    oms_status_enu_t terminate();
    oms_status_enu_t reset();
    oms_status_enu_t stepUntil(double stopTime, void (*cb)(const char* ident, double time, oms_status_enu_t status));

    double getTime() const {return time;}

    oms_status_enu_t updateInputs(DirectedGraph& graph);
    oms_status_enu_t solveAlgLoop(DirectedGraph& graph, const std::vector< std::pair<int, int> >& SCC);

    std::string getSolverName() const;
    oms_status_enu_t setSolverMethod(std::string);

    oms_status_enu_t setSolver(oms_solver_enu_t solver) {if (solver > oms_solver_sc_min && solver < oms_solver_sc_max) {solverMethod=solver; return oms_status_ok;} return oms_status_error;}

  protected:
    SystemSC(const ComRef& cref, Model* parentModel, System* parentSystem);

    // stop the compiler generating methods copying the object
    SystemSC(SystemSC const& copy);            ///< not implemented
    SystemSC& operator=(SystemSC const& copy); ///< not implemented

  private:
    double time;

    std::vector<ComponentFMUME*> fmus;

    std::vector<fmi2_boolean_t> callEventUpdate;
    std::vector<fmi2_boolean_t> terminateSimulation;
    std::vector<size_t> nStates;
    std::vector<size_t> nEventIndicators;

    std::vector<double*> states;
    std::vector<double*> states_der;
    std::vector<double*> states_nominal;
    std::vector<double*> event_indicators;
    std::vector<double*> event_indicators_prev;

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
      SolverDataEuler_t euler;
      SolverDataCVODE_t cvode;
    } solverData;

    friend int oms::cvode_rhs(realtype t, N_Vector y, N_Vector ydot, void* user_data);
  };
}

#endif
