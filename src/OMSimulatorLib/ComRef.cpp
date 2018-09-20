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

oms3::ComRef::ComRef(const std::string& path)
{
  cref = new char[path.size() + 1];
  strcpy(cref, path.c_str());
}

oms3::ComRef::~ComRef()
{
  delete[] cref;
}

oms3::ComRef::ComRef(const oms3::ComRef& copy)
{
  cref = new char[strlen(copy.c_str()) + 1];
  strcpy(cref, copy.c_str());
}

oms3::ComRef& oms3::ComRef::operator=(const oms3::ComRef& copy)
{
  // check for self-assignment
  if(&copy == this)
    return *this;

  delete[] cref;
  cref = new char[strlen(copy.c_str()) + 1];
  strcpy(cref, copy.c_str());

  return *this;
}

oms3::ComRef oms3::ComRef::operator+(const oms3::ComRef& rhs)
{
  return oms3::ComRef(std::string(*this) + std::string(".") + std::string(rhs));
}

bool oms3::ComRef::isValidIdent(const std::string& ident)
{
  return oms_regex_match(ident, regex_ident);
}

bool oms3::ComRef::isValidIdent() const
{
  return isValidIdent(cref);
}

bool oms3::ComRef::isEmpty() const
{
  return !(cref && cref[0] != '\0');
}

oms3::ComRef oms3::ComRef::front()
{
  int dot=0;

  for(int i=0; cref[i] && dot==0; ++i)
    if(cref[i] == '.')
      dot = i;

  if (dot)
    cref[dot] = '\0';

  oms3::ComRef front(cref);

  if (dot)
    cref[dot] = '.';
  return front;
}

oms3::ComRef oms3::ComRef::pop_front()
{
  int i=0;
  for(; cref[i]; ++i)
    if(cref[i] == '.')
    {
      cref[i] = '\0';
      i++;
      break;
    }

  oms3::ComRef front(cref);
  *this = oms3::ComRef(cref + i);
  return front;
}

std::string oms3::operator+(const std::string& lhs, const oms3::ComRef& rhs)
{
  return lhs + rhs;
}

bool oms3::operator==(const oms3::ComRef& lhs, const oms3::ComRef& rhs)
{
  return (0 == strcmp(lhs.c_str(), rhs.c_str()));
}

bool oms3::operator!=(const oms3::ComRef& lhs, const oms3::ComRef& rhs)
{
  return !(lhs == rhs);
}

bool oms3::operator<(const oms3::ComRef& lhs, const oms3::ComRef& rhs)
{
  return (0 < strcmp(lhs.c_str(), rhs.c_str()));
}

/* ************************************ */
/* oms2                                 */
/*                                      */
/*                                      */
/* ************************************ */

#include "Logging.h"

#include <deque>
#include <string>

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

oms2::ComRef oms2::ComRef::operator+(const oms2::ComRef& rhs)
{
  oms2::ComRef cref = *this;
  cref.append(rhs);
  return cref;
}

bool oms2::ComRef::isValidIdent(const std::string& ident)
{
  oms_regex re("^[a-zA-Z][a-zA-Z0-9_]*$");
  return oms_regex_match(ident, re);
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

oms2::ComRef oms2::ComRef::first() const
{
  if (path.empty())
    return oms2::ComRef();

  return oms2::ComRef(path.front());
}

oms2::ComRef oms2::ComRef::last() const
{
  if (path.empty())
    return oms2::ComRef();

  return oms2::ComRef(path.back());
}

void oms2::ComRef::popFirst()
{
  if (!path.empty())
    path.pop_front();
}

void oms2::ComRef::popLast()
{
  if (!path.empty())
    path.pop_back();
}

oms2::ComRef& oms2::ComRef::append(const oms2::ComRef& cref)
{
  for(auto n : cref.path)
    this->path.push_back(n);

  return *this;
}

bool oms2::ComRef::match(const oms2::ComRef& cref)
{
  for (int i=path.size()-1, j=cref.path.size()-1; i >= 0 && j >= 0; --i, --j)
    if (path[i] != cref.path[i])
      return false;
  return true;
}

std::string oms2::operator+(const std::string& lhs, const oms2::ComRef& rhs)
{
  return lhs + rhs.toString();
}

bool oms2::operator<(const oms2::ComRef& lhs, const oms2::ComRef& rhs)
{
  return lhs.toString() < rhs.toString();
}

bool oms2::operator==(const oms2::ComRef& lhs, const oms2::ComRef& rhs)
{
  if (lhs.path.size() != rhs.path.size())
    return false;

  for (int i=0; i<lhs.path.size(); ++i)
    if (lhs.path[i] != rhs.path[i])
      return false;

  return true;
}
