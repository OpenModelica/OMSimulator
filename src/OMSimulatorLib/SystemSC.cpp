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

#include "SystemSC.h"

#include "Component.h"
#include "ComponentFMUME.h"
#include "ComponentFMU3ME.h"
#include "ComponentTable.h"
#include "Flags.h"
#include "Model.h"
#include "ssd/Tags.h"

#include <algorithm>
#include <cstring>
#include <sstream>
#include <iostream>

namespace
{
  /// The output grid is accumulated in floating-point arithmetic, so a grid
  /// point can end up a few ulp (Unit in the Last Place) away from an exact
  /// event time. Both would be written to the result file, leaving a redundant
  /// data point right next to the event, so let the event time win whenever the
  /// two are that close.
  void snapToEventTime(fmi2Real& endTime, fmi2Real eventTime)
  {
    if (fabs(endTime - eventTime) <= 1e-12 * (1.0 + fabs(endTime)))
      endTime = eventTime;
  }
}

int oms::cvode_rhs(sunrealtype t, N_Vector y, N_Vector ydot, void* user_data)
{
  SystemSC* system = (SystemSC*)user_data;
  oms_status_enu_t status;

  // set time and states in all FMUs for this residual evaluation
  for (size_t i=0, j_y=0; i < system->fmus.size(); ++i)
  {
    system->fmus[i]->setTime(t);

    if (0 == system->nStates[i])
      continue;

    for (size_t k = 0; k < system->nStates[i]; k++, j_y++)
      system->states[i][k] = NV_Ith_S(y, j_y);

    // set states
    status = system->fmus[i]->setContinuousStates(system->states[i]);
    if (oms_status_ok != status) return status;
  }

  // Propagate connected outputs into inputs (e.g. algebraic feedthrough across
  // FMU boundaries) so that the derivatives below see coupling inputs that are
  // consistent with the current state guess, instead of the value left over
  // from the last accepted step. Without this, tightly-coupled FMUs (e.g. two
  // FMUs closing an algebraic loop through connected signals) only get their
  // coupling updated once per accepted step, which is an explicit one-step-
  // delayed (Jacobi-style) coupling hidden inside what should be a fully
  // implicit combined ODE solve, and can destabilize the integration.
  status = system->updateInputs(system->simulationGraph);
  if (oms_status_ok != status) return status;

  // get state derivatives
  for (size_t i=0, j_ydot=0; i < system->fmus.size(); ++i)
  {
    if (0 == system->nStates[i])
      continue;

    status = system->fmus[i]->getDerivatives(system->states_der[i]);
    if (oms_status_ok != status) return status;

    for (size_t k=0; k < system->nStates[i]; ++k, ++j_ydot)
      NV_Ith_S(ydot, j_ydot) = system->states_der[i][k];
  }

  return 0;
}

int oms::cvode_rhs_algebraic(sunrealtype t, N_Vector y, N_Vector ydot, void* user_data)
{
  SystemSC* system = (SystemSC*)user_data;

  for (size_t i=0; i < system->fmus.size(); ++i)
    system->fmus[i]->setTime(t);

  NV_Ith_S(ydot, 0) = 0.0;

  return 0;
}

int oms::cvode_roots(sunrealtype t, N_Vector y, sunrealtype *gout, void *user_data)
{
  logDebug("cvode_roots at time " + std::to_string(t));
  SystemSC* system = (SystemSC*)user_data;
  oms_status_enu_t status;

  for (size_t i=0, j_y=0, j_gout=0; i < system->fmus.size(); ++i)
  {
    system->fmus[i]->setTime(t);

    if (0 != system->nStates[i])
    {
      for (size_t k = 0; k < system->nStates[i]; k++, j_y++)
        system->states[i][k] = NV_Ith_S(y, j_y);

      // set states
      status = system->fmus[i]->setContinuousStates(system->states[i]);
      if (oms_status_ok != status) return status;
    }

    status  = system->fmus[i]->getEventindicators(system->event_indicators[i], system->nEventIndicators[i]);
    if (oms_status_ok != status) return status;

    for (size_t k=0; k < system->nEventIndicators[i]; k++, j_gout++)
      gout[j_gout] = system->event_indicators[i][k];
  }

  return 0;
}

/* ---------------------------------------------------------------------------
 * fmi-ls-dae: the system as one implicit DAE
 *
 * The unknowns are laid out per component, [x_i | z_i], and so are the rows: a
 * component in DAE mode contributes the residuals it declares, an ODE component
 * the explicit rows x'_i - f_i(x, u, t). Both blocks are square, so the system
 * is, and a system may mix the two.
 *
 * The coupling is where it was for CVODE: updateInputs() propagates connected
 * outputs into inputs before the rows are evaluated, so the residuals see a
 * consistent coupling rather than the value left over from the last accepted
 * step.
 * ------------------------------------------------------------------------- */

/**
 * \brief Which algebraic loops of the simulation graph become rows of the
 *        global system, and what their unknowns are.
 *
 * Every connection of a loop contributes one unknown, the value of the input it
 * drives, and one row, output - input = 0 — the same system the inner Newton
 * solved, handed to IDA instead. A loop is only lifted if all of its
 * connections carry reals: an integer or boolean connection is not something a
 * continuous unknown can stand for, so such a loop keeps its inner solver.
 */
std::vector<bool> oms::SystemSC::allLoopsOf(DirectedGraph& graph) const
{
  size_t loops = 0;
  for (const scc_t& scc : graph.getSortedConnections())
    if (scc.thisIsALoop)
      ++loops;
  return std::vector<bool>(loops, true);
}

bool oms::SystemSC::anyComponentInDaeMode()
{
  for (const auto& component : getComponents())
  {
    const ComponentFMU3ME* fmu3 = dynamic_cast<const ComponentFMU3ME*>(component.second);
    if (fmu3 && fmu3->isInDaeMode())
      return true;
  }
  return false;
}

void oms::SystemSC::collectLiftedLoops()
{
  liftedConnections.clear();
  liftedLoops.clear();

  const std::vector<scc_t>& sortedConnections = simulationGraph.getSortedConnections();
  for (const scc_t& scc : sortedConnections)
  {
    if (!scc.thisIsALoop)
      continue;

    bool allReal = true;
    for (const auto& connection : scc.connections)
      if (simulationGraph.getNodes()[connection.second].getType() != oms_signal_type_real ||
          simulationGraph.getNodes()[connection.first].getType() != oms_signal_type_real)
        allReal = false;

    liftedLoops.push_back(allReal);
    if (!allReal)
    {
      logInfo("system \"" + std::string(getFullCref()) + "\": an algebraic loop over connections that are not all real keeps its own solver");
      continue;
    }

    for (const auto& connection : scc.connections)
      liftedConnections.push_back({connection.first, connection.second});
  }

  if (!liftedConnections.empty())
    logInfo("system \"" + std::string(getFullCref()) + "\": " + std::to_string(liftedConnections.size()) +
            " connection(s) of " + std::to_string(std::count(liftedLoops.begin(), liftedLoops.end(), true)) +
            " algebraic loop(s) are rows of the DAE system, not an inner solve");
}

oms_status_enu_t oms::SystemSC::setDaePoint(double t, N_Vector yy, N_Vector yp)
{
  oms_status_enu_t status;

  for (size_t i = 0, j = 0; i < fmus.size(); ++i)
  {
    fmus[i]->setTime(t);

    const size_t xoff = j;   // where this component's states sit in y and yp
    for (size_t k = 0; k < nStates[i]; ++k, ++j)
      states[i][k] = NV_Ith_S(yy, j);
    for (size_t k = 0; k < nAlgebraic[i]; ++k, ++j)
      algebraicVars[i][k] = NV_Ith_S(yy, j);

    if (nStates[i] > 0)
    {
      status = fmus[i]->setContinuousStates(states[i]);
      if (oms_status_ok != status) return status;
    }

    if (daeFmus[i])
    {
      // The implicit form's residuals are a function of (t, x, x', z), so the
      // derivatives are knowns the master hands over, not something the FMU owes.
      if (nStates[i] > 0)
      {
        for (size_t k = 0; k < nStates[i]; ++k)
          states_der[i][k] = NV_Ith_S(yp, xoff + k);
        status = daeFmus[i]->setDerivatives(states_der[i]);
        if (oms_status_ok != status) return status;
      }
      if (nAlgebraic[i] > 0)
      {
        status = daeFmus[i]->setAlgebraicVariables(algebraicVars[i]);
        if (oms_status_ok != status) return status;
      }
    }
  }

  // The loop connections IDA owns: the unknown is the value of the driven input.
  // Set before the propagation below, which then leaves those loops alone.
  for (size_t k = 0; k < liftedConnections.size(); ++k)
  {
    const double value = NV_Ith_S(yy, nDaeUnknowns - liftedConnections.size() + k);
    if (oms_status_ok != setReal(simulationGraph.getNodes()[liftedConnections[k].input].getName(), value))
      return oms_status_error;
  }

  // The rest of the coupling, before the rows below are asked for.
  return updateInputsInternal(simulationGraph, &liftedLoops);
}

oms_status_enu_t oms::SystemSC::getDaePoint(N_Vector yy, N_Vector yp)
{
  oms_status_enu_t status;

  for (size_t i = 0, j = 0; i < fmus.size(); ++i)
  {
    if (nStates[i] > 0)
    {
      status = fmus[i]->getContinuousStates(states[i]);
      if (oms_status_ok != status) return status;
      status = fmus[i]->getDerivatives(states_der[i]);
      if (oms_status_ok != status) return status;
    }
    if (daeFmus[i] && nAlgebraic[i] > 0)
    {
      status = daeFmus[i]->getAlgebraicVariables(algebraicVars[i]);
      if (oms_status_ok != status) return status;
    }

    for (size_t k = 0; k < nStates[i]; ++k, ++j)
    {
      NV_Ith_S(yy, j) = states[i][k];
      NV_Ith_S(yp, j) = states_der[i][k];
    }
    for (size_t k = 0; k < nAlgebraic[i]; ++k, ++j)
    {
      NV_Ith_S(yy, j) = algebraicVars[i][k];
      NV_Ith_S(yp, j) = 0.0;   // an algebraic unknown has no derivative row
    }
  }

  // The lifted loop connections, seeded with the values the inputs hold — after
  // initialization, or after an event, that is the best guess there is.
  for (size_t k = 0, j = nDaeUnknowns - liftedConnections.size(); k < liftedConnections.size(); ++k, ++j)
  {
    double value = 0.0;
    if (oms_status_ok != getReal(simulationGraph.getNodes()[liftedConnections[k].input].getName(), value))
      return oms_status_error;
    NV_Ith_S(yy, j) = value;
    NV_Ith_S(yp, j) = 0.0;
  }

  return oms_status_ok;
}

int oms::ida_res(sunrealtype t, N_Vector yy, N_Vector yp, N_Vector rr, void* user_data)
{
  SystemSC* system = (SystemSC*)user_data;

  if (oms_status_ok != system->setDaePoint(t, yy, yp))
    return -1;

  for (size_t i = 0, j = 0; i < system->fmus.size(); ++i)
  {
    if (system->daeFmus[i])
    {
      const size_t n = system->nStates[i] + system->nAlgebraic[i];
      if (n == 0)
        continue;
      if (oms_status_ok != system->daeFmus[i]->getDaeResiduals(system->residuals[i]))
        return -1;
      for (size_t k = 0; k < n; ++k, ++j)
        NV_Ith_S(rr, j) = system->residuals[i][k];
    }
    else
    {
      // An ODE component closes its own rows: x' - f(x, u, t).
      if (0 == system->nStates[i])
        continue;
      if (oms_status_ok != system->fmus[i]->getDerivatives(system->states_der[i]))
        return -1;
      for (size_t k = 0; k < system->nStates[i]; ++k, ++j)
        NV_Ith_S(rr, j) = NV_Ith_S(yp, j) - system->states_der[i][k];
    }
  }

  // The lifted loop rows: what the driving output now says, against the value
  // IDA is carrying for the input it drives.
  for (size_t k = 0, j = system->nDaeUnknowns - system->liftedConnections.size();
       k < system->liftedConnections.size(); ++k, ++j)
  {
    double value = 0.0;
    if (oms_status_ok != system->getReal(system->simulationGraph.getNodes()[system->liftedConnections[k].output].getName(), value))
      return -1;
    NV_Ith_S(rr, j) = value - NV_Ith_S(yy, j);
  }

  return 0;
}

int oms::ida_roots(sunrealtype t, N_Vector yy, N_Vector yp, sunrealtype *gout, void* user_data)
{
  SystemSC* system = (SystemSC*)user_data;

  if (oms_status_ok != system->setDaePoint(t, yy, yp))
    return -1;

  for (size_t i = 0, j_gout = 0; i < system->fmus.size(); ++i)
  {
    if (0 == system->nEventIndicators[i])
      continue;
    if (oms_status_ok != system->fmus[i]->getEventindicators(system->event_indicators[i], system->nEventIndicators[i]))
      return -1;
    for (size_t k = 0; k < system->nEventIndicators[i]; ++k, ++j_gout)
      gout[j_gout] = system->event_indicators[i][k];
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
    case oms_solver_sc_ida:
      return std::string("ida");
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
  else if (std::string("ida") == solver)
    solverMethod = oms_solver_sc_ida;
  else
    return oms_status_error;

  return oms_status_ok;
}

oms_status_enu_t oms::SystemSC::exportToSSD_SimulationInformation(pugi::xml_node& node) const
{
  std::ostringstream ssAbsoluteTolerance, ssRelativeTolerance, ssMinimumStepSize, ssMaximumStepSize, ssInitialStepSize;
  ssRelativeTolerance << relativeTolerance;
  ssMinimumStepSize << minimumStepSize;
  ssMaximumStepSize << maximumStepSize;
  ssInitialStepSize << initialStepSize;

  /* ssd:SimulationInformation should be added as vendor specific annotations from Version 1.0 */
  pugi::xml_node node_simulation_information = node.append_child(oms::ssp::Version1_0::simulation_information);

  pugi::xml_node node_solver = node_simulation_information.append_child(oms::ssp::Version1_0::VariableStepSolver);
  node_solver.append_attribute("description") = getSolverName().c_str();
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

  for (const auto& component : getComponents())
    if (oms_status_ok != component.second->instantiate())
      return oms_status_error;

  if (oms_solver_sc_explicit_euler == solverMethod)
    ;
  else if (oms_solver_sc_cvode == solverMethod)
    solverData.cvode.mem = nullptr;
  else if (oms_solver_sc_ida == solverMethod)
    solverData.ida.mem = nullptr;
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

  // fmi-ls-dae: a component in DAE mode does not solve for its own algebraic
  // outputs — the master does — so a loop through one cannot be closed by the
  // inner Newton here. Leave every loop of the initialization graph to the
  // consistent-initial-conditions solve at the end of this function.
  daeMode = anyComponentInDaeMode();
  if (daeMode)
  {
    std::vector<bool> allLoops = allLoopsOf(initializationGraph);
    if (oms_status_ok != updateInputsInternal(initializationGraph, &allLoops))
      return oms_status_error;
  }
  else if (oms_status_ok != updateInputs(initializationGraph))
    return oms_status_error;

  for (const auto& subsystem : getSubSystems())
    if (oms_status_ok != subsystem.second->initialize())
      return oms_status_error;

  size_t n_states = 0;
  for (const auto& component : getComponents())
  {
    if (oms_status_ok != component.second->initialize())
      return oms_status_error;

    Component * fmu = nullptr;
    if (component.second->getType() == oms_component_fmu)
      fmu = dynamic_cast<ComponentFMUME*>(component.second);
    else if (component.second->getType() == oms_component_fmu3)
      fmu = dynamic_cast<ComponentFMU3ME*>(component.second);

    if (!fmu)
      continue; // skip non-FMU components or failed casts

    fmus.push_back(fmu);

    nStates.push_back(fmus.back()->getNumberOfContinuousStates());
    n_states += nStates.back();
    nEventIndicators.push_back(fmus.back()->getNumberOfEventIndicators());

    states.push_back((double*)calloc(nStates.back(), sizeof(double)));
    states_der.push_back((double*)calloc(nStates.back(), sizeof(double)));
    states_nominal.push_back((double*)calloc(nStates.back(), sizeof(double)));
    event_indicators.push_back((double*)calloc(nEventIndicators.back(), sizeof(double)));
    event_indicators_prev.push_back((double*)calloc(nEventIndicators.back(), sizeof(double)));

    // fmi-ls-dae: a component that was switched into DAE mode carries algebraic
    // unknowns of its own and answers with residuals instead of derivatives.
    ComponentFMU3ME* fmu3 = dynamic_cast<ComponentFMU3ME*>(fmu);
    const bool inDaeMode = fmu3 && fmu3->isInDaeMode();
    daeFmus.push_back(inDaeMode ? fmu3 : nullptr);
    nAlgebraic.push_back(inDaeMode ? fmu3->getNumberOfAlgebraicVariables() : 0);
    algebraicVars.push_back((double*)calloc(nAlgebraic.back(), sizeof(double)));
    residuals.push_back((double*)calloc(inDaeMode ? fmu3->getNumberOfDaeResiduals() : 0, sizeof(double)));
    if (inDaeMode)
      daeMode = true;   // already true from anyComponentInDaeMode(), kept for clarity
    nDaeUnknowns += nStates.back() + nAlgebraic.back();
   }

  // fmi-ls-dae: with IDA driving the system, an algebraic loop over connections
  // can be rows of it instead of an inner Newton of its own.
  if (daeMode)
  {
    collectLiftedLoops();
    nDaeUnknowns += liftedConnections.size();
  }

  if (daeMode && oms_solver_sc_ida != solverMethod)
  {
    // DAE mode is not a choice the user makes: a component that declares a DAE
    // formulation has no explicit ODE for CVODE to integrate.
    logInfo("system \"" + std::string(getFullCref()) + "\" contains a component in DAE mode (fmi-ls-dae); integrating with IDA instead of " + getSolverName());
    solverMethod = oms_solver_sc_ida;
  }
  else if (!daeMode && oms_solver_sc_ida == solverMethod)
  {
    logInfo("system \"" + std::string(getFullCref()) + "\" contains no component in DAE mode; integrating with CVODE instead of ida");
    solverMethod = oms_solver_sc_cvode;
  }

  // Now that fmus is filled, the per-FMU flags the integrator steps write to
  // can be sized. make_unique value-initializes them, i.e. all false.
  callEventUpdate = std::make_unique<bool[]>(fmus.size());
  terminateSimulation = std::make_unique<bool[]>(fmus.size());

  if (n_states == 0)
    logInfo("model doesn't contain any continuous state");

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

    algebraic = n_states == 0;
    if (algebraic)
      n_states = 1;

    /* Create the SUNDIALS context every other SUNDIALS object is created with */
    if (SUNContext_Create(SUN_COMM_NULL, &solverData.cvode.sunctx) != SUN_SUCCESS)
      logError("SUNDIALS_ERROR: SUNContext_Create() failed");

    /* Mute SUNDIALS' own output, use OMSimulator's logger */
    {
      SUNLogger logger = NULL;
      if (SUNContext_GetLogger(solverData.cvode.sunctx, &logger) == SUN_SUCCESS && logger != NULL)
      {
        SUNLogger_SetErrorFilename(logger, "");
        SUNLogger_SetWarningFilename(logger, "");
        SUNLogger_SetInfoFilename(logger, "");
        SUNLogger_SetDebugFilename(logger, "");
      }
    }

    solverData.cvode.y = N_VNew_Serial(static_cast<long>(n_states), solverData.cvode.sunctx);
    if (!solverData.cvode.y) logError("SUNDIALS_ERROR: N_VNew_Serial() failed - returned NULL pointer");

    if (algebraic)
      NV_Ith_S(solverData.cvode.y, 0) = 0.0;
    else
      for (size_t j=0, k=0; j < fmus.size(); ++j)
        for (size_t i=0; i < nStates[j]; ++i, ++k)
          NV_Ith_S(solverData.cvode.y, k) = states[j][i];
    //N_VPrint_Serial(solverData.cvode.y);

    solverData.cvode.abstol = N_VNew_Serial(static_cast<long>(n_states), solverData.cvode.sunctx);
    if (!solverData.cvode.abstol) logError("SUNDIALS_ERROR: N_VNew_Serial() failed - returned NULL pointer");

    if (algebraic)
      NV_Ith_S(solverData.cvode.abstol, 0) = relativeTolerance;
    else
      for (size_t j=0, k=0; j < fmus.size(); ++j)
        for (size_t i=0; i < nStates[j]; ++i, ++k)
          NV_Ith_S(solverData.cvode.abstol, k) = relativeTolerance*states_nominal[j][i];
    //N_VPrint_Serial(solverData.cvode.abstol);

    // Call CVodeCreate to create the solver memory and specify the
    // Backward Differentiation Formula and the use of a Newton iteration
    solverData.cvode.mem = CVodeCreate(CV_BDF, solverData.cvode.sunctx);
    if (!solverData.cvode.mem) logError("SUNDIALS_ERROR: CVodeCreate() failed - returned NULL pointer");

    int flag = CVodeSetUserData(solverData.cvode.mem, (void*)this);
    if (flag < 0) logError("SUNDIALS_ERROR: CVodeSetUserData() failed with flag = " + std::to_string(flag));

    // Call CVodeInit to initialize the integrator memory and specify the
    // user's right hand side function in y'=cvode_rhs(t,y), the inital time T0, and
    // the initial dependent variable vector y.
    flag = CVodeInit(solverData.cvode.mem, algebraic ? cvode_rhs_algebraic : cvode_rhs, time, solverData.cvode.y);
    if (flag < 0) logError("SUNDIALS_ERROR: CVodeInit() failed with flag = " + std::to_string(flag));

    flag = CVodeRootInit(solverData.cvode.mem, n_event_indicators, cvode_roots);
    if (flag != CV_SUCCESS) logError("SUNDIALS_ERROR: CVodeRootInit() failed with flag = " + std::to_string(flag));

    // Call CVodeSVtolerances to specify the scalar relative tolerance
    // and vector absolute tolerances
    flag = CVodeSVtolerances(solverData.cvode.mem, relativeTolerance, solverData.cvode.abstol);
    if (flag < 0) logError("SUNDIALS_ERROR: CVodeSVtolerances() failed with flag = " + std::to_string(flag));

    // Call N_VNew_Serial and SUNDenseMatrix to generate dense vector abd natrix for lin. solver module
    solverData.cvode.liny = N_VNew_Serial(n_states, solverData.cvode.sunctx);
    if (solverData.cvode.liny == NULL) logError("SUNDIALS_ERROR: N_VNew_Serial() failed");
    solverData.cvode.J = SUNDenseMatrix(n_states, n_states, solverData.cvode.sunctx);
    if (solverData.cvode.J == NULL) logError("SUNDIALS_ERROR: SUNDenseMatrix() failed");

    // Call SUNLinSol_Dense to creat linear solver object
    solverData.cvode.linSol = SUNLinSol_Dense(solverData.cvode.liny, solverData.cvode.J, solverData.cvode.sunctx);
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
    flag = CVodeSetMaxOrd(solverData.cvode.mem, 5);                                      // MAXIMUM ORDER
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
  else if (oms_solver_sc_ida == solverMethod)
  {
    // fmi-ls-dae: one implicit system over [states | algebraic variables].
    if (SUNContext_Create(SUN_COMM_NULL, &solverData.ida.sunctx) != SUN_SUCCESS)
      logError("SUNDIALS_ERROR: SUNContext_Create() failed");

    /* Mute SUNDIALS' own output, use OMSimulator's logger */
    {
      SUNLogger logger = NULL;
      if (SUNContext_GetLogger(solverData.ida.sunctx, &logger) == SUN_SUCCESS && logger != NULL)
      {
        SUNLogger_SetErrorFilename(logger, "");
        SUNLogger_SetWarningFilename(logger, "");
        SUNLogger_SetInfoFilename(logger, "");
        SUNLogger_SetDebugFilename(logger, "");
      }
    }

    const long n = static_cast<long>(nDaeUnknowns);
    solverData.ida.y = N_VNew_Serial(n, solverData.ida.sunctx);
    solverData.ida.yp = N_VNew_Serial(n, solverData.ida.sunctx);
    solverData.ida.id = N_VNew_Serial(n, solverData.ida.sunctx);
    solverData.ida.abstol = N_VNew_Serial(n, solverData.ida.sunctx);
    if (!solverData.ida.y || !solverData.ida.yp || !solverData.ida.id || !solverData.ida.abstol)
      return logError("SUNDIALS_ERROR: N_VNew_Serial() failed - returned NULL pointer");

    // The point the components hold after their own initialization, and which
    // of the unknowns are differential: IDACalcIC solves for the algebraic ones
    // and for the derivatives of the differential ones.
    if (oms_status_ok != getDaePoint(solverData.ida.y, solverData.ida.yp))
      return oms_status_error;

    for (size_t i = 0, j = 0; i < fmus.size(); ++i)
    {
      for (size_t k = 0; k < nStates[i]; ++k, ++j)
      {
        NV_Ith_S(solverData.ida.id, j) = 1.0;
        NV_Ith_S(solverData.ida.abstol, j) = relativeTolerance * states_nominal[i][k];
      }
      for (size_t k = 0; k < nAlgebraic[i]; ++k, ++j)
      {
        NV_Ith_S(solverData.ida.id, j) = 0.0;
        // The FMU gives no nominal for an algebraic unknown of its own, so the
        // relative tolerance stands in for it.
        NV_Ith_S(solverData.ida.abstol, j) = relativeTolerance;
      }
    }

    for (size_t k = 0, j = nDaeUnknowns - liftedConnections.size(); k < liftedConnections.size(); ++k, ++j)
    {
      NV_Ith_S(solverData.ida.id, j) = 0.0;
      NV_Ith_S(solverData.ida.abstol, j) = relativeTolerance;
    }

    solverData.ida.mem = IDACreate(solverData.ida.sunctx);
    if (!solverData.ida.mem) return logError("SUNDIALS_ERROR: IDACreate() failed - returned NULL pointer");

    int flag = IDASetUserData(solverData.ida.mem, (void*)this);
    if (flag < 0) return logError("SUNDIALS_ERROR: IDASetUserData() failed with flag = " + std::to_string(flag));

    flag = IDAInit(solverData.ida.mem, ida_res, time, solverData.ida.y, solverData.ida.yp);
    if (flag < 0) return logError("SUNDIALS_ERROR: IDAInit() failed with flag = " + std::to_string(flag));

    flag = IDASVtolerances(solverData.ida.mem, relativeTolerance, solverData.ida.abstol);
    if (flag < 0) return logError("SUNDIALS_ERROR: IDASVtolerances() failed with flag = " + std::to_string(flag));

    flag = IDASetId(solverData.ida.mem, solverData.ida.id);
    if (flag < 0) return logError("SUNDIALS_ERROR: IDASetId() failed with flag = " + std::to_string(flag));

    if (n_event_indicators > 0)
    {
      flag = IDARootInit(solverData.ida.mem, static_cast<int>(n_event_indicators), ida_roots);
      if (flag < 0) return logError("SUNDIALS_ERROR: IDARootInit() failed with flag = " + std::to_string(flag));
    }

    // A dense Jacobian, differenced by IDA. The manifest's residual dependencies
    // would give a sparsity pattern for KLU; that is the next step and matters
    // as soon as the system is more than small.
    solverData.ida.J = SUNDenseMatrix(n, n, solverData.ida.sunctx);
    if (!solverData.ida.J) return logError("SUNDIALS_ERROR: SUNDenseMatrix() failed");
    solverData.ida.linSol = SUNLinSol_Dense(solverData.ida.y, solverData.ida.J, solverData.ida.sunctx);
    if (!solverData.ida.linSol) return logError("SUNDIALS_ERROR: SUNLinSol_Dense() failed");
    flag = IDASetLinearSolver(solverData.ida.mem, solverData.ida.linSol, solverData.ida.J);
    if (flag < 0) return logError("SUNDIALS_ERROR: IDASetLinearSolver() failed with flag = " + std::to_string(flag));

    flag = IDASetMaxStep(solverData.ida.mem, maximumStepSize);
    if (flag < 0) return logError("SUNDIALS_ERROR: IDASetMaxStep() failed with flag = " + std::to_string(flag));
    if (initialStepSize > 0.0)
    {
      flag = IDASetInitStep(solverData.ida.mem, initialStepSize);
      if (flag < 0) return logError("SUNDIALS_ERROR: IDASetInitStep() failed with flag = " + std::to_string(flag));
    }
    flag = IDASetMaxNumSteps(solverData.ida.mem, Flags::CVODEMaxSteps());
    if (flag < 0) return logError("SUNDIALS_ERROR: IDASetMaxNumSteps() failed with flag = " + std::to_string(flag));

    // The start point the components initialized to need not satisfy the
    // coupled system; IDA solves for a consistent one before the first step.
    if (oms_status_ok != calcConsistentInitialConditions(time + maximumStepSize))
      return oms_status_error;
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

  if (oms_solver_sc_ida == solverMethod && solverData.ida.mem)
  {
    long int nst = 0, nre = 0, nsetups = 0, nni = 0, ncfn = 0, netf = 0;
    IDAGetNumSteps(solverData.ida.mem, &nst);
    IDAGetNumResEvals(solverData.ida.mem, &nre);
    IDAGetNumLinSolvSetups(solverData.ida.mem, &nsetups);
    IDAGetNumErrTestFails(solverData.ida.mem, &netf);
    IDAGetNumNonlinSolvIters(solverData.ida.mem, &nni);
    IDAGetNumNonlinSolvConvFails(solverData.ida.mem, &ncfn);

    std::string msg = "Final Statistics for '" + std::string(getFullCref()) + "':\n";
    msg += "NumSteps = " + std::to_string(nst) + " NumResEvals  = " + std::to_string(nre) + " NumLinSolvSetups = " + std::to_string(nsetups) + "\n";
    msg += "NumNonlinSolvIters = " + std::to_string(nni) + " NumNonlinSolvConvFails = " + std::to_string(ncfn) + " NumErrTestFails = " + std::to_string(netf);
    logInfo(msg);

    SUNMatDestroy(solverData.ida.J);
    SUNLinSolFree(solverData.ida.linSol);
    N_VDestroy_Serial(solverData.ida.y);
    N_VDestroy_Serial(solverData.ida.yp);
    N_VDestroy_Serial(solverData.ida.id);
    N_VDestroy_Serial(solverData.ida.abstol);
    IDAFree(&(solverData.ida.mem));
    SUNContext_Free(&(solverData.ida.sunctx));
    solverData.ida.mem = nullptr;
  }

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
    SUNContext_Free(&(solverData.cvode.sunctx));
    solverData.cvode.mem = NULL;
  }

  for (size_t i=0; i<fmus.size(); ++i)
  {
    free(states[i]);
    free(states_der[i]);
    free(states_nominal[i]);
    free(event_indicators[i]);
    free(event_indicators_prev[i]);
    free(algebraicVars[i]);
    free(residuals[i]);
  }

  fmus.clear();
  daeFmus.clear();
  callEventUpdate.reset();
  terminateSimulation.reset();
  nStates.clear();
  nEventIndicators.clear();
  nAlgebraic.clear();
  algebraicVars.clear();
  residuals.clear();
  daeMode = false;
  nDaeUnknowns = 0;
  liftedConnections.clear();
  liftedLoops.clear();
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

  if (oms_solver_sc_ida == solverMethod && solverData.ida.mem)
  {
    long int nst = 0, nre = 0, nsetups = 0, nni = 0, ncfn = 0, netf = 0;
    IDAGetNumSteps(solverData.ida.mem, &nst);
    IDAGetNumResEvals(solverData.ida.mem, &nre);
    IDAGetNumLinSolvSetups(solverData.ida.mem, &nsetups);
    IDAGetNumErrTestFails(solverData.ida.mem, &netf);
    IDAGetNumNonlinSolvIters(solverData.ida.mem, &nni);
    IDAGetNumNonlinSolvConvFails(solverData.ida.mem, &ncfn);

    std::string msg = "Final Statistics for '" + std::string(getFullCref()) + "':\n";
    msg += "NumSteps = " + std::to_string(nst) + " NumResEvals  = " + std::to_string(nre) + " NumLinSolvSetups = " + std::to_string(nsetups) + "\n";
    msg += "NumNonlinSolvIters = " + std::to_string(nni) + " NumNonlinSolvConvFails = " + std::to_string(ncfn) + " NumErrTestFails = " + std::to_string(netf);
    logInfo(msg);

    SUNMatDestroy(solverData.ida.J);
    SUNLinSolFree(solverData.ida.linSol);
    N_VDestroy_Serial(solverData.ida.y);
    N_VDestroy_Serial(solverData.ida.yp);
    N_VDestroy_Serial(solverData.ida.id);
    N_VDestroy_Serial(solverData.ida.abstol);
    IDAFree(&(solverData.ida.mem));
    SUNContext_Free(&(solverData.ida.sunctx));
    solverData.ida.mem = nullptr;
  }

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
    SUNContext_Free(&(solverData.cvode.sunctx));
    solverData.cvode.mem = nullptr;
  }

  // Free arrays allocated by initialize() and clear the per-FMU tracking
  // vectors it populated, so a subsequent initialize() call starts fresh
  // instead of appending duplicate entries for every FMU (which corrupts
  // solver setup and causes lifecycle calls like enterContinuousTimeMode()
  // to be issued twice for the same FMU).
  for (double* ptr : algebraicVars) free(ptr);
  for (double* ptr : residuals) free(ptr);
  for (double* ptr : states) free(ptr);
  for (double* ptr : states_der) free(ptr);
  for (double* ptr : states_nominal) free(ptr);
  for (double* ptr : event_indicators) free(ptr);
  for (double* ptr : event_indicators_prev) free(ptr);
  fmus.clear();
  daeFmus.clear();
  callEventUpdate.reset();
  terminateSimulation.reset();
  nStates.clear();
  nEventIndicators.clear();
  nAlgebraic.clear();
  algebraicVars.clear();
  residuals.clear();
  daeMode = false;
  nDaeUnknowns = 0;
  liftedConnections.clear();
  liftedLoops.clear();
  states.clear();
  states_der.clear();
  states_nominal.clear();
  event_indicators.clear();
  event_indicators_prev.clear();

  return oms_status_ok;
}

oms_status_enu_t oms::SystemSC::doStep()
{
  switch(solverMethod)
  {
    case oms_solver_sc_explicit_euler:
      return doStepEuler();

    case oms_solver_sc_cvode:
      return doStepCVODE();

    case oms_solver_sc_ida:
      return doStepIDA();

    default:
      return logError_InternalError;
  }
}

oms_status_enu_t oms::SystemSC::doStepEuler()
{
  oms_status_enu_t status;

  // Step 1: Initialize state variables and time
  fmi2Real end_time = std::min(time + maximumStepSize, getModel().getStopTime());
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
    status = fmus[i]->getEventindicators(event_indicators_prev[i], nEventIndicators[i]);
    if (oms_status_ok != status) return status;
  }

  fmi2Real step_size_adjustment = maximumStepSize;
  bool event_detected = false;

  fmi2Real tnext = end_time + 1.0;
  bool terminated = false;
  for (size_t i = 0; i < fmus.size(); ++i)
  {
    if (fmus[i]->getNextEventTimeDefined() && (tnext > fmus[i]->getNextEventTime()) && (time < fmus[i]->getNextEventTime()))
      tnext = fmus[i]->getNextEventTime();

    if(fmus[i]->getTerminateSimulation())
    {
      logInfo("Simulation terminated by FMU " + std::string(fmus[i]->getFullCref()) + " at time " + std::to_string(time));
      getModel().setStopTime(time);
      terminated = true;
    }
  }
  snapToEventTime(end_time, tnext);

  fmi2Real event_time = end_time;

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
      status = fmus[i]->getEventindicators(event_indicators[i], nEventIndicators[i]);
      if (oms_status_ok != status) return status;

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
          status = fmus[i]->completedIntegratorStep(true, callEventUpdate[i], terminateSimulation[i]);
          if (oms_status_ok != status) return status;
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
          status = fmus[i]->completedIntegratorStep(true, callEventUpdate[i], terminateSimulation[i]);
          if (oms_status_ok != status) return status;

          status = fmus[i]->enterEventMode();
          if (oms_status_ok != status) return status;

          fmus[i]->doEventIteration();

          status = fmus[i]->enterContinuousTimeMode();
          if (oms_status_ok != status) return status;

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
          if (fmus[i]->getNextEventTimeDefined() && (tnext > fmus[i]->getNextEventTime()) && (time < fmus[i]->getNextEventTime()))
            tnext = fmus[i]->getNextEventTime();

          if(fmus[i]->getTerminateSimulation())
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

oms_status_enu_t oms::SystemSC::doStepCVODE()
{
  fmi2Status fmistatus;
  oms_status_enu_t status;
  int flag;

  fmi2Real end_time = std::min(time + maximumStepSize, getModel().getStopTime());

  // find next time event
  fmi2Real tnext = end_time+1.0;
  for (size_t i = 0; i < fmus.size(); ++i)
  {
    if (fmus[i]->getNextEventTimeDefined() && (tnext > fmus[i]->getNextEventTime()))
      tnext = fmus[i]->getNextEventTime();

    if(fmus[i]->getTerminateSimulation())
    {
      logInfo("Simulation terminated by FMU " + std::string(fmus[i]->getFullCref()) + " at time " + std::to_string(time));
      getModel().setStopTime(time);
      time = end_time;
    }
  }
  snapToEventTime(end_time, tnext);
  logDebug("tnext: " + std::to_string(tnext));

  while (time < end_time)
  {
    const fmi3Float64 tout = std::min(tnext, end_time);

    // CVode rejects an interval it cannot tell apart from zero (CV_TOO_CLOSE)
    // There is nothing left to integrate in it; assume end_time is reached.
    if (tout - time < 2.0 * SUN_UNIT_ROUNDOFF * std::max(std::fabs(time), std::fabs(tout)))
    {
      logDebug("CVode: skipping degenerate interval " + std::to_string(time) + " -> " + std::to_string(tout));
      time = end_time;
      break;
    }

    logDebug("CVode: " + std::to_string(time) + " -> " + std::to_string(tout));
    for (size_t j=0, k=0; j < fmus.size(); ++j)
      for (size_t i=0; i < nStates[j]; ++i, ++k)
        NV_Ith_S(solverData.cvode.y, k) = states[j][i];

    flag = CVode(solverData.cvode.mem, tout, solverData.cvode.y, &time, CV_NORMAL);

    for (size_t i = 0, j=0; i < fmus.size(); ++i)
    {
      if (0 == nStates[i])
        continue;

      for (size_t k = 0; k < nStates[i]; k++, j++)
        states[i][k] = NV_Ith_S(solverData.cvode.y, j);

      // set states
      status = fmus[i]->setContinuousStates(states[i]);
      if (oms_status_ok != status) return status;
    }

    if (flag == CV_ROOT_RETURN || time == tnext)
    {
      logDebug("event found!!! " + std::to_string(time));

      // set time
      for (const auto& component : getComponents())
        component.second->setTime(time);

      for (size_t i = 0; i < fmus.size(); ++i)
      {
        status = fmus[i]->completedIntegratorStep(true, callEventUpdate[i], terminateSimulation[i]);
        if (oms_status_ok != status) return status;
      }

      // emit the left limit of the event (if it hasn't already been emitted)
      if (isTopLevelSystem())
        getModel().emit(time, false);

      // Enter event mode and handle discrete state updates for each FMU
      for (size_t i = 0; i < fmus.size(); ++i)
      {
        status = fmus[i]->enterEventMode();
        if (oms_status_ok != status) return status;

        fmus[i]->doEventIteration();
      }

      updateInputs(eventGraph);

      for (size_t i = 0; i < fmus.size(); ++i)
      {
        status = fmus[i]->enterContinuousTimeMode();
        if (oms_status_ok != status) return status;
      }

      for (size_t i = 0; i < fmus.size(); ++i)
      {
        if (0 == nStates[i])
          continue;

        status = fmus[i]->getContinuousStates(states[i]);
        if (oms_status_ok != status) return status;
      }

      // find next time event
      tnext = end_time+1.0;
      for (size_t i = 0; i < fmus.size(); ++i)
      {
        if (fmus[i]->getNextEventTimeDefined() && (tnext > fmus[i]->getNextEventTime()))
          tnext = fmus[i]->getNextEventTime();

        if(fmus[i]->getTerminateSimulation())
        {
          logInfo("Simulation terminated by FMU " + std::string(fmus[i]->getFullCref()) + " at time " + std::to_string(time));
          getModel().setStopTime(time);
          time = end_time;
        }
      }
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

      continue;
    }

    if (flag == CV_SUCCESS)
    {
      logDebug("CVode completed successfully at t = " + std::to_string(time));

      // set time
      for (const auto& component : getComponents())
        component.second->setTime(time);

      for (size_t i = 0; i < fmus.size(); ++i)
      {
        status = fmus[i]->completedIntegratorStep(true, callEventUpdate[i], terminateSimulation[i]);
        if (oms_status_ok != status) return status;

        if (0 == nStates[i])
          continue;

        status = fmus[i]->getContinuousStates(states[i]);
        if (oms_status_ok != status) return status;
      }

      updateInputs(simulationGraph);
      if (isTopLevelSystem())
        getModel().emit(time, false);
    }
    else
      return logError("CVode failed with flag = " + std::to_string(flag));
  }

  return oms_status_ok;

}

/**
 * \brief IDACalcIC over the point the components hold, then read the consistent
 *        point back into them.
 *
 * The components initialize on their own, each consistent with itself but not
 * necessarily with the coupled system; and after an event the same is true
 * again. IDA_YA_YDP_INIT solves for the algebraic unknowns and the derivatives
 * of the differential ones, keeping the states where they are.
 */
oms_status_enu_t oms::SystemSC::calcConsistentInitialConditions(double tout)
{
  // IDACalcIC wants a time it may integrate towards; anything but the current
  // one will do, and the step it takes is thrown away.
  if (tout <= time)
    tout = time + 1.0;

  int flag = IDACalcIC(solverData.ida.mem, IDA_YA_YDP_INIT, tout);
  if (flag < 0)
    return logError("SUNDIALS_ERROR: IDACalcIC() failed with flag = " + std::to_string(flag) +
                    " at time " + std::to_string(time) +
                    " (the coupled DAE has no consistent point there, or its Jacobian is singular)");

  flag = IDAGetConsistentIC(solverData.ida.mem, solverData.ida.y, solverData.ida.yp);
  if (flag < 0)
    return logError("SUNDIALS_ERROR: IDAGetConsistentIC() failed with flag = " + std::to_string(flag));

  // Leave the components standing at the consistent point, so what is emitted
  // and what the next step starts from agree with it.
  return setDaePoint(time, solverData.ida.y, solverData.ida.yp);
}

oms_status_enu_t oms::SystemSC::doStepIDA()
{
  oms_status_enu_t status;
  int flag;

  double end_time = std::min(time + maximumStepSize, getModel().getStopTime());

  // find next time event
  double tnext = end_time + 1.0;
  for (size_t i = 0; i < fmus.size(); ++i)
  {
    if (fmus[i]->getNextEventTimeDefined() && (tnext > fmus[i]->getNextEventTime()))
      tnext = fmus[i]->getNextEventTime();

    if (fmus[i]->getTerminateSimulation())
    {
      logInfo("Simulation terminated by FMU " + std::string(fmus[i]->getFullCref()) + " at time " + std::to_string(time));
      getModel().setStopTime(time);
      time = end_time;
    }
  }
  snapToEventTime(end_time, tnext);

  while (time < end_time)
  {
    const double tout = std::min(tnext, end_time);

    // Nothing left to integrate in an interval IDA cannot tell apart from zero.
    if (tout - time < 2.0 * SUN_UNIT_ROUNDOFF * std::max(std::fabs(time), std::fabs(tout)))
    {
      logDebug("IDA: skipping degenerate interval " + std::to_string(time) + " -> " + std::to_string(tout));
      time = end_time;
      break;
    }

    logDebug("IDA: " + std::to_string(time) + " -> " + std::to_string(tout));
    flag = IDASolve(solverData.ida.mem, tout, &time, solverData.ida.y, solverData.ida.yp, IDA_NORMAL);
    if (flag < 0)
      return logError("IDA failed with flag = " + std::to_string(flag) + " at time " + std::to_string(time));

    // The components stand at the point IDA returned.
    status = setDaePoint(time, solverData.ida.y, solverData.ida.yp);
    if (oms_status_ok != status) return status;

    for (const auto& component : getComponents())
      component.second->setTime(time);

    for (size_t i = 0; i < fmus.size(); ++i)
    {
      status = fmus[i]->completedIntegratorStep(true, callEventUpdate[i], terminateSimulation[i]);
      if (oms_status_ok != status) return status;
    }

    if (IDA_ROOT_RETURN == flag || time == tnext)
    {
      logDebug("event found!!! " + std::to_string(time));

      // emit the left limit of the event
      if (isTopLevelSystem())
        getModel().emit(time, false);

      for (size_t i = 0; i < fmus.size(); ++i)
      {
        status = fmus[i]->enterEventMode();
        if (oms_status_ok != status) return status;

        fmus[i]->doEventIteration();
      }

      {
        // As during initialization: the loops are the integrator's, and
        // calcConsistentInitialConditions() below closes them.
        std::vector<bool> allLoops = allLoopsOf(eventGraph);
        updateInputsInternal(eventGraph, &allLoops);
      }

      for (size_t i = 0; i < fmus.size(); ++i)
      {
        status = fmus[i]->enterContinuousTimeMode();
        if (oms_status_ok != status) return status;
      }

      // find next time event
      tnext = end_time + 1.0;
      for (size_t i = 0; i < fmus.size(); ++i)
      {
        if (fmus[i]->getNextEventTimeDefined() && (tnext > fmus[i]->getNextEventTime()))
          tnext = fmus[i]->getNextEventTime();

        if (fmus[i]->getTerminateSimulation())
        {
          logInfo("Simulation terminated by FMU " + std::string(fmus[i]->getFullCref()) + " at time " + std::to_string(time));
          getModel().setStopTime(time);
          time = end_time;
        }
      }

      // The event moved the components; take the point they hold now, restart
      // IDA there and let it find a consistent one again — the algebraic
      // unknowns and the derivatives need not have survived the event.
      status = getDaePoint(solverData.ida.y, solverData.ida.yp);
      if (oms_status_ok != status) return status;

      flag = IDAReInit(solverData.ida.mem, time, solverData.ida.y, solverData.ida.yp);
      if (flag < 0) return logError("SUNDIALS_ERROR: IDAReInit() failed with flag = " + std::to_string(flag));

      status = calcConsistentInitialConditions(std::min(tnext, end_time));
      if (oms_status_ok != status) return status;

      // emit the right limit of the event
      {
        std::vector<bool> allLoops = allLoopsOf(eventGraph);
        updateInputsInternal(eventGraph, &allLoops);
      }
      if (isTopLevelSystem())
        getModel().emit(time, true);

      continue;
    }

    updateInputs(simulationGraph);
    if (isTopLevelSystem())
      getModel().emit(time, false);
  }

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
  while (time < std::min(stopTime, getModel().getStopTime()) && oms_status_ok == status)
  {
    status = doStep();
    if (status != oms_status_ok)
      logWarning("Bad return code at time " + std::to_string(time));

    if (isTopLevelSystem() && Flags::ProgressBar())
      Log::ProgressBar(startTime, stopTime, time);
  }

  if (isTopLevelSystem() && Flags::ProgressBar())
    Log::TerminateBar();

  return status;
}

oms_status_enu_t oms::SystemSC::updateInputs(DirectedGraph& graph)
{
  return updateInputsInternal(graph, nullptr);
}

oms_status_enu_t oms::SystemSC::updateInputsInternal(DirectedGraph& graph, const std::vector<bool>* liftedLoops)
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

        // Check for unit conversion and suppressUnitConversion. Set the value multiplied by factor.
        // By default, factor = 1.0. For example, mm to m will be (factor * value) => (1e-3 * value).
        if (sortedConnections[i].suppressUnitConversion)
          value = value;
        else
          value = sortedConnections[i].factor*value;

        // Check for linear transformation. Set the value multiplied by factor and added by offset i.e. (factor * value + offset).
        if (sortedConnections[i].linearTransformation.isSet)
          value = sortedConnections[i].linearTransformation.factor*value + sortedConnections[i].linearTransformation.offset;

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
      // A loop IDA owns is already satisfied by the point it set; solving it
      // again here would fight the integrator for the same unknowns.
      const bool lifted = liftedLoops && loopNum < static_cast<int>(liftedLoops->size()) && (*liftedLoops)[loopNum];
      if (!lifted)
      {
        status = solveAlgLoop(graph, loopNum);
        if (oms_status_ok != status)
        {
          forceLoopsToBeUpdated();
          return status;
        }
      }
      loopNum++;
    }
  }
  return oms_status_ok;
}
