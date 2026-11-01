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

#ifndef _OMS_SYSTEM_SC_H_
#define _OMS_SYSTEM_SC_H_

#include "ComRef.h"
#include "System.h"
#include "OMSimulator/Types.h"

#include <memory>

#include <sundials/sundials_context.h>  /* SUNContext */
#include <sundials/sundials_logger.h>   /* SUNLogger */
#include <cvode/cvode.h>                /* prototypes for CVODE fcts., consts. */
#include <nvector/nvector_serial.h>     /* serial N_Vector types, fcts., macros */
#include <ida/ida.h>                    /* prototypes for IDA fcts., consts. */
#include <sunlinsol/sunlinsol_dense.h>  /* Default dense linear solver */
#include <sunlinsol/sunlinsol_klu.h>    /* KLU, for the structured DAE Jacobian */
#include <sunmatrix/sunmatrix_sparse.h>

namespace oms
{
  class Model;
  class ComponentFMUME;
  class ComponentFMU3ME;
  class Component;
  int cvode_rhs(sunrealtype t, N_Vector y, N_Vector ydot, void* user_data);
  int cvode_rhs_algebraic(sunrealtype t, N_Vector y, N_Vector ydot, void* user_data);
  int cvode_roots(sunrealtype t, N_Vector y, sunrealtype *gout, void* user_data);
  int ida_res(sunrealtype t, N_Vector yy, N_Vector yp, N_Vector rr, void* user_data);
  int ida_roots(sunrealtype t, N_Vector yy, N_Vector yp, sunrealtype *gout, void* user_data);
  int ida_jac(sunrealtype t, sunrealtype cj, N_Vector yy, N_Vector yp, N_Vector rr,
              SUNMatrix Jac, void* user_data, N_Vector tmp1, N_Vector tmp2, N_Vector tmp3);

  class SystemSC : public System
  {
  public:
    ~SystemSC();

    static System* NewSystem(const oms::ComRef& cref, Model* parentModel, System* parentSystem);
    oms_status_enu_t exportToSSD_SimulationInformation(pugi::xml_node& node) const;
    oms_status_enu_t importFromSSD_SimulationInformation(const pugi::xml_node& node, const std::string& sspVersion);

    oms_status_enu_t instantiate();
    oms_status_enu_t initialize();
    oms_status_enu_t terminate();
    oms_status_enu_t reset();
    oms_status_enu_t doStep();
    oms_status_enu_t stepUntil(double stopTime);

    oms_status_enu_t updateInputs(DirectedGraph& graph);

    std::string getSolverName() const;
    oms_status_enu_t setSolverMethod(std::string);

    oms_status_enu_t setSolver(oms_solver_enu_t solver) {if (solver > oms_solver_sc_min && solver < oms_solver_sc_max) {solverMethod=solver; return oms_status_ok;} return oms_status_error;}

  private:
    oms_status_enu_t doStepEuler();
    oms_status_enu_t doStepCVODE();
    oms_status_enu_t doStepIDA();
    /**
     * \brief updateInputs, with the algebraic loops IDA owns left alone.
     *
     * \param liftedLoops  one flag per loop of the graph, in the order the
     *                     sorted connections hold them; a loop marked true is
     *                     skipped, because its connections are rows of the
     *                     global system and IDA has already set their inputs.
     *                     nullptr solves every loop, which is what updateInputs
     *                     itself does.
     */
    oms_status_enu_t updateInputsInternal(DirectedGraph& graph, const std::vector<bool>* liftedLoops);
    /// Which loops of the simulation graph can become rows: every connection real.
    void collectLiftedLoops();
    /// A flag per loop of the graph, all true: leave every loop to the integrator.
    std::vector<bool> allLoopsOf(DirectedGraph& graph) const;
    /// Whether any component was switched into DAE mode; readable before initialize().
    bool anyComponentInDaeMode();
    /**
     * \brief The structure of the DAE Jacobian, out of the components' manifests.
     *
     * Which unknowns each row reaches, and a colouring of the columns from it: two
     * columns share a colour when no row reaches both, so one residual evaluation
     * differences all of them at once. A dense difference quotient costs one
     * residual evaluation per unknown — and every one of those calls into every
     * FMU — where a coloured one costs as many as there are colours, which for a
     * coupled system is a handful however many unknowns there are.
     */
    void buildDaeJacobianSparsity();
    /// Distribute the DAE unknowns y (and the state derivatives yp) into the FMUs.
    oms_status_enu_t setDaePoint(double t, N_Vector yy, N_Vector yp);
    /// Read the point the FMUs hold back into y and yp, after a solve or an event.
    oms_status_enu_t getDaePoint(N_Vector yy, N_Vector yp);
    /// IDACalcIC over the current point, then read the consistent point back.
    oms_status_enu_t calcConsistentInitialConditions(double tout);

  protected:
    SystemSC(const ComRef& cref, Model* parentModel, System* parentSystem);

    // stop the compiler generating methods copying the object
    SystemSC(SystemSC const& copy);            ///< not implemented
    SystemSC& operator=(SystemSC const& copy); ///< not implemented

  private:
    std::vector<Component*> fmus; // use Component Base class to support FMI 2 ME and FMI 3 ME

    // One entry per FMU, allocated by initialize() once fmus is filled.
    std::unique_ptr<bool[]> callEventUpdate;     //initialized with false
    std::unique_ptr<bool[]> terminateSimulation; //initialized with false

    std::vector<size_t> nStates;
    std::vector<size_t> nEventIndicators;

    /**
     * fmi-ls-dae. A component that declares a DAE formulation is integrated in
     * DAE mode: it owns no explicit ODE, so the master carries its algebraic
     * variables beside the states and drives the residuals of
     * F(t, x, x', z) = 0 to zero with IDA.
     *
     * The unknowns are laid out per component, [x_i | z_i], and so are the
     * rows: a DAE component contributes its residuals (one per state and per
     * algebraic variable, checked square when DAE mode is switched on), an ODE
     * component the explicit rows x'_i - f_i(x, u, t). So a system may mix the
     * two, and one with no DAE component at all is not affected by any of this.
     */
    /// Index-aligned with fmus: the component as an FMI 3.0 ME one when it runs
    /// in DAE mode, else nullptr.
    std::vector<ComponentFMU3ME*> daeFmus;
    std::vector<size_t> nAlgebraic;    ///< per component, 0 unless in DAE mode
    std::vector<double*> algebraicVars;
    std::vector<double*> residuals;
    bool daeMode = false;              ///< any component runs in DAE mode
    size_t nDaeUnknowns = 0;           ///< states, algebraic variables and lifted loop connections

    /**
     * An algebraic loop lifted into the global system. The loop's connections
     * are what the inner Newton solved for; here each becomes one unknown — the
     * value of the driven input — and one row, output - input = 0. So a loop
     * spanning components is resolved by the integrator, with its own error
     * control, instead of being iterated to convergence inside every residual
     * evaluation on top of whatever each FMU hides internally.
     *
     * These unknowns follow the components' in y, and their rows the components'
     * rows. They are algebraic: their entry in IDASetId's vector is 0.
     */
    struct LiftedConnection
    {
      int output;   ///< graph node driving the connection
      int input;    ///< graph node it drives, and the unknown
    };
    std::vector<LiftedConnection> liftedConnections;
    /// Per loop of the simulation graph, whether it was lifted; passed to
    /// updateInputsInternal so the inner solver leaves those alone.
    std::vector<bool> liftedLoops;

    /// Per unknown, the rows that reach it; empty when the structure is unknown
    /// and IDA differences the Jacobian itself.
    std::vector<std::vector<int>> daeJacRowsByColumn;
    std::vector<int> daeJacColorOfColumn;
    std::vector<std::vector<int>> daeJacColumnsOfColor;
    /// The increment each column was perturbed by, kept between the two passes.
    std::vector<double> daeJacIncrement;
    /// Whether the Jacobian is held sparse and factorized by KLU, which needs a
    /// structure; without one it is dense and IDA differences it itself.
    bool daeJacSparse = false;

    std::vector<double*> states;
    std::vector<double*> states_der;
    std::vector<double*> states_nominal;
    std::vector<double*> event_indicators;
    std::vector<double*> event_indicators_prev;

    bool algebraic = false;

    struct SolverDataEuler_t
    {
    };

    struct SolverDataCVODE_t
    {
      SUNContext sunctx;      /* SUNDIALS simulation context */
      void *mem;
      N_Vector y;
      SUNLinearSolver linSol; /* linear solver object */
      SUNMatrix J;            /* Matrix used by linear solver */
      N_Vector liny;          /* Vector used by linear solver */
      N_Vector abstol;
    };

    struct SolverDataIDA_t
    {
      SUNContext sunctx;      /* SUNDIALS simulation context */
      void *mem;
      N_Vector y;             /* [states | algebraic variables], per component */
      N_Vector yp;            /* the state derivatives; unused rows for the algebraic variables */
      N_Vector id;            /* 1.0 differential, 0.0 algebraic — what IDASetId takes */
      SUNLinearSolver linSol;
      SUNMatrix J;
      N_Vector abstol;
      N_Vector ewt;           /* the error weights, for the Jacobian's increments */
    };

    union SolverData_t
    {
      SolverDataEuler_t euler;
      SolverDataCVODE_t cvode;
      SolverDataIDA_t ida;
    } solverData;

    friend int oms::cvode_rhs(sunrealtype t, N_Vector y, N_Vector ydot, void* user_data);
    friend int oms::cvode_rhs_algebraic(sunrealtype t, N_Vector y, N_Vector ydot, void* user_data);
    friend int oms::cvode_roots(sunrealtype t, N_Vector y, sunrealtype *gout, void* user_data);
    friend int oms::ida_res(sunrealtype t, N_Vector yy, N_Vector yp, N_Vector rr, void* user_data);
    friend int oms::ida_roots(sunrealtype t, N_Vector yy, N_Vector yp, sunrealtype *gout, void* user_data);
    friend int oms::ida_jac(sunrealtype t, sunrealtype cj, N_Vector yy, N_Vector yp, N_Vector rr,
                            SUNMatrix Jac, void* user_data, N_Vector tmp1, N_Vector tmp2, N_Vector tmp3);
  };
}

#endif
