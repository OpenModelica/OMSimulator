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

#include "ConnectionGeometry.h"

#include "../Logging.h"
#include "Tags.h"

#include <string.h>

oms2::ssd::ConnectionGeometry::ConnectionGeometry()
{
  logTrace();

  this->pointsX = NULL;
  this->pointsY = NULL;
  this->n = 0;
}

oms2::ssd::ConnectionGeometry::ConnectionGeometry(const oms2::ssd::ConnectionGeometry& rhs)
{
  logTrace();

  this->n = rhs.n;

  if (rhs.n > 0)
  {
    this->pointsX = new double[rhs.n];
    this->pointsY = new double[rhs.n];
    memcpy(this->pointsX, rhs.pointsX, n*sizeof(double));
    memcpy(this->pointsY, rhs.pointsY, n*sizeof(double));
  }
  else
  {
    this->pointsX = NULL;
    this->pointsY = NULL;
  }
}

oms2::ssd::ConnectionGeometry::~ConnectionGeometry()
{
  logTrace();

  if (n > 0)
  {
    delete[] pointsX;
    delete[] pointsY;
  }
}

oms2::ssd::ConnectionGeometry& oms2::ssd::ConnectionGeometry::operator=(oms2::ssd::ConnectionGeometry const& rhs)
{
  logTrace();

  // check for self-assignment
  if(&rhs == this)
    return *this;

  if (this->n > 0)
  {
    delete[] this->pointsX;
    delete[] this->pointsY;
  }

  this->n = rhs.n;

  if (rhs.n > 0)
  {
    this->pointsX = new double[rhs.n];
    this->pointsY = new double[rhs.n];
    memcpy(this->pointsX, rhs.pointsX, n*sizeof(double));
    memcpy(this->pointsY, rhs.pointsY, n*sizeof(double));
  }
  else
  {
    this->pointsX = NULL;
    this->pointsY = NULL;
  }

  return *this;
}

void oms2::ssd::ConnectionGeometry::setPoints(unsigned int n_, double* pointsX_, double* pointsY_)
{
  if (this->n > 0)
  {
    delete[] this->pointsX;
    delete[] this->pointsY;
  }

  this->n = n_;

  if (n_ > 0)
  {
    this->pointsX = new double[n_];
    this->pointsY = new double[n_];
    memcpy(this->pointsX, pointsX_, n_*sizeof(double));
    memcpy(this->pointsY, pointsY_, n_*sizeof(double));
  }
  else
  {
    this->pointsX = NULL;
    this->pointsY = NULL;
  }
}

oms_status_enu_t oms2::ssd::ConnectionGeometry::exportToSSD(pugi::xml_node& root) const
{
  // export ssd:ConnectionGeometry
  if (getLength() > 0)
  {
    pugi::xml_node node = root.append_child(oms2::ssd::ssd_connection_geometry);
    const double* pointsX = getPointsX();
    const double* pointsY = getPointsY();
    std::string pointsXStr, pointsYStr;
    for (int i = 0 ; i < getLength() ; i++)
    {
      pointsXStr += std::to_string(pointsX[i]);
      pointsYStr += std::to_string(pointsY[i]);
      if (i != getLength() - 1)
      {
        pointsXStr += " ";
        pointsYStr += " ";
      }
    }
    node.append_attribute("pointsX") = pointsXStr.c_str();
    node.append_attribute("pointsY") = pointsYStr.c_str();
  }

  return oms_status_ok;
}
