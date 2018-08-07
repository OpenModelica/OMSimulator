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

#include "Solver.h"

#include "Connector.h"
#include "FMUWrapper.h"
#include "Logging.h"
#include "Option.h"
#include "Scope.h"
#include "Variable.h"
#include "ssd/Tags.h"

#include <fmilib.h>
#include <JM/jm_portability.h>

#include "cvode/cvode.h"             /* prototypes for CVODE fcts., consts. */
#include "nvector/nvector_serial.h"  /* serial N_Vector types, fcts., macros */
#include "cvode/cvode_dense.h"       /* prototype for CVDense */
#include "sundials/sundials_dense.h" /* definitions DlsMat DENSE_ELEM */
#include "sundials/sundials_types.h" /* definition of type realtype */

int oms2::cvode_rhs(realtype t, N_Vector y, N_Vector ydot, void *user_data)
{
  //std::cout << "\n[oms2::cvode_rhs] t=" << t << std::endl;
  Solver *solver = (Solver*)user_data;

  // update states in FMUs
  for (int j=0, k=0; j < solver->fmus.size(); ++j)
  {
    for (size_t i=0; i < solver->nStates[j]; ++i, ++k)
      solver->states[j][i] = NV_Ith_S(y, k);
  }
  //std::cout << "[oms2::cvode_rhs] y" << std::endl;
  //N_VPrint_Serial(y);

  // set states
  fmi2_status_t fmistatus;
  for (int j=0; j < solver->fmus.size(); ++j)
  {
    fmistatus = fmi2_import_set_continuous_states(solver->fmus[j]->getFMU(), solver->states[j], solver->nStates[j]);
    if (fmi2_status_ok != fmistatus) logError("fmi2_import_set_continuous_states failed");
  }

  // get state derivatives
  for (int j=0, k=0; j < solver->fmus.size(); ++j)
  {
    fmistatus = fmi2_import_get_derivatives(solver->fmus[j]->getFMU(), solver->states_der[j], solver->nStates[j]);
    if (fmi2_status_ok != fmistatus) logError("fmi2_import_get_derivatives failed");

    for (size_t i=0; i < solver->nStates[j]; ++i, ++k)
      NV_Ith_S(ydot, k) = solver->states_der[j][i];
  }

  //std::cout << "[oms2::cvode_rhs] ydot" << std::endl;
  //N_VPrint_Serial(ydot);

  return 0;
}

oms2::Solver::Solver(const oms2::ComRef& name, oms_solver_enu_t solverMethod)
  : name(name), solverMethod(solverMethod)
{
}

oms2::Solver::~Solver()
{
}

oms_status_enu_t oms2::Solver::addFMU(oms2::FMUWrapper* fmu)
{
  if (!fmu)
    return logError("[oms2::Solver::addFMU] NULL pointer");

  if (fmu->getSolver())
    fmu->getSolver()->removeFMU(fmu->getName());

  fmus.push_back(fmu);
  fmu->setSolver(this);

  return oms_status_ok;
}

oms_status_enu_t oms2::Solver::removeFMU(const oms2::ComRef& fmuCref)
{
  for (int i=0; i<fmus.size(); ++i)
  {
    if (fmus[i]->getName() == fmuCref)
    {
      fmus[i]->setSolver(NULL);
      fmus[i] = fmus.back();
      fmus.pop_back();
    }
  }

  return oms_status_ok;
}

oms_status_enu_t oms2::Solver::initializeFMU(oms2::FMUWrapper* fmu)
{
  fmi2_event_info_t ei;
  ei.newDiscreteStatesNeeded = fmi2_false;
  ei.terminateSimulation = fmi2_false;
  ei.nominalsOfContinuousStatesChanged = fmi2_false;
  ei.valuesOfContinuousStatesChanged = fmi2_true;
  ei.nextEventTimeDefined = fmi2_false;
  ei.nextEventTime = -0.0;
  eventInfo.push_back(ei);

  nStates.push_back(0);
  nEventIndicators.push_back(0);

  terminateSimulation.push_back(fmi2_false);
  callEventUpdate.push_back(fmi2_false);
  states.push_back(NULL);
  states_der.push_back(NULL);
  states_nominal.push_back(NULL);
  event_indicators.push_back(NULL);
  event_indicators_prev.push_back(NULL);
  fmuTime.push_back(0.0);

  int fmuIndex = eventInfo.size() - 1;

  // TODO: FIX ME
  relativeTolerance = fmi2_import_get_default_experiment_tolerance(fmu->getFMU());
  if (relativeTolerance < 1e-60) relativeTolerance = 1e-4;
  logDebug("relative tolerance: " + std::to_string(relativeTolerance));

  if (oms_solver_internal == solverMethod)
  {
    // initialize co-simulation
    if (oms_fmi_kind_cs != fmu->getFMUInfo()->getKind() && oms_fmi_kind_me_and_cs != fmu->getFMUInfo()->getKind())
      return logError("[oms2::Solver::initializeFMU] FMU \"" + fmu->getName() + "\" doesn't provide any internal solver (no co-simulation)");
  }
  else
  {
    // initialize me-simulation
    if (oms_fmi_kind_me != fmu->getFMUInfo()->getKind() && oms_fmi_kind_me_and_cs != fmu->getFMUInfo()->getKind())
      return logError("[oms2::Solver::initializeFMU] FMU \"" + fmu->getName() + "\" doesn't provide me-simulation functionality");

    // fmi2_import_exit_initialization_mode leaves FMU in event mode
    doEventIteration(fmuIndex);

    fmi2_import_enter_continuous_time_mode(fmu->getFMU());

    nStates[fmuIndex] = fmi2_import_get_number_of_continuous_states(fmu->getFMU());
    nEventIndicators[fmuIndex] = fmi2_import_get_number_of_event_indicators(fmu->getFMU());

    logDebug(std::to_string(nStates[fmuIndex]) + " states");
    logDebug(std::to_string(nEventIndicators[fmuIndex]) + " event indicators");

    states[fmuIndex] = (double*)calloc(nStates[fmuIndex], sizeof(double));
    states_der[fmuIndex] = (double*)calloc(nStates[fmuIndex], sizeof(double));
    states_nominal[fmuIndex] = (double*)calloc(nStates[fmuIndex], sizeof(double));
    event_indicators[fmuIndex] = (double*)calloc(nEventIndicators[fmuIndex], sizeof(double));
    event_indicators_prev[fmuIndex] = (double*)calloc(nEventIndicators[fmuIndex], sizeof(double));

    fmi2_status_t fmistatus;
    if (nStates[fmuIndex] > 0)
    {
      // get states and state derivatives
      fmistatus = fmi2_import_get_continuous_states(fmu->getFMU(), states[fmuIndex], nStates[fmuIndex]);
      if (fmi2_status_ok != fmistatus) logError("fmi2_import_get_continuous_states failed");
      fmistatus = fmi2_import_get_derivatives(fmu->getFMU(), states_der[fmuIndex], nStates[fmuIndex]);
      if (fmi2_status_ok != fmistatus) logError("fmi2_import_get_derivatives failed");
      fmistatus = fmi2_import_get_nominals_of_continuous_states(fmu->getFMU(), states_nominal[fmuIndex], nStates[fmuIndex]);
      if (fmi2_status_ok != fmistatus) logError("fmi2_import_get_nominals_of_continuous_states failed");
    }
    fmistatus = fmi2_import_get_event_indicators(fmu->getFMU(), event_indicators[fmuIndex], nEventIndicators[fmuIndex]);
    if (fmi2_status_ok != fmistatus) logError("fmi2_import_get_event_indicators failed");
  }

  return oms_status_ok;
}

oms_status_enu_t oms2::Solver::initializeSolver(double startTime)
{
  if (0 == fmus.size())
  {
    logWarning("Solver \"" + name + "\" isn't associated with any FMU");
    return oms_status_ok;
  }

  time = startTime;
  for (int j=0, k=0; j < fmus.size(); ++j)
    fmuTime[j] = time;

  if (oms_solver_internal == solverMethod)
  {
    return oms_status_ok;
  }
  else if (oms_solver_explicit_euler == solverMethod)
  {
  }
  else if (oms_solver_cvode == solverMethod)
  {
    size_t n_states = 0;
    for (int i=0; i < fmus.size(); ++i)
      n_states += nStates[i];

    solverData.cvode.y = N_VNew_Serial(static_cast<long>(n_states));
    if (!solverData.cvode.y) logError("SUNDIALS_ERROR: N_VNew_Serial() failed - returned NULL pointer");
    for (int j=0, k=0; j < fmus.size(); ++j)
      for (size_t i=0; i < nStates[j]; ++i, ++k)
        NV_Ith_S(solverData.cvode.y, k) = states[j][i];
    //N_VPrint_Serial(solverData.cvode.y);

    solverData.cvode.abstol = N_VNew_Serial(static_cast<long>(n_states));
    if (!solverData.cvode.abstol) logError("SUNDIALS_ERROR: N_VNew_Serial() failed - returned NULL pointer");
    for (int j=0, k=0; j < fmus.size(); ++j)
      for (size_t i=0; i < nStates[j]; ++i, ++k)
        NV_Ith_S(solverData.cvode.abstol, k) = 0.01*relativeTolerance*states_nominal[j][i];
    //N_VPrint_Serial(solverData.cvode.abstol);

    // Call CVodeCreate to create the solver memory and specify the
    // Backward Differentiation Formula and the use of a Newton iteration
    solverData.cvode.mem = CVodeCreate(CV_BDF, CV_NEWTON);
    if (!solverData.cvode.mem) logError("SUNDIALS_ERROR: CVodeCreate() failed - returned NULL pointer");

    int flag = CVodeSetUserData(solverData.cvode.mem, (void*)this);
    if (flag < 0) logError("SUNDIALS_ERROR: CVodeSetUserData() failed with flag = " + std::to_string(flag));

    // Call CVodeInit to initialize the integrator memory and specify the
    // user's right hand side function in y'=cvode_rhs(t,y), the inital time T0, and
    // the initial dependent variable vector y.
    flag = CVodeInit(solverData.cvode.mem, cvode_rhs, time, solverData.cvode.y);
    if (flag < 0) logError("SUNDIALS_ERROR: CVodeInit() failed with flag = " + std::to_string(flag));

    // Call CVodeSVtolerances to specify the scalar relative tolerance
    // and vector absolute tolerances
    flag = CVodeSVtolerances(solverData.cvode.mem, relativeTolerance, solverData.cvode.abstol);
    if (flag < 0) logError("SUNDIALS_ERROR: CVodeSVtolerances() failed with flag = " + std::to_string(flag));

    // Call CVDense to specify the CVDENSE dense linear solver */
    flag = CVDense(solverData.cvode.mem, static_cast<long>(n_states));
    if (flag < 0) logError("SUNDIALS_ERROR: CVDense() failed with flag = " + std::to_string(flag));

    const fmi2_real_t stopTime = 1.0;
    double max_h = (stopTime - time) / 10.0;
    logInfo("maximum step size for '" + getName() + "': " + std::to_string(max_h));
    flag = CVodeSetMaxStep(solverData.cvode.mem, max_h);
    if (flag < 0) logError("SUNDIALS_ERROR: CVodeSetMaxStep() failed with flag = " + std::to_string(flag));

    // further settings from cpp runtime
    flag = CVodeSetInitStep(solverData.cvode.mem, 1e-6);        // INITIAL STEPSIZE
    if (flag < 0) logError("SUNDIALS_ERROR: CVodeSetInitStep() failed with flag = " + std::to_string(flag));
    flag = CVodeSetMaxOrd(solverData.cvode.mem, 5);             // MAXIMUM ORDER
    if (flag < 0) logError("SUNDIALS_ERROR: CVodeSetMaxOrd() failed with flag = " + std::to_string(flag));
    flag = CVodeSetMaxConvFails(solverData.cvode.mem, 100);     // MAXIMUM NUMBER OF NONLINEAR CONVERGENCE FAILURES
    if (flag < 0) logError("SUNDIALS_ERROR: CVodeSetMaxConvFails() failed with flag = " + std::to_string(flag));
    flag = CVodeSetStabLimDet(solverData.cvode.mem, TRUE);      // STABILITY DETECTION
    if (flag < 0) logError("SUNDIALS_ERROR: CVodeSetStabLimDet() failed with flag = " + std::to_string(flag));
    flag = CVodeSetMinStep(solverData.cvode.mem, 1e-12);        // MINIMUM STEPSIZE
    if (flag < 0) logError("SUNDIALS_ERROR: CVodeSetMinStep() failed with flag = " + std::to_string(flag));
    flag = CVodeSetMaxNonlinIters(solverData.cvode.mem, 5);     // MAXIMUM NUMBER OF ITERATIONS
    if (flag < 0) logError("SUNDIALS_ERROR: CVodeSetMaxNonlinIters() failed with flag = " + std::to_string(flag));
    flag = CVodeSetMaxErrTestFails(solverData.cvode.mem, 100);  // MAXIMUM NUMBER OF ERROR TEST FAILURES
    if (flag < 0) logError("SUNDIALS_ERROR: CVodeSetMaxErrTestFails() failed with flag = " + std::to_string(flag));
    flag = CVodeSetMaxNumSteps(solverData.cvode.mem, 1000);     // MAXIMUM NUMBER OF STEPS
    if (flag < 0) logError("SUNDIALS_ERROR: CVodeSetMaxNumSteps() failed with flag = " + std::to_string(flag));
  }

  return oms_status_error;
}

void oms2::Solver::doEventIteration(int i)
{
  if (i < 0 || i >= fmus.size())
  {
    logError("[oms2::Solver::doEventIteration] invalid input");
    return;
  }

  eventInfo[i].newDiscreteStatesNeeded = fmi2_true;
  eventInfo[i].terminateSimulation = fmi2_false;
  while (eventInfo[i].newDiscreteStatesNeeded && !eventInfo[i].terminateSimulation)
    fmi2_import_new_discrete_states(fmus[i]->getFMU(), &eventInfo[i]);
}

oms_status_enu_t oms2::Solver::doStep(double stopTime)
{
  if (0 == fmus.size())
    return oms_status_ok;

  fmi2_status_t fmistatus;
  double hdef = (stopTime-time) / 1.0;

  // HACK for certain FMUs
  for (auto& fmu : fmus)
    fmu->fetchAllVars();

  if (oms_solver_internal == solverMethod)
  {
    for (int i=0; i < fmus.size(); ++i)
    {
      while (fmuTime[i] < stopTime)
      {
        fmistatus = fmi2_import_do_step(fmus[i]->getFMU(), fmuTime[i], hdef, fmi2_true);
        fmuTime[i] += hdef;
      }
    }
    time = stopTime;
  }
  else // model exchange
  {
    fmi2_boolean_enu_t terminate_sim = fmi2_false;

    // main simulation loop
    while (time < stopTime && !terminate_sim)
    {
      fmi2_real_t tlast = time;
      fmi2_real_t tnext = time + hdef;

      // event handling
      for (int i=0; i < fmus.size(); ++i)
      {
        fmistatus = fmi2_import_set_time(fmus[i]->getFMU(), time);
        if (fmi2_status_ok != fmistatus) logError("fmi2_import_set_time failed");

        // swap event_indicators and event_indicators_prev
        {
          fmi2_real_t *temp = event_indicators[i];
          event_indicators[i] = event_indicators_prev[i];
          event_indicators_prev[i] = temp;

          fmistatus = fmi2_import_get_event_indicators(fmus[i]->getFMU(), event_indicators[i], nEventIndicators[i]);
          if (fmi2_status_ok != fmistatus) logError("fmi2_import_get_event_indicators failed");
        }

        // check if an event indicator has triggered
        int zero_crossing_event = 0;
        for (int k=0; k < nEventIndicators[i]; k++)
        {
          if ((event_indicators[i][k] > 0) != (event_indicators_prev[i][k] > 0))
          {
            zero_crossing_event = 1;
            break;
          }
        }

        // handle events
        if (callEventUpdate[i] || zero_crossing_event || (eventInfo[i].nextEventTimeDefined && time == eventInfo[i].nextEventTime))
        {
          fmistatus = fmi2_import_enter_event_mode(fmus[i]->getFMU());
          if (fmi2_status_ok != fmistatus) logError("fmi2_import_enter_event_mode failed");

          doEventIteration(i);

          fmistatus = fmi2_import_enter_continuous_time_mode(fmus[i]->getFMU());
          if (fmi2_status_ok != fmistatus) logError("fmi2_import_enter_continuous_time_mode failed");
          if (nStates[i] > 0)
          {
            fmistatus = fmi2_import_get_continuous_states(fmus[i]->getFMU(), states[i], nStates[i]);
            if (fmi2_status_ok != fmistatus) logError("fmi2_import_get_continuous_states failed");
            fmistatus = fmi2_import_get_derivatives(fmus[i]->getFMU(), states_der[i], nStates[i]);
            if (fmi2_status_ok != fmistatus) logError("fmi2_import_get_derivatives failed");
          }
          fmistatus = fmi2_import_get_event_indicators(fmus[i]->getFMU(), event_indicators[i], nEventIndicators[i]);
          if (fmi2_status_ok != fmistatus) logError("fmi2_import_get_event_indicators failed");

          if (oms_solver_cvode == solverMethod)
          {
            size_t offset=0;
            for (size_t k=0; k < i; ++k)
              offset += nStates[k];
            for (size_t k=0; k < nStates[k]; ++k)
              NV_Ith_S(solverData.cvode.y, offset+k) = states[i][k];
            int flag = CVodeReInit(solverData.cvode.mem, time, solverData.cvode.y);
            if (flag < 0) logError("SUNDIALS_ERROR: CVodeReInit() failed with flag = " + std::to_string(flag));
          }
        }

        // calculate next time step
        if (eventInfo[i].nextEventTimeDefined && (tnext >= eventInfo[i].nextEventTime))
          tnext = eventInfo[i].nextEventTime;
      }

      // adjust time step
      fmi2_real_t hcur = tnext - tlast;
      time = tnext;
      if (tnext > stopTime - hcur / 1e16)
      {
        // adjust final step size
        time = stopTime;
        hcur = time - tlast;
      }

      // integrate using specified solver
      if (oms_solver_internal == solverMethod)
      {
      }
      else if (oms_solver_explicit_euler == solverMethod)
      {
        for (int i=0; i < fmus.size(); ++i)
        {
          if (0 == nStates[i])
            continue;

          // get state derivatives
          fmistatus = fmi2_import_get_derivatives(fmus[i]->getFMU(), states_der[i], nStates[i]);
          if (fmi2_status_ok != fmistatus) logError("fmi2_import_get_derivatives failed");

          for (int k = 0; k < nStates[i]; k++)
            states[i][k] = states[i][k] + hcur*states_der[i][k];

          // set states
          fmistatus = fmi2_import_set_continuous_states(fmus[i]->getFMU(), states[i], nStates[i]);
          if (fmi2_status_ok != fmistatus) logError("fmi2_import_set_continuous_states failed");
        }
      }
      else if (oms_solver_cvode == solverMethod)
      {
        double cvode_time = tlast;
        int flag = CVode(solverData.cvode.mem, tnext, solverData.cvode.y, &cvode_time, CV_NORMAL);
        if (flag < 0) logError("SUNDIALS_ERROR: CVode() failed with flag = " + std::to_string(flag));

        // set states
        for (int j=0, k=0; j < fmus.size(); ++j)
        {
          for (size_t i=0; i < nStates[j]; ++i, ++k)
            states[j][i] = NV_Ith_S(solverData.cvode.y, k);

          fmistatus = fmi2_import_set_continuous_states(fmus[j]->getFMU(), states[j], nStates[j]);
          if (fmi2_status_ok != fmistatus) logError("fmi2_import_set_continuous_states failed");
        }

        // set time
        time = cvode_time;
      }
      else
        logError("Unknown solver method");

      // step is complete
      for (int i=0; i < fmus.size(); ++i)
      {
        fmistatus = fmi2_import_completed_integrator_step(fmus[i]->getFMU(), fmi2_true, &callEventUpdate[i], &terminateSimulation[i]);
        if (fmi2_status_ok != fmistatus) logError("fmi2_import_completed_integrator_step failed");
      }
    }
    time = stopTime;
  }

  return oms_status_ok;
}

oms_status_enu_t oms2::Solver::freeSolver()
{
  if (0 == fmus.size())
    return oms_status_ok;

  if (oms_solver_internal == solverMethod)
  {
    // CS
  }
  else
  {
    // free solver data (ME)
    if (oms_solver_explicit_euler == solverMethod)
    {
    }
    else if (oms_solver_cvode == solverMethod)
    {
      long int nst, nfe, nsetups, nni, ncfn, netf;
      int flag;

      flag = CVodeGetNumSteps(solverData.cvode.mem, &nst);
      if (flag < 0) logError("SUNDIALS_ERROR: CVodeGetNumSteps() failed with flag = " + std::to_string(flag));
      flag = CVodeGetNumRhsEvals(solverData.cvode.mem, &nfe);
      if (flag < 0) logError("SUNDIALS_ERROR: CVodeGetNumRhsEvals() failed with flag = " + std::to_string(flag));
      flag = CVodeGetNumLinSolvSetups(solverData.cvode.mem, &nsetups);
      if (flag < 0) logError("SUNDIALS_ERROR: CVodeGetNumLinSolvSetups() failed with flag = " + std::to_string(flag));
      flag = CVodeGetNumErrTestFails(solverData.cvode.mem, &netf);
      if (flag < 0) logError("SUNDIALS_ERROR: CVodeGetNumErrTestFails() failed with flag = " + std::to_string(flag));
      flag = CVodeGetNumNonlinSolvIters(solverData.cvode.mem, &nni);
      if (flag < 0) logError("SUNDIALS_ERROR: CVodeGetNumNonlinSolvIters() failed with flag = " + std::to_string(flag));
      flag = CVodeGetNumNonlinSolvConvFails(solverData.cvode.mem, &ncfn);
      if (flag < 0) logError("SUNDIALS_ERROR: CVodeGetNumNonlinSolvConvFails() failed with flag = " + std::to_string(flag));

      std::string msg = "Final Statistics for '" + getName() + "':\n";
      msg += "NumSteps = " + std::to_string(nst) + " NumRhsEvals  = " + std::to_string(nfe) + " NumLinSolvSetups = " + std::to_string(nsetups) + "\n";
      msg += "NumNonlinSolvIters = " + std::to_string(nni) + " NumNonlinSolvConvFails = " + std::to_string(ncfn) + " NumErrTestFails = " + std::to_string(netf);
      logInfo(msg);

      N_VDestroy_Serial(solverData.cvode.y);
      N_VDestroy_Serial(solverData.cvode.abstol);
      CVodeFree(&(solverData.cvode.mem));
    }
    else
      logError("Unknown solver method");

    // free common data
    for (int i=0; i<fmus.size(); ++i)
    {
      if (states[i]) free(states[i]); states[i]=NULL;
      if (states_der[i]) free(states_der[i]); states_der[i]=NULL;
      if (states_nominal[i]) free(states_nominal[i]); states_nominal[i]=NULL;
      if (event_indicators[i]) free(event_indicators[i]); event_indicators[i]=NULL;
      if (event_indicators_prev[i]) free(event_indicators_prev[i]); event_indicators_prev[i]=NULL;
    }
    states.clear();
    states_der.clear();
    states_nominal.clear();
    event_indicators.clear();
    event_indicators_prev.clear();

    fmuTime.clear();
    nStates.clear();
    nEventIndicators.clear();
    eventInfo.clear();
    callEventUpdate.clear();
    terminateSimulation.clear();
  }

  return oms_status_ok;
}
