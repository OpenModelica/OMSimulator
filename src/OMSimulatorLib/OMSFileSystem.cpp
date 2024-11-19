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

#include "OMSFileSystem.h"
#include <cstring>

#if defined(_MSC_VER) || defined(__MINGW32__) || defined(__MINGW64__)
#ifdef __cplusplus
extern "C"
{
#endif
#define NOMINMAX
#include <windows.h>
#ifdef __cplusplus
}
#endif
#endif

filesystem::path naive_uncomplete(const filesystem::path& path, const filesystem::path& base)
{
  if (path.has_root_path())
  {
    if (path.root_path() != base.root_path())
      return path;
    else
      return naive_uncomplete(path.relative_path(), base.relative_path());
  }

  if (base.has_root_path())
    throw "cannot uncomplete a relative path from a rooted base";

  typedef filesystem::path::const_iterator path_iterator;
  path_iterator path_it = path.begin();
  path_iterator base_it = base.begin();
  while (path_it != path.end() && base_it != base.end())
  {
    if (*path_it != *base_it)
      break;
    ++path_it; ++base_it;
  }

  filesystem::path result;
  for (; base_it != base.end(); ++base_it)
    result /= "..";
  for (; path_it != path.end(); ++path_it)
    result /= *path_it;
  return result;
}

filesystem::path oms_unique_path(const std::string& prefix)
{
  const char lt[] = "0123456789abcdefghijklmnopqrstuvwxyz";
  int size = strlen(lt);

  std::string s = prefix + "-";
  for(int i=0; i<8; i++)
    s += std::string(1, lt[rand() % size]);

  return filesystem::path(s);
}

void oms_copy_file(const filesystem::path& from, const filesystem::path& to)
{
#if defined(__MINGW32__) || defined(__MINGW64__)
  /* The MINGW implementation succeeds for filesystem::copy_file, but does not
     copy the entire file.
   */
  if (!CopyFile(from.string().c_str(), to.string().c_str(), 0 /* overwrite existing */)) {
    throw std::runtime_error(std::string("Failed to copy file: ") + from.string() + "  to: " + to.string());
  }
#else
  filesystem::copy_file(from, to, filesystem::copy_options::overwrite_existing);
#endif
}

filesystem::path oms_canonical(const filesystem::path& p)
{
  try
  {
    return filesystem::canonical(p);
  }
  catch (const std::exception&)
  {
    // do nothing, canonical fails if the directory contains a junction or a symlink!
    return p;
  }

}

filesystem::path oms_absolute(const filesystem::path& p)
{
  return filesystem::absolute(p);
}

filesystem::path oms_temp_directory_path(void)
{
  return filesystem::temp_directory_path();
}
