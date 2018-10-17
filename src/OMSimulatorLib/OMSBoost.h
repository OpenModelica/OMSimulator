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

#ifndef _OMS_BOOST_H_
#define _OMS_BOOST_H_

 
#ifdef __cplusplus
extern "C"
{
#endif

#if (BOOST_VERSION < 104600)
#if defined(_MSC_VER) || defined(__MINGW32__)
#include <windows.h>
#endif
#endif 

#ifdef __cplusplus
}
#endif
 
#include <cstdlib>
#include <string>
#include <boost/version.hpp>
// boost version < 1.57 has issues linking boost::filesystem::copy_file
// https://svn.boost.org/trac10/ticket/6124
#if (BOOST_VERSION < 105700)
#define BOOST_NO_CXX11_SCOPED_ENUMS
#endif // #if (BOOST_VERSION < 105700)
#include <boost/filesystem.hpp>
#if (BOOST_VERSION < 105700)
#undef BOOST_NO_CXX11_SCOPED_ENUMS
#endif // #if (BOOST_VERSION < 105700)

#if (BOOST_VERSION >= 105300)
#include <boost/lockfree/queue.hpp>
#include <ctpl.h>
#else // use the standard queue
#include <ctpl_stl.h>
#endif


boost::filesystem::path oms_temp_directory_path(void);
boost::filesystem::path oms_canonical(boost::filesystem::path p);
boost::filesystem::path oms_unique_path(std::string prefix);

#endif