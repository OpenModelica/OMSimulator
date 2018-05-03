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

      cref_orderedIOAcceses_map[outputFMU].push_back(output);
      cref_orderedIOAcceses_map[inputFMU].push_back(input);
      output_connectedInputs_map[output].push_back(input);
      int dummy_k = 1; // TODO support multi-rate activation ratio
      RateTransition dummy_rt = RateTransition::SAME; // TODO support multi-rate activation ratio
      // https://stackoverflow.com/questions/27960325/stdmap-emplace-without-copying-value
      input_channel_map.emplace(std::piecewise_construct,std::forward_as_tuple(input), std::forward_as_tuple(dummy_rt, dummy_k));
    }
    else
    {
      logError("oms2::PMRChannelMap::PMRChannelMap: Algebraic loops not supported by this master algorithm");
    }
  }
}


#endif // _OMS2_PMR_CHANNEL_MAP_H_
