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

#include "oms2_ComRef.h"
#include "oms2_Logging.h"

#include <deque>
#include <string>
#include <regex>

oms2::ComRef::ComRef()
{
}

oms2::ComRef::ComRef(const std::string& path)
{
  std::string buf;
  for(auto n : path)
  {
    if(n != '.')
      buf += n;
    else
    {
      this->path.push_back(buf);
      buf.clear();
    }
  }
  this->path.push_back(buf);

  if (!isValidIdent())
    logDebug("ComRef: invalid cref: " + toString());
}

oms2::ComRef::~ComRef()
{
}

oms2::ComRef::ComRef(oms2::ComRef const& copy)
{
  path = copy.path;
}

oms2::ComRef& oms2::ComRef::operator=(oms2::ComRef const& copy)
{
  // check for self-assignment
  if(&copy == this)
    return *this;

  path = copy.path;
  return *this;
}

bool oms2::ComRef::operator<(oms2::ComRef const& rhs)
{
  return toString() < rhs.toString();
}

oms2::ComRef oms2::ComRef::operator+(const oms2::ComRef& rhs)
{
  oms2::ComRef cref = *this;
  cref.append(rhs);
  return cref;
}

bool oms2::ComRef::isValidIdent(const std::string& ident)
{
  std::regex re("^[a-zA-Z]+[a-zA-Z0-9_]*$");
  return std::regex_match(ident, re);
}

bool oms2::ComRef::isValidIdent() const
{
  return path.size() == 1 && isValidIdent(path[0]);
}

bool oms2::ComRef::isValidQualified() const
{
  if (path.empty())
    return false;

  for(auto ident : path)
    if (!isValidIdent(ident))
      return false;

  return true;
}

bool oms2::ComRef::isQualified() const
{
  return path.size() > 1;
}

bool oms2::ComRef::isIdent() const
{
  return path.size() == 1;
}

std::string oms2::ComRef::toString() const
{
  std::string buf;

  for(auto n : path)
    if (buf.empty())
      buf += n;
    else
      buf += "." + n;

  return buf;
}

oms2::ComRef oms2::ComRef::last() const
{
  if (path.empty())
    return oms2::ComRef();

  return oms2::ComRef(path.back());
}

oms2::ComRef& oms2::ComRef::append(const oms2::ComRef& cref)
{
  for(auto n : cref.path)
    this->path.push_back(n);

  return *this;
}
