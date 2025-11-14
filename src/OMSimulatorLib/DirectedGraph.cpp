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

#include "DirectedGraph.h"
#include "Connection.h"
#include "Logging.h"
#include "Util.h"
#include "Variable.h"

#include <algorithm>
#include <deque>
#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <stack>
#include <stdlib.h>
#include <string>

oms::DirectedGraph::DirectedGraph()
{
  sortedConnectionsAreValid = true;
}

oms::DirectedGraph::~DirectedGraph()
{
}

void oms::DirectedGraph::clear()
{
  G.clear();
  sortedConnections.clear();
  nodes.clear();
  edges.connections.clear();
  sortedConnectionsAreValid = true;
}

int oms::DirectedGraph::addNode(const oms::Connector& var)
{
  nodes.push_back(var);
  std::vector<int> row;
  G.push_back(row);
  return static_cast<int>(nodes.size()) - 1;
}

void oms::DirectedGraph::addEdge(const oms::Connector& var1, const oms::Connector& var2)
{
  int index1 = -1;
  int index2 = -1;

  for (int i = 0; i < nodes.size(); ++i)
  {
    if (var1 == nodes[i])
      index1 = i;

    if (var2 == nodes[i])
      index2 = i;

    if (index1 != -1 && index2 != -1)
      break;
  }

  if (-1 == index1)
    index1 = addNode(var1);
  if (-1 == index2)
    index2 = addNode(var2);

  edges.connections.push_back(std::pair<int, int>(index1, index2));
  G[index1].push_back(index2);
  sortedConnectionsAreValid = false;
}

void oms::DirectedGraph::dotExport(const std::string& filename)
{
  /*
   * digraph G
   * {
   *   0 [label="instance.name", shape=box];
   *   1 [label="instance.name", shape=box];
   *   2 [label="instance.name", shape=box];
   *
   *   0 -> 1;
   *   0 -> 2;
   * }
   */
  std::ofstream dotFile(filename.c_str());
  dotFile << "digraph G" << std::endl;
  dotFile << "{" << std::endl;
  for (int i = 0; i < nodes.size(); i++)
  {
    dotFile << "  " << i << " [label=\"" << std::string(nodes[i]) << "\", ";
    if (nodes[i].isInput())
      dotFile << "color=\"red\", ";
    else if (nodes[i].isOutput())
      dotFile << "color=\"green\", ";
    dotFile << "shape=box];" << std::endl;
  }
  dotFile << std::endl;

  for (int i = 0; i < edges.connections.size(); i++)
  {
    dotFile << "  " << edges.connections[i].first << " -> " << edges.connections[i].second;
    if (nodes[edges.connections[i].first].isOutput() && nodes[edges.connections[i].second].isInput())
      dotFile << " [color=\"red\"];" << std::endl;
    else
      dotFile << std::endl;
  }
  dotFile << "}" << std::endl;
  dotFile.close();
}

void oms::DirectedGraph::includeGraph(const oms::DirectedGraph& graph, const oms::ComRef& prefix)
{
  for (int i = 0; i < graph.nodes.size(); i++)
    addNode(graph.nodes[i].addPrefix(prefix));

  for (int i = 0; i < graph.edges.connections.size(); i++)
    addEdge(graph.nodes[graph.edges.connections[i].first].addPrefix(prefix), graph.nodes[graph.edges.connections[i].second].addPrefix(prefix));
}

int oms::DirectedGraph::getEdgeIndex(const scc_t& edges, int from, int to)
{
  for (int i = 0; i < edges.connections.size(); ++i)
    if (edges.connections[i].first == from && edges.connections[i].second == to)
      return i;

  logError("getEdgeIndex failed");
  return -1;
}

void oms::DirectedGraph::strongconnect(int v, std::vector< std::vector<int> > G, int& index, int *d, int *low, std::stack<int>& S, bool *stacked, std::deque< std::vector<int> >& components)
{
  // Set the depth index for v to the smallest unused index
  d[v] = index;
  low[v] = index;
  index++;
  S.push(v);
  stacked[v] = true;

  // Consider successors of v
  std::vector<int> successors = G[edges.connections[v].second];
  for (int i = 0; i < successors.size(); ++i)
  {
    int w = getEdgeIndex(edges, edges.connections[v].second, successors[i]);
    if (d[w] == -1)
    {
      // Successor w has not yet been visited; recurse on it
      strongconnect(w, G, index, d, low, S, stacked, components);
      low[v] = (std::min)(low[v], low[w]); // this is done to make windows compile std::min => (std::min)
    }
    else if (stacked[w])
    {
      // Successor w is in stack S and hence in the current SCC
      // Note: The next line may look odd - but is correct.
      // It says w.index not w.lowlink; that is deliberate and from the original paper
      low[v] = (std::min)(low[v], d[w]);  // this is done to make windows compile std::min => (std::min)
    }
  }

  // If v is a root node, pop the stack and generate an SCC
  if (low[v] == d[v])
  {
    // start a new strongly connected component
    std::vector<int> SCC;
    int w;
    do
    {
      w = S.top();
      S.pop();
      stacked[w] = false;
      // add w to current strongly connected component
      SCC.push_back(w);
    } while (w != v);
    // output the current strongly connected component
    components.push_front(SCC);
  }
}

std::deque< std::vector<int> > oms::DirectedGraph::getSCCs()
{
  //std::cout << "Tarjan's strongly connected components algorithm" << std::endl;

  size_t numVertices = edges.connections.size();
  int *d = new int[numVertices];
  std::fill(d, d + numVertices, -1);
  int *low = new int[numVertices];
  int *scc = new int[numVertices];
  bool *stacked = new bool[numVertices];
  std::stack<int> S;
  int index = 0;
  std::deque< std::vector<int> > components;

  for (int v = 0; v < numVertices; ++v)
  {
    if (d[v] == -1)
      strongconnect(v, G, index, d, low, S, stacked, components);
  }

  // dump strongly connected components
  //std::cout << "Strongly connected components:" << std::endl;
  //for (int i=0; i<components.size(); ++i)
  //{
  //  for (int j=0; j<components[i].size(); ++j)
  //  {
  //    int v = components[i][j];
  //    std::cout << nodes[v].getFMUInstanceName() << "." << nodes[v].getName() << " ";
  //  }
  //  std::cout << std::endl;
  //}

  delete[] d;
  delete[] low;
  delete[] scc;
  delete[] stacked;

  return components;
}

const std::vector<oms::scc_t>& oms::DirectedGraph::getSortedConnections()
{
  if (!sortedConnectionsAreValid)
    calculateSortedConnections();
  return sortedConnections;
}

void oms::DirectedGraph::calculateSortedConnections()
{
  std::deque< std::vector<int> > components = getSCCs();
  sortedConnections.clear();

  for (int i = 0; i < components.size(); ++i)
  {
    scc_t scc;
    for (int j = 0; j < components[i].size(); ++j)
    {
      Connector& conA = nodes[edges.connections[components[i][j]].first];
      Connector& conB = nodes[edges.connections[components[i][j]].second];
      if (oms::Connection::isValid(conA.getName(), conB.getName(), conA, conB) ||
          oms::Connection::isValidExportConnectorName(conA, conB))
      {
        // std::cout << "\n valid connection from " << conA.getName().c_str() << " ==> " << conB.getName().c_str();
        // std::cout << "\n*******" << std::endl;
        scc.connections.push_back(std::pair<int, int>(edges.connections[components[i][j]]));
        scc.component_names.insert(conA.getOwner());
        scc.component_names.insert(conB.getOwner());
        // check for factor in connector units
        if (nodes[edges.connections[components[i][j]].first].connectorUnits.empty() || nodes[edges.connections[components[i][j]].second].connectorUnits.empty())
        {
          scc.factor = 1.0;
          scc.suppressUnitConversion = true;
        }
        else
        {
          double factorA = 1.0;
          double factorB = 1.0;
          for (const auto & connectorA : nodes[edges.connections[components[i][j]].first].connectorUnits)
          {
            for (const auto &baseunit : connectorA.second)
            {
              if (baseunit.first == "factor")
                factorA = std::stod(baseunit.second);
            }
            //std::cout << "\n factorA ==> " << conA.getName().c_str() << "==>" << connectorA.first << "==>" << factorA;
          }
          for (const auto &connectorB : nodes[edges.connections[components[i][j]].second].connectorUnits)
          {
            for (const auto &baseunit : connectorB.second)
            {
              if (baseunit.first == "factor")
                factorB = std::stod(baseunit.second);
            }
            //std::cout << "\n factorB ==> " << conB.getName().c_str() << "==>" << connectorB.first << "==>" << factorB;
          }
          // factor = output_Connector_Factor/ input_Connector_Factor
          scc.factor = (factorA/factorB);

          // set suppressUnitConversion = true or false
          scc.suppressUnitConversion = false;
          for (const auto &it : unitConversion)
          {
            if (it.conA == conA.getName() && it.conB == conB.getName())
            {
              scc.suppressUnitConversion = it.unitConversion;
              break;
            }
          }
        }
      }
    }


    // size of loop incl. internal connections: components[i].size()
    // size of loop excl. internal connections: connections.size()
    scc.thisIsALoop = (components[i].size() > 1);
    scc.size = scc.connections.size();
    scc.size_including_internal = components[i].size();

    if (scc.size > 0)
    {
      if (scc.thisIsALoop)
      {
        std::stringstream ss;
        ss << "Alg. loop (size " << scc.size << "/" << scc.size_including_internal << ")" << std::endl;
        for (const auto& name: scc.component_names)
          ss << "  " << std::string(name) << std::endl;
        logInfo(ss.str());
      }
      sortedConnections.push_back(scc);
    }
  }

  sortedConnectionsAreValid = true;
}

void oms::DirectedGraph::setUnits(Connector* conA, Connector* conB, bool suppressUnitConversion)
{
  /* get the full cref to check the connector owner with nodes
     (e.g) model.root.A.y1 ==> A.y1
  */
  ComRef crefA(conA->getOwner() + conA->getName());
  ComRef tailA = crefA.pop_front();
  ComRef tailB = crefA.pop_front();

  ComRef crefB(conB->getOwner() + conB->getName());
  ComRef tailA1 = crefB.pop_front();
  ComRef tailB1 = crefB.pop_front();

  unitConversion.push_back({crefA, crefB, suppressUnitConversion});

  for (auto &it : nodes)
  {
    if (it.getName() == crefA)
    {
      for (const auto &con : conA->connectorUnits)
        it.connectorUnits[con.first] = con.second;
      // set the export name for the connector
      it.setExportName(conA->getExportName());
    }
    if (it.getName() == crefB)
    {
      for (const auto &con : conB->connectorUnits)
        it.connectorUnits[con.first] = con.second;
      // set the export name for the connector
      it.setExportName(conB->getExportName());
    }
  }
}

void oms::DirectedGraph::dumpNodes() const
{
  std::string msg = "";
  for (int i = 0; i < nodes.size(); ++i)
    msg += std::to_string(i+1) + ": " + std::string(nodes[i].getName()) + "\n";
  logInfo(msg);
}
