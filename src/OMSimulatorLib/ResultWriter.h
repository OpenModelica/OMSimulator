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

#ifndef _OMS_RESULTWRITER_H_
#define _OMS_RESULTWRITER_H_

#include <string>
#include <vector>

enum SignalType_t
{
  SignalType_REAL,
  SignalType_INT,
  SignalType_BOOL
};

union SignalValue_t
{
  double realValue;
  int intValue;
  bool boolValue;
};

struct Signal
{
  std::string name;
  std::string description;
  SignalType_t type;
};

struct Parameter
{
  Signal signal;
  SignalValue_t value;
};

class ResultWriter
{
public:
  ResultWriter(unsigned int bufferSize);
  virtual ~ResultWriter();

  unsigned int addSignal(const std::string& name, const std::string& description, SignalType_t type);
  void addParameter(const std::string& name, const std::string& description, SignalType_t type, SignalValue_t value);

  bool create(const std::string& filename, double startTime, double stopTime);
  void close();

  void updateSignal(unsigned int id, SignalValue_t value);
  void emit(double time);

private:
  // Stop the compiler generating methods for copying the object
  ResultWriter(ResultWriter const& copy);            // Not Implemented
  ResultWriter& operator=(ResultWriter const& copy); // Not Implemented

protected:
  virtual bool createFile(const std::string& filename, double startTime, double stopTime) = 0;
  virtual void closeFile() = 0;
  virtual void writeFile() = 0;

  std::vector<Signal> signals;
  std::vector<Parameter> parameters;

  double* data_2;
  unsigned int bufferSize;
  unsigned int nEmits;
};

class VoidWriter :
  public ResultWriter
{
public:
  VoidWriter(unsigned int bufferSize) :ResultWriter(bufferSize) {}
  ~VoidWriter() {}

protected:
  bool createFile(const std::string& filename, double startTime, double stopTime) {return true;}
  void closeFile() {}
  void writeFile() {}
};

#endif
