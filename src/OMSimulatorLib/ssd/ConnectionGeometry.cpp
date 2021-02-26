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

#include "ConnectionGeometry.h"

#include "../Logging.h"
#include "Tags.h"
#include "Util.h"

#include <string.h>
#include <sstream>
#include <vector>

oms::ssd::ConnectionGeometry::ConnectionGeometry()
{
  logTrace();

  this->pointsX = NULL;
  this->pointsY = NULL;
  this->n = 0;
}

oms::ssd::ConnectionGeometry::ConnectionGeometry(const oms::ssd::ConnectionGeometry& rhs, bool inverse)
{
  logTrace();

  this->n = rhs.n;

  if (rhs.n > 0)
  {
    this->pointsX = new double[rhs.n];
    this->pointsY = new double[rhs.n];
    memcpy(this->pointsX, rhs.pointsX, n*sizeof(double));
    memcpy(this->pointsY, rhs.pointsY, n*sizeof(double));
    if (inverse)
    {
      reverseArray(this->pointsX, this->n);
      reverseArray(this->pointsY, this->n);
    }
  }
  else
  {
    this->pointsX = NULL;
    this->pointsY = NULL;
  }
}

oms::ssd::ConnectionGeometry::~ConnectionGeometry()
{
  logTrace();

  if (n > 0)
  {
    delete[] pointsX;
    delete[] pointsY;
  }
}

oms::ssd::ConnectionGeometry& oms::ssd::ConnectionGeometry::operator=(oms::ssd::ConnectionGeometry const& rhs)
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

void oms::ssd::ConnectionGeometry::setPoints(unsigned int n_, double* pointsX_, double* pointsY_)
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

oms_status_enu_t oms::ssd::ConnectionGeometry::exportToSSD(pugi::xml_node& root) const
{
  // export ssd:ConnectionGeometry
  if (getLength() > 0)
  {
    pugi::xml_node node = root.append_child(oms::ssp::Draft20180219::ssd::connection_geometry);
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

oms_status_enu_t oms::ssd::ConnectionGeometry::importFromSSD(const pugi::xml_node& node)
{
  std::string pointsXStr = node.attribute("pointsX").as_string();
  std::istringstream pointsXStream(pointsXStr);
  std::vector<std::string> pointsXVector(std::istream_iterator<std::string>{pointsXStream}, std::istream_iterator<std::string>());

  std::string pointsYStr = node.attribute("pointsY").as_string();
  std::istringstream pointsYStream(pointsYStr);
  std::vector<std::string> pointsYVector(std::istream_iterator<std::string>{pointsYStream}, std::istream_iterator<std::string>());

  if (pointsXVector.size() != pointsYVector.size())
    return logError("wrong xml schema detected: " + std::string(node.name()));

  double* pointsX = new double[pointsXVector.size()];
  int i = 0;
  for (const auto& px : pointsXVector)
    pointsX[i++] = std::atof(px.c_str());

  double* pointsY = new double[pointsYVector.size()];
  i = 0;
  for (const auto& py : pointsYVector)
    pointsY[i++] = std::atof(py.c_str());

  setPoints(pointsXVector.size(), pointsX, pointsY);

  delete[] pointsX;
  delete[] pointsY;

  return oms_status_ok;
}
