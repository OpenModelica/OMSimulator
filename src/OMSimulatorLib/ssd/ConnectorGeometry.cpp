/*
 * This file is part of OpenModelica.
 *
 * Copyright (c) 1998-2026, Open Source Modelica Consortium (OSMC),
 * c/o Linköpings universitet, Department of Computer and Information Science,
 * SE-58183 Linköping, Sweden.
 *
 * All rights reserved.
 *
 * THIS PROGRAM IS PROVIDED UNDER THE TERMS OF AGPL VERSION 3 LICENSE OR
 * THIS OSMC PUBLIC LICENSE (OSMC-PL) VERSION 1.8.
 * ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS PROGRAM CONSTITUTES
 * RECIPIENT'S ACCEPTANCE OF THE OSMC PUBLIC LICENSE OR THE GNU AGPL
 * VERSION 3, ACCORDING TO RECIPIENTS CHOICE.
 *
 * The OpenModelica software and the OSMC (Open Source Modelica Consortium)
 * Public License (OSMC-PL) are obtained from OSMC, either from the above
 * address, from the URLs:
 * http://www.openmodelica.org or
 * https://github.com/OpenModelica/ or
 * http://www.ida.liu.se/projects/OpenModelica,
 * and in the OpenModelica distribution.
 *
 * GNU AGPL version 3 is obtained from:
 * https://www.gnu.org/licenses/licenses.html#GPL
 *
 * This program is distributed WITHOUT ANY WARRANTY; without
 * even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE, EXCEPT AS EXPRESSLY SET FORTH
 * IN THE BY RECIPIENT SELECTED SUBSIDIARY LICENSE CONDITIONS OF OSMC-PL.
 *
 * See the full OSMC Public License conditions for more details.
 *
 */

#include "ConnectorGeometry.h"

#include "../Logging.h"
#include "Tags.h"

#include <string.h>

oms::ssd::ConnectorGeometry::ConnectorGeometry(double x, double y)
{
  logTrace();

  this->x = x;
  this->y = y;
}

oms::ssd::ConnectorGeometry::ConnectorGeometry(const oms::ssd::ConnectorGeometry& rhs)
{
  logTrace();

  this->x = rhs.x;
  this->y = rhs.y;
}

oms::ssd::ConnectorGeometry::~ConnectorGeometry()
{
  logTrace();
}

oms::ssd::ConnectorGeometry& oms::ssd::ConnectorGeometry::operator=(oms::ssd::ConnectorGeometry const& rhs)
{
  logTrace();

  // check for self-assignment
  if(&rhs == this)
    return *this;

  this->x = rhs.x;
  this->y = rhs.y;

  return *this;
}

oms_status_enu_t oms::ssd::ConnectorGeometry::exportToSSD(pugi::xml_node& root) const
{
  pugi::xml_node node = root.append_child(oms::ssp::Draft20180219::ssd::connector_geometry);
  node.append_attribute("x") = std::to_string(x).c_str();
  node.append_attribute("y") = std::to_string(y).c_str();

  return oms_status_ok;
}
