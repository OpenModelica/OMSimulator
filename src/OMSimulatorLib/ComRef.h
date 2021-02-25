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

#ifndef _OMS_COM_REF_H_
#define _OMS_COM_REF_H_

#include <string>
#include <cstring>

namespace oms
{
  /**
   * \brief ComRef - component reference
   *
   * A component reference is a qualified name of a component. It uses
   * '.' as component separator. It may also contain a ':' followed by
   * a suffix string which is used to define attributes or filenames.
   */
  class ComRef
  {
  public:
    ComRef();
    ComRef(const std::string& path);
    ComRef(const char* path);
    ~ComRef();

    // methods to copy the component reference
    ComRef(const ComRef& copy);
    ComRef& operator=(const ComRef& copy);
    ComRef operator+(const ComRef& rhs) const;

    static bool isValidIdent(const std::string& ident);
    bool isValidIdent() const;

    bool isEmpty() const; ///< return true if the no component is specified (still, a suffix might be present)
    bool isRootOf(ComRef child) const;

    ComRef front() const;
    ComRef pop_front();

    std::string suffix() const; ///< returns the suffix as string
    std::string pop_suffix(); ///< returns the suffix as string and removes it from the current object
    bool pop_suffix(const std::string& suffix);

    bool hasSuffix() const; ///< returns true if the cref has a suffix, i.e. contains ":"
    bool hasSuffix(const std::string& suffix) const; ///< returns true if the cref has a suffix that matches the argument

    const char* c_str() const {return cref;}
    size_t size() {return strlen(cref);}
    operator std::string() const {return std::string(cref);}

  private:
    char* cref;
  };

  bool operator==(const ComRef& lhs, const ComRef& rhs);
  bool operator!=(const ComRef& lhs, const ComRef& rhs);
  bool operator<(const ComRef& lhs, const ComRef& rhs);
}

namespace std
{
  template <>
  struct hash<oms::ComRef>
  {
    size_t operator()(const oms::ComRef& cref) const
    {
      return hash<std::string>()(std::string(cref));
    }
  };
}

#endif
