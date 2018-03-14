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

#include "ConnectorGeometry.h"
#include "../Logging.h"

#include <string.h>

oms2::ssd::ConnectorGeometry::ConnectorGeometry(double x, double y)
{
  logTrace();

  this->x = x;
  this->y = y;
}

oms2::ssd::ConnectorGeometry::ConnectorGeometry(const oms2::ssd::ConnectorGeometry& rhs)
{
  logTrace();

  this->x = rhs.x;
  this->y = rhs.y;
}

oms2::ssd::ConnectorGeometry::~ConnectorGeometry()
{
  logTrace();
}

oms2::ssd::ConnectorGeometry& oms2::ssd::ConnectorGeometry::operator=(oms2::ssd::ConnectorGeometry const& rhs)
{
  logTrace();

  // check for self-assignment
  if(&rhs == this)
    return *this;

  this->x = rhs.x;
  this->y = rhs.y;

  return *this;
}

oms_status_enu_t oms2::ssd::ConnectorGeometry::exportToSSD(pugi::xml_node& root) const
{
  pugi::xml_node node = root.append_child("ssd:ConnectorGeometry");
  node.append_attribute("x") = std::to_string(x).c_str();
  node.append_attribute("y") = std::to_string(y).c_str();

  return oms_status_ok;
}
