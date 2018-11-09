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

#include "SystemSC.h"

#include "Component.h"
#include "ComponentFMUME.h"
#include "Model.h"
#include "Types.h"
#include "ssd/Tags.h"

oms3::SystemSC::SystemSC(const ComRef& cref, Model* parentModel, System* parentSystem)
  : oms3::System(cref, oms_system_sc, parentModel, parentSystem)
{
}

oms3::SystemSC::~SystemSC()
{
}

oms3::System* oms3::SystemSC::NewSystem(const oms3::ComRef& cref, oms3::Model* parentModel, oms3::System* parentSystem)
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

std::string oms3::SystemSC::getSolverName() const
{
  switch (solverMethod)
  {
    case oms_solver_explicit_euler:
      return std::string("euler");
    case oms_solver_cvode:
      return std::string("cvode");
  }

  return std::string("unknown");
}

oms_status_enu_t oms3::SystemSC::setSolverMethod(std::string solver)
{
  if (std::string("euler") == solver)
    solverMethod = oms_solver_explicit_euler;
  else if (std::string("cvode") == solver)
    solverMethod = oms_solver_cvode;
  else
    return oms_status_error;

  return oms_status_ok;
}

oms_status_enu_t oms3::SystemSC::exportToSSD_SimulationInformation(pugi::xml_node& node) const
{
  pugi::xml_node node_simulation_information = node.append_child(oms2::ssd::ssd_simulation_information);

  pugi::xml_node node_solver = node_simulation_information.append_child("VariableStepSolver");
  node_solver.append_attribute("description") = getSolverName().c_str();
  node_solver.append_attribute("absoluteTolerance") = std::to_string(absoluteTolerance).c_str();
  node_solver.append_attribute("relativeTolerance") = std::to_string(relativeTolerance).c_str();
  node_solver.append_attribute("minimumStepSize") = std::to_string(minimumStepSize).c_str();
  node_solver.append_attribute("maximumStepSize") = std::to_string(maximumStepSize).c_str();
  node_solver.append_attribute("initialStepSize") = std::to_string(initialStepSize).c_str();

  return oms_status_ok;
}

oms_status_enu_t oms3::SystemSC::importFromSSD_SimulationInformation(const pugi::xml_node& node)
{
  std::string solverName = node.child("VariableStepSolver").attribute("description").as_string();
  if (oms_status_ok != setSolverMethod(solverName))
    return oms_status_error;
  absoluteTolerance = node.child("VariableStepSolver").attribute("absoluteTolerance").as_double();
  relativeTolerance = node.child("VariableStepSolver").attribute("relativeTolerance").as_double();
  minimumStepSize = node.child("VariableStepSolver").attribute("minimumStepSize").as_double();
  maximumStepSize = node.child("VariableStepSolver").attribute("maximumStepSize").as_double();
  initialStepSize = node.child("VariableStepSolver").attribute("initialStepSize").as_double();
  return oms_status_ok;
}

oms_status_enu_t oms3::SystemSC::instantiate()
{
  // there shouldn't be any substem
  for (const auto& subsystem : getSubSystems())
    if (oms_status_ok != subsystem.second->instantiate())
      return oms_status_error;

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
      nEventIndicators.push_back(fmus.back()->getNumberOfEventIndicators());

      states.push_back((double*)calloc(nStates.back(), sizeof(double)));
      states_der.push_back((double*)calloc(nStates.back(), sizeof(double)));
      states_nominal.push_back((double*)calloc(nStates.back(), sizeof(double)));
      event_indicators.push_back((double*)calloc(nEventIndicators.back(), sizeof(double)));
      event_indicators_prev.push_back((double*)calloc(nEventIndicators.back(), sizeof(double)));
    }
  }

  if (fmus.size() == 0)
    solverMethod = oms_solver_explicit_euler;

  if (oms_solver_explicit_euler == solverMethod)
    ;
  else if (oms_solver_cvode == solverMethod)
    solverData.cvode.mem = NULL;
  else
    return logError_InternalError;

  return oms_status_ok;
}

oms_status_enu_t oms3::SystemSC::initialize()
{
  time = getModel()->getStartTime();

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
    status = fmus[i]->getContinuousStates(states[i]);
    if (oms_status_ok != status) return status;
    status = fmus[i]->getDerivatives(states_der[i]);
    if (oms_status_ok != status) return status;
    status = fmus[i]->getNominalsOfContinuousStates(states_nominal[i]);
    if (oms_status_ok != status) return status;
    status = fmus[i]->getEventindicators(event_indicators[i]);
    if (oms_status_ok != status) return status;
  }

  if (oms_status_ok != updateDependencyGraphs())
    return oms_status_error;

  if (oms_status_ok != updateInputs(initialUnknownsGraph))
    return oms_status_error;

  return oms_status_ok;
}

oms_status_enu_t oms3::SystemSC::terminate()
{
  for (const auto& subsystem : getSubSystems())
    if (oms_status_ok != subsystem.second->terminate())
      return oms_status_error;

  for (const auto& component : getComponents())
    if (oms_status_ok != component.second->terminate())
      return oms_status_error;

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

oms_status_enu_t oms3::SystemSC::stepUntil(double stopTime, void (*cb)(const char* ident, double time, oms_status_enu_t status))
{
  if (0 == fmus.size())
    return oms_status_ok;

  ComRef modelName = this->getModel()->getCref();
  fmi2_status_t fmistatus;
  oms_status_enu_t status;
  double hdef = maximumStepSize;

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

        if (oms_solver_cvode == solverMethod)
        {
          size_t offset=0;
          for (size_t k=0; k < i; ++k)
            offset += nStates[k];
          for (size_t k=0; k < nStates[i]; ++k)
            NV_Ith_S(solverData.cvode.y, offset+k) = states[i][k];
          int flag = CVodeReInit(solverData.cvode.mem, time, solverData.cvode.y);
          if (flag < 0) logError("SUNDIALS_ERROR: CVodeReInit() failed with flag = " + std::to_string(flag));
        }
      }

      // calculate next time step
      if (fmus[i]->getEventInfo()->nextEventTimeDefined && (tnext >= fmus[i]->getEventInfo()->nextEventTime))
        tnext = fmus[i]->getEventInfo()->nextEventTime;
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
    if (oms_solver_explicit_euler == solverMethod)
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
    }
    else if (oms_solver_cvode == solverMethod)
    {
      logError_NotImplemented;
    }
    else
      logError("Unknown solver method");

    // step is complete
    for (int i=0; i < fmus.size(); ++i)
    {
      fmistatus = fmi2_import_completed_integrator_step(fmus[i]->getFMU(), fmi2_true, &callEventUpdate[i], &terminateSimulation[i]);
      if (fmi2_status_ok != fmistatus) logError_FMUCall("fmi2_import_completed_integrator_step", fmus[i]);
    }

    if (isTopLevelSystem())
      getModel()->emit(time);
    updateInputs(outputsGraph);
    if (isTopLevelSystem())
      getModel()->emit(time);

    if (cb)
      cb(modelName.c_str(), time, oms_status_ok);

    if (isTopLevelSystem() && getModel()->cancelSimulation())
    {
      cb(modelName.c_str(), time, oms_status_discard);
      return oms_status_discard;
    }
  }
  time = stopTime;

  return oms_status_ok;
}

oms_status_enu_t oms3::SystemSC::updateInputs(oms3::DirectedGraph& graph)
{
  // input := output
  const std::vector< std::vector< std::pair<int, int> > >& sortedConnections = graph.getSortedConnections();
  for(int i=0; i<sortedConnections.size(); i++)
  {
    if (sortedConnections[i].size() == 1)
    {
      int output = sortedConnections[i][0].first;
      int input = sortedConnections[i][0].second;

      if (graph.getNodes()[input].getType() == oms_signal_type_real)
      {
        double value = 0.0;
        getReal(graph.getNodes()[output].getName(), value);
        setReal(graph.getNodes()[input].getName(), value);
      }
      else
        return logError("Only continuous connections are supported yet");
    }
    else
    {
      return logError("Alg. loops are not supported yet");
    }
  }
  return oms_status_ok;
}
