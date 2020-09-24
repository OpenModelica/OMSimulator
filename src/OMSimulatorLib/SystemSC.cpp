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

#include "SystemSC.h"

#include "Component.h"
#include "ComponentFMUME.h"
#include "ComponentTable.h"
#include "Flags.h"
#include "Model.h"
#include "Types.h"
#include "ssd/Tags.h"
#include "cvode/cvode.h"                /* prototypes for CVODE fcts., consts. */
#include "nvector/nvector_serial.h"     /* serial N_Vector types, fcts., macros */
#include <sunlinsol/sunlinsol_dense.h>  /* Default dense linear solver */
#include "sundials/sundials_dense.h"    /* definitions DlsMat DENSE_ELEM */
#include "sundials/sundials_types.h"    /* definition of type realtype */

int oms::cvode_rhs(realtype t, N_Vector y, N_Vector ydot, void* user_data)
{
  //std::cout << "\n[oms::cvode_rhs] t=" << t << std::endl;
  SystemSC* system = (SystemSC*)user_data;
  oms_status_enu_t status;

  // update states in FMUs
  for (int i=0, j=0; i < system->fmus.size(); ++i)
  {
    if (0 == system->nStates[i])
      continue;

    for (int k = 0; k < system->nStates[i]; k++, j++)
      system->states[i][k] = NV_Ith_S(y, j);

    // set states
    status = system->fmus[i]->setContinuousStates(system->states[i]);
    if (oms_status_ok != status) return status;
  }
  //std::cout << "[oms::cvode_rhs] y" << std::endl;
  //N_VPrint_Serial(y);

  system->updateInputs(system->outputsGraph);

  // get state derivatives
  for (int j=0, k=0; j < system->fmus.size(); ++j)
  {
    if (0 == system->nStates[j])
      continue;

    // get state derivatives
    status = system->fmus[j]->getDerivatives(system->states_der[j]);
    if (oms_status_ok != status) return status;

    for (size_t i=0; i < system->nStates[j]; ++i, ++k)
      NV_Ith_S(ydot, k) = system->states_der[j][i];
  }

  return 0;
}

oms::SystemSC::SystemSC(const ComRef& cref, Model* parentModel, System* parentSystem)
  : oms::System(cref, oms_system_sc, parentModel, parentSystem, oms_solver_sc_cvode)
{
}

oms::SystemSC::~SystemSC()
{
}

oms::System* oms::SystemSC::NewSystem(const oms::ComRef& cref, oms::Model* parentModel, oms::System* parentSystem)
{
  if (!cref.isValidIdent())
  {
    logError_InvalidIdent(cref);
    return NULL;
  }

  if ((parentModel && parentSystem) || (!parentModel && !parentSystem))
  {
    logError_InternalError;
    return NULL;
  }

  System* system = new SystemSC(cref, parentModel, parentSystem);
  return system;
}

std::string oms::SystemSC::getSolverName() const
{
  switch (solverMethod)
  {
    case oms_solver_sc_explicit_euler:
      return std::string("euler");
    case oms_solver_sc_cvode:
      return std::string("cvode");
  }

  return std::string("unknown");
}

oms_status_enu_t oms::SystemSC::setSolverMethod(std::string solver)
{
  if (std::string("euler") == solver)
    solverMethod = oms_solver_sc_explicit_euler;
  else if (std::string("cvode") == solver)
    solverMethod = oms_solver_sc_cvode;
  else
    return oms_status_error;

  return oms_status_ok;
}

oms_status_enu_t oms::SystemSC::exportToSSD_SimulationInformation(pugi::xml_node& node) const
{
  pugi::xml_node node_annotations = node.append_child(oms::ssp::Draft20180219::ssd::annotations);
  pugi::xml_node node_annotation = node_annotations.append_child(oms::ssp::Version1_0::ssc::annotation);
  node_annotation.append_attribute("type") = oms::ssp::Draft20180219::annotation_type;

  /* ssd:SimulationInformation should be added as vendor specific annotations from Version 1.0 */
  pugi::xml_node node_simulation_information = node_annotation.append_child(oms::ssp::Version1_0::simulation_information);

  pugi::xml_node node_solver = node_simulation_information.append_child(oms::ssp::Version1_0::VariableStepSolver);
  node_solver.append_attribute("description") = getSolverName().c_str();
  node_solver.append_attribute("absoluteTolerance") = std::to_string(absoluteTolerance).c_str();
  node_solver.append_attribute("relativeTolerance") = std::to_string(relativeTolerance).c_str();
  node_solver.append_attribute("minimumStepSize") = std::to_string(minimumStepSize).c_str();
  node_solver.append_attribute("maximumStepSize") = std::to_string(maximumStepSize).c_str();
  node_solver.append_attribute("initialStepSize") = std::to_string(initialStepSize).c_str();

  return oms_status_ok;
}

oms_status_enu_t oms::SystemSC::importFromSSD_SimulationInformation(const pugi::xml_node& node, const std::string& sspVersion)
{
  std::string solverName = "";
  const char* VariableStepSolver = "";

  pugi::xml_node variableStepSolver = node.child(oms::ssp::Version1_0::VariableStepSolver);
  if (variableStepSolver)
  {
    solverName = variableStepSolver.attribute("description").as_string();
    VariableStepSolver = oms::ssp::Version1_0::VariableStepSolver;
  }
  else
  {
    solverName = node.child("VariableStepSolver").attribute("description").as_string();
    VariableStepSolver = "VariableStepSolver";
  }

  if (oms_status_ok != setSolverMethod(solverName))
    return oms_status_error;

  absoluteTolerance = node.child(VariableStepSolver).attribute("absoluteTolerance").as_double();
  relativeTolerance = node.child(VariableStepSolver).attribute("relativeTolerance").as_double();
  minimumStepSize = node.child(VariableStepSolver).attribute("minimumStepSize").as_double();
  maximumStepSize = node.child(VariableStepSolver).attribute("maximumStepSize").as_double();
  initialStepSize = node.child(VariableStepSolver).attribute("initialStepSize").as_double();
  return oms_status_ok;
}

oms_status_enu_t oms::SystemSC::instantiate()
{
  time = getModel()->getStartTime();

  // there shouldn't be any subsystem
  for (const auto& subsystem : getSubSystems())
    if (oms_status_ok != subsystem.second->instantiate())
      return oms_status_error;

  size_t n_states = 0;
  for (const auto& component : getComponents())
  {
    if (oms_status_ok != component.second->instantiate())
      return oms_status_error;

    if (component.second->getType() == oms_component_fmu)
    {
      fmus.push_back(dynamic_cast<ComponentFMUME*>(component.second));

      callEventUpdate.push_back(fmi2_false);
      terminateSimulation.push_back(fmi2_false);
      nStates.push_back(fmus.back()->getNumberOfContinuousStates());
      n_states += nStates.back();
      nEventIndicators.push_back(fmus.back()->getNumberOfEventIndicators());

      states.push_back((double*)calloc(nStates.back(), sizeof(double)));
      states_der.push_back((double*)calloc(nStates.back(), sizeof(double)));
      states_nominal.push_back((double*)calloc(nStates.back(), sizeof(double)));
      event_indicators.push_back((double*)calloc(nEventIndicators.back(), sizeof(double)));
      event_indicators_prev.push_back((double*)calloc(nEventIndicators.back(), sizeof(double)));
    }
  }

  if (n_states == 0)
  {
    solverMethod = oms_solver_sc_explicit_euler;
    logInfo("model doesn't contain any continuous state");
  }

  if (oms_solver_sc_explicit_euler == solverMethod)
    ;
  else if (oms_solver_sc_cvode == solverMethod)
    solverData.cvode.mem = NULL;
  else
    return logError_InternalError;

  return oms_status_ok;
}

oms_status_enu_t oms::SystemSC::initialize()
{
  clock.reset();
  CallClock callClock(clock);

  if (oms_status_ok != updateDependencyGraphs())
    return oms_status_error;

  if (oms_status_ok != updateInputs(initialUnknownsGraph))
    return oms_status_error;

  for (const auto& subsystem : getSubSystems())
    if (oms_status_ok != subsystem.second->initialize())
      return oms_status_error;

  for (const auto& component : getComponents())
    if (oms_status_ok != component.second->initialize())
      return oms_status_error;

  oms_status_enu_t status;
  for (size_t i=0; i<fmus.size(); ++i)
  {
    // get states and state derivatives
    if (fmus[i]->getNumberOfContinuousStates() > 0)
    {
      status = fmus[i]->getContinuousStates(states[i]);
      if (oms_status_ok != status) return status;
      status = fmus[i]->getDerivatives(states_der[i]);
      if (oms_status_ok != status) return status;
      status = fmus[i]->getNominalsOfContinuousStates(states_nominal[i]);
      if (oms_status_ok != status) return status;
    }
    if (fmus[i]->getNumberOfEventIndicators() > 0)
    {
      status = fmus[i]->getEventindicators(event_indicators[i]);
      if (oms_status_ok != status) return status;
    }
  }

  if (oms_solver_sc_cvode == solverMethod)
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
    solverData.cvode.mem = CVodeCreate(CV_BDF);
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

    // Call N_VNew_Serial and SUNDenseMatrix to generate dense vector abd natrix for lin. solver module
    solverData.cvode.liny = N_VNew_Serial(n_states);
    if (solverData.cvode.liny == NULL) logError("SUNDIALS_ERROR: N_VNew_Serial() failed");
    solverData.cvode.J = SUNDenseMatrix(n_states, n_states);
    if (solverData.cvode.J == NULL) logError("SUNDIALS_ERROR: N_VNew_Serial() failed");

    // Call SUNLinSol_Dense to creat linear solver object
    solverData.cvode.linSol = SUNLinSol_Dense(solverData.cvode.liny, solverData.cvode.J);
    if (solverData.cvode.linSol == NULL) logError("SUNDIALS_ERROR: SUNLinSol_Dense() failed");

    // Call CVodeSetLinearSolver to set the dense linear solver */
    flag = CVodeSetLinearSolver(solverData.cvode.mem, solverData.cvode.linSol, solverData.cvode.J);
    if (flag < 0) logError("SUNDIALS_ERROR: CVDense() failed with flag = " + std::to_string(flag));

    const fmi2_real_t stopTime = 1.0;
    double max_h = (stopTime - time) / 10.0;
    logInfo("maximum step size for '" + std::string(getFullCref()) + "': " + std::to_string(max_h));
    flag = CVodeSetMaxStep(solverData.cvode.mem, max_h);
    if (flag < 0) logError("SUNDIALS_ERROR: CVodeSetMaxStep() failed with flag = " + std::to_string(flag));

    // further settings from cpp runtime
    flag = CVodeSetInitStep(solverData.cvode.mem, 1e-6);        // INITIAL STEPSIZE
    if (flag < 0) logError("SUNDIALS_ERROR: CVodeSetInitStep() failed with flag = " + std::to_string(flag));
    flag = CVodeSetMaxOrd(solverData.cvode.mem, 5);             // MAXIMUM ORDER
    if (flag < 0) logError("SUNDIALS_ERROR: CVodeSetMaxOrd() failed with flag = " + std::to_string(flag));
    flag = CVodeSetMaxConvFails(solverData.cvode.mem, 100);     // MAXIMUM NUMBER OF NONLINEAR CONVERGENCE FAILURES
    if (flag < 0) logError("SUNDIALS_ERROR: CVodeSetMaxConvFails() failed with flag = " + std::to_string(flag));
    flag = CVodeSetStabLimDet(solverData.cvode.mem, true);      // STABILITY DETECTION
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

  return oms_status_ok;
}

oms_status_enu_t oms::SystemSC::terminate()
{
  for (const auto& subsystem : getSubSystems())
    if (oms_status_ok != subsystem.second->terminate())
      return oms_status_error;

  for (const auto& component : getComponents())
    if (oms_status_ok != component.second->terminate())
      return oms_status_error;

  if (oms_solver_sc_cvode == solverMethod && solverData.cvode.mem)
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

    std::string msg = "Final Statistics for '" + std::string(getFullCref()) + "':\n";
    msg += "NumSteps = " + std::to_string(nst) + " NumRhsEvals  = " + std::to_string(nfe) + " NumLinSolvSetups = " + std::to_string(nsetups) + "\n";
    msg += "NumNonlinSolvIters = " + std::to_string(nni) + " NumNonlinSolvConvFails = " + std::to_string(ncfn) + " NumErrTestFails = " + std::to_string(netf);
    logInfo(msg);

    SUNMatDestroy(solverData.cvode.J);
    N_VDestroy(solverData.cvode.liny);
    SUNLinSolFree(solverData.cvode.linSol);
    N_VDestroy_Serial(solverData.cvode.y);
    N_VDestroy_Serial(solverData.cvode.abstol);
    CVodeFree(&(solverData.cvode.mem));
    solverData.cvode.mem = NULL;
  }

  for (size_t i=0; i<fmus.size(); ++i)
  {
    free(states[i]);
    free(states_der[i]);
    free(states_nominal[i]);
    free(event_indicators[i]);
    free(event_indicators_prev[i]);
  }

  fmus.clear();
  callEventUpdate.clear();
  terminateSimulation.clear();
  nStates.clear();
  nEventIndicators.clear();
  states.clear();
  states_der.clear();
  states_nominal.clear();
  event_indicators.clear();
  event_indicators_prev.clear();

  return oms_status_ok;
}

oms_status_enu_t oms::SystemSC::reset()
{
  for (const auto& subsystem : getSubSystems())
    if (oms_status_ok != subsystem.second->reset())
      return oms_status_error;

  for (const auto& component : getComponents())
    if (oms_status_ok != component.second->reset())
      return oms_status_error;

  time = getModel()->getStartTime();

  if (oms_solver_sc_cvode == solverMethod && solverData.cvode.mem)
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

    std::string msg = "Final Statistics for '" + std::string(getFullCref()) + "':\n";
    msg += "NumSteps = " + std::to_string(nst) + " NumRhsEvals  = " + std::to_string(nfe) + " NumLinSolvSetups = " + std::to_string(nsetups) + "\n";
    msg += "NumNonlinSolvIters = " + std::to_string(nni) + " NumNonlinSolvConvFails = " + std::to_string(ncfn) + " NumErrTestFails = " + std::to_string(netf);
    logInfo(msg);

    N_VDestroy_Serial(solverData.cvode.y);
    N_VDestroy_Serial(solverData.cvode.abstol);
    CVodeFree(&(solverData.cvode.mem));
    solverData.cvode.mem = NULL;
  }

  return oms_status_ok;
}

oms_status_enu_t oms::SystemSC::stepUntil(double stopTime, void (*cb)(const char* ident, double time, oms_status_enu_t status))
{
  CallClock callClock(clock);

  ComRef modelName = this->getModel()->getCref();
  fmi2_status_t fmistatus;
  oms_status_enu_t status;
  double hdef = maximumStepSize;

  fmi2_boolean_enu_t terminate_sim = fmi2_false;
  double startTime=time;

  if (Flags::ProgressBar())
    logInfo("stepUntil [" + std::to_string(startTime) + "; " + std::to_string(stopTime) + "]");

  // main simulation loop
  while (time < stopTime && !terminate_sim)
  {
    fmi2_real_t tlast = time;
    fmi2_real_t tnext = time + hdef;

    // event handling
    for (int i=0; i < fmus.size(); ++i)
    {
      fmistatus = fmi2_import_set_time(fmus[i]->getFMU(), time);
      if (fmi2_status_ok != fmistatus) logError_FMUCall("fmi2_import_set_time", fmus[i]);

      // swap event_indicators and event_indicators_prev
      {
        fmi2_real_t *temp = event_indicators[i];
        event_indicators[i] = event_indicators_prev[i];
        event_indicators_prev[i] = temp;

        fmistatus = fmi2_import_get_event_indicators(fmus[i]->getFMU(), event_indicators[i], nEventIndicators[i]);
        if (fmi2_status_ok != fmistatus) logError_FMUCall("fmi2_import_get_event_indicators", fmus[i]);
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
      if (callEventUpdate[i] || zero_crossing_event || (fmus[i]->getEventInfo()->nextEventTimeDefined && time == fmus[i]->getEventInfo()->nextEventTime))
      {
        logDebug("Event detected in FMU \"" + std::string(fmus[i]->getFullCref()) + "\" at time=" + std::to_string(time));

        // emit the left limit of the event (if it hasn't already been emitted)
        if (Flags::EmitEvents() && isTopLevelSystem())
          getModel()->emit(time, false);

        fmistatus = fmi2_import_enter_event_mode(fmus[i]->getFMU());
        if (fmi2_status_ok != fmistatus) logError_FMUCall("fmi2_import_enter_event_mode", fmus[i]);

        fmus[i]->doEventIteration();

        fmistatus = fmi2_import_enter_continuous_time_mode(fmus[i]->getFMU());
        if (fmi2_status_ok != fmistatus) logError_FMUCall("fmi2_import_enter_continuous_time_mode", fmus[i]);
        if (nStates[i] > 0)
        {
          status = fmus[i]->getContinuousStates(states[i]);
          if (oms_status_ok != status) return status;
          status = fmus[i]->getDerivatives(states_der[i]);
          if (oms_status_ok != status) return status;
        }
        status = fmus[i]->getEventindicators(event_indicators[i]);
        if (oms_status_ok != status) return status;

        if (oms_solver_sc_cvode == solverMethod)
        {
          size_t offset=0;
          for (size_t k=0; k < i; ++k)
            offset += nStates[k];
          for (size_t k=0; k < nStates[i]; ++k)
            NV_Ith_S(solverData.cvode.y, offset+k) = states[i][k];
          int flag = CVodeReInit(solverData.cvode.mem, time, solverData.cvode.y);
          if (flag < 0) logError("SUNDIALS_ERROR: CVodeReInit() failed with flag = " + std::to_string(flag));
        }

        // emit the right limit of the event
        updateInputs(outputsGraph);
        if (Flags::EmitEvents() && isTopLevelSystem())
          getModel()->emit(time, true);

        // restart event iteration from the beginning
        i=-1;
        continue;
      }

      // calculate next time step
      if (fmus[i]->getEventInfo()->nextEventTimeDefined && (tnext >= fmus[i]->getEventInfo()->nextEventTime))
        tnext = fmus[i]->getEventInfo()->nextEventTime;
    }

    // adjust time step
    fmi2_real_t hcur = tnext - tlast;
    if (tnext > stopTime - hcur / 1e16)
    {
      // adjust final step size
      tnext = stopTime;
      hcur = tnext - tlast;
    }

    // integrate using specified solver
    if (oms_solver_sc_explicit_euler == solverMethod)
    {
      for (int i=0; i < fmus.size(); ++i)
      {
        if (0 == nStates[i])
          continue;

        // get state derivatives
        status = fmus[i]->getDerivatives(states_der[i]);
        if (oms_status_ok != status) return status;

        for (int k = 0; k < nStates[i]; k++)
          states[i][k] = states[i][k] + hcur*states_der[i][k];

        // set states
        status = fmus[i]->setContinuousStates(states[i]);
        if (oms_status_ok != status) return status;
      }

      // set time
      time = tnext;
    }
    else if (oms_solver_sc_cvode == solverMethod)
    {
      double cvode_time = tlast;
      int flag = CVode(solverData.cvode.mem, tnext, solverData.cvode.y, &cvode_time, CV_NORMAL);
      if (flag < 0) logError("SUNDIALS_ERROR: CVode() failed with flag = " + std::to_string(flag));

      // set states
      for (int i=0, j=0; i < fmus.size(); ++i)
      {
        if (0 == nStates[i])
          continue;

        for (int k = 0; k < nStates[i]; k++, j++)
          states[i][k] = NV_Ith_S(solverData.cvode.y, j);

        // set states
        status = fmus[i]->setContinuousStates(states[i]);
        if (oms_status_ok != status) return status;
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
      if (fmi2_status_ok != fmistatus) logError_FMUCall("fmi2_import_completed_integrator_step", fmus[i]);
    }

    updateInputs(outputsGraph);
    if (isTopLevelSystem())
      getModel()->emit(time);

    if (cb)
      cb(modelName.c_str(), time, oms_status_ok);

    if (Flags::ProgressBar())
      Log::ProgressBar(startTime, stopTime, time);

    if (isTopLevelSystem() && getModel()->cancelSimulation())
    {
      cb(modelName.c_str(), time, oms_status_discard);
      return oms_status_discard;
    }
  }

  time = stopTime;
  if (Flags::ProgressBar())
  {
    Log::ProgressBar(startTime, stopTime, time);
    Log::TerminateBar();
  }
  return oms_status_ok;
}

oms_status_enu_t oms::SystemSC::updateInputs(DirectedGraph& graph)
{
  CallClock callClock(clock);

  if (getModel()->validState(oms_modelState_simulation))
  {
    // update time
    for (const auto& component : getComponents())
    {
      switch (component.second->getType())
      {
        case oms_component_fmu:
          if (fmi2_status_ok != fmi2_import_set_time(dynamic_cast<ComponentFMUME*>(component.second)->getFMU(), time))
            logError_FMUCall("fmi2_import_set_time", dynamic_cast<ComponentFMUME*>(component.second));
          break;
        case oms_component_table:
          dynamic_cast<ComponentTable*>(component.second)->stepUntil(time);
          break;
        default:
          break;
      }
    }
  }

  // input := output
  const std::vector< std::vector< std::pair<int, int> > >& sortedConnections = graph.getSortedConnections();

  // TODO: Move to a different place, maybe in System.cpp, and only call function
  // Instantiate loops
  if (!loopsInstantiated)
  {
    int systCount = 0;
    for(int i=0; i<sortedConnections.size(); i++)
    {
      if (sortedConnections[i].size() > 1)
      {
        addAlgLoop(systCount, sortedConnections[i]);
        systCount++;
      }
    }
    loopsInstantiated = true;
  }

  for(int i=0; i<sortedConnections.size(); i++)
  {
    if (sortedConnections[i].size() == 1)
    {
      int output = sortedConnections[i][0].first;
      int input = sortedConnections[i][0].second;

      if (graph.getNodes()[input].getType() == oms_signal_type_real)
      {
        double value = 0.0;
        if (oms_status_ok != getReal(graph.getNodes()[output].getName(), value)) return oms_status_error;
        if (oms_status_ok != setReal(graph.getNodes()[input].getName(), value)) return oms_status_error;
      }
      else if (graph.getNodes()[input].getType() == oms_signal_type_integer)
      {
        int value = 0.0;
        if (oms_status_ok != getInteger(graph.getNodes()[output].getName(), value)) return oms_status_error;
        if (oms_status_ok != setInteger(graph.getNodes()[input].getName(), value)) return oms_status_error;
      }
      else if (graph.getNodes()[input].getType() == oms_signal_type_boolean)
      {
        bool value = 0.0;
        if (oms_status_ok != getBoolean(graph.getNodes()[output].getName(), value)) return oms_status_error;
        if (oms_status_ok != setBoolean(graph.getNodes()[input].getName(), value)) return oms_status_error;
      }
      else
        return logError_InternalError;
    }
    else
    {
      if (oms_status_ok != solveAlgLoop(graph, i)) return oms_status_error;
    }
  }
  return oms_status_ok;
}
