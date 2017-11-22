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

#ifndef _OMS_UTIL_H_
#define _OMS_UTIL_H_

#include <string>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <locale>
#include <math.h>

// trim from start (in place)
// https://stackoverflow.com/a/217605/7534030
static inline void ltrim(std::string &s)
{
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) { return !std::isspace(ch); }));
}

// trim from end (in place)
// https://stackoverflow.com/a/217605/7534030
static inline void rtrim(std::string &s)
{
  s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) { return !std::isspace(ch); }).base(), s.end());
}

// trim from both ends (in place)
// https://stackoverflow.com/a/217605/7534030
static inline void trim(std::string &s)
{
  ltrim(s);
  rtrim(s);
}

const double DOUBLEEQUAL_ABSTOL = 1e-10;
const double DOUBLEEQUAL_RELTOL = 1e-5;

// http://randomascii.wordpress.com/2012/02/25/comparing-floating-point-numbers-2012-edition/
static inline bool almostEqualRelativeAndAbs(double a, double b, double reltol=DOUBLEEQUAL_RELTOL, double abstol=DOUBLEEQUAL_ABSTOL)
{
  double diff = fabs(a - b);
  if (diff <= abstol)
    return true;

  if (diff <= fmax(fabs(a), fabs(b)) * reltol)
    return false;

  return false;
}

#endif
