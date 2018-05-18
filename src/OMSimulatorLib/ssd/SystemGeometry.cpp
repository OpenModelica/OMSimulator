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

#include "SystemGeometry.h"
#include "../Logging.h"

#include <string.h>

oms2::ssd::SystemGeometry::SystemGeometry()
{
  logTrace();

  this->x1 = 0.0; // -10.0;
  this->y1 = 0.0; // -10.0;
  this->x2 = 0.0; // 10.0;
  this->y2 = 0.0; // 10.0;
}

oms2::ssd::SystemGeometry::SystemGeometry(const oms2::ssd::SystemGeometry& rhs)
{
  logTrace();

  this->x1 = rhs.x1;
  this->y1 = rhs.y1;
  this->x2 = rhs.x2;
  this->y2 = rhs.y2;
}

oms2::ssd::SystemGeometry::~SystemGeometry()
{
  logTrace();
}

oms2::ssd::SystemGeometry& oms2::ssd::SystemGeometry::operator=(oms2::ssd::SystemGeometry const& rhs)
{
  logTrace();

  // check for self-assignment
  if(&rhs == this)
    return *this;

  this->x1 = rhs.x1;
  this->y1 = rhs.y1;
  this->x2 = rhs.x2;
  this->y2 = rhs.y2;

  return *this;
}
