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

#include <vector>
#include <string>
#include "oms3/SimulationUnit.h"


oms3::Component::Component(const std::string& name, const std::string& type,
                            const std::string& path, const std::vector<std::string>& variables)
{
  this->name = name;
  this->type = type;
  this->path = path;
  this->variables = variables;
}

oms3::Component::~Component()
{
}

oms3::Solver::Solver(const std::string& type, double stepSize, double tolerance)
{
  // Default constructor logic if needed
  this->type = type;
  this->stepSize = stepSize;
  this->tolerance = tolerance;
}
oms3::Solver::Solver()
{
  this->type = "default"; // Default solver type
  this->stepSize = 0.01; // Default step size
  this->tolerance = 1e-6; // Default tolerance
}

oms3::SimulationUnit::SimulationUnit(const std::vector<oms3::Component>& components,
                                     oms3::Solver& solver,
                                     const std::vector<oms::Connection>& connections)
{
  this->components = components;
  this->solver = solver;
  this->connections = connections;
}
oms3::SimulationUnit::~SimulationUnit()
{
    // Destructor logic if needed
}