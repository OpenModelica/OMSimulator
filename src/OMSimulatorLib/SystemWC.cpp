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

#include "SystemWC.h"

#include "Component.h"
#include "ComponentFMUCS.h"
#include "Flags.h"
#include "Model.h"
#include "ssd/Tags.h"
#include "SystemTLM.h"
#include "Types.h"
#include <thread>

oms::SystemWC::SystemWC(const ComRef& cref, Model* parentModel, System* parentSystem)
  : oms::System(cref, oms_system_wc, parentModel, parentSystem, oms_solver_wc_ma)
{
}

oms::SystemWC::~SystemWC()
{
  if (derBuffer)
    delete[] derBuffer;
}

oms::System* oms::SystemWC::NewSystem(const oms::ComRef& cref, oms::Model* parentModel, oms::System* parentSystem)
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

  System* system = new SystemWC(cref, parentModel, parentSystem);
  return system;
}

std::string oms::SystemWC::getSolverName() const
{
  switch (solverMethod)
  {
    case oms_solver_wc_ma:
      return std::string("oms-ma");
    case oms_solver_wc_mav:
      return std::string("oms-mav");
  }

  return std::string("unknown");
}

oms_status_enu_t oms::SystemWC::setSolverMethod(std::string solver)
{
  if (std::string("oms-ma") == solver)
    solverMethod = oms_solver_wc_ma;
  else if (std::string("oms-mav") == solver)
    solverMethod = oms_solver_wc_mav;
  else
    return oms_status_error;

  return oms_status_ok;
}

oms_status_enu_t oms::SystemWC::exportToSSD_SimulationInformation(pugi::xml_node& node) const
{
  pugi::xml_node node_simulation_information = node.append_child(oms::ssd::ssd_simulation_information);

  pugi::xml_node node_solver = node_simulation_information.append_child("FixedStepMaster");
  node_solver.append_attribute("description") = getSolverName().c_str();
  node_solver.append_attribute("stepSize") = std::to_string(maximumStepSize).c_str();

  return oms_status_ok;
}

oms_status_enu_t oms::SystemWC::importFromSSD_SimulationInformation(const pugi::xml_node& node)
{
  std::string solverName = node.child("FixedStepMaster").attribute("description").as_string();
  if (oms_status_ok != setSolverMethod(solverName))
    return oms_status_error;
  initialStepSize=minimumStepSize=maximumStepSize = node.child("FixedStepMaster").attribute("stepSize").as_double();
  return oms_status_ok;
}

oms_status_enu_t oms::SystemWC::instantiate()
{
  time = getModel()->getStartTime();

  for (const auto& subsystem : getSubSystems())
    if (oms_status_ok != subsystem.second->instantiate())
      return oms_status_error;

  for (const auto& component : getComponents())
    if (oms_status_ok != component.second->instantiate())
      return oms_status_error;

  return oms_status_ok;
}

unsigned int oms::SystemWC::getMaxOutputDerivativeOrder()
{
  unsigned int order = 0;

  for (auto& component : getComponents())
  {
    if (oms_component_fmu == component.second->getType())
      if (order < component.second->getFMUInfo()->getMaxOutputDerivativeOrder())
        order = component.second->getFMUInfo()->getMaxOutputDerivativeOrder();
  }

  return order;
}

oms_status_enu_t oms::SystemWC::initialize()
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

  if (derBuffer)
    delete[] derBuffer;
  derBuffer = NULL;
  if (Flags::InputDerivatives())
    derBuffer = new double[getMaxOutputDerivativeOrder()];

  return oms_status_ok;
}

oms_status_enu_t oms::SystemWC::terminate()
{
  for (const auto& subsystem : getSubSystems())
    if (oms_status_ok != subsystem.second->terminate())
      return oms_status_error;

  for (const auto& component : getComponents())
    if (oms_status_ok != component.second->terminate())
      return oms_status_error;

  return oms_status_ok;
}

oms_status_enu_t oms::SystemWC::reset()
{
  for (const auto& subsystem : getSubSystems())
    if (oms_status_ok != subsystem.second->reset())
      return oms_status_error;

  for (const auto& component : getComponents())
    if (oms_status_ok != component.second->reset())
      return oms_status_error;

  time = getModel()->getStartTime();

  return oms_status_ok;
}

oms_status_enu_t oms::SystemWC::stepUntil(double stopTime, void (*cb)(const char* ident, double time, oms_status_enu_t status))
{
  CallClock callClock(clock);
  ComRef modelName = this->getModel()->getCref();
  auto start = std::chrono::steady_clock::now() + std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::duration<double>(time));

  double startTime=time;

  if (Flags::ProgressBar())
    logInfo("stepUntil [" + std::to_string(startTime) + "; " + std::to_string(stopTime) + "]");

  if (isTopLevelSystem())
    getModel()->emit(time);

  while (time < stopTime)
  {
    double tNext = time+maximumStepSize;
    if (tNext > stopTime)
      tNext = stopTime;

    logDebug("doStep: " + std::to_string(time) + " -> " + std::to_string(tNext));

    oms_status_enu_t status;
    for (const auto& subsystem : getSubSystems())
    {
      status = subsystem.second->stepUntil(tNext, NULL);
      if (oms_status_ok != status)
      {
        if (cb)
          cb(modelName.c_str(), tNext, status);
        return status;
      }
    }

    for (const auto& component : getComponents())
    {
      status = component.second->stepUntil(tNext);
      if (oms_status_ok != status)
      {
        if (cb)
          cb(modelName.c_str(), tNext, status);
        return status;
      }
    }

    if (Flags::RealTime())
    {
      auto now = std::chrono::steady_clock::now();
      // seems a cast to a sufficient high resolution of time is necessary for avoiding truncation errors
      auto next = start + std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::duration<double>(tNext));
      std::chrono::duration<double> margin = std::chrono::duration<double>(next - now);
      if (margin < std::chrono::duration<double>(0))
        logWarning("real-time frame overrun, time=" + std::to_string(tNext) + "s, exceeded margin=" + std::to_string(margin.count()) + "s");
      else
        std::this_thread::sleep_until(next);
    }

    time = tNext;
    if (isTopLevelSystem())
      getModel()->emit(time);
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

  if (Flags::ProgressBar())
  {
    Log::ProgressBar(startTime, stopTime, time);
    Log::TerminateBar();
  }
  return oms_status_ok;
}

oms_status_enu_t oms::SystemWC::getRealOutputDerivative(const ComRef& cref, double*& value, unsigned int& order)
{
  if (!value)
    return oms_status_ok;

  if (!getModel()->validState(oms_modelState_simulation))
    return logError_ModelInWrongState(getModel());

  oms::ComRef tail(cref);
  oms::ComRef head = tail.pop_front();

  auto component = getComponents().find(head);
  if (component != getComponents().end() && oms_component_fmu == component->second->getType())
  {
    order = component->second->getFMUInfo()->getMaxOutputDerivativeOrder();
    if (order > 0)
      return dynamic_cast<ComponentFMUCS*>(component->second)->getRealOutputDerivative(tail, value);
  }

  return oms_status_error;
}

oms_status_enu_t oms::SystemWC::setRealInputDerivative(const ComRef& cref, double* value, unsigned int order)
{
  if (!value)
    return oms_status_ok;

  if (!getModel()->validState(oms_modelState_simulation))
    return logError_ModelInWrongState(getModel());

  oms::ComRef tail(cref);
  oms::ComRef head = tail.pop_front();

  auto component = getComponents().find(head);
  if (component != getComponents().end() && oms_component_fmu == component->second->getType())
  {
    if (order > 0)
      return dynamic_cast<ComponentFMUCS*>(component->second)->setRealInputDerivative(tail, value, order);
  }

  return oms_status_error;
}

oms_status_enu_t oms::SystemWC::setRealInputDerivative(const ComRef& cref, double value)
{
  return setRealInputDerivative(cref, &value, 1);
}

oms_status_enu_t oms::SystemWC::updateInputs(oms::DirectedGraph& graph)
{
  CallClock callClock(clock);

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
        if (oms_status_ok != getReal(graph.getNodes()[output].getName(), value)) return oms_status_error;
        if (oms_status_ok != setReal(graph.getNodes()[input].getName(), value)) return oms_status_error;

        // derivatives
        if (derBuffer)
        {
          unsigned int order;
          if (oms_status_ok == getRealOutputDerivative(graph.getNodes()[output].getName(), derBuffer, order))
          {
            //logInfo(graph.getNodes()[output].getName() + " -> " + graph.getNodes()[input].getName() + ": " + std::to_string(derBuffer[0]));
            if (oms_status_ok != setRealInputDerivative(graph.getNodes()[input].getName(), derBuffer, order)) return oms_status_error;
          }
        }
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
      if (oms_status_ok != solveAlgLoop(graph, sortedConnections[i])) return oms_status_error;
    }
  }
  return oms_status_ok;
}


oms_status_enu_t oms::SystemWC::solveAlgLoop(DirectedGraph& graph, const std::vector< std::pair<int, int> >& SCC)
{
  CallClock callClock(clock);

  const int size = SCC.size();
  const int maxIterations = 10;
  double maxRes;
  double *res = new double[size]();

  int it=0;
  do
  {
    it++;
    // get old values
    for (int i=0; i<size; ++i)
    {
      int output = SCC[i].first;
      if (oms_status_ok != getReal(graph.getNodes()[output].getName(), res[i]))
      {
        delete[] res;
        return oms_status_error;
      }
    }

    // update inputs
    for (int i=0; i<size; ++i)
    {
      int input = SCC[i].second;
      if (oms_status_ok != setReal(graph.getNodes()[input].getName(), res[i]))
      {
        delete[] res;
        return oms_status_error;
      }
    }

    // calculate residuals
    maxRes = 0.0;
    double value;
    for (int i=0; i<size; ++i)
    {
      int output = SCC[i].first;
      if (oms_status_ok != getReal(graph.getNodes()[output].getName(), value))
      {
        delete[] res;
        return oms_status_error;
      }
      res[i] -= value;

      if (fabs(res[i]) > maxRes)
        maxRes = fabs(res[i]);
    }
  } while(maxRes > absoluteTolerance && it < maxIterations);

  delete[] res;

  if (it >= maxIterations)
    return logError("max. number of iterations (" + std::to_string(maxIterations) + ") exceeded at time = " + std::to_string(getTime()));
  logDebug("CompositeModel::solveAlgLoop: maxRes: " + std::to_string(maxRes) + ", iterations: " + std::to_string(it) + " at time = " + std::to_string(getTime()));
  return oms_status_ok;
}
