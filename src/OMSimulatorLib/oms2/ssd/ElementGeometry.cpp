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

#include "ElementGeometry.h"
#include "../Logging.h"

#include <string.h>

oms2::ssd::ElementGeometry::ElementGeometry()
{
  logTrace();

  this->x1 = -10.0;
  this->y1 = -10.0;
  this->x2 = 10.0;
  this->y2 = 10.0;
  this->rotation = 0.0;
  this->iconSource = NULL;
  this->iconRotation = 0.0;
  this->iconFlip = false;
  this->iconFixedAspectRatio = false;
}

oms2::ssd::ElementGeometry::ElementGeometry(const oms2::ssd::ElementGeometry& rhs)
{
  logTrace();

  this->x1 = rhs.x1;
  this->y1 = rhs.y1;
  this->x2 = rhs.x2;
  this->y2 = rhs.y2;
  this->rotation = rhs.rotation;

  if (rhs.iconSource)
  {
    size_t size = strlen(rhs.iconSource) + 1;
    this->iconSource = new char[size];
    memcpy(this->iconSource, rhs.iconSource, size*sizeof(char));
  }
  else
    this->iconSource = NULL;

  this->iconRotation = rhs.iconRotation;
  this->iconFlip = rhs.iconFlip;
  this->iconFixedAspectRatio = rhs.iconFixedAspectRatio;
}

oms2::ssd::ElementGeometry::~ElementGeometry()
{
  logTrace();

  if (this->iconSource)
    delete[] this->iconSource;
}

oms2::ssd::ElementGeometry& oms2::ssd::ElementGeometry::operator=(oms2::ssd::ElementGeometry const& rhs)
{
  logTrace();

  // check for self-assignment
  if(&rhs == this)
    return *this;

  this->x1 = rhs.x1;
  this->y1 = rhs.y1;
  this->x2 = rhs.x2;
  this->y2 = rhs.y2;
  this->rotation = rhs.rotation;

  if (this->iconSource)
    delete[] this->iconSource;

  if (rhs.iconSource)
  {
    size_t size = strlen(rhs.iconSource) + 1;
    this->iconSource = new char[size];
    memcpy(this->iconSource, rhs.iconSource, size*sizeof(char));
  }
  else
    this->iconSource = NULL;

  this->iconRotation = rhs.iconRotation;
  this->iconFlip = rhs.iconFlip;
  this->iconFixedAspectRatio = rhs.iconFixedAspectRatio;

  return *this;
}
