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

#include "Element.h"

#include "OMSString.h"


oms::Element::Element(oms_element_enu_t type, const oms::ComRef& name)
{
  this->type = type;
  this->name = allocateAndCopyString(name.c_str());
  this->elements = nullptr;
  this->connectors = nullptr;
  this->busconnectors = nullptr;
  this->geometry = reinterpret_cast<ssd_element_geometry_t*>(new oms::ssd::ElementGeometry());
}

oms::Element::~Element()
{
  if (this->name)
    delete[] this->name;

  // lochel: don't delete the sub-elements

  if (this->geometry)
    delete reinterpret_cast<oms::ssd::ElementGeometry*>(this->geometry);
}

void oms::Element::setName(const oms::ComRef& name)
{
  if (this->name)
    delete[] this->name;

  this->name = allocateAndCopyString(name.c_str());
}

void oms::Element::setGeometry(const oms::ssd::ElementGeometry* newGeometry)
{
  if (this->geometry)
  {
    delete reinterpret_cast<oms::ssd::ElementGeometry*>(this->geometry);
    this->geometry = nullptr;
  }

  if (newGeometry)
    this->geometry = reinterpret_cast<ssd_element_geometry_t*>(new oms::ssd::ElementGeometry(*newGeometry));
}

void oms::Element::setConnectors(oms::Connector** newConnectors)
{
  this->connectors = reinterpret_cast<oms_connector_t**>(newConnectors);
}

void oms::Element::setBusConnectors(oms::BusConnector **newBusConnectors)
{
  this->busconnectors = reinterpret_cast<oms_busconnector_t**>(newBusConnectors);
}

void oms::Element::setSubElements(oms_element_t** subelements)
{
  this->elements = subelements;
}
