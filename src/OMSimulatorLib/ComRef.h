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

#ifndef _OMS2_COM_REF_H_
#define _OMS2_COM_REF_H_

#include <string>
#include <cstring>

namespace oms3
{
  /**
   * \brief ComRef - component reference
   */
  class ComRef
  {
  public:
    ComRef(const std::string& path);
    ~ComRef();

    // methods to copy the component reference
    ComRef(const ComRef& copy);
    ComRef& operator=(const ComRef& copy);
    ComRef operator+(const ComRef& rhs);

    static bool isValidIdent(const std::string& ident);
    bool isValidIdent() const;
    bool isEmpty() const;

    ComRef front();
    ComRef pop_front();

    const char* c_str() const {return cref;}
    operator std::string() const {return std::string(cref);}

  private:
    char* cref = NULL;
  };

  std::string operator+(const std::string& lhs, const ComRef& rhs);
  bool operator==(const ComRef& lhs, const ComRef& rhs);
  bool operator!=(const ComRef& lhs, const ComRef& rhs);
  bool operator<(const ComRef& lhs, const ComRef& rhs);
}

/* ************************************ */
/* oms2                                 */
/*                                      */
/*                                      */
/* ************************************ */

#include <deque>

namespace oms2
{
  /**
   * \brief ComRef - component reference
   */
  class ComRef
  {
  public:
    ComRef();
    ComRef(const std::string& path);
    ~ComRef();

    // methods to copy the component reference
    ComRef(ComRef const& copy);
    ComRef& operator=(ComRef const& copy);
    ComRef operator+(const ComRef& rhs);

    static bool isValidIdent(const std::string& ident);
    bool isValidIdent() const;
    bool isValidQualified() const;

    const char* c_str() const {return isIdent() ? path[0].c_str() : NULL;}

    bool isQualified() const; // true if qualified component name, e.g. a.b.c
    bool isIdent() const;     // true if non-qualifed component name, e.g. x

    std::string toString() const;
    ComRef first() const;
    ComRef last() const;
    void popFirst();
    void popLast();
    ComRef& append(const ComRef& cref);

    /**
     * \brief Can be used to compare crefs.
     *
     * x.y.z == x.y.z
     * x.y.z == z
     *     z == x.y.z
     * x.y   != x
     * x     != x.y
     */
    bool match(const ComRef& cref);

    bool isEqual(const char* str) const {return toString().compare(str) == 0;}

  private:
    std::deque<std::string> path;

    friend bool operator==(const ComRef& lhs, const ComRef& rhs);
  };

  std::string operator+(const std::string& lhs, const ComRef& rhs);
  bool operator<(const ComRef& lhs, const ComRef& rhs);
  bool operator==(const ComRef& lhs, const ComRef& rhs);
}

#endif
