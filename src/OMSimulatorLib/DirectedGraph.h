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

#ifndef _OMS_DIRECTED_GRAPH_H_
#define _OMS_DIRECTED_GRAPH_H_

#include "ComRef.h"
#include "Connector.h"
#include "Variable.h"

#include <deque>
#include <fmilib.h>
#include <map>
#include <set>
#include <stack>
#include <string>
#include <vector>

namespace oms
{
  /**
   * @brief Strong connected components data type.
   *
   * A vector of pairs of connected components.
   */
  struct oms_ssc_t
  {
    std::vector< std::pair<int, int> > connections;
    bool thisIsALoop; // needed because a SSC with just one connection can be a loop! fmu.y -> fmu.u
  };

  class DirectedGraph
  {
  public:
    DirectedGraph();
    ~DirectedGraph();

    void clear();

    int addNode(const Connector& var);
    void addEdge(const Connector& var1, const Connector& var2);

    void dotExport(const std::string& filename);

    void includeGraph(const DirectedGraph& graph, const ComRef& prefix);

    const std::vector< oms_ssc_t >& getSortedConnections();

    const std::vector<Connector>& getNodes() const {return nodes;}
    const oms_ssc_t& getEdges() const {return edges;}
    const std::set<oms::ComRef>& getStronglyConnectedComponents(int i) const { return component_names[i]; }

    void dumpNodes() const;

  private:
    std::deque< std::vector<int> > getSCCs();
    void calculateSortedConnections();
    void strongconnect(int v, std::vector< std::vector<int> > G, int& index, int *d, int *low, std::stack<int>& S, bool *stacked, std::deque< std::vector<int> >& components);

    static int getEdgeIndex(const oms_ssc_t& edges, int from, int to);

  private:
    std::vector<Connector> nodes;
    oms_ssc_t edges;

    std::vector< std::set<oms::ComRef> > component_names;
    std::vector< std::vector<int> > G;
    std::vector< oms_ssc_t > sortedConnections;
    bool sortedConnectionsAreValid;
  };
}

#endif
