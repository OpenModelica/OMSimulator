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

#ifndef _OMS2_PMR_CHANNEL_MAP_H_
#define _OMS2_PMR_CHANNEL_MAP_H_


#include <string>
#include <map>
#include <vector>

#include "Pkg_oms2.h"
#include "DirectedGraph.h"
#include "PMRChannel.h"
#include "Logging.h"
#include "Scope.h"

namespace oms2
{
  /**
   * /brief Map input ports to communication channels.
   */
  template <template<class> class PMRChannel>
  class PMRChannelMap
  {
  public:
    explicit PMRChannelMap(DirectedGraph* directedGraph);
    ~PMRChannelMap() noexcept {}
    PMRChannelMap(const PMRChannelMap& other) = delete; //!< Copy constructor
    PMRChannelMap (PMRChannelMap&& other) noexcept = delete; //!< Move constructor
    PMRChannelMap& operator= (const PMRChannelMap& other) = delete; //!< Copy assignment operator
    PMRChannelMap& operator= (PMRChannelMap&& other) noexcept = delete; //!< Move assignment operator

    const std::vector<int>& orderedIOAccess(std::string modelCref) const {return cref_orderedIOAcceses_map.at(modelCref);}
    const std::vector<int>& connectedInputs(int outputNodeIndex) const {return output_connectedInputs_map.at(outputNodeIndex);}
    PMRChannel<double>& realChannel(int inputNodeIndex) {return input_channel_map.at(inputNodeIndex);}
    // TODO `integerChannel(..)` `booleanChannel(..)` ...

    DirectedGraph* graph;

  private:
    std::map<std::string, std::vector<int> > cref_orderedIOAcceses_map;
    std::map<int, std::vector<int> > output_connectedInputs_map;
    std::map<int, PMRChannel<double> > input_channel_map;
  };
}


template <template<class> class PMRChannel>
oms2::PMRChannelMap<PMRChannel>::PMRChannelMap(DirectedGraph* dg) : graph(dg)
{
  logTrace();
  if(!dg)
  {
    logError("oms2::PMRChannelMap::PMRChannelMap: Invalid pointer");
    exit(1);
  }

  // input = output
  const std::vector< std::vector< std::pair<int, int> > >& sc = dg->getSortedConnections();
  for(int i=0; i<sc.size(); i++)
  {
    if (sc[i].size() == 1)
    {
      int output = sc[i][0].first;
      int input = sc[i][0].second;

      const std::string& outputFMU = dg->nodes[output].getCref().toString();
      logDebug("oms2::PMRChannelMap::PMRChannelMap: Creating channel connecting input "
      + dg->nodes[input].toString() + std::string(" to output ") + dg->nodes[output].toString());

      const std::string& inputFMU = dg->nodes[input].getCref().toString();

      // std::cout << "oms2::PMRChannelMap::PMRChannelMap: outputFMU "
      // + outputFMU + std::string(" to inputFMU ") + inputFMU << std::endl;
      // std::cout << "oms2::PMRChannelMap::PMRChannelMap: describeModel(outputFMU) "
      //   << oms2::Scope::GetInstance().describeModel(dg->nodes[output].getCref()) << std::endl;

      // ********************************************
      // bthiele: Below is rather ugly code to determine the rate transition using the experimental quick-and-dirty facilities.
      // Should do for experimenting with it, but a more mature implementation should aim for a nicer way.
      int arInput=1, arOutput=1, k=1;
      RateTransition rt;
      Model* model = oms2::Scope::GetInstance().getModel(dg->nodes[input].getCref().first());
      if (!model) logError("[oms2::PMRChannelMap<PMRChannel>::PMRChannelMap] failed");
      // FMI model?
      if (oms_component_fmi == model->getType())
      {
        FMICompositeModel* fmiModel = model->getFMICompositeModel();
        FMISubModel* inputModel = fmiModel->getSubModel(dg->nodes[input].getCref());
        if (!inputModel) logError("[oms2::PMRChannelMap<PMRChannel>::PMRChannelMap] failed");
        arInput = inputModel->getActivationRatio();
        FMISubModel* outputModel = fmiModel->getSubModel(dg->nodes[output].getCref());
        if (!outputModel) logError("[oms2::PMRChannelMap<PMRChannel>::PMRChannelMap] failed");
        arOutput = outputModel->getActivationRatio();
      }
      else
      {
        logError("[oms2::PMRChannelMap<PMRChannel>::PMRChannelMap] is only implemented for FMI models yet");
      }
      if (arOutput < arInput)
      {
        rt = RateTransition::FASTTOSLOW;
        if ((arInput % arOutput) != 0)
        {
          logError("[oms2::PMRChannelMap<PMRChannel>::PMRChannelMap] activation ratios between " +
          inputFMU + std::string(" and " ) + outputFMU + " have non-integer value. Exiting...");
          exit(1);
        }
        k = arInput  / arOutput;
      }
      else if (arOutput > arInput)
      {
        rt = RateTransition::SLOWTOFAST;
        if ((arOutput % arInput) != 0)
        {
          logError("[oms2::PMRChannelMap<PMRChannel>::PMRChannelMap] activation ratios between " +
          outputFMU + std::string(" and " ) + inputFMU + " have non-integer value. Exiting...");
          exit(1);
        }
        k = arOutput / arInput;
      }
      else {
        rt = RateTransition::SAME;
        k = 1;
      }
      std::cout << "[oms2::PMRChannelMap<PMRChannel>::PMRChannelMap] Creating " << static_cast<uint32_t>(rt) << " channel with k=" << k << " connecting input "
      + dg->nodes[input].toString() + std::string(" to output ") + dg->nodes[output].toString() << std::endl;
      // ********************************************

      cref_orderedIOAcceses_map[outputFMU].push_back(output);
      cref_orderedIOAcceses_map[inputFMU].push_back(input);
      output_connectedInputs_map[output].push_back(input);
      // https://stackoverflow.com/questions/27960325/stdmap-emplace-without-copying-value
      input_channel_map.emplace(std::piecewise_construct,std::forward_as_tuple(input), std::forward_as_tuple(rt, k));
    }
    else
    {
      logError("oms2::PMRChannelMap::PMRChannelMap: Algebraic loops not supported by this master algorithm");
    }
  }
}


#endif // _OMS2_PMR_CHANNEL_MAP_H_
