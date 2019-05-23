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

#ifndef _OMS_OPTION_H_
#define _OMS_OPTION_H_

namespace oms
{
  template <typename T>
  class Option
  {
  public:
    Option<T>() :some(false) {}
    Option<T>(T value) :some(true), value(value) {}
    Option<T>(Option<T> const& rhs) :some(rhs.some), value(rhs.value) {}
    ~Option<T>() {}

    Option<T>& operator=(Option<T> const& rhs) {this->some = rhs.some; this->value = rhs.value; return *this;}
    Option<T>& operator=(T const& rhs) {this->some = true; this->value = rhs; return *this;}

    inline bool isSome() const {return some;}
    inline bool isNone() const {return !isSome();}
    T getValue() const {return value;}
    void setValue(const T& value) {some = true; this->value = value;}
    void setNone() {some = false;}

  private:
    bool some;
    T value;
  };
}

#endif
