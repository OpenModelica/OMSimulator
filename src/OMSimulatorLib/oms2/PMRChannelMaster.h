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

#include "PMRChannel.h"
#include "PMRChannelMap.h"
#include "FMISubModel.h"
#include "SignalRef.h"

#ifndef _OMS2_PMR_CHANNEL_MASTER_H_
#define _OMS2_PMR_CHANNEL_MASTER_H_

namespace oms2
{
  /**
   * \brief Spawns threads for Parallel Multi-Rate simulation using a communication channels based approach.
   *
   * This template function can be used with different implementations of the utilized communication channel (see file PMRChannel.h).
   *
   * It spawns threads with the function threadPMRChannel().
   */
  template <template<class> class PMRChannel>
  oms_status_enu_t stepUntilPMRChannel(ResultWriter& resultWriter, double stopTime, double communicationInterval, std::string compositeName, oms2::DirectedGraph& outputsGraph, std::map<oms2::ComRef, oms2::FMISubModel*>& subModels, bool realtime_sync);

  /**
   * \brief Thread for communication channel based parallel multi-rate simulation approach.
   *
   * PMR experiment, with experimental "clocks".
   *
   */
  template <template<class> class PMRChannel>
  void threadPMRChannel(int tid, std::string subModelCrefStr, oms2::PMRChannelMap<PMRChannel>& channels, oms2::FMISubModel* fmu, double stopTime, double communicationInterval, bool realtime_sync);

  template <template<class> class PMRChannel>
  inline void writeOutputToConnectedInputChannels(int output, oms2::PMRChannelMap<PMRChannel>& channels, oms2::FMISubModel* fmu);

  template <template<class> class PMRChannel>
  inline void writeInputChannelToFMU(int input, oms2::PMRChannelMap<PMRChannel>& channels, oms2::FMISubModel* fmu);

} // namespace oms2


/* ************************************ */
/* Template function implementations    */
/* ************************************ */

template <template<class> class PMRChannel>
oms_status_enu_t oms2::stepUntilPMRChannel(ResultWriter& resultWriter, double stopTime, double communicationInterval, std::string compositeName, oms2::DirectedGraph& outputsGraph, std::map<oms2::ComRef, oms2::FMISubModel*>& subModels, bool realtime_sync)
{
  logTrace();

  oms2::PMRChannelMap<PMRChannel> channels(&outputsGraph);

  /* Spawn threads */
  std::vector<std::thread> t;
  int i;

  std::map<oms2::ComRef, oms2::FMISubModel*>::iterator it;
  for (it=subModels.begin(), i=0; it != subModels.end(); ++it, ++i) {
    std::string fmuInstName = it->first.toString();
    /* Question: Nicer to use actual ComRef objects as key instead of strings? */
    std::string cref_str =  compositeName + std::string(".") + fmuInstName;
    oms2::FMISubModel* fmiSubModel = it->second;
    int activationRatio = fmiSubModel->getActivationRatio();
    // logInfo(std::string("oms2::simulatePMRChannel: Spawning thread for ") + cref_str);
    t.push_back(std::thread(threadPMRChannel<PMRChannel>, i, cref_str, std::ref(channels), fmiSubModel,
      stopTime, communicationInterval*activationRatio, realtime_sync));
  }

  /* Join the threads with the main thread */
  for (int i=0; i < subModels.size(); ++i) {
    t[i].join();
  }

  return oms_status_ok;
}

template <template<class> class PMRChannel>
void oms2::threadPMRChannel(int tid, std::string subModelCrefStr, oms2::PMRChannelMap<PMRChannel>& channels, oms2::FMISubModel* fmu, double stopTime, double communicationInterval, bool realtime_sync)
{
  logTrace();
  logInfo(std::string("oms2::threadPMRChannel: Started thread ") + std::to_string(tid) + " for submodule (FMU): " + subModelCrefStr);

  oms2::DirectedGraph* graph = channels.graph;

  std::vector<int> orderedIOAcceses = channels.orderedIOAccess(subModelCrefStr);
  double tcur = 0;
  auto start = std::chrono::steady_clock::now();
  while(tcur < stopTime - 0.1*communicationInterval) // FIXME seem to need some tolerance, but not sure why?
  {
    // std::cout << std::string("oms2::threadPMRChannel: step START while ") + std::to_string(tid) + " for submodule (FMU): " + subModelCrefStr << " tcur=" << tcur << std::endl;
    for (int i: orderedIOAcceses) {
      if (graph->nodes[i].isOutput()) {
        writeOutputToConnectedInputChannels(i, channels, fmu);
      } else  {
        assert(graph->nodes[i].isInput());
        writeInputChannelToFMU(i, channels, fmu);
      }
    }

    tcur += communicationInterval;
    if (tcur > stopTime)
      tcur = stopTime;
    fmu->doStep(tcur);

    if (realtime_sync)
    {
      auto now = std::chrono::steady_clock::now();
      // seems a cast to a sufficient high resolution of time is necessary for avoiding truncation errors
      auto next = start + std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::duration<double>(tcur));
      std::chrono::duration<double> margin = next - now;
      // std::cout << "oms2::threadPMRChannel] doStep: " << std::to_string(tcur  - communicationInterval) << "s -> " << std::to_string(tcur) << "s, submodule (FMU)=" << subModelCrefStr << ", real-time margin=" << std::to_string(margin.count()) << "s" << std::endl;
      if (margin < std::chrono::duration<double>(0))
        logError(std::string("[oms2::threadPMRChannel] real-time frame overrun, time=") + std::to_string(tcur) + std::string("s, submodule (FMU)=") + subModelCrefStr + std::string(", exceeded margin=") + std::to_string(margin.count()) + std::string("s\n"));

      std::this_thread::sleep_until(next);
    }

    // std::cout << std::string("oms2::threadPMRChannel: step END while ") + std::to_string(tid) + " for submodule (FMU): " + subModelCrefStr << " tcur=" << tcur << std::endl;
  }
  // std::cout << std::string("oms2::threadPMRChannel: Ending thread ") + std::to_string(tid) + " for submodule (FMU): " + subModelCrefStr + "\n";
}

template <template<class> class PMRChannel>
inline void oms2::writeOutputToConnectedInputChannels(int output, oms2::PMRChannelMap<PMRChannel>& channels, oms2::FMISubModel* fmu)
{
  logTrace();
  oms2::DirectedGraph* graph = channels.graph;
  if (graph->nodes[output].isTypeReal())
  {
      double value = 0;
      fmu->getReal(graph->nodes[output].getSignalRef(), value);
      for (int input: channels.connectedInputs(output)) {
        channels.realChannel(input).write(value);
      }
  }
  else if (graph->nodes[output].isTypeInteger())
  {
    int value = 0;
    logError("writeOutputToConnectedInputChannels: Support for Integer type not implemented, yet");
    // fmu->getInteger(graph->nodes[output].getSignalRef(), value);
    // for (int input: channels.connectedInputs(output)) {
    //   channels.integerChannel(input).write(value);
    // }
  }
  else if (graph->nodes[output].isTypeBoolean())
  {
    int value = 0; //
    logError("writeOutputToConnectedInputChannels: Support for Boolean type not implemented, yet");
    // fmu->getBoolean(graph->nodes[output].getSignalRef(), value);
    // for (int input: channels.connectedInputs(output)) {
    //   channels.integerChannel(input).write(value); // also use integer channel for Booleans?
    // }
  }
  else {
    logError("writeOutputToConnectedInputChannels: Unsupported type");
  }
}

template <template<class> class PMRChannel>
inline void oms2::writeInputChannelToFMU(int input, oms2::PMRChannelMap<PMRChannel>& channels, oms2::FMISubModel* fmu)
{
  logTrace();
  oms2::DirectedGraph* graph = channels.graph;
  if (graph->nodes[input].isTypeReal())
  {
    double value = channels.realChannel(input).read();
    fmu->setReal(graph->nodes[input].getSignalRef(), value);
  }
  else if (graph->nodes[input].isTypeInteger())
  {
    logError("writeInputChannelToFMU: Support for Integer type not implemented, yet");
  }
  else if (graph->nodes[input].isTypeBoolean())
  {
    logError("writeInputChannelToFMU: Support for Boolean type not implemented, yet");
  }
  else {
    logError("writeInputChannelToFMU: Unsupported type");
  }
}

#endif // _OMS2_PMR_CHANNEL_MASTER_H_
