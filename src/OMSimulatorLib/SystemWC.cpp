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

#include "SystemWC.h"

#include "Component.h"
#include "ComponentFMUCS.h"
#include "ComponentFMU3CS.h"
#include "Flags.h"
#include "Model.h"
#include "ssd/Tags.h"

#include <future>
#include <math.h>
#include <thread>

oms::SystemWC::SystemWC(const ComRef& cref, Model* parentModel, System* parentSystem)
  : oms::System(cref, oms_system_wc, parentModel, parentSystem, oms_solver_wc_ma)
{
}

oms::SystemWC::~SystemWC()
{
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
    case oms_solver_wc_mav2:
      return std::string("oms-mav2");
    default:
      return std::string("unknown");
  }
}

oms_status_enu_t oms::SystemWC::setSolverMethod(std::string solver)
{
  if (std::string("oms-ma") == solver)
    solverMethod = oms_solver_wc_ma;
  else if (std::string("oms-mav") == solver)
    solverMethod = oms_solver_wc_mav;
  else if (std::string("oms-mav2") == solver)
    solverMethod = oms_solver_wc_mav2;
  else
    return oms_status_error;

  return oms_status_ok;
}

oms_status_enu_t oms::SystemWC::exportToSSD_SimulationInformation(pugi::xml_node& node) const
{
  /* ssd:SimulationInformation should be added as vendor specific annotations from Version 1.0 */
  pugi::xml_node node_simulation_information = node.append_child(oms::ssp::Version1_0::simulation_information);
  pugi::xml_node node_solver;

  if (solverMethod == oms_solver_wc_mav || solverMethod == oms_solver_wc_mav2)
  {
    node_solver = node_simulation_information.append_child(oms::ssp::Version1_0::VariableStepMaster);
    node_solver.append_attribute("description") = getSolverName().c_str();
    node_solver.append_attribute("initialStepSize") = std::to_string(initialStepSize).c_str();
    node_solver.append_attribute("minimumStepSize") = std::to_string(minimumStepSize).c_str();
    node_solver.append_attribute("maximumStepSize") = std::to_string(maximumStepSize).c_str();
  }
  else if (solverMethod == oms_solver_wc_ma)
  {
    node_solver = node_simulation_information.append_child(oms::ssp::Version1_0::FixedStepMaster);
    node_solver.append_attribute("description") = getSolverName().c_str();
    node_solver.append_attribute("stepSize") = std::to_string(maximumStepSize).c_str();
  }

  node_solver.append_attribute("relativeTolerance") = std::to_string(relativeTolerance).c_str();

  return oms_status_ok;
}

oms_status_enu_t oms::SystemWC::importFromSSD_SimulationInformation(const pugi::xml_node& node, const std::string& sspVersion)
{
  std::string solverName = "";
  const char* FixedStepMaster = "";
  const char* VariableStepMaster = "";

  pugi::xml_node fixedStepMaster = node.child(oms::ssp::Version1_0::FixedStepMaster);
  if (fixedStepMaster)
  {
    if (sspVersion == "1.0" || sspVersion == "2.0")
    {
      solverName = fixedStepMaster.attribute("description").as_string();
      FixedStepMaster = oms::ssp::Version1_0::FixedStepMaster;
    }
    else
    {
      solverName = node.child("FixedStepMaster").attribute("description").as_string();
      FixedStepMaster = "FixedStepMaster";
    }
    initialStepSize = minimumStepSize = maximumStepSize = node.child(FixedStepMaster).attribute("stepSize").as_double();
    relativeTolerance = node.child(FixedStepMaster).attribute("relativeTolerance").as_double();
  }

  pugi::xml_node variableStepMaster = node.child(oms::ssp::Version1_0::VariableStepMaster);
  if (variableStepMaster)
  {
    if (sspVersion == "1.0" || sspVersion == "2.0")
    {
      solverName = variableStepMaster.attribute("description").as_string();
      VariableStepMaster = oms::ssp::Version1_0::VariableStepMaster;
    }
    else
    {
      solverName = node.child("VariableStepMaster").attribute("description").as_string();
      VariableStepMaster = "VariableStepMaster";
    }
    minimumStepSize = node.child(VariableStepMaster).attribute("minimumStepSize").as_double();
    maximumStepSize = node.child(VariableStepMaster).attribute("maximumStepSize").as_double();
    initialStepSize = node.child(VariableStepMaster).attribute("initialStepSize").as_double();
    relativeTolerance = node.child(VariableStepMaster).attribute("relativeTolerance").as_double();
  }

  if (oms_status_ok != setSolverMethod(solverName))
    return oms_status_error;

  return oms_status_ok;
}

oms_status_enu_t oms::SystemWC::instantiate()
{
  time = getModel().getStartTime();

  for (const auto& subsystem : getSubSystems())
    if (oms_status_ok != subsystem.second->instantiate())
      return oms_status_error;

  // The thread pool is disabled for instantiation because it caused problems on Windows for certain FMUs.
  // https://github.com/OpenModelica/OMSimulator/issues/858
  if (false && useThreadPool())
  {
    ctpl::thread_pool& pool = getThreadPool();
    std::vector<std::future<oms_status_enu_t>> results(getComponents().size());
    int i=0;
    for (const auto& component : getComponents())
    {
      results[i] = pool.push([&component](int id){ /*logInfo("Id: " + std::to_string(id));*/ return component.second->instantiate(); });
      i++;
    }

    for (auto& r : results)
    {
      oms_status_enu_t status = r.get();
      if (oms_status_ok != status)
        return oms_status_error;
    }
  }
  else
  {
    for (const auto& component : getComponents())
      if (oms_status_ok != component.second->instantiate())
        return oms_status_error;
  }

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

  if (oms_status_ok != updateInputs(initializationGraph))
    return oms_status_error;

  for (const auto& subsystem : getSubSystems())
    if (oms_status_ok != subsystem.second->initialize())
      return oms_status_error;

  for (const auto& component : getComponents())
    if (oms_status_ok != component.second->initialize())
      return oms_status_error;

  // prepare data structures for simulation
  if (solverMethod == oms_solver_wc_mav || solverMethod == oms_solver_wc_mav2)
  {
    stepSize = initialStepSize;
    bool mav_doDoubleStep = (solverMethod == oms_solver_wc_mav2); // Should we double step or not?

    mav_canGetAndSetStateFMUcomponents.clear();
    mav_FMUcomponents.clear();

    for (const auto& component : getComponents()) // Map the FMUs.
    {
      if (component.second->getCanGetAndSetState())
        mav_canGetAndSetStateFMUcomponents.insert(std::pair<ComRef, Component*>(component.first, component.second));
      else
        mav_FMUcomponents.insert(std::pair<ComRef, Component*>(component.first, component.second));
    }

    logDebug("DEBUGGING: mav_canGetAndSetStateFMUcomponents is size: " + std::to_string(mav_canGetAndSetStateFMUcomponents.size()));
    logDebug("DEBUGGING: mav_FMUcomponents is size: " + std::to_string(mav_FMUcomponents.size()));

    // make sure we can reset FMUs
    if (mav_canGetAndSetStateFMUcomponents.size() == 0)
      return logError("The adaptive step solver requires components (e.g. FMUs) that can rollback their states. None of the involved components in this model provide this functionality.");

    // check if we can double step
    if (mav_FMUcomponents.size() != 0 && mav_doDoubleStep)
      return logError("The double step approach requires that all the components can rollback their states. At least one component doesn't provide this functionality.");

  }
  else if (solverMethod == oms_solver_wc_ma)
  {
    masiMax = 2;
    if (Flags::InputExtrapolation())
    {
      for (const auto& component : getComponents())
        if (!component.second->getCanGetAndSetState())
        {
          masiMax = 1;
          logWarning("Not all components support \"canGetAndSetState\"; an explicit master algorithm will be used");
          break;
        }
    }
    else
      masiMax = 1;
  }
  else
    return logError("Invalid solver selected");

  // mark algebraic loops to be updated on next call
  forceLoopsToBeUpdated();

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

  time = getModel().getStartTime();

  return oms_status_ok;
}

oms_status_enu_t oms::SystemWC::doStep()
{
  auto start = std::chrono::steady_clock::now() + std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::duration<double>(time));

  if (solverMethod == oms_solver_wc_mav || solverMethod == oms_solver_wc_mav2)
  {
    std::vector<double> inputVectEnd;
    std::vector<double> outputVectEnd;
    std::vector<double> inputVect;
    std::vector<double> outputVect;

    if (stepSize > maximumStepSize) stepSize = maximumStepSize;
    if (stepSize < minimumStepSize) stepSize = minimumStepSize;

    double tNext = time+stepSize;
    const double stopTime = getModel().getStopTime();
    if (tNext > stopTime)
    {
      tNext = stopTime;
      stepSize = tNext-time;
    }

    logDebug("doStep: " + std::to_string(time) + " -> " + std::to_string(tNext));

    oms_status_enu_t status;

    // Get states of FMUs that can get state
    for (const auto& component : mav_canGetAndSetStateFMUcomponents)
      component.second->saveState();

    const int howManySteps = mav_doDoubleStep ? 3 : 1;
    for (int whichStepIndex = 0; whichStepIndex < howManySteps; whichStepIndex++)
    {
      // stepUntil for FMUs that can get state
      for (const auto& component : mav_canGetAndSetStateFMUcomponents)
      {
        status = component.second->stepUntil(tNext);
        if (oms_status_ok != status)
          return status;
      }

      // stepUntill for subsystems (ME-FMUs), TODO: Fix rollback here too.
      for (const auto& subsystem : getSubSystems())
      {
        status = subsystem.second->stepUntil(tNext);
        if (oms_status_ok != status)
          return status;
      }

      logDebug("DEBUGGING: Doing error control");
      // get inputs and outputs at the end of all steps.
      if (whichStepIndex == 0)
      {
        if (oms_status_ok != getInputAndOutput(eventGraph, inputVect, outputVect, mav_canGetAndSetStateFMUcomponents))
          return oms_status_error;

        if (mav_doDoubleStep) // Rollback for small steppies.
        {
          // Rollback all FMUs
          for (const auto& component : mav_canGetAndSetStateFMUcomponents)
          {
            component.second->restoreState();
          }

          //Fix time
          time = tNext-stepSize;
          for (const auto& component : getComponents())
          {
            if (oms_component_fmu == component.second->getType())
            {
              dynamic_cast<ComponentFMUCS*>(component.second)->setFmuTime(time);
            }
            if (oms_component_fmu3 == component.second->getType())
            {
              dynamic_cast<ComponentFMU3CS*>(component.second)->setFmuTime(time);
            }
          }
        }
      }
      else if (whichStepIndex == 1)
        updateInputs(eventGraph);
      else if (whichStepIndex == 2)
        if (oms_status_ok != getInputAndOutput(eventGraph, inputVectEnd, outputVectEnd, mav_canGetAndSetStateFMUcomponents))
          return oms_status_error;
    }
    logDebug("DEBUGGING: Lets do Error control");
    logDebug("DEBUGGING: inputVect is size:  " + std::to_string(inputVect.size()));
    logDebug("DEBUGGING: outputVect is size: " + std::to_string(outputVect.size()));
    if (inputVect.size() != outputVect.size())
      return oms_status_error;

    double safety_factor = 0.90;
    double maxChange = 1.5;
    double minChange = 0.5;
    maxError = 0.0;
    for (int n=0; n < inputVect.size(); n++) // Calculate error in the FMUs we do error_control on.
    {
      double error;
      if (!mav_doDoubleStep)
        error = fabs(inputVect[n]-outputVect[n]);
      else
        error = fabs(outputVectEnd[n]-outputVect[n]);

      double absoluteTolerance = relativeTolerance*1.0; //TODO: Use nominal values
      logDebug("DEBUGGING: Error is:"+std::to_string(error)+" and Scale factor is: "+std::to_string((fabs(outputVect[n])*relativeTolerance+absoluteTolerance)));

      normError = normError+pow(error/(fabs(outputVect[n])*relativeTolerance+absoluteTolerance),2);
      if (error/(fabs(outputVect[n])*relativeTolerance+absoluteTolerance) > maxError)
      {
        maxError = error/(fabs(outputVect[n])*relativeTolerance+absoluteTolerance);
        logDebug("DEBUGGING: scaled error is: " + std::to_string(error/(fabs(outputVect[n])*relativeTolerance+absoluteTolerance)) + " New biggest Differance is: " + std::to_string(maxError));
      }
    }
    normError = pow(normError, 0.5);
    double fixRatio = pow(1.0/maxError, 0.5);
    logDebug("DEBUGGING: fixRatio is: " + std::to_string(fixRatio));
    if (fixRatio < 1.0 && minimumStepSize < stepSize) //Going to rollback.
    {
      // Rollback FMUs
      for (const auto& component : mav_canGetAndSetStateFMUcomponents)
      {
        component.second->restoreState();
      }

      // Fix time
      time = tNext-stepSize;

      // Fix stepSize
      fixRatio = fixRatio*safety_factor;
      if (fixRatio < minChange) fixRatio = minChange;
      stepSize = stepSize*fixRatio;
      logDebug("DEBUGGING: Rollbacking New h is: " + std::to_string(stepSize));
      rollBackIt++;
    }
    else // Not going to rollback.
    {
      if (!mav_FMUcomponents.empty())
      {
        for (const auto& component : mav_FMUcomponents) // These FMUs cant rollback, so only simulating them when we have decided on a step to take.
        {
          if (mav_doDoubleStep)
            return logError("We shouldn't be doing double stepping when we have FMUs that can't get or set states.");
          status = component.second->stepUntil(tNext);
          if (oms_status_ok != status)
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
      bool emitted;
      if (isTopLevelSystem())
        getModel().emit(time, false, &emitted);
      updateInputs(eventGraph);
      if (isTopLevelSystem())
        getModel().emit(time, emitted);

      rollBackIt = 0;
      fixRatio = fixRatio*safety_factor;
      if (fixRatio > 1.0)
        if (fixRatio > maxChange)
          fixRatio = maxChange;
      stepSize = stepSize*fixRatio;
    }

    for (const auto& component : mav_canGetAndSetStateFMUcomponents)
      component.second->freeState();

    return oms_status_ok;
  }
  else if (solverMethod == oms_solver_wc_ma)
  {
    std::vector<double> inputVect1;
    std::vector<double> inputVect2;
    std::vector<double> inputDer;

    double tNext = time+maximumStepSize;
    const double stopTime = this->getModel().getStopTime();
    if (tNext > stopTime)
      tNext = stopTime;

    double h = tNext - time;
    logDebug("doStep: " + std::to_string(time) + " -> " + std::to_string(tNext) + ", stopTime " + std::to_string(stopTime) + ", h " + std::to_string(h));

    // save component's state
    if (masiMax > 1)
    {
      for (const auto& component : getComponents())
        component.second->saveState();
    }

    getInputs(eventGraph, inputVect1);
    for (int masi=0; masi<masiMax; masi++)
    {
      oms_status_enu_t status;
      if (useThreadPool())
      {
        ctpl::thread_pool& pool = getThreadPool();
        std::vector<std::future<oms_status_enu_t>> results(getSubSystems().size());
        int i=0;
        for (const auto& subsystem : getSubSystems())
        {
          results[i] = pool.push([&subsystem, tNext](int id){ /*logInfo("Id: " + std::to_string(id));*/ return subsystem.second->stepUntil(tNext); });
          i++;
        }

        for (auto& r : results)
        {
          status = r.get();
          if (oms_status_ok != status)
            return status;
        }
      }
      else
      {
        for (const auto& subsystem : getSubSystems())
        {
          status = subsystem.second->stepUntil(tNext);
          if (oms_status_ok != status)
            return status;
        }
      }

      if (useThreadPool())
      {
        ctpl::thread_pool& pool = getThreadPool();
        std::vector<std::future<oms_status_enu_t>> results(getComponents().size());
        int i=0;
        for (const auto& component : getComponents())
        {
          results[i] = pool.push([&component, tNext](int id){ /*logInfo("Id: " + std::to_string(id));*/ return component.second->stepUntil(tNext); });
          i++;
        }

        for (auto& r : results)
        {
          status = r.get();
          if (oms_status_ok != status)
            return status;
        }
      }
      else
      {
        for (const auto& component : getComponents())
        {
          status = component.second->stepUntil(tNext);
          if (oms_status_ok != status)
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

      if (masi < masiMax-1)
      {
        updateInputs(eventGraph);
        getInputs(eventGraph, inputVect2);
        inputDer.clear();
        for (int inputI=0; inputI<inputVect1.size(); ++inputI)
          inputDer.push_back((inputVect2[inputI]-inputVect1[inputI]) / h);

        // Restore component's state
        for (const auto& component : getComponents())
          component.second->restoreState();

        //updateInputs(outputsGraph);
        setInputsDer(eventGraph, inputDer);
      }
      else
      {
        time = tNext;
        bool emitted;
        if (isTopLevelSystem())
          getModel().emit(time, false, &emitted);
        updateInputs(eventGraph);
        if (isTopLevelSystem())
          getModel().emit(time, emitted);
      }
    }

    return oms_status_ok;
  }

  return logError("Invalid solver selected");
}

oms_status_enu_t oms::SystemWC::stepUntil(double stopTime)
{
  CallClock callClock(clock);

  // set input derivatives
  updateInputs(eventGraph);

  ComRef modelName = this->getModel().getCref();
  auto start = std::chrono::steady_clock::now() + std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::duration<double>(time));

  fmi2Status fmi_status;
  double startTime = time;
  if (Flags::ProgressBar())
    logInfo("step WC system [" + std::to_string(startTime) + "; " + std::to_string(stopTime) + "] with step size [" + std::to_string(initialStepSize) + "; " + std::to_string(minimumStepSize) + "; " + std::to_string(maximumStepSize) + "]");

  if (isTopLevelSystem())
    getModel().emit(time);

  if (solverMethod == oms_solver_wc_mav || solverMethod == oms_solver_wc_mav2)
  {
    logDebug("DEBUGGING: Entering VariableStep solver");

    // main simulation loop
    oms_status_enu_t status = oms_status_ok;
    while (time < std::min(stopTime, getModel().getStopTime()) && oms_status_ok == status)
    {
      status = doStep();

      if (isTopLevelSystem() && Flags::ProgressBar())
        Log::ProgressBar(startTime, stopTime, time);
    }

    if (isTopLevelSystem() && Flags::ProgressBar())
      Log::TerminateBar();

    return oms_status_ok;
  }
  else if (solverMethod == oms_solver_wc_ma)
  {
    logDebug("DEBUGGING: Entering FixedStep solver");

    // main simulation loop
    oms_status_enu_t status = oms_status_ok;
    while (time < std::min(stopTime, getModel().getStopTime()) && oms_status_ok == status)
    {
      status = doStep();

      if (isTopLevelSystem() && Flags::ProgressBar())
        Log::ProgressBar(startTime, stopTime, time);
    }

    if (isTopLevelSystem() && Flags::ProgressBar())
      Log::TerminateBar();

    return status;
  }
  else
    return logError("Invalid solver selected");
}

oms_status_enu_t oms::SystemWC::getRealOutputDerivative(const ComRef& cref, SignalDerivative& der)
{
  if (!getModel().validState(oms_modelState_simulation))
    return logError_ModelInWrongState(getModel().getCref());

  oms::ComRef tail(cref);
  oms::ComRef head = tail.pop_front();

  auto component = getComponents().find(head);
  if (component != getComponents().end())
    return component->second->getRealOutputDerivative(tail, der);

  return oms_status_error;
}

oms_status_enu_t oms::SystemWC::setRealInputDerivative(const ComRef& cref, const SignalDerivative& der)
{
  if (!getModel().validState(oms_modelState_simulation))
    return logError_ModelInWrongState(getModel().getCref());

  oms::ComRef tail(cref);
  oms::ComRef head = tail.pop_front();

  auto component = getComponents().find(head);
  if (component != getComponents().end())
    return component->second->setRealInputDerivative(tail, der);

  return oms_status_error;
}

oms_status_enu_t oms::SystemWC::getInputs(oms::DirectedGraph& graph, std::vector<double>& inputs)
{
  inputs.clear();
  const std::vector< scc_t >& sortedConnections = graph.getSortedConnections();
  for(int i=0; i < sortedConnections.size(); i++)
  {
    if (!sortedConnections[i].thisIsALoop)
    {
      int input = sortedConnections[i].connections[0].second;

      if (graph.getNodes()[input].getType() == oms_signal_type_real)
      {
        double value = 0.0;
        if (oms_status_ok != getReal(graph.getNodes()[input].getName(), value)) return oms_status_error;
        inputs.push_back(value);
      }
    }
  }
  return oms_status_ok;
}

oms_status_enu_t oms::SystemWC::setInputsDer(oms::DirectedGraph& graph, const std::vector<double>& inputsDer)
{
  int derI = 0;
  const std::vector<scc_t>& sortedConnections = graph.getSortedConnections();
  for(int i=0; i < sortedConnections.size(); i++)
  {
    if (!sortedConnections[i].thisIsALoop)
    {
      int input = sortedConnections[i].connections[0].second;

      if (graph.getNodes()[input].getType() == oms_signal_type_real)
      {
        if (oms_status_ok != setRealInputDerivative(graph.getNodes()[input].getName(), inputsDer[derI++]))
          return oms_status_error;
      }
    }
  }
  return oms_status_ok;
}

oms_status_enu_t oms::SystemWC::getInputAndOutput(oms::DirectedGraph& graph, std::vector<double>& inputVect, std::vector<double>& outputVect, std::map<ComRef, Component*> FMUcomponents)
{
  // FMUcomponents in will be list of FMUs that CAN GET FMUs
  const std::vector< scc_t >& sortedConnections = graph.getSortedConnections();
  inputVect.clear();
  int inCount = 0;
  outputVect.clear();
  int outCount = 0;

  for(int i=0; i < sortedConnections.size(); i++)
  {
    if (!sortedConnections[i].thisIsALoop)
    {
      logDebug("DEBUGGING: Size of sortedConnections[i] is: "+std::to_string(sortedConnections[i].connections.size()));
      int input = sortedConnections[i].connections[0].second;
      oms::ComRef inputName(graph.getNodes()[input].getName());
      oms::ComRef inputModel = inputName.pop_front();
      logDebug(inputModel);
      int output = sortedConnections[i].connections[0].first;
      oms::ComRef outputName(graph.getNodes()[output].getName());
      oms::ComRef outputModel = outputName.pop_front();
      logDebug(outputModel);
      if (FMUcomponents.find(inputModel) != FMUcomponents.end())
      {
        if (FMUcomponents.find(outputModel) != FMUcomponents.end())
        {
          if (graph.getNodes()[input].getType() == oms_signal_type_real)
          {
            double inValue = 0.0;
            if (oms_status_ok != getReal(graph.getNodes()[input].getName(), inValue)) return oms_status_error;
            logDebug("DEBUGGING: found a real input called: "+std::string(graph.getNodes()[output].getName()));
            inputVect.push_back(inValue);
            inCount++;
          }
          if (graph.getNodes()[output].getType() == oms_signal_type_real)
          {
            double outValue = 0.0;
            if (oms_status_ok != getReal(graph.getNodes()[output].getName(), outValue)) return oms_status_error;
            logDebug("DEBUGGING: found a real output called: "+std::string(graph.getNodes()[output].getName()));
            outputVect.push_back(outValue);
            outCount++;
          }
        }
      }
    }
    else
    {
      logDebug("DEBUGGING: Exiting cuz algebraic loop!");
      return oms_status_error; // algebraic loop: TODO
    }
    logDebug("DEBUGGING: we have added "+std::to_string(inCount)+" inputs and "+std::to_string(outCount)+" outputs to the vectors.");
  }

  return oms_status_ok;
}

oms_status_enu_t oms::SystemWC::updateInputs(oms::DirectedGraph& graph)
{
  CallClock callClock(clock);
  oms_status_enu_t status;
  int loopNum = 0;

  // input := output
  const std::vector<scc_t>& sortedConnections = graph.getSortedConnections();
  updateAlgebraicLoops(sortedConnections, graph);

  for(size_t i=0; i < sortedConnections.size(); i++)
  {
    if (!sortedConnections[i].thisIsALoop)
    {
      size_t output = sortedConnections[i].connections[0].first;
      size_t input = sortedConnections[i].connections[0].second;

      if (graph.getNodes()[input].getType() == oms_signal_type_real)
      {
        double value = 0.0;
        if (oms_status_ok != getReal(graph.getNodes()[output].getName(), value)) return oms_status_error;

        // Check for unit conversion and suppressUnitConversion. Set the value multiplied by factor.
        // By default, factor = 1.0. For example, mm to m will be (factor * value) => (10^-3 * value).
        if (sortedConnections[i].suppressUnitConversion)
          value = value;
        else
          value = sortedConnections[i].factor*value;

        if (oms_status_ok != setReal(graph.getNodes()[input].getName(), value)) return oms_status_error;

        // derivatives
        if (Flags::InputExtrapolation() && getModel().validState(oms_modelState_simulation))
        {
          SignalDerivative der;
          if (oms_status_ok == getRealOutputDerivative(graph.getNodes()[output].getName(), der))
          {
            //logInfo(graph.getNodes()[output].getName() + " -> " + graph.getNodes()[input].getName() + ": " + std::string(der));
            if (oms_status_ok != setRealInputDerivative(graph.getNodes()[input].getName(), der)) return oms_status_error;
          }
        }
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

oms_status_enu_t oms::SystemWC::registerSignalsForResultFile(ResultWriter& resultFile)
{
  if (Flags::WallTime())
    clock_id = resultFile.addSignal(std::string(getFullCref() + ComRef("$wallTime")), "wall-clock time [s]", SignalType_REAL);
  else
    clock_id = 0;

  if ((getSolver() == oms_solver_wc_mav || getSolver() == oms_solver_wc_mav2) && Flags::SolverStats())
  {
    h_id = resultFile.addSignal(std::string(getFullCref() + ComRef("$h")), "Step-size h [s]", SignalType_REAL);
    roll_iter_id = resultFile.addSignal(std::string(getFullCref() + ComRef("$rollbackIterations")), "How many Rollbacks were made", SignalType_INT);
    max_error_id = resultFile.addSignal(std::string(getFullCref() + ComRef("$errorMax")), "Max error", SignalType_REAL);
    error_id = resultFile.addSignal(std::string(getFullCref() + ComRef("$errorNorm")), "Normalized error from all signals", SignalType_REAL);
  }
  else
  {
    h_id = 0;
    roll_iter_id = 0;
    max_error_id = 0;
    error_id = 0;
  }

  for (const auto& component : getComponents())
    if (oms_status_ok != component.second->registerSignalsForResultFile(resultFile))
      return oms_status_error;

  for (const auto& subsystem : getSubSystems())
    if (oms_status_ok != subsystem.second->registerSignalsForResultFile(resultFile))
      return oms_status_error;

  resultFileMapping.clear();
  Connector** connectors = getConnectors();
  for (int i=0; connectors[i]; ++i)
  {
    if (!connectors[i])
      continue;
    // check for full cref in exportConnectors
    if (!exportConnectors[getFullCref()+connectors[i]->getName()])
      continue;

    auto const& connector = connectors[i];

    // check for exportName, to be used in result file to map the variable to the correct signal in ssp
    std::string name;
    if (!connector->getExportName().empty())
      name = connector->getExportName().c_str();
    else
      name = getFullCref().c_str();

  if (oms_signal_type_real == connector->getType())
  {
    unsigned int ID = resultFile.addSignal(std::string(ComRef(name) + connector->getName()), "connector", SignalType_REAL);
    resultFileMapping[ID] = i;
  }
    else if (oms_signal_type_integer == connector->getType())
    {
      unsigned int ID = resultFile.addSignal(std::string(ComRef(name) + connector->getName()), "connector", SignalType_INT);
      resultFileMapping[ID] = i;
    }
    else if (oms_signal_type_boolean == connector->getType())
    {
      unsigned int ID = resultFile.addSignal(std::string(ComRef(name) + connector->getName()), "connector", SignalType_BOOL);
      resultFileMapping[ID] = i;
    }
  }

  return oms_status_ok;
}

oms_status_enu_t oms::SystemWC::updateSignals(ResultWriter& resultFile)
{
  if (clock_id)
  {
    SignalValue_t wallTime;
    wallTime.realValue = clock.getElapsedWallTime();
    resultFile.updateSignal(clock_id, wallTime);
  }

  if ((getSolver() == oms_solver_wc_mav || getSolver() == oms_solver_wc_mav2) && Flags::SolverStats())
  {
    SignalValue_t stepS;
    stepS.realValue = stepSize;
    resultFile.updateSignal(h_id, stepS);
    SignalValue_t rollB;
    rollB.intValue = rollBackIt;
    resultFile.updateSignal(roll_iter_id, rollB);
    SignalValue_t maxErr;
    maxErr.realValue = maxError;
    resultFile.updateSignal(max_error_id, maxErr);
    SignalValue_t errNorm;
    errNorm.realValue = normError;
    resultFile.updateSignal(error_id, errNorm);
  }

  for (const auto& component : getComponents())
    if (oms_status_ok != component.second->updateSignals(resultFile))
      return oms_status_error;

  for (const auto& subsystem : getSubSystems())
    if (oms_status_ok != subsystem.second->updateSignals(resultFile))
      return oms_status_error;

  Connector** connectors = getConnectors();
  for (auto const &it : resultFileMapping)
  {
    unsigned int ID = it.first;
    auto const& connector = connectors[it.second];
    SignalValue_t value;
    if (oms_signal_type_real == connector->getType())
    {
      if (oms_status_ok != getReal(connector->getName(), value.realValue))
        return logError("failed to fetch connector " + std::string(connector->getName()));
      resultFile.updateSignal(ID, value);
    }
    else if (oms_signal_type_integer == connector->getType())
    {
      if (oms_status_ok != getInteger(connector->getName(), value.intValue))
        return logError("failed to fetch variable " + std::string(connector->getName()));
      resultFile.updateSignal(ID, value);
    }
    else if (oms_signal_type_boolean == connector->getType())
    {
      if (oms_status_ok != getBoolean(connector->getName(), value.boolValue))
        return logError("failed to fetch variable " + std::string(connector->getName()));
      resultFile.updateSignal(ID, value);
    }
  }

  return oms_status_ok;
}
