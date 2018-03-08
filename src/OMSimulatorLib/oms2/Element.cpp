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

#include "Element.h"
#include "ssd/ElementGeometry.h"
#include "Logging.h"
#include "Connector.h"

#include <cstring>

oms2::Element::Element(oms_element_type_enu_t type, const oms2::ComRef& name)
{
  this->type = type;

  std::string str = name.toString();
  this->name = new char[str.size()+1];
  strcpy(this->name, str.c_str());

  this->interfaces = NULL;

  this->geometry = reinterpret_cast<ssd_element_geometry_t*>(new oms2::ssd::ElementGeometry());
}

oms2::Element::~Element()
{
  if (this->name)
    delete[] this->name;
  if (this->interfaces)
  {
    for (int i=0; this->interfaces[i]; ++i)
      delete this->interfaces[i];
    delete[] this->interfaces;
  }
  if (this->geometry)
    delete reinterpret_cast<oms2::ssd::ElementGeometry*>(this->geometry);
}

void oms2::Element::setName(const oms2::ComRef& name)
{
  if (this->name)
    delete[] this->name;

  std::string str = name.toString();
  this->name = new char[str.size()+1];
  strcpy(this->name, str.c_str());
}

void oms2::Element::setGeometry(const oms2::ssd::ElementGeometry* newGeometry)
{
  if (this->geometry)
  {
    delete reinterpret_cast<oms2::ssd::ElementGeometry*>(this->geometry);
    this->geometry = NULL;
  }

  if (newGeometry)
  {
    this->geometry = reinterpret_cast<ssd_element_geometry_t*>(new oms2::ssd::ElementGeometry(*newGeometry));
  }
}

void oms2::Element::setInterfaces(const std::vector<oms2::Connector> newInterfaces)
{
  logTrace();

  if (this->interfaces)
  {
    for (int i=0; this->interfaces[i]; ++i)
      delete this->interfaces[i];
    delete[] this->interfaces;
  }

  this->interfaces = reinterpret_cast<oms_connector_t**>(new oms2::Connector*[newInterfaces.size()+1]);
  this->interfaces[newInterfaces.size()] = NULL;

  for (int i=0; i<newInterfaces.size(); ++i)
    this->interfaces[i] = reinterpret_cast<oms_connector_t*>(new oms2::Connector(newInterfaces[i]));
}
