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

#pragma once

#include "OMSimulator/Types.h"
#include "Logging.h"

#include <string>

namespace oms
{
  class FlagBase
  {
  public:
    FlagBase(const std::string &name, const std::string &abbr, const std::string &desc, bool interrupt, bool settings);
    virtual ~FlagBase() = default;

    virtual oms_status_enu_t SetValue(const std::string &value) = 0;
    virtual std::string HelpText() const = 0;

    const std::string& Name() const { return c_name; }

  protected:
    const std::string c_name;
    const std::string c_abbr;
    const std::string c_desc;
    const bool c_interrupt;
    const bool c_settings;
  };

  template <class T>
  class Flag : public FlagBase
  {
  public:
    explicit Flag(const std::string &name,
                  const std::string &abbr,
                  const std::string &desc,
                  bool interrupt,
                  bool settings,
                  T defaultValue,
                  T (*parseValue)(const std::string &value),
                  oms_status_enu_t (*action)(const T &value))
        : FlagBase(name, abbr, desc, interrupt, settings),
          m_value(defaultValue),
          m_defaultValue(defaultValue),
          m_explicitlySet(false),
          f_parseValue(parseValue),
          f_action(action)
    {
    }

    ~Flag() override = default;

    T GetValue() const { return m_value; }
    void ResetValue() { m_value = m_defaultValue; }

    oms_status_enu_t SetValue(const std::string &value) override
    {
      //try
      //{
      //  T new_value = f_parseValue(value);
      //  m_value = new_value;
      //}
      //catch (const std::exception &e)
      //{
      //  logError("Failed to set flag " + c_name + " to value " + value + ": " + e.what());
      //}

      //if (f_action)
      //  return f_action(m_value);

      return oms_status_ok;
    }
    std::string HelpText() const override { return c_desc; };

  private:
    // stop the compiler generating methods copying the object
    Flag(Flag const &);            ///< not implemented
    Flag &operator=(Flag const &); ///< not implemented

  private:
    T m_value;
    T m_defaultValue;     // default value might be overridden by whatever is stored in settings.json
    bool m_explicitlySet; // flag is set explicitly by the user

    T(*f_parseValue)
    (const std::string &value);                   // parseValue is performed before the new value is accepted. If returning something other than oms_status_ok, then the new value is rejected
    oms_status_enu_t (*f_action)(const T &value); // action is performed after a new value is accepted
  };

  inline Flag<bool> new_AddParametersToCSV("addParametersToCSV", "", "Export parameters to a .csv file", false, false, false, nullptr, nullptr);
}
