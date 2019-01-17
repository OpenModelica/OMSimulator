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

#include "ComRef.h"
#include "Types.h"
#include "Identifier.h"

oms::ComRef::ComRef(const std::string& path)
{
  cref = new char[path.size() + 1];
  strcpy(cref, path.c_str());
}

oms::ComRef::ComRef(const char* path)
{
  cref = new char[strlen(path) + 1];
  strcpy(cref, path);
}

oms::ComRef::~ComRef()
{
  delete[] cref;
}

oms::ComRef::ComRef(const oms::ComRef& copy)
{
  cref = new char[strlen(copy.c_str()) + 1];
  strcpy(cref, copy.c_str());
}

oms::ComRef& oms::ComRef::operator=(const oms::ComRef& copy)
{
  // check for self-assignment
  if(&copy == this)
    return *this;

  delete[] cref;
  cref = new char[strlen(copy.c_str()) + 1];
  strcpy(cref, copy.c_str());

  return *this;
}

oms::ComRef oms::ComRef::operator+(const oms::ComRef& rhs) const
{
  return oms::ComRef(std::string(*this) + "." + std::string(rhs));
}

bool oms::ComRef::isValidIdent(const std::string& ident)
{
  return oms_regex_match(ident, regex_ident);
}

bool oms::ComRef::isValidIdent() const
{
  return isValidIdent(cref);
}

bool oms::ComRef::isEmpty() const
{
  return !(cref && cref[0] != '\0');
}

oms::ComRef oms::ComRef::front() const
{
  int dot=0;

  for(int i=0; cref[i] && dot==0; ++i)
    if(cref[i] == '.')
      dot = i;

  if (dot)
    cref[dot] = '\0';

  oms::ComRef front(cref);

  if (dot)
    cref[dot] = '.';
  return front;
}

oms::ComRef oms::ComRef::pop_front()
{
  int i=0;
  for(; cref[i]; ++i)
    if(cref[i] == '.')
    {
      cref[i] = '\0';
      i++;
      break;
    }

  oms::ComRef front(cref);
  *this = oms::ComRef(cref + i);
  return front;
}

bool oms::operator==(const oms::ComRef& lhs, const oms::ComRef& rhs)
{
  return (0 == strcmp(lhs.c_str(), rhs.c_str()));
}

bool oms::operator!=(const oms::ComRef& lhs, const oms::ComRef& rhs)
{
  return !(lhs == rhs);
}

bool oms::operator<(const oms::ComRef& lhs, const oms::ComRef& rhs)
{
  return (0 < strcmp(lhs.c_str(), rhs.c_str()));
}
