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

#include "AlgLoopSolver.h"

#include "Component.h"
#include "DirectedGraph.h"
#include "Flags.h"
#include "System.h"

oms_status_enu_t oms::AlgLoopSolver::solveAlgLoop(System& syst, DirectedGraph& graph, const std::vector< std::pair<int, int> >& SCC)
{
  const int size = SCC.size();
  const int maxIterations = Flags::MaxLoopIteration();
  double maxRes;
  double absoluteTolerance, relativeTolerance;
  double *res = new double[size]();

  syst.getTolerance(&absoluteTolerance, &relativeTolerance);

  logDebug("Solving algebraic loop formed by connections\n" + dumpLoopVars(graph, SCC));

  int it=0;
  do
  {
    it++;
    // get old values
    for (int i=0; i<size; ++i)
    {
      int output = SCC[i].first;
      if (oms_status_ok != syst.getReal(graph.getNodes()[output].getName(), res[i]))
      {
        delete[] res;
        return oms_status_error;
      }
    }

    // update inputs
    for (int i=0; i<size; ++i)
    {
      int input = SCC[i].second;
      if (oms_status_ok != syst.setReal(graph.getNodes()[input].getName(), res[i]))
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
      if (oms_status_ok != syst.getReal(graph.getNodes()[output].getName(), value))
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
  {
    return logError("max. number of iterations (" + std::to_string(maxIterations) + ") exceeded at time = " + std::to_string(syst.getTime()));
  }
  logDebug("CompositeModel::solveAlgLoop: maxRes: " + std::to_string(maxRes) + ", iterations: " + std::to_string(it) + " at time = " + std::to_string(syst.getTime()));
  return oms_status_ok;
}

std::string oms::AlgLoopSolver::dumpLoopVars(DirectedGraph& graph, const std::vector< std::pair<int, int> >& SCC)
{
  const int size = SCC.size();
  std::string varNames = "\t";
  int output;

  for (int i=0; i<size-1; ++i)
  {
    output = SCC[i].first;
    varNames.append(graph.getNodes()[output].getName().c_str());
    varNames.append(" <-> ");
    output = SCC[i].second;
    varNames.append(graph.getNodes()[output].getName().c_str());
    varNames.append("\n\t");
  }
  output = SCC[size-1].first;
  varNames.append(graph.getNodes()[output].getName().c_str());
  varNames.append(" <-> ");
  output = SCC[size-1].second;
  varNames.append(graph.getNodes()[output].getName().c_str());

  return varNames;
}