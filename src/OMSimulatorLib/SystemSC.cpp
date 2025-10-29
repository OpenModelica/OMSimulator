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
#include "ssd/Tags.h"

#include <algorithm>
#include <cstring>
#include <sstream>

int oms::cvode_rhs(realtype t, N_Vector y, N_Vector ydot, void* user_data)
{
  SystemSC* system = (SystemSC*)user_data;
  oms_status_enu_t status;
  fmi2Status fmistatus;

  // update states in FMUs
  for (size_t i=0, j=0; i < system->fmus.size(); ++i)
  {
    system->fmus[i]->setTime(t);

    if (0 == system->nStates[i])
      continue;

    for (size_t k = 0; k < system->nStates[i]; k++, j++)
      system->states[i][k] = NV_Ith_S(y, j);

    // set states
    status = system->fmus[i]->setContinuousStates(system->states[i]);
    if (oms_status_ok != status) return status;
  }

  system->updateInputs(system->eventGraph);

  // get state derivatives
  for (size_t j=0, k=0; j < system->fmus.size(); ++j)
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

int oms::cvode_roots(realtype t, N_Vector y, realtype *gout, void *user_data)
{
  logDebug("cvode_roots at time " + std::to_string(t));
  SystemSC* system = (SystemSC*)user_data;
  oms_status_enu_t status;
  fmi2Status fmistatus;

  // update states in FMUs
  for (size_t i=0, j=0; i < system->fmus.size(); ++i)
  {
    system->fmus[i]->setTime(t);

    if (0 == system->nStates[i])
      continue;

    for (size_t k = 0; k < system->nStates[i]; k++, j++)
      system->states[i][k] = NV_Ith_S(y, j);

    // set states
    status = system->fmus[i]->setContinuousStates(system->states[i]);
    if (oms_status_ok != status) return status;
  }

  system->updateInputs(system->eventGraph);

  for (size_t i = 0, j=0; i < system->fmus.size(); ++i)
  {
    fmistatus = fmi2_getEventIndicators(system->fmus[i]->getFMU(), system->event_indicators[i], system->nEventIndicators[i]);
    if (fmi2OK != fmistatus) logError_FMUCall("fmi2_getEventIndicators", system->fmus[i]);

    for (size_t k=0; k < system->nEventIndicators[i]; k++, j++)
      gout[j] = system->event_indicators[i][k];
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
    default:
      return std::string("unknown");
  }
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
  std::ostringstream ssAbsoluteTolerance, ssRelativeTolerance, ssMinimumStepSize, ssMaximumStepSize, ssInitialStepSize;
  ssAbsoluteTolerance << absoluteTolerance;
  ssRelativeTolerance << relativeTolerance;
  ssMinimumStepSize << minimumStepSize;
  ssMaximumStepSize << maximumStepSize;
  ssInitialStepSize << initialStepSize;

  /* ssd:SimulationInformation should be added as vendor specific annotations from Version 1.0 */
  pugi::xml_node node_simulation_information = node.append_child(oms::ssp::Version1_0::simulation_information);

  pugi::xml_node node_solver = node_simulation_information.append_child(oms::ssp::Version1_0::VariableStepSolver);
  node_solver.append_attribute("description") = getSolverName().c_str();
  node_solver.append_attribute("absoluteTolerance") = ssAbsoluteTolerance.str().c_str();
  node_solver.append_attribute("relativeTolerance") = ssRelativeTolerance.str().c_str();
  node_solver.append_attribute("minimumStepSize") = ssMinimumStepSize.str().c_str();
  node_solver.append_attribute("maximumStepSize") = ssMaximumStepSize.str().c_str();
  node_solver.append_attribute("initialStepSize") = ssInitialStepSize.str().c_str();

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
  time = getModel().getStartTime();

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

      callEventUpdate.push_back(fmi2False);
      terminateSimulation.push_back(fmi2False);
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

  if (oms_status_ok != updateInputs(initializationGraph))
    return oms_status_error;

  for (const auto& subsystem : getSubSystems())
    if (oms_status_ok != subsystem.second->initialize())
      return oms_status_error;

  for (const auto& component : getComponents())
    if (oms_status_ok != component.second->initialize())
      return oms_status_error;

  oms_status_enu_t status;
  size_t n_event_indicators = 0;
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

      // Check if nominals are greater 0
      bool illegalNominals = false;
      for(size_t l=0; l<nStates[i]; l++)
      {
        if (states_nominal[i][l] <= 0)
        {
          if (Flags::ZeroNominal())
          {
            if (!illegalNominals)
              logWarning(std::string(fmus[i]->getFullCref()) + ": Illegal nominal value will be replaced with 1.0 because the flag --zeroNominal is used");
            illegalNominals = true;
            states_nominal[i][l] = 1.0;
          }
          else
            return logError(std::string(fmus[i]->getFullCref()) + ": Illegal nominal value provided by the FMU.");
        }
      }
    }
    n_event_indicators += nEventIndicators[i];
    if (fmus[i]->getNumberOfEventIndicators() > 0)
    {
      status = fmus[i]->getEventindicators(event_indicators[i]);
      if (oms_status_ok != status) return status;
    }
  }

  if (oms_solver_sc_cvode == solverMethod)
  {
    size_t n_states = 0;
    for (size_t i=0; i < fmus.size(); ++i)
      n_states += nStates[i];

    solverData.cvode.y = N_VNew_Serial(static_cast<long>(n_states));
    if (!solverData.cvode.y) logError("SUNDIALS_ERROR: N_VNew_Serial() failed - returned NULL pointer");
    for (size_t j=0, k=0; j < fmus.size(); ++j)
      for (size_t i=0; i < nStates[j]; ++i, ++k)
        NV_Ith_S(solverData.cvode.y, k) = states[j][i];
    //N_VPrint_Serial(solverData.cvode.y);

    solverData.cvode.abstol = N_VNew_Serial(static_cast<long>(n_states));
    if (!solverData.cvode.abstol) logError("SUNDIALS_ERROR: N_VNew_Serial() failed - returned NULL pointer");
    for (size_t j=0, k=0; j < fmus.size(); ++j)
      for (size_t i=0; i < nStates[j]; ++i, ++k)
        NV_Ith_S(solverData.cvode.abstol, k) = 0.01*absoluteTolerance*states_nominal[j][i];
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

    flag = CVodeRootInit(solverData.cvode.mem, n_event_indicators, cvode_roots);
    if (flag != CV_SUCCESS) logError("SUNDIALS_ERROR: CVodeRootInit() failed with flag = " + std::to_string(flag));

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

    logInfo("maximum step size for '" + std::string(getFullCref()) + "': " + std::to_string(maximumStepSize));
    flag = CVodeSetMaxStep(solverData.cvode.mem, maximumStepSize);
    if (flag < 0) logError("SUNDIALS_ERROR: CVodeSetMaxStep() failed with flag = " + std::to_string(flag));

    // further settings from cpp runtime
    flag = CVodeSetInitStep(solverData.cvode.mem, initialStepSize);                      // INITIAL STEPSIZE
    if (flag < 0) logError("SUNDIALS_ERROR: CVodeSetInitStep() failed with flag = " + std::to_string(flag));
    flag = CVodeSetMaxOrd(solverData.cvode.mem, Flags::CVODEMaxOrder());                 // MAXIMUM ORDER
    if (flag < 0) logError("SUNDIALS_ERROR: CVodeSetMaxOrd() failed with flag = " + std::to_string(flag));
    flag = CVodeSetMaxConvFails(solverData.cvode.mem, Flags::CVODEMaxNLSFailures());     // MAXIMUM NUMBER OF NONLINEAR CONVERGENCE FAILURES
    if (flag < 0) logError("SUNDIALS_ERROR: CVodeSetMaxConvFails() failed with flag = " + std::to_string(flag));
    flag = CVodeSetStabLimDet(solverData.cvode.mem, true);                               // STABILITY DETECTION
    if (flag < 0) logError("SUNDIALS_ERROR: CVodeSetStabLimDet() failed with flag = " + std::to_string(flag));
    flag = CVodeSetMinStep(solverData.cvode.mem, minimumStepSize);                       // MINIMUM STEPSIZE
    if (flag < 0) logError("SUNDIALS_ERROR: CVodeSetMinStep() failed with flag = " + std::to_string(flag));
    flag = CVodeSetMaxNonlinIters(solverData.cvode.mem, Flags::CVODEMaxNLSIterations()); // MAXIMUM NUMBER OF ITERATIONS
    if (flag < 0) logError("SUNDIALS_ERROR: CVodeSetMaxNonlinIters() failed with flag = " + std::to_string(flag));
    flag = CVodeSetMaxErrTestFails(solverData.cvode.mem, Flags::CVODEMaxErrTestFails()); // MAXIMUM NUMBER OF ERROR TEST FAILURES
    if (flag < 0) logError("SUNDIALS_ERROR: CVodeSetMaxErrTestFails() failed with flag = " + std::to_string(flag));
    flag = CVodeSetMaxNumSteps(solverData.cvode.mem, Flags::CVODEMaxSteps());            // MAXIMUM NUMBER OF STEPS
    if (flag < 0) logError("SUNDIALS_ERROR: CVodeSetMaxNumSteps() failed with flag = " + std::to_string(flag));
  }

  // Mark algebraic loops to be updated on next call
  forceLoopsToBeUpdated();

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
    N_VDestroy_Serial(solverData.cvode.liny);
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

  time = getModel().getStartTime();

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
    N_VDestroy_Serial(solverData.cvode.liny);
    SUNLinSolFree(solverData.cvode.linSol);
    N_VDestroy_Serial(solverData.cvode.y);
    N_VDestroy_Serial(solverData.cvode.abstol);
    CVodeFree(&(solverData.cvode.mem));
    solverData.cvode.mem = NULL;
  }

  return oms_status_ok;
}

oms_status_enu_t oms::SystemSC::doStep()
{
  switch(solverMethod)
  {
    case oms_solver_sc_explicit_euler:
      return doStepEuler(time + maximumStepSize);

    case oms_solver_sc_cvode:
      return doStepCVODE(time + maximumStepSize);

    default:
      return logError_InternalError;
  }
}

oms_status_enu_t oms::SystemSC::doStepEuler(double stopTime)
{
  fmi2Status fmistatus;
  oms_status_enu_t status;

  // Step 1: Initialize state variables and time
  const fmi2Real end_time = std::min(time + maximumStepSize, stopTime);
  const fmi2Real event_time_tolerance = 1e-4;

  logDebug("doStepEuler: " + std::to_string(time) + " -> " + std::to_string(end_time));

  // Step 2: Backup states for potential rollback
  std::vector<double *> states_backup;
  std::vector<double *> states_der_backup;

  for (size_t i = 0; i < fmus.size(); ++i)
  {
    if (nStates[i] > 0)
    {
      states_backup.push_back((double*)calloc(nStates[i], sizeof(double)));
      states_der_backup.push_back((double*)calloc(nStates[i], sizeof(double)));
    }
    else
    {
      states_backup.push_back(nullptr);
      states_der_backup.push_back(nullptr);
    }
  }
  for (size_t i = 0; i < fmus.size(); ++i)
  {
    if (nStates[i] > 0)
    {
      status = fmus[i]->getContinuousStates(states_backup[i]);
      if (oms_status_ok != status) return status;
      status = fmus[i]->getDerivatives(states_der_backup[i]);
      if (oms_status_ok != status) return status;
    }
    fmistatus = fmi2_getEventIndicators(fmus[i]->getFMU(), event_indicators_prev[i], nEventIndicators[i]);
    if (fmi2OK != fmistatus) logError_FMUCall("fmi2_getEventIndicators", fmus[i]);
  }

  fmi2Real step_size_adjustment = maximumStepSize;
  fmi2Real event_time = end_time;
  bool event_detected = false;

  fmi2Real tnext = end_time + 1.0;
  bool terminated = false;
  for (size_t i = 0; i < fmus.size(); ++i)
  {
    if (fmus[i]->getEventInfo()->nextEventTimeDefined && (tnext > fmus[i]->getEventInfo()->nextEventTime) && (time < fmus[i]->getEventInfo()->nextEventTime))
      tnext = fmus[i]->getEventInfo()->nextEventTime;

    if(fmus[i]->getEventInfo()->terminateSimulation)
    {
      logInfo("Simulation terminated by FMU " + std::string(fmus[i]->getFullCref()) + " at time " + std::to_string(time));
      getModel().setStopTime(time);
      terminated = true;
    }
  }

  // Step 3: Main integration loop
  while (time < end_time && !terminated)
  {
    if(tnext < event_time)
      event_time = tnext;

    step_size_adjustment *= 0.5; // reduce the step size in each iteration

    // a. Evaluate derivatives for each FMU
    const fmi2Real step_size = event_time - time;  // Substep size, do one step from current time to the event
    logDebug("step_size: " + std::to_string(step_size) + " | " + std::to_string(time) + " -> " + std::to_string(event_time));
    for (size_t i = 0; i < fmus.size(); ++i)
    {
      if (0 == nStates[i])
        continue;

      for (size_t k = 0; k < nStates[i]; ++k)
        states[i][k] = states_backup[i][k] + step_size * states_der_backup[i][k];

      status = fmus[i]->setContinuousStates(states[i]);
      if (oms_status_ok != status) return status;
    }

    // b. Event Detection
    event_detected = event_time == tnext;
    logDebug("Event detected: " + std::to_string(event_detected));
    for (size_t i = 0; i < fmus.size() && !event_detected; ++i)
    {
      fmistatus = fmi2_getEventIndicators(fmus[i]->getFMU(), event_indicators[i], nEventIndicators[i]);
      if (fmi2OK != fmistatus) logError_FMUCall("fmi2_getEventIndicators", fmus[i]);

      for (size_t k=0; k < nEventIndicators[i]; k++)
      {
        if ((event_indicators[i][k] > 0) != (event_indicators_prev[i][k] > 0))
        {
          logDebug("event detected");
          event_detected = true;
          break;
        }
      }
    }

    // c. Event handling and step advancement
    if (!event_detected)
    {
      logDebug("no event detected");

      if (event_time == end_time)
      {
        // Integrate normally to the end time if no events are ahead
        time = event_time;
        step_size_adjustment = maximumStepSize;

        // set time
        for (const auto& component : getComponents())
          component.second->setTime(time);

        for (size_t i = 0; i < fmus.size(); ++i)
        {
          fmistatus = fmi2_completedIntegratorStep(fmus[i]->getFMU(), fmi2True, &callEventUpdate[i], &terminateSimulation[i]);
          if (fmi2OK != fmistatus) return logError_FMUCall("fmi2_completedIntegratorStep", fmus[i]);
        }

        // emit the left limit of the event (if it hasn't already been emitted)
        updateInputs(simulationGraph); //pass the continuousTimeMode dependency graph which involves only connections of type Real
        if (isTopLevelSystem())
          getModel().emit(time, false);

        logDebug("integrate normally to the end time if no events are ahead");
      }
      else
      {
        // Advance to the tentative event time and check again
        event_time += step_size_adjustment;
        logDebug("advance to the tentative event time and check again");
      }
    }
    else
    {
      if (event_time == tnext || step_size_adjustment < event_time_tolerance)
      {
        logDebug("event found!!! " + std::to_string(event_time));

        // Event detected: Restore to last "safe" state and integrate directly to event time
        time = event_time;
        step_size_adjustment = maximumStepSize;
        event_time = end_time;

        // emit the left limit of the event (if it hasn't already been emitted)
        if (isTopLevelSystem())
          getModel().emit(time, false);

        // set time
        for (const auto& component : getComponents())
          component.second->setTime(time);

        // Enter event mode and handle discrete state updates for each FMU
        for (size_t i = 0; i < fmus.size(); ++i)
        {
          fmistatus = fmi2_completedIntegratorStep(fmus[i]->getFMU(), fmi2True, &callEventUpdate[i], &terminateSimulation[i]);
          if (fmi2OK != fmistatus) return logError_FMUCall("fmi2_completedIntegratorStep", fmus[i]);

          fmistatus = fmi2_enterEventMode(fmus[i]->getFMU());
          if (fmi2OK != fmistatus) logError_FMUCall("fmi2_enterEventMode", fmus[i]);

          fmus[i]->doEventIteration();

          fmistatus = fmi2_enterContinuousTimeMode(fmus[i]->getFMU());
          if (fmi2OK != fmistatus) logError_FMUCall("fmi2_enterContinuousTimeMode", fmus[i]);

          if (nStates[i] > 0)
          {
            status = fmus[i]->getContinuousStates(states_backup[i]);
            if (oms_status_ok != status) return status;
            status = fmus[i]->getDerivatives(states_der_backup[i]);
            if (oms_status_ok != status) return status;
          }

          status = fmus[i]->getEventindicators(event_indicators_prev[i]);
          if (oms_status_ok != status) return status;
        }

        // find next time event
        tnext = end_time + 1.234;
        for (size_t i = 0; i < fmus.size(); ++i)
        {
          if (fmus[i]->getEventInfo()->nextEventTimeDefined && (tnext > fmus[i]->getEventInfo()->nextEventTime) && (time < fmus[i]->getEventInfo()->nextEventTime))
            tnext = fmus[i]->getEventInfo()->nextEventTime;

          if(fmus[i]->getEventInfo()->terminateSimulation)
          {
            logInfo("Simulation terminated by FMU " + std::string(fmus[i]->getFullCref()) + " at time " + std::to_string(time));
            getModel().setStopTime(time);
            terminated = true;
          }
        }

        // emit the right limit of the event
        updateInputs(eventGraph);
        if (isTopLevelSystem())
          getModel().emit(time, true);
      }
      else
      {
        // Ok, event must be little earlier
        event_time -= step_size_adjustment;
      }
    }
  }

  for (size_t i=0; i < fmus.size(); ++i)
  {
    if (nStates[i] > 0)
    {
      free(states_backup[i]);
      free(states_der_backup[i]);
    }
  }

  return oms_status_ok;
}

oms_status_enu_t oms::SystemSC::doStepCVODE(double stopTime)
{
  fmi2Status fmistatus;
  oms_status_enu_t status;
  int flag = 0;
  bool tnext_is_event = false;

  const fmi2Real end_time = stopTime;

  // find next time event
  fmi2Real tnext = end_time+1.0;
  for (size_t i = 0; i < fmus.size(); ++i)
  {
    if (fmus[i]->getEventInfo()->nextEventTimeDefined && (tnext > fmus[i]->getEventInfo()->nextEventTime))
      tnext = fmus[i]->getEventInfo()->nextEventTime;

    if(fmus[i]->getEventInfo()->terminateSimulation)
    {
      logInfo("Simulation terminated by FMU " + std::string(fmus[i]->getFullCref()) + " at time " + std::to_string(time));
      getModel().setStopTime(time);
      time = end_time;
    }
  }

  tnext_is_event = tnext <= end_time;
  tnext = std::min(tnext, end_time);

  logDebug("tnext: " + std::to_string(tnext));

  //while (time < end_time)
  //{
    logDebug("CVode: " + std::to_string(time) + " -> " + std::to_string(end_time));
    for (size_t j=0, k=0; j < fmus.size(); ++j)
      for (size_t i=0; i < nStates[j]; ++i, ++k)
        NV_Ith_S(solverData.cvode.y, k) = states[j][i];

    // Advance integrator (to end of step or next root)
    double cvode_time = time;
    int task = tnext > time + maximumStepSize ? CV_ONE_STEP : CV_NORMAL;
    flag = CVode(solverData.cvode.mem, tnext, solverData.cvode.y, &cvode_time, task);
    if (flag < 0)
      return logError("SUNDIALS_ERROR: CVode() failed with flag = " + std::to_string(flag));
 
    // Sanity check, should not be triggered.
    // To avoid resorting to this, CV_NORMAL is used above when tnext is too close.
    if (cvode_time > tnext)
    {
      logWarning("SystemSC::doStepCVODE: Stopping time overstepped by CVODE");

      // This fails to do the necessary interpolation when the previous call has stopped at a root.
      // flag = CVode(solverData.cvode.mem, tnext, solverData.cvode.y, &cvode_time, CV_NORMAL);
      // if (flag < 0)
      //   return logError("SUNDIALS_ERROR: CVode() failed with flag = " + std::to_string(flag));

      // Issue with the approach below:
      // - Internal time of CVODE stays at previously returned value.
      // - This may cause it to skip a root.

      // Interpolate states to value at tnext
      int retval = CVodeGetDky(solverData.cvode.mem, tnext, 0, solverData.cvode.y);
      if (retval != CV_SUCCESS)
        return logError("SUNDIALS_ERROR: CVodeGetDky() failed with flag = " + std::to_string(retval));

      flag = CV_TSTOP_RETURN;
      cvode_time = tnext;
    }

    time = cvode_time;

    for (size_t i = 0, j=0; i < fmus.size(); ++i)
    {
      for (size_t k = 0; k < nStates[i]; k++, j++)
        states[i][k] = NV_Ith_S(solverData.cvode.y, j);

      // set states
      status = fmus[i]->setContinuousStates(states[i]);
      if (oms_status_ok != status) return status;
    }

    if (flag == CV_ROOT_RETURN || tnext_is_event && time == tnext)
    {
      logDebug("event found!!! " + std::to_string(time));

      // set time
      for (const auto& component : getComponents())
        component.second->setTime(time);

      for (size_t i = 0; i < fmus.size(); ++i)
      {
        fmistatus = fmi2_completedIntegratorStep(fmus[i]->getFMU(), fmi2True, &callEventUpdate[i], &terminateSimulation[i]);
        if (fmi2OK != fmistatus) return logError_FMUCall("fmi2_completedIntegratorStep", fmus[i]);
      }

      updateInputs(eventGraph);
      if (isTopLevelSystem())
        getModel().emit(time, false);

      // Enter event mode and handle discrete state updates for each FMU
      for (size_t i = 0; i < fmus.size(); ++i)
      {
        fmistatus = fmi2_enterEventMode(fmus[i]->getFMU());
        if (fmi2OK != fmistatus) logError_FMUCall("fmi2_enterEventMode", fmus[i]);

        fmus[i]->doEventIteration();

        fmistatus = fmi2_enterContinuousTimeMode(fmus[i]->getFMU());
        if (fmi2OK != fmistatus) logError_FMUCall("fmi2_enterContinuousTimeMode", fmus[i]);
      }

      // find next time event
      tnext = end_time+1.0;
      for (size_t i = 0; i < fmus.size(); ++i)
      {
        if (fmus[i]->getEventInfo()->nextEventTimeDefined && (tnext > fmus[i]->getEventInfo()->nextEventTime))
          tnext = fmus[i]->getEventInfo()->nextEventTime;

        if(fmus[i]->getEventInfo()->terminateSimulation)
        {
          logInfo("Simulation terminated by FMU " + std::string(fmus[i]->getFullCref()) + " at time " + std::to_string(time));
          getModel().setStopTime(time);
          time = end_time;
        }
      }

      tnext_is_event = tnext <= end_time;
      tnext = std::min(tnext, end_time);

      logDebug("tnext: " + std::to_string(tnext));

      // emit the right limit of the event
      updateInputs(eventGraph);
      if (isTopLevelSystem())
        getModel().emit(time, true);

      for (size_t i = 0; i < fmus.size(); ++i)
      {
        if (0 == nStates[i])
          continue;

        status = fmus[i]->getContinuousStates(states[i]);
        if (oms_status_ok != status) return status;
      }

      for (size_t j=0, k=0; j < fmus.size(); ++j)
        for (size_t i=0; i < nStates[j]; ++i, ++k)
          NV_Ith_S(solverData.cvode.y, k) = states[j][i];

      flag = CVodeReInit(solverData.cvode.mem, time, solverData.cvode.y);
      if (flag < 0) return logError("SUNDIALS_ERROR: CVodeReInit() failed with flag = " + std::to_string(flag));

      return oms_status_ok;
    }

    if (flag >= 0) // Some kind of success
    {
      logDebug("CVode completed successfully at t = " + std::to_string(time));

      // set time
      for (const auto& component : getComponents())
        component.second->setTime(time);

      for (size_t i = 0; i < fmus.size(); ++i)
      {
        fmistatus = fmi2_completedIntegratorStep(fmus[i]->getFMU(), fmi2True, &callEventUpdate[i], &terminateSimulation[i]);
        if (fmi2OK != fmistatus) return logError_FMUCall("fmi2_completedIntegratorStep", fmus[i]);

        if (0 == nStates[i])
          continue;

        status = fmus[i]->getContinuousStates(states[i]);
        if (oms_status_ok != status) return status;
      }

      updateInputs(eventGraph);
      if (isTopLevelSystem())
        getModel().emit(time, false);
    }
    else
      return logError("CVode failed with flag = " + std::to_string(flag));
  //}

  return oms_status_ok;

}

oms_status_enu_t oms::SystemSC::stepUntil(double stopTime)
{
  CallClock callClock(clock);
  const double startTime=time;

  if (Flags::ProgressBar())
    logInfo("step SC system [" + std::to_string(startTime) + "; " + std::to_string(stopTime) + "] with step size [" + std::to_string(initialStepSize) + "; " + std::to_string(minimumStepSize) + "; " + std::to_string(maximumStepSize) + "]");

  // main simulation loop
  oms_status_enu_t status = oms_status_ok;
  double endTime = std::min(stopTime, getModel().getStopTime());
  double lastTime = time;
  while (time < endTime && oms_status_ok == status)
  {
    if (solverMethod == oms_solver_sc_explicit_euler)
      status = doStepEuler(endTime);
    else if (solverMethod == oms_solver_sc_cvode)
      status = doStepCVODE(endTime);
    else
      return logError_InternalError;

    if (status != oms_status_ok)
      logWarning("Bad return code at time " + std::to_string(time));

    // Check whether stopping time has changed due to a request from an FMU
    if (getModel().getStopTime() < endTime)
      endTime = getModel().getStopTime();

    if (isTopLevelSystem() && Flags::ProgressBar() && time > lastTime + maximumStepSize)
    {
      Log::ProgressBar(startTime, stopTime, time);
      lastTime = time;
    }
  }

  if (isTopLevelSystem() && Flags::ProgressBar())
    Log::TerminateBar();

  return status;
}

oms_status_enu_t oms::SystemSC::updateInputs(DirectedGraph& graph)
{
  CallClock callClock(clock);
  oms_status_enu_t status;
  int loopNum = 0;

  // input := output
  const std::vector< scc_t >& sortedConnections = graph.getSortedConnections();
  updateAlgebraicLoops(sortedConnections, graph);

  for(size_t i=0; i<sortedConnections.size(); i++)
  {
    if (!sortedConnections[i].thisIsALoop)
    {
      int output = sortedConnections[i].connections[0].first;
      int input = sortedConnections[i].connections[0].second;

      if (graph.getNodes()[input].getType() == oms_signal_type_real)
      {
        double value = 0.0;
        if (oms_status_ok != getReal(graph.getNodes()[output].getName(), value)) return oms_status_error;
        /* check for unit conversion and suppressUnitConversion and set the value multiplied by factor,
         * by default, factor = 1.0, (e.g) mm to m will be (factor*value) => (10^-3 * value)
        */
        if (sortedConnections[i].suppressUnitConversion)
          value = value;
        else
          value = sortedConnections[i].factor*value;

        if (oms_status_ok != setReal(graph.getNodes()[input].getName(), value)) return oms_status_error;
      }
      else if (graph.getNodes()[input].getType() == oms_signal_type_integer || graph.getNodes()[input].getType() == oms_signal_type_enum)
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
      status = solveAlgLoop(graph, loopNum);
      if (oms_status_ok != status)
      {
        forceLoopsToBeUpdated();
        return status;
      }
      loopNum++;
    }
  }
  return oms_status_ok;
}
