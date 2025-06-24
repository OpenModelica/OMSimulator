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

#pragma once
#include <vector>
#include <string>
#include "Connection.h"

namespace oms3
{
  class Solver
  {
    public:
      std::string type; // Default type
      double stepSize;  // Default step size
      double tolerance; // Default tolerance
      Solver();
      Solver(const std::string &type, double stepSize, double tolerance);
  };
  class Component
  {
    public:
      std::string name;
      std::string type;
      std::string path;
      std::vector<std::string> variables;
      Component(const std::string &name, const std::string &type,
                const std::string &path, const std::vector<std::string> &variables);
      ~Component();
  };
  class SimulationUnit
  {
    public:
      SimulationUnit(const std::vector<oms3::Component> &components,
                     oms3::Solver &solver,
                     const std::vector<oms::Connection> &connections);
      ~SimulationUnit();
      std::vector<oms3::Component> components;
      oms3::Solver solver;
      std::vector<oms::Connection> connections;
  };
} // namespace oms3
