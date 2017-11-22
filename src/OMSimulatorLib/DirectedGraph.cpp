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

#include "DirectedGraph.h"
#include "Variable.h"
#include "Logging.h"
#include "Util.h"

#include <iostream>
#include <string>
#include <fstream>
#include <map>
#include <sstream>
#include <stdlib.h>
#include <stack>
#include <algorithm>
#include <deque>

DirectedGraph::DirectedGraph()
{
  sortedConnectionsAreValid = true;
}

DirectedGraph::~DirectedGraph()
{

}

int DirectedGraph::addVariable(const Variable& var)
{
  nodes.push_back(var);
  std::vector<int> row;
  G.push_back(row);
  return static_cast<int>(nodes.size()) - 1;
}

void DirectedGraph::addEdge(const Variable& var1, const Variable& var2)
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
    index1 = addVariable(var1);
  if (-1 == index2)
    index2 = addVariable(var2);

  edges.push_back(std::pair<int, int>(index1, index2));
  G[index1].push_back(index2);
  sortedConnectionsAreValid = false;
}

void DirectedGraph::dotExport(const std::string& filename)
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
    dotFile << "  " << i << " [label=\"" << nodes[i].getFMUInstanceName() << "." << nodes[i].getName() << "\", ";
    if (nodes[i].isInput())
      dotFile << "color=\"red\", ";
    else if (nodes[i].isOutput())
      dotFile << "color=\"green\", ";
    dotFile << "shape=box];" << std::endl;
  }
  dotFile << std::endl;

  for (int i = 0; i < edges.size(); i++)
  {
    dotFile << "  " << edges[i].first << " -> " << edges[i].second;
    if (nodes[edges[i].first].isOutput() && nodes[edges[i].second].isInput())
      dotFile << " [color=\"red\"];" << std::endl;
    else
      dotFile << std::endl;
  }
  dotFile << "}" << std::endl;
  dotFile.close();
}

void DirectedGraph::includeGraph(const DirectedGraph& graph)
{
  for (int i = 0; i < graph.nodes.size(); i++)
    addVariable(graph.nodes[i]);

  for (int i = 0; i < graph.edges.size(); i++)
    addEdge(graph.nodes[graph.edges[i].first], graph.nodes[graph.edges[i].second]);
}

int getEdgeIndex(const std::vector< std::pair<int, int> >& edges, int from, int to)
{
  for (int i = 0; i < edges.size(); ++i)
    if (edges[i].first == from && edges[i].second == to)
      return i;

  logFatal("getEdgeIndex failed");
  return -1;
}

void DirectedGraph::strongconnect(int v, std::vector< std::vector<int> > G, int& index, int *d, int *low, std::stack<int>& S, bool *stacked, std::deque< std::vector<int> >& components)
{
  // Set the depth index for v to the smallest unused index
  d[v] = index;
  low[v] = index;
  index++;
  S.push(v);
  stacked[v] = true;

  // Consider successors of v
  std::vector<int> successors = G[edges[v].second];
  for (int i = 0; i < successors.size(); ++i)
  {
    int w = getEdgeIndex(edges, edges[v].second, successors[i]);
    if (d[w] == -1)
    {
      // Successor w has not yet been visited; recurse on it
      strongconnect(w, G, index, d, low, S, stacked, components);
      low[v] = std::min(low[v], low[w]);
    }
    else if (stacked[w])
    {
      // Successor w is in stack S and hence in the current SCC
      // Note: The next line may look odd - but is correct.
      // It says w.index not w.lowlink; that is deliberate and from the original paper
      low[v] = std::min(low[v], d[w]);
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

std::deque< std::vector<int> > DirectedGraph::getSCCs()
{
  //std::cout << "Tarjan's strongly connected components algorithm" << std::endl;

  size_t numVertices = edges.size();
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

  return components;
}

const std::vector< std::vector< std::pair<int, int> > >& DirectedGraph::getSortedConnections()
{
  if (!sortedConnectionsAreValid)
    calculateSortedConnections();
  return sortedConnections;
}

void DirectedGraph::calculateSortedConnections()
{
  std::deque< std::vector<int> > components = getSCCs();
  std::vector< std::pair<int, int> > SCC;
  sortedConnections.clear();

  for (int i = 0; i < components.size(); ++i)
  {
    SCC.clear();
    for (int j = 0; j < components[i].size(); ++j)
      if (nodes[edges[components[i][j]].first].isOutput() && nodes[edges[components[i][j]].second].isInput())
        SCC.push_back(std::pair<int, int>(edges[components[i][j]]));

    if (SCC.size() > 0)
      sortedConnections.push_back(SCC);

    if (SCC.size() > 1)
      logWarning("Alg. loop (size " + std::to_string(SCC.size()) + ")");
  }

  sortedConnectionsAreValid = true;
}
