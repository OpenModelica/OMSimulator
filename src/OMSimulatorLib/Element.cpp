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
#if !defined(NO_TLM)
#include "TLMBusConnector.h"
#endif

#include <cstring>
#include <iostream>

oms3::Element::Element(oms3_element_enu_t type, const oms3::ComRef& name)
{
  this->type = type;

  std::string str = name;
  this->name = new char[str.size()+1];
  strcpy(this->name, str.c_str());

  this->elements = NULL;
  this->connectors = NULL;

  this->geometry = reinterpret_cast<ssd_element_geometry_t*>(new oms3::ssd::ElementGeometry());

  this->busconnectors = NULL;

#if !defined(NO_TLM)
  this->tlmbusconnectors = NULL;
#endif
}

oms3::Element::~Element()
{
  if (this->name)
    delete[] this->name;

  // lochel: don't delete the sub-elements

  if (this->geometry)
    delete reinterpret_cast<oms3::ssd::ElementGeometry*>(this->geometry);
}

void oms3::Element::setName(const oms3::ComRef& name)
{
  if (this->name)
    delete[] this->name;

  std::string str = name;
  this->name = new char[str.size()+1];
  strcpy(this->name, str.c_str());
}

void oms3::Element::setGeometry(const oms3::ssd::ElementGeometry* newGeometry)
{
  if (this->geometry)
  {
    delete reinterpret_cast<oms3::ssd::ElementGeometry*>(this->geometry);
    this->geometry = NULL;
  }

  if (newGeometry)
  {
    this->geometry = reinterpret_cast<ssd_element_geometry_t*>(new oms3::ssd::ElementGeometry(*newGeometry));
  }
}

void oms3::Element::setConnectors(oms3::Connector** newConnectors)
{
  this->connectors = reinterpret_cast<oms_connector_t**>(newConnectors);
}

void oms3::Element::setBusConnectors(oms3::BusConnector **newBusConnectors)
{
  this->busconnectors = reinterpret_cast<oms3_busconnector_t**>(newBusConnectors);
}

#if !defined(NO_TLM)
void oms3::Element::setTLMBusConnectors(oms3::TLMBusConnector **newBusConnectors)
{
  this->tlmbusconnectors = reinterpret_cast<oms3_tlmbusconnector_t**>(newBusConnectors);
}
#endif

void oms3::Element::setSubElements(oms3_element_t** subelements)
{
  this->elements = subelements;
}
